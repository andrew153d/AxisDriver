#include "MotorController.h"
#include "LedController/LedController.h"
float readVUSB()
{
  return (float)analogRead(PIN_VUSB) * (3.3f / 1024.0f) * 2;
}

float readVBUS()
{
  return analogRead(PIN_VBUS) * (24.0f / 553.0f);
}

bool UserButtonPressed()
{
  return !digitalRead(USR_INPUT);
}

long MotorController::degreesToSteps(double degrees)
{
  return (degrees / 360) * 8 * 200;
}

double MotorController::stepsToDegrees(long steps)
{
  return (steps / 64) * 360 / 200;
}

void TC0_Handler()
{
  if (TC0->COUNT16.INTFLAG.bit.OVF)
  {
    motorController.OnTimer();
    TC0->COUNT16.INTFLAG.bit.OVF = 1;
  }
}

void init_timer()
{
  // Enable the TC0 module
  MCLK->APBAMASK.bit.TC0_ = 1;

  // Configure the Generic Clock Generator 0 (GCLK0) for TC0
  GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;
  while (GCLK->PCHCTRL[TC0_GCLK_ID].bit.CHEN == 0)
    ;

  GCLK->GENCTRL[1].bit.DIV = 0x00;
  while (GCLK->SYNCBUSY.bit.GENCTRL1)
    ;

  GCLK->GENCTRL[1].bit.DIVSEL = 0;
  while (GCLK->SYNCBUSY.bit.GENCTRL1)
    ;

  Serial.printf("Prescaler: %d\n", GCLK->GENCTRL[0].bit.DIV);

  // Reset TC0
  TC0->COUNT16.CTRLA.bit.SWRST = 1;
  while (TC0->COUNT16.SYNCBUSY.bit.SWRST)
    ;

  TC0->COUNT16.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT16;
  TC0->COUNT16.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV8_Val;
  TC0->COUNT16.WAVE.bit.WAVEGEN = TC_WAVE_WAVEGEN_MFRQ_Val;
  while (TC0->COUNT16.SYNCBUSY.bit.ENABLE)
    ;
  // Set the period (TOP value) for the timer
  TC0->COUNT16.CC[0].reg = TIMER_COUNT;
  while (TC0->COUNT16.SYNCBUSY.bit.CC0)
    ;

  // Enable the overflow interrupt
  TC0->COUNT16.INTENSET.bit.OVF = 1;

  // Enable the TC0 interrupt in the NVIC
  NVIC_EnableIRQ(TC0_IRQn);
  NVIC_SetPriority(TC0_IRQn, 1);

  // Enable TC0
  TC0->COUNT16.CTRLA.bit.ENABLE = 1;
  while (TC0->COUNT16.SYNCBUSY.bit.ENABLE)
    ;
}

void SetTimerIntervalUs(unsigned long interval_us)
{
  // Set the timer period (TOP value) for the timer
  TC0->COUNT16.CC[0].reg = US_TO_TIMER_COUNT(interval_us);
  while (TC0->COUNT16.SYNCBUSY.bit.CC0)
    ;
}

void MotorController::OnStart()
{

  pinMode(MOTOR_EN, OUTPUT);
  pinMode(MOTOR_STEP, OUTPUT);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_M0, OUTPUT);
  pinMode(MOTOR_M1, OUTPUT);
  pinMode(MOTOR_SPREAD, OUTPUT);
  pinMode(MOTOR_DIAG, INPUT);

  digitalWrite(MOTOR_M0, LOW);
  digitalWrite(MOTOR_M1, LOW);
  digitalWrite(MOTOR_EN, HIGH);
  digitalWrite(MOTOR_SPREAD, LOW);

  stepper.setEnablePin(MOTOR_EN);
  stepper.setPinsInverted(true, false, true);
  stepper.disableOutputs();
  stepper.setAcceleration(400 * 8);
  stepper.setMaxSpeed(100 * 8);

  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);

  SetMotorState(MotorStates::OFF);
  state_change_time_ = millis();
  target_position = 0;

  init_timer();

  stepper.enableOutputs();
}

void MotorController::OnStop()
{
  stepper.disableOutputs();
}

