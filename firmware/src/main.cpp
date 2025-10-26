#include <Arduino.h>
#include "wiring_private.h"
#include "Task/Task.h"
#include "MessageProcessor/MessageProcessor.hpp"
#include "LedController/LedController.h"
#include "SerialTextInterface/SerialTextInterface.h"
#include "MotorController/MotorController.h"
#include "EncoderController/EncoderController.h"
#include "DeviceManager/DeviceManager.h"
#include "FlashStorage/FlashStorage.h"
#include "EthernetHAT/AxisEthernet.h"
#include "EthernetHAT/AxisMqtt.h"
#include "AxisMessages.h"
#include <cstdint>
#include <cstdio>
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
  Serial.begin(115200);

  Wire1.begin();
  Wire1.setClock(400000);

  pinMode(USR_INPUT, INPUT);
  addrLedController.Start();
  addrLedController.SetLEDColor(CRGB(0x001100));
  uint32_t start_time = millis();
  while(digitalRead(USR_INPUT) && (millis()-start_time)<1500){delay(10);}
  if (!digitalRead(USR_INPUT))
  {
    while (!Serial)
    {
      addrLedController.Start();
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

  messageProcessor.AddExternalInterface(&serialTextInterface);
  messageProcessor.AddExternalInterface(&AEthernet);
  messageProcessor.AddExternalInterface(&mqttTask);

  manager.AddTask(&serialTextInterface);
  manager.AddTask(&messageProcessor);
  manager.AddTask(&statusLight);
  manager.AddTask(&addrLedController);
  manager.AddTask(&motorController);
  manager.AddTask(&encoderController);
  //  start the interfaces
  serialTextInterface.Start();

  // start the message processor
  messageProcessor.Start();
  addrLedController.Start();
  addrLedController.SetRainbowBrightness(100);

  statusLight.Start();

  encoderController.Start();
  motorController.Start();
  motorController.setEncoderValueSource(&encoderController);

  EvaluateHatType();
}

void EvaluateHatType()
{
  // check for the the ethernet hat
  AEthernet.Start();

  if (AEthernet.IsPresent())
  {
    addrLedController.AddLedStep(CRGB::Purple, 100);
    addrLedController.AddLedStep(CRGB::Black, 100);
    manager.AddTask(&AEthernet);
    mqttTask.Start();
    manager.AddTask(&mqttTask);
  }
  else
  {
    addrLedController.AddLedStep(CRGB::Red, 100);
    addrLedController.AddLedStep(CRGB::Black, 100);
  }
}
uint32_t loop_count = 0;
uint32_t loop_count_timer = 15000;
void loop()
{
  loop_count++;
  if(millis()-loop_count_timer>1000)
  {
    DEBUG_PRINTF("Loops per second: %d\n", loop_count);
    loop_count_timer=millis();
    loop_count=0;
  }
  manager.RunTasks();
  FlashStorage::Task();
}