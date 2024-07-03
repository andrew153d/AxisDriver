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
        FastLED.show();
    }

void LedController::HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size = 0)
{
    if(recv_bytes_size < HEADER_SIZE+FOOTER_SIZE){
        Serial.println("Size too small");
    }

    Header* header = (Header*)recv_bytes;
    
    if(recv_bytes_size < HEADER_SIZE+header->body_size+FOOTER_SIZE){
        Serial.printf("Invalid body size: recv_bytes:%d, header_size:%d, body_size:%d, footer_size:%d\n", recv_bytes_size, HEADER_SIZE, header->body_size, FOOTER_SIZE);
        return;
    }
    
    Serial.printf("MessageType: 0x%X\n", header->message_type);

    switch(header->message_type){
        case MessageTypes::SetLed:
            if(header->body_size == 3){
                leds[0] = (CRGB)*(&recv_bytes[HEADER_SIZE]);

                FastLED.show();
            }else{
                Serial.println("SetLed body sizze incorrect");
            }
        break;
        case MessageTypes::GetLed:

        break;
        default:
        break;
    }


    Serial.println("Led Controller got message");
}

void LedController::SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size){
    Serial.println(" Trying to send message ");
}
