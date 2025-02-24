#include <Arduino.h>
#include "wiring_private.h"

void setup()
{
  // enable input, to support reading back values, with pullups disabled
  PORT->Group[g_APinDescription[STAT_LED].ulPort].PINCFG[g_APinDescription[STAT_LED].ulPin].reg = (uint8_t)(PORT_PINCFG_INEN | PORT_PINCFG_DRVSTR);

  // Set pin to output mode
  PORT->Group[g_APinDescription[STAT_LED].ulPort].DIRSET.reg = 1 << g_APinDescription[STAT_LED].ulPin;
}

void myanalogWrite(uint32_t pin, uint32_t value)
{
  PinDescription pinDesc = g_APinDescription[pin];
  uint32_t attr = pinDesc.ulPinAttribute;

  

    uint32_t tcNum = GetTCNumber(pinDesc.ulPWMChannel);
    uint8_t tcChannel = GetTCChannelNumber(pinDesc.ulPWMChannel);
    static bool tcEnabled[TCC_INST_NUM + TC_INST_NUM];

    if (attr & PIN_ATTR_PWM_E)
      pinPeripheral(pin, PIO_TIMER);
    else if (attr & PIN_ATTR_PWM_F)
      pinPeripheral(pin, PIO_TIMER_ALT);
    else if (attr & PIN_ATTR_PWM_G)
      pinPeripheral(pin, PIO_TCC_PDEC);

    if (!tcEnabled[tcNum])
    {
      tcEnabled[tcNum] = true;
      GCLK->PCHCTRL[GCLK_CLKCTRL_IDs[tcNum]].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos); // use clock generator 0

      // Set PORT
      if (tcNum >= TCC_INST_NUM)
      {
        // -- Configure TC
        Tc *TCx = (Tc *)GetTC(pinDesc.ulPWMChannel);

        // reset
        TCx->COUNT8.CTRLA.bit.SWRST = 1;
        while (TCx->COUNT8.SYNCBUSY.bit.SWRST)
          ;

        // Disable TCx
        TCx->COUNT8.CTRLA.bit.ENABLE = 0;
        while (TCx->COUNT8.SYNCBUSY.bit.ENABLE)
          ;
        // Set Timer counter Mode to 8 bits, normal PWM, prescaler 1/256
        TCx->COUNT8.CTRLA.reg = TC_CTRLA_MODE_COUNT8 | TC_CTRLA_PRESCALER_DIV256;
        TCx->COUNT8.WAVE.reg = TC_WAVE_WAVEGEN_NPWM;

        while (TCx->COUNT8.SYNCBUSY.bit.CC0)
          ;
        // Set the initial value
        TCx->COUNT8.CC[tcChannel].reg = (uint8_t)value;
        while (TCx->COUNT8.SYNCBUSY.bit.CC0)
          ;
        // Set PER to maximum counter value (resolution : 0xFF)
        TCx->COUNT8.PER.reg = 0xFF;
        while (TCx->COUNT8.SYNCBUSY.bit.PER)
          ;
        // Enable TCx
        TCx->COUNT8.CTRLA.bit.ENABLE = 1;
        while (TCx->COUNT8.SYNCBUSY.bit.ENABLE)
          ;
      }
      else
      {
        // -- Configure TCC
        Tcc *TCCx = (Tcc *)GetTC(pinDesc.ulPWMChannel);

        TCCx->CTRLA.bit.SWRST = 1;
        while (TCCx->SYNCBUSY.bit.SWRST)
          ;

        // Disable TCCx
        TCCx->CTRLA.bit.ENABLE = 0;
        while (TCCx->SYNCBUSY.bit.ENABLE)
          ;
        // Set prescaler to 1/256
        TCCx->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV256 | TCC_CTRLA_PRESCSYNC_GCLK;

        // Set TCx as normal PWM
        TCCx->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
        while (TCCx->SYNCBUSY.bit.WAVE)
          ;

        while (TCCx->SYNCBUSY.bit.CC0 || TCCx->SYNCBUSY.bit.CC1)
          ;
        // Set the initial value
        TCCx->CC[tcChannel].reg = (uint32_t)value;
        while (TCCx->SYNCBUSY.bit.CC0 || TCCx->SYNCBUSY.bit.CC1)
          ;
        // Set PER to maximum counter value (resolution : 0xFF)
        TCCx->PER.reg = 0xFF;
        while (TCCx->SYNCBUSY.bit.PER)
          ;
        // Enable TCCx
        TCCx->CTRLA.bit.ENABLE = 1;
        while (TCCx->SYNCBUSY.bit.ENABLE)
          ;
      }
    }
    else
    {
      if (tcNum >= TCC_INST_NUM)
      {
        Tc *TCx = (Tc *)GetTC(pinDesc.ulPWMChannel);
        TCx->COUNT8.CC[tcChannel].reg = (uint8_t)value;
        while (TCx->COUNT8.SYNCBUSY.bit.CC0 || TCx->COUNT8.SYNCBUSY.bit.CC1)
          ;
      }
      else
      {
        Tcc *TCCx = (Tcc *)GetTC(pinDesc.ulPWMChannel);
        while (TCCx->SYNCBUSY.bit.CTRLB)
          ;
        while (TCCx->SYNCBUSY.bit.CC0 || TCCx->SYNCBUSY.bit.CC1)
          ;
        TCCx->CCBUF[tcChannel].reg = (uint32_t)value;
        while (TCCx->SYNCBUSY.bit.CC0 || TCCx->SYNCBUSY.bit.CC1)
          ;
        TCCx->CTRLBCLR.bit.LUPD = 1;
        while (TCCx->SYNCBUSY.bit.CTRLB)
          ;
      }
    }
}

void loop()
{
  for (int brightness = 0; brightness <= 255; brightness++)
  {
    myanalogWrite(STAT_LED, brightness); // Increase brightness
    delay(10);                         // Wait for 10 milliseconds
  }
  for (int brightness = 255; brightness >= 0; brightness--)
  {
    myanalogWrite(STAT_LED, brightness); // Decrease brightness
    delay(10);                         // Wait for 10 milliseconds
  }
}
