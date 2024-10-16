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

Message processoe
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

TaskManager manager;
MessageProcessor messageProcessor(0);
//external interfaces
SerialTextInterface serialTextInterface(0);

//internal devices
//AddrLedController addrLedController(500);
StatusLedDriver statusLight(1000);
EncoderController encoderController(500);
//MotorController motorController(0);

DeviceManager deviceManager(0xFFFFFFFF);
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Axis");
  //  connect the SerialTextInterface to the Message Processor
  messageProcessor.AddControllerInterface(&addrLedController, JsonMessageTypes::Led, MessageTypes::LedControlMessageTypeLowerBounds, MessageTypes::LedControlMessageTypeUpperBounds);
  //messageProcessor.AddControllerInterface(&deviceManager, "", MessageTypes::DeviceInfoMessageTypeLowerBounds, MessageTypes::DeviceInfoMessageTypeUpperBounds);
  messageProcessor.AddControllerInterface(&motorController, JsonMessageTypes::Motor, MessageTypes::MotorControlMessageTypeLowerBounds, MessageTypes::MotorControlMessageTypeUpperBounds);
  messageProcessor.AddControllerInterface(&encoderController, JsonMessageTypes::Encoder, MessageTypes::EncoderControlMessageTypeLowerBounds, MessageTypes::EncoderControlMessageTypeUpperBounds);
  messageProcessor.AddExternalInterface(&serialTextInterface);
  
  manager.AddTask(&serialTextInterface);
  manager.AddTask(&messageProcessor);
  manager.AddTask(&statusLight);
  manager.AddTask(&addrLedController);
  manager.AddTask(&encoderController);
  manager.AddTask(&motorController);
  //  start the interfaces
  serialTextInterface.Start();

  // start the message processor
  messageProcessor.Start();
  pinMode(USR_INPUT, INPUT);
  addrLedController.Start();
  addrLedController.SetRainbowBrightness(100);
  addrLedController.SetLedState(SOLID);
  addrLedController.SetLedState(BOOTUP);

  statusLight.Start();

  encoderController.Start();
  motorController.Start();
  motorController.setEncoderValueSource(&encoderController);
}

void loop()
{
  manager.RunTasks();
}