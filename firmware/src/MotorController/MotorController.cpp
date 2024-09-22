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
  stepper.enableOutputs();

  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);
  driver.setMicrostepsPerStep(4);
  driver.enableAutomaticCurrentScaling();
  driver.enableStealthChop();
  driver.useExternalSenseResistors();
  //driver.enableAutomaticGradientAdaptation();
  driver.setAllCurrentValues(1, 1, 100);
  driver.enable();
  // driver.getSettings();
  stepper.enableOutputs();
  controlMode = ControlMode::MOTOR_OFF;
  start_millis = millis();
  target = 0;
}

void MotorController::OnStop()
{
  stepper.disableOutputs();
}

void MotorController::OnRun()
{
  if (controlMode == MOTOR_OFF && UserButtonPressed() && (millis() - start_millis > 500))
  {
    // stepper.setSpeed(200);
    // start_millis = millis();
    // while((millis()-start_millis)<1000){
    //   stepper.runSpeed();
    // }
    // stepper.setSpeed(-200);
    // start_millis = millis();
    // while((millis()-start_millis)<1000){
    //   stepper.runSpeed();
    // }
    // stepper.setSpeed(0);
    controlMode = TEST2;
    start_millis = millis();
    stepper.setAcceleration(1000*driver.getMicrostepsPerStep());
    stepper.setMaxSpeed(500*driver.getMicrostepsPerStep());

    if(target == 0)
    {
      target = 500*driver.getMicrostepsPerStep();
    }else{
      target = 0;
    }
    stepper.moveTo(target);
  }
  if (controlMode == TEST2 && UserButtonPressed() && (millis() - start_millis > 500))
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
    //stepper.setSpeed(0);
    break;
  case DETECTSTEPS:
  {
    static uint8_t step = 0;
    static uint32_t start_time = millis();
    static float speed = 0;
    static float speed_incr = 100*driver.getMicrostepsPerStep();
    static float pos_target = 0;
    switch (step)
    {
    case 0:
      stepper.setMaxSpeed(speed+=speed_incr);
      stepper.setAcceleration(2000*driver.getMicrostepsPerStep());
      if(pos_target==0){
        pos_target = (1400*driver.getMicrostepsPerStep());
      }else{
        pos_target = 0;
      }
      stepper.moveTo(pos_target);

      start_time = millis();
      step++;
      break;
    case 1:
      stepper.run();
      if (stepper.distanceToGo() == 0)
      {
        float error = encoder_ptr->GetPositionDegrees()*(200.0/360.0)*driver.getMicrostepsPerStep() - (float)stepper.currentPosition();
        Serial.printf("%f, %f, %f ,%lu, %f\n", speed, encoder_ptr->GetPositionDegrees(), encoder_ptr->GetPositionDegrees()*(200.0/360.0)*driver.getMicrostepsPerStep(), stepper.currentPosition(), error);
        
        if(abs(error)<10){
          step=0;
          break;
        }
        
        step = 2;
      }
      break;
    case 2:
        step = 0;
        controlMode = MOTOR_OFF;
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
    stepper.run();
    if(stepper.distanceToGo() == 0){
      controlMode = MOTOR_OFF;
    }
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