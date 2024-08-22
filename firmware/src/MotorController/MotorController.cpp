#include "MotorController.h"

void MotorController::OnStart()
{

  pinMode(MOTOR_EN, OUTPUT);
  pinMode(MOTOR_STEP, OUTPUT);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_M0, OUTPUT);
  pinMode(MOTOR_M1, OUTPUT);

  digitalWrite(MOTOR_M0, LOW);
  digitalWrite(MOTOR_M1, LOW);
  digitalWrite(MOTOR_EN, LOW);

  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);
  delay(100);
  Serial.println(driver.getVersion());

  stepper.setMaxSpeed(1000000);
  stepper.setAcceleration(500000);
  stepper.setEnablePin(MOTOR_EN);
  stepper.setPinsInverted(false, false, true);
  stepper.disableOutputs();
}

void MotorController::OnStop()
{
}

void MotorController::OnRun()
{
  target = ((int)(millis()/2000))*110;
  float error = target-*encoder_ptr;
  float speed = pid(error);
  stepper.setSpeed(speed);
  if(abs(error)<0.5){
    stepper.disableOutputs();
  }else{
    //stepper.enableOutputs();
  }
  stepper.runSpeed();
  //Serial.printf("Target: %8.1f, Position: %8.1f, Error: %8.1f, Speed: %8.1f\n", target, *encoder_ptr, error, speed);
  //Serial.printf("%8.1f %8.1f %8.1f %8.1f\n", target, *encoder_ptr, error, speed);
}

void MotorController::setEncoderValueSource(float *encoder_value)
{
  encoder_ptr = encoder_value;
}