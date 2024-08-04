#include "LedController.h"

void LedController::OnStart()
{
    FastLED.addLeds<NEOPIXEL, NEOPIX>(leds, 1);
}

void LedController::OnStop()
{
}

void LedController::OnRun()
{
    switch (ledState)
    {
    case OFF:
        break;
    case FLASH_ERROR:
        switch (step_counter)
        {
        case 0:
            leds[0] = CRGB::Red;
            FastLED.show();
            break;
        case 1:
            leds[0] = 0;
            FastLED.show();
            break;
        case 2:
            leds[0] = CRGB::Red;
            FastLED.show();
            break;
        case 3:
            leds[0] = 0;
            FastLED.show();
            break;
        case 4:
            SetLedState(prevState);
            break;
        }
        step_counter++;
        break;
    case ERROR:
        switch (step_counter)
        {
        case 0:
            leds[0] = CRGB::Red;
            FastLED.show();
            break;
        case 1:
            leds[0] = 0;
            FastLED.show();
            break;
        }
        step_counter++;
        step_counter%=2;
        break;
    case BOOTUP:
        switch (step_counter)
        {
        case 0:
            leds[0] = CRGB::Blue;
            FastLED.show();
            break;
        case 1:
            leds[0] = 0;
            FastLED.show();
            break;
        case 2:
            leds[0] = CRGB::Red;
            FastLED.show();
            break;
        case 3:
            leds[0] = 0;
            FastLED.show();
            break;
        case 4:
            leds[0] = CRGB::Green;
            FastLED.show();
            break;
        case 5:
            leds[0] = 0;
            FastLED.show();
            break;
        case 6:
            SetLedState(RAINBOW);
            break;
        }
        step_counter++;
        break;
    case RAINBOW:
        hue = (hue + 1);
        leds[0] = CHSV(hue, 255, 100);
        FastLED.show();
        break;
    }
}

void LedController::SetLedState(LedStates state)
{
    prevState = ledState;
    ledState = state;
    switch (ledState)
    {
    case OFF:
        leds[0] = CRGB::Black;
        FastLED.show();
        executionPeriod = 0xFFFFFFFF;
        break;
    case FLASH_ERROR:
        step_counter = 0;
        executionPeriod = FLASH_ERROR_EXEC_PERIOD;
        break;
    case ERROR:
        step_counter = 0;
        executionPeriod = ERROR_EXEC_PERIOD;
        break;
    case BOOTUP:
        step_counter = 0;
        executionPeriod = BOOTUP_EXEC_PERIOD;
        break;
    case RAINBOW:
        hue = (hue + 1) % 255;
        leds[0] = CHSV(hue, 255, 100);
        FastLED.show();
        executionPeriod = RAINBOW_EXEC_PERIOD;
        break;
    }
}

void LedController::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size = 0)
{
    if (recv_bytes_size < HEADER_SIZE + FOOTER_SIZE)
    {
        Serial.println("Size too small");
    }

    Header *header = (Header *)recv_bytes;

    if (recv_bytes_size < HEADER_SIZE + header->body_size + FOOTER_SIZE)
    {
        Serial.printf("Invalid body size: recv_bytes:%d, header_size:%d, body_size:%d, footer_size:%d\n", recv_bytes_size, HEADER_SIZE, header->body_size, FOOTER_SIZE);
        return;
    }

    Serial.printf("MessageType: 0x%X\n", header->message_type);

    switch (header->message_type)
    {
    case MessageTypes::SetLed:
        if (header->body_size == 3)
        {
            leds[0] = (CRGB) * (&recv_bytes[HEADER_SIZE]);

            FastLED.show();
        }
        else
        {
            Serial.println("SetLed body size incorrect");
        }
        break;
    case MessageTypes::GetLed:

        break;
    default:
        break;
    }

    Serial.println("Led Controller got message");
}

void LedController::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
{
    Serial.println(" Trying to send message ");
}
