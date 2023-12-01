#include <TMC2209.h>
#include "wiring_private.h" // pinPeripheral() function

// This example will not work on Arduino boards without HardwareSerial ports,
// such as the Uno, Nano, and Mini.
//
// See this reference for more details:
// https://www.arduino.cc/reference/en/language/functions/communication/serial/
Uart Serial2 (&sercom3, SCL, SDA, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM3_Handler()
{
  Serial2.IrqHandler();
}
HardwareSerial & serial_stream = Serial2;

const long SERIAL_BAUD_RATE = 115200;
const int DELAY = 100;

// Instantiate TMC2209
TMC2209 stepper_driver;


void setup()
{
  pinMode(SCL, OUTPUT);
  pinPeripheral(SCL, PIO_SERCOM);
  pinPeripheral(SDA, PIO_SERCOM);
  Serial.begin(SERIAL_BAUD_RATE);

  stepper_driver.setup(serial_stream, 9600);
}


void loop()
{
  if (stepper_driver.isSetupAndCommunicating())
  {
    Serial.println("good");
  }
  else if (stepper_driver.isCommunicatingButNotSetup())
  {
    Serial.println("Stepper driver is communicating but not setup!");
    Serial.println("Running setup again...");
    stepper_driver.setup(serial_stream, 9600);
  }
  else
  {
    Serial.println(".5");
  }
  Serial.println();
  delay(DELAY);
}
