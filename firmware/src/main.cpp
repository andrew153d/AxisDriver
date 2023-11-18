#include <Arduino.h>
#include <TMC2209.h>
#include "wiring_private.h" // pinPeripheral() function

#define STEP 6
#define DIR 5

Uart Serial2(&sercom3, SCL, SDA, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM3_Handler()
{
  Serial2.IrqHandler();
}
HardwareSerial &serial_stream = Serial2;

const long SERIAL_BAUD_RATE = 115200;
const int DELAY = 100;

// Instantiate TMC2209
TMC2209 stepper_driver;

void setup()
{
  pinPeripheral(SCL, PIO_SERCOM);
  pinPeripheral(SDA, PIO_SERCOM);
  Serial.begin(SERIAL_BAUD_RATE);
  //while (!Serial)
  //  ;
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(13, OUTPUT);
  stepper_driver.setup(serial_stream, 9600);

  while (!stepper_driver.isCommunicating())
  {
    Serial.println("waiting for comms");
    delay(10);
  }
  stepper_driver.setRunCurrent(100);
  stepper_driver.enableCoolStep();
  stepper_driver.enable();
  
}

void loop()
{

  Serial.println("running");
  digitalWrite(13, HIGH);
  digitalWrite(DIR, HIGH);
  for (int i = 0; i < 1000; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(1);
    digitalWrite(STEP, LOW);
    delay(1);
  }
  delay(1000);
  digitalWrite(13, LOW);
  digitalWrite(DIR, LOW);
  for (int i = 0; i < 1000; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(1);
    digitalWrite(STEP, LOW);
    delay(1);
  }
  delay(1000);
}
