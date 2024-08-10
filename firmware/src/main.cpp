#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function
#include <AccelStepper.h>
#include <TMCStepper.h>
#include "Task.h"
#include "LedController/LedController.h"
#include "SerialTextInterface/SerialTextInterface.h"
#include "MotorController/MotorController.h"
#include "EncoderController/EncoderController.h"
#include "ExternalInterface.h"
#include "Messages.h"
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
struct InterfaceLimits
{
  ISenderInterface *interface;
  uint16_t lowLimit;
  uint16_t highLimit;
};

class MessageProcessor : public ITask, public ISenderInterface
{
private:
  std::vector<ISenderInterface *> externalInterfaces; // Vector to hold pointers to interfaces
  std::vector<InterfaceLimits> controllerInterfaces;  // Vector to hold pointers to interfaces
public:
  MessageProcessor(uint32_t period)
  {
    executionPeriod = period;
  }

  void AddExternalInterface(ISenderInterface *new_interface)
  {
    externalInterfaces.push_back(new_interface);
    new_interface->SetProcessorInterface(this);
  }

  void AddControllerInterface(ISenderInterface *new_interface, MessageTypes low, MessageTypes high)
  {
    InterfaceLimits interface_to_add;
    interface_to_add.highLimit = (uint16_t)high;
    interface_to_add.lowLimit = (uint16_t)low;
    interface_to_add.interface = new_interface;
    controllerInterfaces.push_back(interface_to_add);
    new_interface->SetProcessorInterface(this);
  }

  void OnStart() override
  {
  }

  void OnStop() override
  {
  }

  void OnRun() override
  {
  }

  void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size = 0)
  {
    Serial.println("message manager handling message: ");
    for (int i = 0; i < recv_bytes_size; i++)
    {
      Serial.printf("%2X, ", recv_bytes[i]);
    }
    Serial.println();

    Header *hdr = (Header *)&recv_bytes[0];

    for (InterfaceLimits interface : controllerInterfaces)
    {
      if ((uint16_t)hdr->message_type > interface.lowLimit && (uint16_t)hdr->message_type < interface.highLimit)
      {
        Serial.println("Sending to led");
        interface.interface->HandleIncomingMsg(recv_bytes, recv_bytes_size);
      }
      else
      {
        Serial.println("Invalid message limits");
      }
    }
  }

  void SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
  {
  }
};

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
SerialTextInterface serialTextInterface(0);
MessageProcessor messageProcessor(0);
AddrLedController addrLedController(500);
StatusLedDriver statusLight(1000);
EncoderController encoderController(100);
// MotorController motorController(0);

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Axis");
  // while(!Serial);
  //  connect the SerialTextInterface to the Message Processor
  //  messageProcessor.AddControllerInterface(&addrLedController, MessageTypes::LedControlMessageTypeLowerBounds, MessageTypes::LedControlMessageTypeUpperBounds);
  //  messageProcessor.AddExternalInterface(&serialTextInterface);
  manager.AddTask(&serialTextInterface);
  manager.AddTask(&messageProcessor);
  manager.AddTask(&statusLight);
  manager.AddTask(&addrLedController);
  manager.AddTask(&encoderController);
  // manager.AddTask(&motorController);
  //  start the interfaces
  serialTextInterface.Start();

  // start the message processor
  messageProcessor.Start();

  addrLedController.Start();
  addrLedController.SetLedState(BOOTUP);
  addrLedController.SetRainbowBrightness(100);

  statusLight.Start();

  encoderController.Start();
  // motorController.Start();
  // Serial1.begin(9600);
}
float angle;
void loop()
{
  manager.RunTasks();

  if (angle != encoderController.GetShaftAngle())
  {
    angle = encoderController.GetShaftAngle();
    Serial.printf("%f %f %f\n",angle,encoderController.GetRunningAverageShaftAngle(), encoderController.GetSlidingWindowShaftAngle());
  }
}