#include "LedController.h"

void AddrLedController::OnStart()
{
    FastLED.addLeds<NEOPIXEL, NEOPIX>(leds, 1);
    prevState = LedStates::OFF;
}

void AddrLedController::OnStop()
{
}

void AddrLedController::OnRun()
{
    switch (ledState)
    {
    case OFF:
        break;
    case FLASH_ERROR:
        switch (step_counter)
        {
        case 0:
            leds[0] = CRGB(0x100000);
            FastLED.show();
            break;
        case 1:
            leds[0] = 0;
            FastLED.show();
            break;
        case 2:
            leds[0] = CRGB(0x100000);
            FastLED.show();
            break;
        case 3:
            leds[0] = 0;
            FastLED.show();
            break;
        case 4:
            //SetLedState(prevState);
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
        step_counter %= 2;
        break;
    case BOOTUP:
        switch (step_counter)
        {
        case 0:
            leds[0] = CRGB(0x110000);
            FastLED.show();
            break;
        case 1:
            leds[0] = 0;
            FastLED.show();
            break;
        case 2:
            leds[0] = CRGB(0x001100);
            FastLED.show();
            break;
        case 3:
            leds[0] = 0;
            FastLED.show();
            break;
        case 4:
            leds[0] = CRGB(0x000011);
            FastLED.show();
            break;
        case 5:
            leds[0] = 0;
            FastLED.show();
            break;
        case 6:
            //SetLedState(prevState);
            break;
        }
        step_counter++;
        break;
    case RAINBOW:
        hue = (hue + 1);
        leds[0] = CHSV(hue, 255, ledConfig.rainbow_brightness);
        FastLED.show();
        break;
    }
}

void AddrLedController::SetLedState(LedStates state)
{

    if(state == ledState)
        return;

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
        leds[0] = CHSV(hue, 255, 50);
        FastLED.show();
        executionPeriod = RAINBOW_EXEC_PERIOD;
        break;
    case SOLID:
        executionPeriod = 0xFFFFFFFF;
    }
}

void AddrLedController::SetLEDColor(CHSV color)
{
    leds[0] = color;
    FastLED.show();
}

void AddrLedController::SetLEDColor(CRGB color)
{
    leds[0] = color;
    FastLED.show();
}

CRGB AddrLedController::GetLedColor()
{
    return leds[0];
}

AddrLedController addrLedController(500);
