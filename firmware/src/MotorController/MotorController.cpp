#include "MotorController.h"

void MotorController::OnStart(){

  pinMode(MOTOR_EN, OUTPUT);
  pinMode(MOTOR_STEP, OUTPUT);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_M0, OUTPUT);
  pinMode(MOTOR_M1, OUTPUT);

  digitalWrite(MOTOR_M0, HIGH);
  digitalWrite(MOTOR_M1, HIGH);
  digitalWrite(MOTOR_EN, LOW);
  
//   while(!Serial2.availableForWrite()){
//     delay(1000);
//     Serial.println("Waiting on Serial2");
//   }

  Serial1.begin(9600);
  delay(1000);
  driver.begin();

  driver.toff(5);
  delay(100);
  driver.rms_current(1200);
  delay(100);
  driver.microsteps(16);
  delay(100);
  driver.en_spreadCycle(false);
  delay(100);
  driver.pwm_autoscale(true);
  delay(100);
  Serial.printf("Got version: 0x%X\n", driver.IOIN());
  Serial.printf("test: 0x%X", driver.freewheel());

  stepper.setMaxSpeed(1000000); 
  stepper.setAcceleration(500000); 
  stepper.setEnablePin(MOTOR_EN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();

  stepper.setSpeed(-10000);
}

void MotorController::OnStop(){

}

void MotorController::OnRun(){
stepper.runSpeed();
}