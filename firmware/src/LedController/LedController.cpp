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
    if(ledSteps.size() > 0)
    {
        if(millis() - led_step_timer > ledSteps.front().duration)
        {
            leds[0] = ledSteps.front().color;
            FastLED.show();
            ledSteps.pop();
            led_step_timer = millis();
        }
        return;
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

void AddrLedController::AddLedStep(CRGB color, uint32_t duration){
      ledSteps.push({color, duration});
}

AddrLedController addrLedController(500);
