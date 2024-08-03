#pragma once

#include <cstdint>
#include <Arduino.h>
#include "Task.h"
#include "FastLED.h"
#include "ExternalInterface.h"
#include "Messages.h"

#if !defined(FASTLED_FORCE_SOFTWARE_PINS) 
#error "FASTLED_FORCE_SOFTWARE_PINS needs to be defined in fastled_config.h". 
#endif 

class LedController : public ITask, public ISenderInterface {
private:
  uint16_t colorWheelAngle = 0;
  CRGB leds[1];
  uint16_t hue = 0;
public:
    LedController(uint32_t period){
      executionPeriod = period;
    }

    void OnStart();

    void OnStop();
    
    void OnRun();

    void HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size);
    
    void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size);

};