#include "MotorController.h"

void MotorController::OnStart()
{

  pinMode(MOTOR_EN, OUTPUT);
  pinMode(MOTOR_STEP, OUTPUT);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_M0, OUTPUT);
  pinMode(MOTOR_M1, OUTPUT);
  pinMode(MOTOR_DIAG, INPUT);

  digitalWrite(MOTOR_M0, LOW);
  digitalWrite(MOTOR_M1, LOW);
  digitalWrite(MOTOR_EN, LOW);
  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(50000);
  stepper.setEnablePin(MOTOR_EN);
  stepper.setPinsInverted(true, false, true);
  stepper.enableOutputs();
  
  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);
  driver.setMicrostepsPerStepPowerOfTwo(4);
  driver.setAllCurrentValues(90, 10, 0);

  driver.getSettings();


  }

void MotorController::OnStop()
{
}

void MotorController::OnRun()
{
  //static uint32_t timer = 0;
  // if(millis()-timer>1000){
  //   timer = millis();
  //   //auto state = driver.getStatus();
  //   auto stat = driver.getStatus();
  //   auto gstat = driver.getGlobalStatus();
  //   Serial.printf("reset: %u\n", gstat.reset);
  //   Serial.printf("drv_error: %u\n", gstat.drv_err);
  //   Serial.printf("uv_cp: %u\n", gstat.uv_cp);
  //   Serial.printf("over_temperature_warning: %u\n", stat.over_temperature_warning);
  //   Serial.printf("over_temperature_shutdown: %u\n", stat.over_temperature_shutdown);
  //   Serial.printf("short_to_ground_a: %u\n", stat.short_to_ground_a);
  //   Serial.printf("short_to_ground_b: %u\n", stat.short_to_ground_b);
  //   Serial.printf("low_side_short_a: %u\n", stat.low_side_short_a);
  //   Serial.printf("low_side_short_b: %u\n", stat.low_side_short_b);
  //   Serial.printf("open_load_a: %u\n", stat.open_load_a);
  //   Serial.printf("open_load_b: %u\n", stat.open_load_b);
  //   Serial.printf("over_temperature_120c: %u\n", stat.over_temperature_120c);
  //   Serial.printf("over_temperature_143c: %u\n", stat.over_temperature_143c);
  //   Serial.printf("over_temperature_150c: %u\n", stat.over_temperature_150c);
  //   Serial.printf("over_temperature_157c: %u\n", stat.over_temperature_157c);
  //   Serial.printf("reserved0: %u\n", stat.reserved0);
  //   Serial.printf("current_scaling: %u\n", stat.current_scaling);
  //   Serial.printf("reserved1: %u\n", stat.reserved1);
  //   Serial.printf("stealth_chop_mode: %u\n", stat.stealth_chop_mode);
  //   Serial.printf("standstill: %u\n", stat.standstill);
  // }

  if(millis() > 10000)
    Stop();

  target = (int)((bool)(((int)(millis()/5000))%2)==0)*(360*4.5);
  float error = target-*encoder_ptr;
  float speed = pid(error);
  if(abs(error)<1)
    speed = 0;
  stepper.setSpeed(speed);
  stepper.runSpeed();

// static uint32_t spot_timer = 0;
//   if(millis()-spot_timer>100){
//     spot_timer = millis();
//     Serial.printf("Target: %8.1f, Position: %8.1f, Error: %8.1f, Speed: %8.1f\n", target, *encoder_ptr, error, speed);
// }

  //Serial.printf("Target: %8.1f, Position: %8.1f, Error: %8.1f, Speed: %8.1f\n", target, *encoder_ptr, error, speed);
  //Serial.printf("%8.1f %8.1f %8.1f %8.1f\n", target, *encoder_ptr, error, speed);

  
}

void MotorController::setEncoderValueSource(float *encoder_value)
{
  encoder_ptr = encoder_value;
}

uint32_t MotorController::GetErrors()
{
  return error_flag;
}
