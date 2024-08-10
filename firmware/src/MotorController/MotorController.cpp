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
  Serial.println("trying to send data");
  
//   while(!Serial2.availableForWrite()){
//     delay(1000);
//     Serial.println("Waiting on Serial2");
//   }

  Serial1.begin(9600);
  delay(1000);
  driver.begin();                 //  SPI: Init CS pins and possible SW SPI pins
                                 // UART: Init SW UART (if selected) with default 115200 baudrate

  driver.toff(5);                 // Enables driver in software
  delay(100);
  driver.rms_current(1200);        // Set motor RMS current
  delay(100);
  driver.microsteps(16);          // Set microsteps to 1/16th
  delay(100);
  driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208/2209/2224
  delay(100);
  driver.pwm_autoscale(true);     // Needed for stealthChop
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