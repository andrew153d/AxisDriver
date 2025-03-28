#pragma once

#include <cstdint>
#include <Arduino.h>
#include "Task/Task.h"
#include "FastLED.h"
#include "MessageProcessor/MessageProcessor.hpp"
#include "AxisMessages.h"
#include "queue"

#define FLASH_ERROR_EXEC_PERIOD 500
#define ERROR_EXEC_PERIOD 500
#define BOOTUP_EXEC_PERIOD 250
#define RAINBOW_EXEC_PERIOD 50

#define GREEN_HUE 85 
#define RED_HUE 0
#define BLUE_HUE 170

struct LedStep
    {
      CRGB color;
      uint32_t duration;
    };

class AddrLedController : public ITask
{
private:
  uint16_t colorWheelAngle = 0;
  CRGB leds[1];
  uint8_t hue = 0;
  LedStates ledState;
  LedStates prevState;
  uint16_t step_counter; //general counter used to step through colors in various led states

  JsonDocument received_json;
  typedef struct{
    uint8_t rainbow_brightness = 255;
  }config;
  config ledConfig;

  std::queue<LedStep> ledSteps;
  uint32_t led_step_timer = 0;
public:

    AddrLedController(uint32_t period){
      executionPeriod = period;
      ledState = LedStates::OFF;
    }

    void OnStart();
    void OnStop();
    void OnRun();

    void SetRainbowBrightness(uint8_t brightness){
      ledConfig.rainbow_brightness = brightness;
    }
  
    void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size);

    void SetLEDColor(CHSV color);
    void SetLEDColor(CRGB color);
    CRGB GetLedColor();

    void AddLedStep(CRGB color, uint32_t duration);
};

extern AddrLedController addrLedController;