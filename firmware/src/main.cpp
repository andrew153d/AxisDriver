#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function
#include <AccelStepper.h>
#include <TMCStepper.h>
#include "Task/Task.h"
#include "MessageProcessor/MessageProcessor.hpp"
#include "LedController/LedController.h"
#include "SerialTextInterface/SerialTextInterface.h"
#include "MotorController/MotorController.h"
#include "EncoderController/EncoderController.h"
#include "DeviceManager/DeviceManager.h"
#include "FlashStorage/FlashStorage.h"
#include "AxisEthernet/AxisEthernet.h"
#include "Messages.h"
#include <cstdint>
#include "Wire.h"
#include "SPI.h"
#include "DebugPrinter.h"

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

void EvaluateHatType();

TaskManager manager;
MessageProcessor messageProcessor(0);
// external interfaces
SerialTextInterface serialTextInterface(0);

// internal devices
StatusLedDriver statusLight(1000);

DeviceManager deviceManager(0xFFFFFFFF);
void setup()
{
  DEBUG_BEGIN(115200);
  
  Wire1.begin();
  Wire1.setClock(400000);

  pinMode(USR_INPUT, INPUT);
  addrLedController.Start();
  addrLedController.SetLedState(SOLID);
  addrLedController.SetLEDColor(CRGB(0x001100));
  uint32_t start_time = millis();
  while(digitalRead(USR_INPUT) && (millis()-start_time)<1500){delay(10);}
  if (!digitalRead(USR_INPUT))
  {
    while (!Serial)
    {
      addrLedController.Start();
      addrLedController.SetLedState(LedStates::SOLID);
      digitalWrite(STAT_LED, LOW);
      addrLedController.SetLEDColor(CRGB(0x0000F0));
      delay(80);
      digitalWrite(STAT_LED, HIGH);
      addrLedController.SetLEDColor(CRGB(0x000000));
      delay(80);
    }
  }

  FlashStorage::Init();

  addrLedController.SetLEDColor(CRGB(0x000000));

  //   connect the SerialTextInterface to the Message Processor
  //messageProcessor.AddControllerInterface(&addrLedController, JsonMessageTypes::Led, MessageTypes::LedControlMessageTypeLowerBounds, MessageTypes::LedControlMessageTypeUpperBounds);
  // messageProcessor.AddControllerInterface(&deviceManager, "", MessageTypes::DeviceInfoMessageTypeLowerBounds, MessageTypes::DeviceInfoMessageTypeUpperBounds);
  //messageProcessor.AddControllerInterface(&motorController, JsonMessageTypes::Motor, MessageTypes::MotorControlMessageTypeLowerBounds, MessageTypes::MotorControlMessageTypeUpperBounds);
  //messageProcessor.AddControllerInterface(&encoderController, JsonMessageTypes::Encoder, MessageTypes::EncoderControlMessageTypeLowerBounds, MessageTypes::EncoderControlMessageTypeUpperBounds);
  messageProcessor.AddExternalInterface(&serialTextInterface);

  manager.AddTask(&serialTextInterface);
  manager.AddTask(&messageProcessor);
  manager.AddTask(&statusLight);
  manager.AddTask(&addrLedController);
  manager.AddTask(&motorController);
  
  //  start the interfaces
  serialTextInterface.Start();

  // start the message processor
  messageProcessor.Start();
  addrLedController.Start();
  addrLedController.SetRainbowBrightness(100);
  addrLedController.SetLedState(BOOTUP);

  statusLight.Start();

  encoderController.Start();
  motorController.Start();
  motorController.setEncoderValueSource(&encoderController);

  // uint8_t b = 0;
  // FlashStorage::readBytes(0x11, &b, 1);
  // b++;
  // FlashStorage::writeRegister(0x11, b);
  // DEBUG_PRINTLN(b);

  EthernetSettingsStruct* s = FlashStorage::GetEthernetSettings();
  //DEBUG_PRINTF("%x, %x", s->ip_address, s->port);  
  //s->port = 0x2ee1; //12001;
  //s->ip_address = 0x6F0CA8C0;
  //FlashStorage::WriteFlash();
  //DEBUG_PRINTF("ip: %x\n", s->ip_address);
  EvaluateHatType();
}

void EvaluateHatType()
{
  // check for the the ethernet hat
  AxisEthernet *AEthernet = new AxisEthernet(10);
  if (AEthernet != nullptr)
    AEthernet->Start();

  if (AEthernet->IsPresent())
  {
    manager.AddTask(AEthernet);
  }
  else
  {
    //delete AEthernet;
    //AEthernet = nullptr;
  }
}

void loop()
{
  manager.RunTasks();
}