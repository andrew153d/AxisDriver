#pragma once

#include <cstdint>
#include <Arduino.h>
#include "Task.h"
#include "FastLED.h"
#include "MessageProcessor/MessageProcessor.h"
#include "Messages.h"

// #if !defined(FASTLED_FORCE_SOFTWARE_PINS) 
// #error "FASTLED_FORCE_SOFTWARE_PINS needs to be defined in fastled_config.h". 
// #endif 

#define FLASH_ERROR_EXEC_PERIOD 500
#define ERROR_EXEC_PERIOD 500
#define BOOTUP_EXEC_PERIOD 250
#define RAINBOW_EXEC_PERIOD 50

#define GREEN_HUE 85 
#define RED_HUE 0
#define BLUE_HUE 170

enum LedStates{
  OFF,
  FLASH_ERROR,
  ERROR,
  BOOTUP,
  RAINBOW,
  SOLID,
};

class AddrLedController : public ITask, public IInternalInterface {
private:
  uint16_t colorWheelAngle = 0;
  CRGB leds[1];
  uint8_t hue = 0;
  LedStates ledState;
  LedStates prevState;
  uint16_t step_counter; //general counter used to step through colors in various led states

  typedef struct{
    uint8_t rainbow_brightness = 255;
  }config;
  config ledConfig;
public:
    AddrLedController(uint32_t period){
      executionPeriod = period;
      ledState = OFF;
    }

    void OnStart();
    void OnStop();
    void OnRun();

    void SetRainbowBrightness(uint8_t brightness){
      ledConfig.rainbow_brightness = brightness;
    }

    void HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size);
    
    void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size);

    void SetLedState(LedStates state);

    void SetLEDColor(CHSV color);

};

extern AddrLedController addrLedController;