void MotorController::OnTimer()
{
  switch (controlMode)
  {
  case MotorStates::OFF:
    break;
  case MotorStates::POSITION:
    stepper.run();
    break;
  case MotorStates::VELOCITY:
    stepper.runSpeed();
    break;
  case MotorStates::VELOCITY_STEP:
  {

    if (stepper.currentPosition() == velocity_step_end)
    {
      if (velocity_steps.size() == 0)
      {
        // DEBUG_PRINTF("Finished velocity step at %ld\n", stepper.currentPosition());
        stepper.setSpeed(0);
        SetMotorState(MotorStates::IDLE_ON);
        return;
      }

      // Done stepping, on to the next
      auto this_move = velocity_steps.front();
      //DEBUG_PRINTF("Current End: %ld, current position: %ld new step: %d\n", velocity_step_end, stepper.currentPosition(), this_move.step);
      if (this_move.positionMode == PositionMode::ABSOLUTE)
      {
        velocity_step_end = this_move.step;
        //DEBUG_PRINTF("New absolute target %ld\n", velocity_step_end);
      }
      else if (this_move.positionMode == PositionMode::RELATIVE)
      {
        velocity_step_end = stepper.currentPosition() + this_move.step;
        //DEBUG_PRINTF("New relative target %ld\n", velocity_step_end);
      }
      if (stepper.currentPosition() > velocity_step_end)
      {
        stepper.setSpeed(-this_move.velocity);
      }
      else
      {
        stepper.setSpeed(this_move.velocity);
      }
      velocity_steps.pop_front();
    }
      stepper.runSpeed();
    break;
  }
  case MotorStates::HOME:
  {
    switch (home_state_)
    {
    case HomeState::RUN1:

      stepper.setSpeed((homeDirection == HomeDirection::CLOCKWISE) ? (float)homing_speed_ * -1 : (float)homing_speed_);
      stepper.runSpeed();
      if ((millis() - state_change_time_ > 300) && abs(encoder_ptr->GetVelocityDegreesPerSecond()) < 1)
      {
        home_state_ = HomeState::BACKUP;
        stepper.setCurrentPosition(0);
        stepper.move(degreesToSteps((homeDirection == HomeDirection::CLOCKWISE) ? 10 : -10));
      }
      break;
    case HomeState::BACKUP:
      stepper.run();
      if (abs(stepper.distanceToGo()) < 1)
      {
        home_state_ = HomeState::RUN2;
        stepper.setSpeed((homeDirection == HomeDirection::CLOCKWISE) ? (float)homing_speed_ / 2 * -1 : (float)homing_speed_ / 2);
        state_change_time_ = millis();
      }

      break;
    case HomeState::RUN2:
      stepper.runSpeed();
      if ((millis() - state_change_time_ > 400) && abs(encoder_ptr->GetVelocityDegreesPerSecond()) < 1)
      {
        SetMotorState(MotorStates::IDLE_ON);
        stepper.setCurrentPosition(0);
        home_state_ = HomeState::RUN1;
      }
      break;
    }
    break;
  }
  case MotorStates::IDLE_ON:
    break;
  }
  unsigned long new_int = stepper.GetStepIntervalUs();
  if(new_int == 0)
  {
    SetTimerIntervalUs(100);
  }else
  {
    SetTimerIntervalUs(new_int);
  }
}

void MotorController::OnRun()
{
  return;
}

void MotorController::SetMotorState(MotorStates state)
{
  state_change_time_ = millis();

  switch (state)
  {
  case MotorStates::OFF:
    stepper.disableOutputs();
    break;
  case MotorStates::POSITION:
    driver.setRunCurrent(100);
    stepper.enableOutputs();
    break;
  case MotorStates::VELOCITY:
    driver.setRunCurrent(100);
    stepper.enableOutputs();
    break;
  case MotorStates::VELOCITY_STEP:
    if (controlMode != MotorStates::VELOCITY_STEP)
    {
      stepper.setSpeed(0); // initialize it to 0
      velocity_step_end = stepper.currentPosition();
      // DEBUG_PRINTF("Starting Velocity Step at %ld\n", velocity_step_end);
    }
    driver.setRunCurrent(100);
    stepper.enableOutputs();
    break;
  case MotorStates::HOME:
    stepper.enableOutputs();
    driver.setRunCurrent(25);
    break;
  case MotorStates::IDLE_ON:
    stepper.enableOutputs();
    break;
  }
  controlMode = state;
}

MotorStates MotorController::GetMotorState()
{
  return controlMode;
}

void MotorController::SetMotorBraking(MotorBrake brake)
{
  motorBrake = brake;
  driver.setStandstillMode((TMC2209base::StandstillMode)brake);
}

