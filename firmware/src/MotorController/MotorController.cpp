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
    // Serial.printf("Motor Error Present: %4x\n", motorErrors.errors);
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

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(50000);
  stepper.setEnablePin(MOTOR_EN);
  stepper.setPinsInverted(true, false, true);
  stepper.disableOutputs();

  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);
  driver.setMicrostepsPerStepPowerOfTwo(4);
  driver.setAllCurrentValues(70, 0, 0);
  driver.enable();
  // driver.getSettings();
  stepper.enableOutputs();
  controlMode = ControlMode::MOTOR_OFF;
  start_millis = millis();
}

void MotorController::OnStop()
{
  stepper.disableOutputs();
}

void MotorController::OnRun()
{
  if (controlMode == MOTOR_OFF && UserButtonPressed() && (millis() - start_millis > 500))
  {
    controlMode = TEST;
    start_millis = millis();
  }
  if (controlMode == TEST && UserButtonPressed() && (millis() - start_millis > 500))
  {
    controlMode = MOTOR_OFF;
    start_millis = millis();
  }

  if (millis() - error_check_timer > error_check_period)
  {
    error_check_timer = millis();
    CheckForErrors();
  }

  switch (controlMode)
  {
  case MOTOR_OFF:
    stepper.setSpeed(0);
    break;
  case DETECTSTEPS:
  {
    static uint8_t step = 0;
    static uint32_t start_time = millis();
    static float speed = 100;
    stepper.runSpeed();
    switch (step)
    {
    case 0:
      stepper.setSpeed(speed += 100);
      start_time = millis();
      step++;
      break;
    case 1:
      if (millis() - start_time > 2000)
      {
        float actual_speed = encoder_ptr->GetVelocityDegreesPerSecond() * (200.0f / 360.0f);
        Serial.printf("Target: %f, Speed: %f, Error: %f\n", speed, actual_speed, speed - (encoder_ptr->GetVelocityDegreesPerSecond() * (200 / 360)));
        Serial.println(driver.getVersion());
        Serial.println(readVBUS());
        Serial.println(readVUSB());
        step = 0;
      }
      break;
    }
  }
  break;
  case TEST:
  {

    stepper.enableOutputs();
    target = (int)((bool)(((int)(millis() / 10000)) % 2) == 0) * (300);
    float error = target - encoder_ptr->GetPositionDegrees();
    float speed = pid(error);
    if (abs(error) < 1)
      speed = 0;
    stepper.setSpeed(speed);
    stepper.runSpeed();
    if (millis() - start_millis > 1000)
    {
      start_millis = millis();
      Serial.printf("Target: %f, Speed: %f, Error: %f\n", target, speed, error);
      Serial.println(driver.getVersion());
      Serial.println(readVBUS());
      Serial.println(readVUSB());
      Serial.println(stepper.speed());
      PrintErrorsToSerial();
    }
    break;
  }
  case POSITION:
    break;
  case VELOCITY:
    break;
  case TORQUE:
    break;
  case TEST2:
  {
    if (millis() - start_millis > 5000)
    {
      controlMode = MOTOR_OFF;
    }
    stepper.setSpeed(100);
    stepper.runSpeed();
  }
  break;
  }

  // static uint32_t spot_timer = 0;
  //   if(millis()-spot_timer>100){
  //     spot_timer = millis();
  //     Serial.printf("Target: %8.1f, Position: %8.1f, Error: %8.1f, Speed: %8.1f\n", target, *encoder_ptr, error, speed);
  // }

  // Serial.printf("Target: %8.1f, Position: %8.1f, Error: %8.1f, Speed: %8.1f\n", target, *encoder_ptr, error, speed);
  // Serial.printf("%8.1f %8.1f %8.1f %8.1f\n", target, *encoder_ptr, error, speed);
}

void MotorController::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
  if (recv_bytes_size < HEADER_SIZE + FOOTER_SIZE)
  {
    Serial.println("Size too small");
  }

  Header *header = (Header *)recv_bytes;
  if (recv_bytes_size < HEADER_SIZE + header->body_size + FOOTER_SIZE)
  {
    Serial.printf("Invalid body size: recv_bytes:%d, header_size:%d, body_size:%d, footer_size:%d\n", recv_bytes_size, HEADER_SIZE, header->body_size, FOOTER_SIZE);
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
  Serial.printf("reset: %u\n", gstat.reset);
  Serial.printf("drv_error: %u\n", gstat.drv_err);
  Serial.printf("uv_cp: %u\n", gstat.uv_cp);
  Serial.printf("over_temperature_warning: %u\n", stat.over_temperature_warning);
  Serial.printf("over_temperature_shutdown: %u\n", stat.over_temperature_shutdown);
  Serial.printf("short_to_ground_a: %u\n", stat.short_to_ground_a);
  Serial.printf("short_to_ground_b: %u\n", stat.short_to_ground_b);
  Serial.printf("low_side_short_a: %u\n", stat.low_side_short_a);
  Serial.printf("low_side_short_b: %u\n", stat.low_side_short_b);
  Serial.printf("open_load_a: %u\n", stat.open_load_a);
  Serial.printf("open_load_b: %u\n", stat.open_load_b);
  Serial.printf("over_temperature_120c: %u\n", stat.over_temperature_120c);
  Serial.printf("over_temperature_143c: %u\n", stat.over_temperature_143c);
  Serial.printf("over_temperature_150c: %u\n", stat.over_temperature_150c);
  Serial.printf("over_temperature_157c: %u\n", stat.over_temperature_157c);
  Serial.printf("reserved0: %u\n", stat.reserved0);
  Serial.printf("current_scaling: %u\n", stat.current_scaling);
  Serial.printf("reserved1: %u\n", stat.reserved1);
  Serial.printf("stealth_chop_mode: %u\n", stat.stealth_chop_mode);
  Serial.printf("standstill: %u\n", stat.standstill);
}

MotorController motorController(0);