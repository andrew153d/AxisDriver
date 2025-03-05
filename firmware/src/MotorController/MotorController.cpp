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
  return (degrees / 360) * 64 * 200;
}

double MotorController::stepsToDegrees(long steps)
{
  return (steps / 64) * 360 / 200;
}

void MotorController::CheckForErrors()
{
  motorErrors.bits.TMC_lost_comms = (driver.getVersion() == 0);
  motorErrors.bits.lostPower = (readVBUS() < 5.0f);

  // if (previousErrors.errors > 0 && motorErrors.errors == 0)
  // {
  //   enableOutputs();
  // }
  if (previousErrors.errors == 0 && motorErrors.errors > 0)
  {
    // disableOutputs();
    // DEBUG_PRINTF("Motor Error Present: %4x\n", motorErrors.errors);
    // addrLedController.SetLedState(FLASH_ERROR);
    // controlMode = ControlMode::MOTOR_OFF;
  }

  previousErrors = motorErrors;
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
  TC0->COUNT16.CC[0].reg = TIMER_COUNT; // 0x4010; // 46875 * (1024 / 48000000) = 1 second
  // TC0->COUNT16.CC[1].reg = 10000;//w 0x4010;
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

  setEnablePin(MOTOR_EN);
  setPinsInverted(true, false, true);
  disableOutputs();
  setAcceleration(400 * 64);
  setMaxSpeed(100 * 64);

  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);
  // driver.setStandstillMode(TMC2209base::StandstillMode::BRAKING);
  // driver.setStandstillMode(TMC2209base::StandstillMode::BRAKING);
  SetMotorState(MotorStates::OFF);
  state_change_time_ = millis();
  target_position = 0;

  init_timer();

  enableOutputs();
  setSpeed(1000 * 64);
  move(200 * 64);
  DEBUG_PRINTF("TMC2209 Version: %d\n", driver.getVersion());
}

void MotorController::OnStop()
{
  disableOutputs();
}

void MotorController::OnTimer()
{
  if (*next_pulse > 0)
  {
    PORT->Group[g_APinDescription[MOTOR_STEP].ulPort].OUTTGL.reg = (1 << g_APinDescription[MOTOR_STEP].ulPin);
    __asm__ __volatile__("nop\n\t"
                         "nop\n\t"); // 1 cycle at 120MHz is 8.33ns, min on time is 10ns for TMC2209
    PORT->Group[g_APinDescription[MOTOR_STEP].ulPort].OUTTGL.reg = (1 << g_APinDescription[MOTOR_STEP].ulPin);
  }
  if (next_pulse == &buffer1[DOUBLE_BUF_SIZE - 1])
  {
    buffer_to_update = &buffer1[0];
    next_pulse = &buffer2[0];
    return;
  }
  if (next_pulse == &buffer2[DOUBLE_BUF_SIZE - 1])
  {
    buffer_to_update = &buffer2[0];
    next_pulse = &buffer1[0];
    return;
  }
  next_pulse++;
}
void MotorController::OnRun()
{
  if (buffer_to_update != nullptr)
  {
    // need to put some data into the buffer

    uint8_t *start_ptr = buffer_to_update;
    uint8_t *end_ptr = buffer_to_update + DOUBLE_BUF_SIZE;
    uint8_t *ptr = start_ptr;
    static uint32_t remaining_time = 0;
    memset(start_ptr, 0, DOUBLE_BUF_SIZE);

    float ptr_inc = 0;

    while (ptr < end_ptr)
    {
      if (remaining_time > 0)
      {
        ptr+=remaining_time;
        *ptr=1;
        remaining_time=0;
        continue;
      }
      unsigned long interval = computeNewSpeed();
      unsigned long ptr_inc = interval * TICKS_PER_US;
      //DEBUG_PRINTF("Interval: %lu, Ptr_inc: %lu\n", interval, ptr_inc);
      if (interval == 0)
        break;
      _currentPos++;
      if (ptr + ptr_inc >= end_ptr)
      {
        remaining_time = ptr_inc - (end_ptr - ptr);
        break;
      }
      ptr+=ptr_inc;
      *ptr = 1;
    }

    // for (uint8_t *p = start_ptr; p < end_ptr; p++)
    // {
    //   if(*p)
    //   {
    //     Serial.println();
    //   }
    //   Serial.print(*p);
    //   Serial.print(" ");
    // }
    //  Serial.println();
    //  Serial.println();
    buffer_to_update = nullptr;
  }

  return;
  if (millis() - error_check_timer > error_check_period)
  {
    error_check_timer = millis();
    CheckForErrors();
  }
  static uint32_t timer = 0;
  switch (controlMode)
  {
  case MotorStates::OFF:
    break;
  case MotorStates::POSITION:
    run();
    // if (distanceToGo() == 0)
    //   disableOutputs();
    break;
  case MotorStates::VELOCITY:
    runSpeed();
    if (target_velocity_duration != 0 && (millis() - target_velocity_timer) > target_velocity_duration)
      // controlMode = MotorStates::OFF;
      break;
  case MotorStates::HOME:
  {
    switch (home_state_)
    {
    case HomeState::RUN1:

      setSpeed((homeDirection == HomeDirection::CLOCKWISE) ? (float)homing_speed_ * -1 : (float)homing_speed_);
      runSpeed();
      if ((millis() - state_change_time_ > 200) && abs(encoder_ptr->GetVelocityDegreesPerSecond()) < 5)
      {
        home_state_ = HomeState::BACKUP;
        setCurrentPosition(0);
        move(degreesToSteps((homeDirection == HomeDirection::CLOCKWISE) ? 10 : -10));
      }
      break;
    case HomeState::BACKUP:
      run();
      if (abs(distanceToGo()) < 1)
      {
        home_state_ = HomeState::RUN2;
        setSpeed((homeDirection == HomeDirection::CLOCKWISE) ? (float)homing_speed_ / 2 * -1 : (float)homing_speed_ / 2);
        state_change_time_ = millis();
      }

      break;
    case HomeState::RUN2:
      runSpeed();
      if ((millis() - state_change_time_ > 400) && abs(encoder_ptr->GetVelocityDegreesPerSecond()) < 5)
      {
        SetMotorState(MotorStates::IDLE_ON);
        setCurrentPosition(0);
        home_state_ = HomeState::RUN1;
      }
      break;
    }
    break;
  }
  case MotorStates::IDLE_ON:
    break;
  }
}

