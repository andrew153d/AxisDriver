#include <Arduino.h>
#include "wiring_private.h"

void setup()
{
  // enable input, to support reading back values, with pullups disabled
  PORT->Group[g_APinDescription[STAT_LED].ulPort].PINCFG[g_APinDescription[STAT_LED].ulPin].reg = (uint8_t)(PORT_PINCFG_INEN | PORT_PINCFG_DRVSTR);

  // Set pin to output mode
  PORT->Group[g_APinDescription[STAT_LED].ulPort].DIRSET.reg = 1 << g_APinDescription[STAT_LED].ulPin;

  uint32_t pin = STAT_LED;
  uint32_t value = 0;
  PinDescription pinDesc = g_APinDescription[pin];
  uint32_t attr = pinDesc.ulPinAttribute;

  uint32_t tcNum = GetTCNumber(pinDesc.ulPWMChannel);
  uint8_t tcChannel = GetTCChannelNumber(pinDesc.ulPWMChannel);

  pinPeripheral(pin, PIO_TCC_PDEC);

  GCLK->PCHCTRL[GCLK_CLKCTRL_IDs[tcNum]].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);

  // -- Configure TCC

  TCC0->CTRLA.bit.SWRST = 1;
  while (TCC0->SYNCBUSY.bit.SWRST)
    ;

  TCC0->CTRLA.bit.ENABLE = 0;
  while (TCC0->SYNCBUSY.bit.ENABLE)
    ;
  // Set prescaler to 1/256
  TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV256 | TCC_CTRLA_PRESCSYNC_GCLK;

  // Set TCx as normal PWM
  TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
  while (TCC0->SYNCBUSY.bit.WAVE)
    ;

  while (TCC0->SYNCBUSY.bit.CC0 || TCC0->SYNCBUSY.bit.CC1)
    ;
  // Set the initial value
  TCC0->CC[tcChannel].reg = (uint32_t)value;
  while (TCC0->SYNCBUSY.bit.CC0 || TCC0->SYNCBUSY.bit.CC1)
    ;
  // Set PER to maximum counter value (resolution : 0xFF)
  TCC0->PER.reg = 0xFF;
  while (TCC0->SYNCBUSY.bit.PER)
    ;
  // Enable TCCx
  TCC0->CTRLA.bit.ENABLE = 1;
  while (TCC0->SYNCBUSY.bit.ENABLE)
    ;
}

void myanalogWrite(uint32_t pin, uint32_t value)
{
  PinDescription pinDesc = g_APinDescription[pin];
  uint32_t attr = pinDesc.ulPinAttribute;

  uint32_t tcNum = GetTCNumber(pinDesc.ulPWMChannel);
  uint8_t tcChannel = GetTCChannelNumber(pinDesc.ulPWMChannel);
 
  
  while (TCC0->SYNCBUSY.bit.CTRLB)
    ;
  while (TCC0->SYNCBUSY.bit.CC0 || TCC0->SYNCBUSY.bit.CC1)
    ;
  TCC0->CCBUF[tcChannel].reg = (uint32_t)value;
  while (TCC0->SYNCBUSY.bit.CC0 || TCC0->SYNCBUSY.bit.CC1)
    ;
  TCC0->CTRLBCLR.bit.LUPD = 1;
  while (TCC0->SYNCBUSY.bit.CTRLB)
    ;
}

void loop()
{
  for (int brightness = 0; brightness <= 255; brightness++)
  {
    myanalogWrite(STAT_LED, brightness); // Increase brightness
    delay(10);                           // Wait for 10 milliseconds
  }
  for (int brightness = 255; brightness >= 0; brightness--)
  {
    myanalogWrite(STAT_LED, brightness); // Decrease brightness
    delay(10);                           // Wait for 10 milliseconds
  }
}
