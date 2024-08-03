#include "LedController.h"

void LedController::OnStart() {
      leds[0] = CHSV(1, 100, 100);
      FastLED.addLeds<NEOPIXEL, 40>(leds, 1);
      FastLED.show();
}

void LedController::OnStop(){

}

void LedController::OnRun(){
    
    hue = (hue + 1)%255;
    leds[0] = CHSV(hue, 255, 100);
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
