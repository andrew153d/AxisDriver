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
  return (degrees/360) * 64 * 200;
}

double MotorController::stepsToDegrees(long steps)
{
  return (steps/64) * 360 / 200;
}

void MotorController::CheckForErrors()
{
  motorErrors.bits.TMC_lost_comms = (driver.getVersion() == 0);
  motorErrors.bits.lostPower = (readVBUS() < 5.0f);

  if (previousErrors.errors > 0 && motorErrors.errors == 0)
  {
    stepper.enableOutputs();
  }
  if (previousErrors.errors == 0 && motorErrors.errors > 0)
  {
    // stepper.disableOutputs();
    // DEBUG_PRINTF("Motor Error Present: %4x\n", motorErrors.errors);
    // addrLedController.SetLedState(FLASH_ERROR);
    // controlMode = ControlMode::MOTOR_OFF;
  }

  previousErrors = motorErrors;
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
  stepper.setAcceleration(400 * 64);
  stepper.setMaxSpeed(100 * 64);

  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);
  // driver.setStandstillMode(TMC2209base::StandstillMode::BRAKING);
  // driver.setStandstillMode(TMC2209base::StandstillMode::BRAKING);
  SetMotorState(MotorStates::OFF);
  state_change_time_ = millis();
  target_position = 0;

  DEBUG_PRINTF("TMC2209 Version: %d\n", driver.getVersion());
}

void MotorController::OnStop()
{
  stepper.disableOutputs();
}

void MotorController::OnRun()
{
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
    stepper.run();
    // if (stepper.distanceToGo() == 0)
    //   stepper.disableOutputs();
    break;
  case MotorStates::VELOCITY:
    stepper.runSpeed();
    if (target_velocity_duration != 0 && (millis() - target_velocity_timer) > target_velocity_duration)
      //controlMode = MotorStates::OFF;
    break;
  case MotorStates::HOME:
  {
    switch (home_state_)
    {
    case HomeState::RUN1:

      stepper.setSpeed((homeDirection == HomeDirection::CLOCKWISE)?(float)homing_speed_*-1:(float)homing_speed_);
      stepper.runSpeed();
      if ((millis() - state_change_time_ > 200) && abs(encoder_ptr->GetVelocityDegreesPerSecond()) < 5)
      {
        home_state_ = HomeState::BACKUP;
        stepper.setCurrentPosition(0);
        stepper.move(degreesToSteps((homeDirection == HomeDirection::CLOCKWISE)?10:-10));
      }
      break;
    case HomeState::BACKUP:
      stepper.run();
      if(abs(stepper.distanceToGo()) < 1)
      {
        home_state_ = HomeState::RUN2;
        stepper.setSpeed((homeDirection == HomeDirection::CLOCKWISE)?(float)homing_speed_/2*-1:(float)homing_speed_/2);
        state_change_time_ = millis();
      }

      break;
    case HomeState::RUN2:
    stepper.runSpeed();
      if ((millis() - state_change_time_ > 400) && abs(encoder_ptr->GetVelocityDegreesPerSecond()) < 5)
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
}

void MotorController::SetMotorState(MotorStates state)
{
  state_change_time_ = millis();
  controlMode = state;
  switch (controlMode)
  {
  case MotorStates::OFF:
    stepper.disableOutputs();
    break;
  case MotorStates::POSITION:
    stepper.enableOutputs();
    break;
  case MotorStates::VELOCITY:
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

void MotorController::SetPositionTargetRelative(double position)
{
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