MotorBrake MotorController::GetMotorBraking()
{
  return (MotorBrake)driver.pwm_config_.freewheel;
}

void MotorController::SetMaxSpeed(uint32_t spd)
{
  stepper.setMaxSpeed(spd);
}

uint32_t MotorController::GetMaxSpeed()
{
  return stepper.maxSpeed();
}

void MotorController::SetAcceleration(uint32_t acl)
{
  stepper.setAcceleration(acl);
}

uint32_t MotorController::GetAcceleration()
{
  return stepper.acceleration();
}

void MotorController::SetPosition(double position)
{
  stepper.setCurrentPosition((long)position);
  //DEBUG_PRINTF("Setting position to %f, stepper position: %ld\n", position, stepper.currentPosition());
}

double MotorController::GetPosition()
{
  return stepper.currentPosition();
}

void MotorController::SetPositionTargetRelative(double position)
{
  // DEBUG_PRINTF("Steps: %f\n", position);
  SetMotorState(MotorStates::POSITION);
  target_position += position;
  stepper.moveTo(target_position);
}

void MotorController::SetPositionTarget(double position)
{
  SetMotorState(MotorStates::POSITION);
  target_position = position;
  stepper.moveTo(position);
}

double MotorController::GetPositionTarget()
{
  return target_position;
}

void MotorController::SetVelocityTarget(double velocity)
{
  stepper.enableOutputs();
  controlMode = MotorStates::VELOCITY;
  target_velocity = velocity;
  stepper.setSpeed(velocity);
}

double MotorController::GetVelocityTarget()
{
  return target_velocity;
}

void MotorController::AddVelocityStep(int32_t velocity, int32_t step, uint8_t position_mode)
{
  // DEBUG_PRINTF("Adding velocity step: %d, %d\n", velocity, step);
  velocity_steps.push_back({(PositionMode)position_mode, abs(velocity), step});
}

void MotorController::StartPath()
{
  SetMotorState(MotorStates::VELOCITY_STEP);
}

void MotorController::setEncoderValueSource(IEncoderInterface *encoder_value)
{
  encoder_ptr = encoder_value;
}

uint32_t MotorController::GetErrors()
{
  return error_flag;
}

void MotorController::SetHomeDirection(HomeDirection direction)
{
  homeDirection = direction;
}

HomeDirection MotorController::GetHomeDirection()
{
  return homeDirection;
}

void MotorController::Home()
{
  SetMotorState(MotorStates::HOME);
}

void MotorController::PrintErrorsToSerial()
{
  auto stat = driver.getStatus();
  auto gstat = driver.getGlobalStatus();
  DEBUG_PRINTF("reset: %u\n", gstat.reset);
  DEBUG_PRINTF("drv_error: %u\n", gstat.drv_err);
  DEBUG_PRINTF("uv_cp: %u\n", gstat.uv_cp);
  DEBUG_PRINTF("over_temperature_warning: %u\n", stat.over_temperature_warning);
  DEBUG_PRINTF("over_temperature_shutdown: %u\n", stat.over_temperature_shutdown);
  DEBUG_PRINTF("short_to_ground_a: %u\n", stat.short_to_ground_a);
  DEBUG_PRINTF("short_to_ground_b: %u\n", stat.short_to_ground_b);
  DEBUG_PRINTF("low_side_short_a: %u\n", stat.low_side_short_a);
  DEBUG_PRINTF("low_side_short_b: %u\n", stat.low_side_short_b);
  DEBUG_PRINTF("open_load_a: %u\n", stat.open_load_a);
  DEBUG_PRINTF("open_load_b: %u\n", stat.open_load_b);
  DEBUG_PRINTF("over_temperature_120c: %u\n", stat.over_temperature_120c);
  DEBUG_PRINTF("over_temperature_143c: %u\n", stat.over_temperature_143c);
  DEBUG_PRINTF("over_temperature_150c: %u\n", stat.over_temperature_150c);
  DEBUG_PRINTF("over_temperature_157c: %u\n", stat.over_temperature_157c);
  DEBUG_PRINTF("reserved0: %u\n", stat.reserved0);
  DEBUG_PRINTF("current_scaling: %u\n", stat.current_scaling);
  DEBUG_PRINTF("reserved1: %u\n", stat.reserved1);
  DEBUG_PRINTF("stealth_chop_mode: %u\n", stat.stealth_chop_mode);
  DEBUG_PRINTF("standstill: %u\n", stat.standstill);
}

MotorController motorController(0);