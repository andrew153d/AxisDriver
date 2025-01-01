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
  stepper.setAcceleration(800 * 64);
  stepper.setMaxSpeed(2500 * 64);

  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);

  controlMode = ControlMode::MOTOR_OFF;
  start_millis = millis();
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

  switch (controlMode)
  {
  case MOTOR_OFF:
    stepper.disableOutputs();
    controlMode = IDLE;
    DEBUG_PRINTLN("Turning off motor");
    break;
  case POSITION:
    stepper.run();
    if (stepper.distanceToGo() == 0)
      controlMode = MOTOR_OFF;
    break;
  case VELOCITY:
    stepper.runSpeed();
    if(target_velocity_duration!=0 && (millis()-target_velocity_timer)>target_velocity_duration)
      controlMode = MOTOR_OFF;
    break;
  case TORQUE:
    break;
  case IDLE:
  case UNKNOWN:
    break;
  }
}

void MotorController::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
  if (recv_bytes_size == 0)
    return;

  if ((char)recv_bytes[0] == '{')
    HandleJsonMsg(recv_bytes, recv_bytes_size);
  else
    HandleByteMsg(recv_bytes, recv_bytes_size);
}



void MotorController::HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
  DeserializationError error = deserializeJson(received_json, recv_bytes, 256);
  if (error)
  {
    Serial.println("Error parsing json in motor controller");
    return;
  }
 
  if (!TryParseJson<String>("mode", &modeString, received_json))
  {
    DEBUG_PRINTLN("Failed to parse mode");
    return;
  }
  
  controlMode = GetModeFromString(modeString);

  switch (controlMode)
  {
  case ControlMode::POSITION:
  {
    int max_speed = 1000;
    int acceleration = 1000;
    int microsteps = 64;

    if(!TryParseJson<int>("steps", &target_position, received_json))
    {
      Serial.println("Failed to parse steps");
      return;
    }

    TryParseJson<int>("max_speed", &max_speed, received_json);
    TryParseJson<int>("acceleration", &acceleration, received_json);
    TryParseJson<int>("microsteps", &microsteps, received_json);
    TryParseJson<bool>("hold", &hold, received_json);

    if (driver.getMicrostepsPerStep() != microsteps)
      driver.setMicrostepsPerStep(microsteps);
    
    stepper.move(target_position);
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(acceleration);
    stepper.enableOutputs();
    break;
  }
  case ControlMode::VELOCITY:
  {
    int microsteps = 64;
    target_velocity_duration = 0;

    if (!TryParseJson("speed", &target_velocity, received_json))
    {
      Serial.println("Failed to parse speed");
      return;
    }

    TryParseJson("duration", &target_velocity_duration, received_json);
    TryParseJson("microsteps", &microsteps, received_json);    

    if (driver.getMicrostepsPerStep() != microsteps)
      driver.setMicrostepsPerStep(microsteps);

    stepper.setSpeed(target_velocity);
    stepper.enableOutputs();
    target_velocity_timer = millis();
    break;
  }
  default:
    Serial.println("Invalid Mode");
    break;
  }
}

void MotorController::HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
  if (recv_bytes_size < HEADER_SIZE + FOOTER_SIZE)
  {
    Serial.println("Size too small");
  }

  Header *header = (Header *)recv_bytes;
  if (recv_bytes_size < HEADER_SIZE + header->body_size + FOOTER_SIZE)
  {
    DEBUG_PRINTF("Invalid body size: recv_bytes:%d, header_size:%d, body_size:%d, footer_size:%d\n", recv_bytes_size, HEADER_SIZE, header->body_size, FOOTER_SIZE);
    return;
  }

  switch (header->message_type)
  {
  case MessageTypes::GetMotorPosition:
  {
    memset(&send_buffer[0], 0, 1024);
    Header *hdr = (Header *)&send_buffer[0];
    uint32_t *position = (uint32_t *)&send_buffer[HEADER_SIZE];
    Footer *ftr = (Footer *)(&send_buffer[HEADER_SIZE + sizeof(uint32_t)]);

    hdr->message_type = MessageTypes::GetMotorPosition;
    hdr->body_size = sizeof(uint32_t);

    *position = (int32_t)encoder_ptr->GetPositionDegrees();
    addrLedController.SetLedState(FLASH_ERROR);
    ftr->checksum = 0;

    processor_interface_->SendMsg(&send_buffer[0], HEADER_SIZE + 4 + FOOTER_SIZE);
  }
  break;
  default:
    break;
  }
}

void MotorController::setEncoderValueSource(IEncoderInterface *encoder_value)
{
  encoder_ptr = encoder_value;
}

uint32_t MotorController::GetErrors()
{
  return error_flag;
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

ControlMode MotorController::GetModeFromString(String mode)
{
  if (mode == "SetPosition")
    return ControlMode::POSITION;
  if (mode == "SetVelocity")
    return ControlMode::VELOCITY;

  return ControlMode::UNKNOWN;
}

MotorController motorController(0);