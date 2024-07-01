#pragma once

#include <cstdint>
#include <Arduino.h>
#include "Task.h"
#include "FastLED.h"
#include "ExternalInterface.h"

class LedController : public ITask, public ISenderInterface {
private:
  uint16_t colorWheelAngle = 0;
  CRGB leds[1];
public:
    LedController(uint32_t period){
      executionPeriod = period;
    }

    void OnStart();

    void OnStop();

    template <typename T>
    T Wrap360(T input){
      if(input>360)
        return input-360;
      if(input<0){
        return input+360;
      }
      return input;
    }

    void OnRun();

    void HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size);
    
    void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size);

};