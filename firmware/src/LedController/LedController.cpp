#include "LedController.h"

void LedController::OnStart() {
      leds[0] = 0;
      FastLED.addLeds<NEOPIXEL, NEOPIXEL_BUILTIN>(leds, NEOPIXEL_BUILTIN);
}

void LedController::OnStop(){

}

void LedController::OnRun(){
        return;
        const uint16_t b = 10;
        const uint16_t brightness = b/2;
        colorWheelAngle += 1;
        colorWheelAngle%=360;
        
        leds[0].r = brightness*cos(radians(Wrap360(colorWheelAngle+0)))+brightness;
        leds[0].g = brightness*cos(radians(Wrap360(colorWheelAngle+120)))+brightness;
        leds[0].b = brightness*cos(radians(Wrap360(colorWheelAngle+240)))+brightness;
        // switch(flashState){
        //   case 0:
        //     leds[0] = 0x010002;
        //     flashState++;
        //   break;
        //   case 1:
        //     leds[0] = 0x010000;
        //     flashState=0;
        //   break;
        // }
        FastLED.show();
    }

void LedController::HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size = 0)
{
    Serial.println("Led Controller got message");
}

void LedController::SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size){
    Serial.println(" Trying to send message ");
}
