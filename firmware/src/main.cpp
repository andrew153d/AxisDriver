#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function
#include <AccelStepper.h>
#include <TMCStepper.h>
#include "Task.h"
#include "MessageProcessor/MessageProcessor.h"
#include "LedController/LedController.h"
#include "SerialTextInterface/SerialTextInterface.h"
#include "MotorController/MotorController.h"
#include "EncoderController/EncoderController.h"
#include "DeviceManager/DeviceManager.h"
#include "FlashStorage/FlashStorage.h"
#include "AxisEthernet/AxisEthernet.h"
#include "Messages.hpp"
#include <cstdint>
#include "Wire.h"
#include "SPI.h"

/*
TaskManager
  Handles tasking
  Calls all registered tasks

IControlInterface
  Inhereted by classes that manage an external interface (Serial, Wifi, I2c, Uart)
  Passes bytes down to MessageProcessor
  Receives Bytes from MessageProcessor to send to send out of interface
  Each interface will have a buffer whose pointer gets passed down

MessageProcessor
  Maintains a list of Controller classes
  Maintains a list of pointers to a control interfaces
  Calls various control functions based on what functionality they control
  Allows functionControllers to send data back to interface

*/

/*
External Interfaces
  need a pointer to MessageProcessor to call HandleIncomingMessage
  need to implement a send interface that can be called by message processor

Message processor
  every internal recevier will need a pointer to call SendUp

*/

class StatusLedDriver : public ITask
{
private:
  uint32_t led_timer = 0;
  bool led_state = 0;

public:
  StatusLedDriver(uint32_t period)
  {
    executionPeriod = period;
  }

  void OnStart() override
  {
    pinMode(STAT_LED, OUTPUT);
  }

  void OnStop() override
  {
  }

  void OnRun() override
  {
    digitalWrite(STAT_LED, led_state = !led_state);
  }
};

enum class HATType
{
  NONE,
  ETHERNET,

};

TaskManager manager;
MessageProcessor messageProcessor(0);
// external interfaces
SerialTextInterface serialTextInterface(0);

// internal devices
StatusLedDriver statusLight(1000);
//EncoderController encoderController(500);

DeviceManager deviceManager(0xFFFFFFFF);
void setup()
{
  delay(100);
  Serial.begin(115200);
  Wire1.begin();
  pinMode(USR_INPUT, INPUT);

  Serial.println("Starting Axis");
  Wire1.setClock(400000);

  // while (!Serial)
  // {
  //   delay(10);
  // }

  // //  Wire1.beginTransmission(0x5E);
  // //  byte error = Wire1.endTransmission();
  // //  if (error)
  // //  {
  // //    Serial.println("Failed to find Endcoder");
  // //  }
  // //  else
  // //  {
  // //    Serial.println("Can communicate with Encoder");
  // //  }
  // uint8_t reg = 0;
  // uint8_t addr = 0x50;
  // while (true)
  // {

  //   // byte error, address;
  //   // int nDevices;

  //   // nDevices = 0;
  //   // for (address = 1; address < 127; address++)
  //   // {
  //   //   Wire1.beginTransmission(address);
  //   //   error = Wire1.endTransmission();

  //   //   if (error == 0)
  //   //   {
  //   //     Serial.print("I2C device found at address 0x");
  //   //     Serial.println(address, HEX);
  //   //     nDevices++;
  //   //   }
  //   //   else if (error == 4)
  //   //   {
  //   //     Serial.print("Unknown error at address 0x");
  //   //     Serial.println(address, HEX);
  //   //   }
  //   //   delay(2);
  //   // }

  //   delay(10);
  //   Wire1.beginTransmission(addr);
  //   Wire1.write(0xFA); // register to read
  //   Wire1.endTransmission(false);
  //   Wire1.requestFrom(addr, (byte)1); // read a byte
  //   uint8_t buf = 0;
  //   Wire1.readBytes(&buf, 1);
  //   Serial.printf("%x, %x", addr, buf);
  //   Serial.println();
  // addr++;
  //   //   // uint8_t mac[6] = {0};
    //   // Wire1.beginTransmission(FLASH_ADDRESS);
    //   // Wire1.write(0xFA); // Register to read
    //   // Wire1.endTransmission();

    //   // Wire1.requestFrom(FLASH_ADDRESS, 6); // Read a byte
    //   // Wire1.readBytes(&mac[0], 6);
    //   // Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    //   uint8_t boots = FlashStorage::readRegister(0x10);
    //   Serial.println(boots);
    //   FlashStorage::writeRegister(0x10, boots+1);

    //   // for(int addr = 0; addr <= 0xFF; addr++)
    //   // {
    //   //   Serial.printf("0x%X ", FlashStorage::readRegister(addr));
    //   //   if(addr%16==0)
    //   //     Serial.println();
    //   //   FlashStorage::writeRegister(addr, 0);
    //   // }

    //   // Serial.println(FlashStorage::GetMacAddressString());

  //   while (!digitalRead(USR_INPUT))
  //   {
  //     delay(10);
  //   }
  //   while (digitalRead(USR_INPUT))
  //   {
  //     delay(10);
  //   }
  // }

  // while (true)
  // {
  //   delay(1000);
  // }

  while(!Serial);

  // Serial.println(FlashStorage::GetMacAddressString());
  //   connect the SerialTextInterface to the Message Processor
  messageProcessor.AddControllerInterface(&addrLedController, JsonMessageTypes::Led, MessageTypes::LedControlMessageTypeLowerBounds, MessageTypes::LedControlMessageTypeUpperBounds);
  // messageProcessor.AddControllerInterface(&deviceManager, "", MessageTypes::DeviceInfoMessageTypeLowerBounds, MessageTypes::DeviceInfoMessageTypeUpperBounds);
  messageProcessor.AddControllerInterface(&motorController, JsonMessageTypes::Motor, MessageTypes::MotorControlMessageTypeLowerBounds, MessageTypes::MotorControlMessageTypeUpperBounds);
  messageProcessor.AddControllerInterface(&encoderController, JsonMessageTypes::Encoder, MessageTypes::EncoderControlMessageTypeLowerBounds, MessageTypes::EncoderControlMessageTypeUpperBounds);
  messageProcessor.AddExternalInterface(&serialTextInterface);

  manager.AddTask(&serialTextInterface);
  manager.AddTask(&messageProcessor);
  manager.AddTask(&statusLight);
  manager.AddTask(&addrLedController);
  //manager.AddTask(&encoderController);
  manager.AddTask(&motorController);
  manager.AddTask(&AEthernet);
  //  start the interfaces
  serialTextInterface.Start();

  // start the message processor
  messageProcessor.Start();
  addrLedController.Start();
  addrLedController.SetRainbowBrightness(100);
  addrLedController.SetLedState(SOLID);
  addrLedController.SetLedState(BOOTUP);

  statusLight.Start();

  encoderController.Start();
  motorController.Start();
  motorController.setEncoderValueSource(&encoderController);

  AEthernet.Start();

  Serial.println(FlashStorage::GetMacAddressString());
  Serial.println(FlashStorage::GetSerialNumberString());
}

void loop()
{
  manager.RunTasks();
}