void MotorController::SetMotorState(MotorStates state)
{
  state_change_time_ = millis();
  controlMode = state;
  switch (controlMode)
  {
  case MotorStates::OFF:
    disableOutputs();
    break;
  case MotorStates::POSITION:
    driver.setRunCurrent(100);
    enableOutputs();
    break;
  case MotorStates::VELOCITY:
    enableOutputs();
    break;
  case MotorStates::HOME:
    enableOutputs();
    driver.setRunCurrent(25);
    break;
  case MotorStates::IDLE_ON:
    enableOutputs();
    break;
  }
}

MotorStates MotorController::GetMotorState()
{
  return controlMode;
}

void MotorController::SetMotorBraking(MotorBrake brake)
{
  motorBrake = brake;
  driver.setStandstillMode((TMC2209base::StandstillMode)brake);
  DEBUG_PRINTF("Setting Motor Brake: %d\n", brake);
}

MotorBrake MotorController::GetMotorBraking()
{
  return (MotorBrake)driver.pwm_config_.freewheel;
}

void MotorController::SetMaxSpeed(uint32_t spd)
{
  setMaxSpeed(spd);
}

uint32_t MotorController::GetMaxSpeed()
{
  return maxSpeed();
}

void MotorController::SetAcceleration(uint32_t acl)
{
  setAcceleration(acl);
}

uint32_t MotorController::GetAcceleration()
{
  return acceleration();
}

void MotorController::SetPositionTargetRelative(double position)
{
  SetMotorState(MotorStates::POSITION);
  target_position += position;
  moveTo(target_position);
}

void MotorController::SetPositionTarget(double position)
{
  SetMotorState(MotorStates::POSITION);
  target_position = position;
  moveTo(position);
}

double MotorController::GetPositionTarget()
{
  return target_position;
}

void MotorController::SetVelocityTarget(double velocity)
{
  enableOutputs();
  controlMode = MotorStates::VELOCITY;
  target_velocity = velocity;
  setSpeed(velocity);
}

double MotorController::GetVelocityTarget()
{
  return target_velocity;
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