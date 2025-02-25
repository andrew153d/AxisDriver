#include <Arduino.h>
#include "wiring_private.h"

#define DMA_BUFFER_SIZE 1024
volatile uint32_t circular_buffer[DMA_BUFFER_SIZE];
volatile DmacDescriptor dmaDescriptor __attribute__ ((aligned (16)));
volatile DmacDescriptor dmaDescriptorWriteback __attribute__ ((aligned (16)));

void configure_dma_descriptor() {
    dmaDescriptor.BTCTRL.bit.VALID = 1;             // Descriptor is valid
    dmaDescriptor.BTCTRL.bit.EVOSEL = 0;            // No event output
    dmaDescriptor.BTCTRL.bit.BLOCKACT = DMAC_BTCTRL_BLOCKACT_INT_Val; // Interrupt after block transfer
    dmaDescriptor.BTCTRL.bit.BEATSIZE = DMAC_BTCTRL_BEATSIZE_WORD_Val; // 32-bit beats
    dmaDescriptor.BTCTRL.bit.SRCINC = 1;            // Source does not increment (fixed)
    dmaDescriptor.BTCTRL.bit.DSTINC = 0;            // Destination increments (circular buffer)
    dmaDescriptor.BTCTRL.bit.STEPSEL = DMAC_BTCTRL_STEPSEL_SRC_Val; // Step size applied to destination
    dmaDescriptor.BTCTRL.bit.STEPSIZE = DMAC_BTCTRL_STEPSIZE_X1_Val; // Step size of 1

    dmaDescriptor.BTCNT.reg = DMA_BUFFER_SIZE;          
    dmaDescriptor.SRCADDR.reg = (uint32_t)circular_buffer + DMA_BUFFER_SIZE; // Source address (circular buffer)
    dmaDescriptor.DSTADDR.reg = (uint32_t)&(PORT->Group[PORTA].OUTSET.reg) + (1 << 2); // Destination address (PA02)
    dmaDescriptor.DESCADDR.reg = (uint32_t)&dmaDescriptor;  // Next dmaDescriptor address (self-reload)
}

void configure_dma() {
    // Enable the DMAC controller
    MCLK->AHBMASK.bit.DMAC_ = 1;
    //PM->AHBMASK.reg |= PM_AHBMASK_DMAC;
    //PM->APBBMASK.reg |= PM_APBBMASK_DMAC;

    // Reset the DMA controller
    DMAC->CTRL.reg = DMAC_CTRL_SWRST;

    // Configure the DMA channel (e.g., channel 0)
    DMAC->Channel[0].CHCTRLA.bit.ENABLE = 0;
    while (DMAC->Channel[0].CHCTRLA.bit.ENABLE)
        ;
      
    DMAC->Channel[0].CHCTRLA.bit.SWRST = 1;
    while (DMAC->Channel[0].CHCTRLA.bit.SWRST)
        ;

    DMAC->Channel[0].CHINTENSET.reg = DMAC_CHINTENSET_TCMPL; //Enable transfer complete interrupt

    // DMAC->Channel[0].CHCTRLA.bit.
    // DMAC_CHCTRLB_LVL(0) |   // Priority level 0
    //                     DMAC_CHCTRLB_TRIGSRC(SERCOM1_DMAC_ID_RX) | // Trigger on SERCOM1 RX
    //                     DMAC_CHCTRLB_TRIGACT_BEAT; // Trigger action: beat

    // DMAC->CHCTRLA.reg = DMAC_CHCTRLA_ENABLE; // Enable the DMA channel
    // DMAC->BASEADDR.reg = (uint32_t)&dmaDescriptorArray[0]; // Set the base dmaDescriptorArray[] address
    // DMAC->WRBADDR.reg = (uint32_t)&dmaDescriptorWritebackArray[0];  // Write-back memory section address

    // // Enable the DMA controller
    // DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf); // Enable the DMA controller
}

// void dma_init() {
//     configure_dma_descriptor();
//     configure_dma();
//     NVIC_EnableIRQ(DMAC_IRQn);
//     NVIC_SetPriority(DMAC_IRQn, 1);
// }



// DMA interrupt handler for handling 50-byte chunk completion
// void DMAC_Handler(void) {
//     // Check if Transfer Complete interrupt occurred for channel 0
//     if (DMAC->CHINTFLAG.bit.TCMPL) {
//         // Clear the interrupt flag
//         DMAC->CHID.reg = DMAC_CHID_ID(0);  // Select Channel 0
//         DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;  // Clear Transfer Complete interrupt flag

//         // Reset DMA descriptor for the next 50-byte block
//         dmaDescriptorArray[0].BTCNT.reg = DMA_BURST_SIZE;  // Set the next block transfer size to 50 bytes
//         dmaBufferPosition = (dmaBufferPosition % DMA_BUFFER_SIZE)  + DMA_BURST_SIZE; //Move position by 50 bytes
//         dmaDescriptorArray[0].DSTADDR.reg = (uint32_t)(circular_buffer) + dmaBufferPosition;// The destination address wraps within the 500-byte buffer
//     }
// }

// void setup()
// {
//   pinMode(STAT_LED, OUTPUT);
//   // // enable input, to support reading back values, with pullups disabled
//   // PORT->Group[g_APinDescription[STAT_LED].ulPort].PINCFG[g_APinDescription[STAT_LED].ulPin].reg = (uint8_t)(PORT_PINCFG_INEN | PORT_PINCFG_DRVSTR);

//   // // Set pin to output mode
//   // PORT->Group[g_APinDescription[STAT_LED].ulPort].DIRSET.reg = 1 << g_APinDescription[STAT_LED].ulPin;

//   // uint32_t pin = STAT_LED;
//   // uint32_t value = 0;
//   // PinDescription pinDesc = g_APinDescription[pin];
//   // uint32_t attr = pinDesc.ulPinAttribute;

//   // uint32_t tcNum = GetTCNumber(pinDesc.ulPWMChannel);
//   // uint8_t tcChannel = GetTCChannelNumber(pinDesc.ulPWMChannel);

//   // pinPeripheral(pin, PIO_TCC_PDEC);

//   // GCLK->PCHCTRL[GCLK_CLKCTRL_IDs[tcNum]].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);

//   // // -- Configure TCC

//   // TCC0->CTRLA.bit.SWRST = 1;
//   // while (TCC0->SYNCBUSY.bit.SWRST)
//   //   ;

//   // TCC0->CTRLA.bit.ENABLE = 0;
//   // while (TCC0->SYNCBUSY.bit.ENABLE)
//   //   ;
//   // // Set prescaler to 1/256
//   // TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV256 | TCC_CTRLA_PRESCSYNC_GCLK;

//   // // Set TCx as normal PWM
//   // TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_DSBOTH;
//   // while (TCC0->SYNCBUSY.bit.WAVE)
//   //   ;

//   // while (TCC0->SYNCBUSY.bit.CC0 || TCC0->SYNCBUSY.bit.CC1)
//   //   ;
//   // // Set the initial value
//   // TCC0->CC[tcChannel].reg = (uint32_t)value;
//   // while (TCC0->SYNCBUSY.bit.CC0 || TCC0->SYNCBUSY.bit.CC1)
//   //   ;
//   // // Set PER to maximum counter value (resolution : 0xFF)
//   // TCC0->PER.reg = 0xFF;
//   // while (TCC0->SYNCBUSY.bit.PER)
//   //   ;
//   // // Enable TCCx
//   // TCC0->CTRLA.bit.ENABLE = 1;
//   // while (TCC0->SYNCBUSY.bit.ENABLE)
//   //   ;
// }

// void myanalogWrite(uint32_t pin, uint32_t value)
// {
//   PinDescription pinDesc = g_APinDescription[pin];
//   uint32_t attr = pinDesc.ulPinAttribute;

//   uint32_t tcNum = GetTCNumber(pinDesc.ulPWMChannel);
//   uint8_t tcChannel = GetTCChannelNumber(pinDesc.ulPWMChannel);
 
//   Serial.printf("tcNum: %d, tcChannel: %d\n", tcNum, tcChannel);
  
//   while (TCC0->SYNCBUSY.bit.CTRLB)
//     ;
//   while (TCC0->SYNCBUSY.bit.CC0 || TCC0->SYNCBUSY.bit.CC1)
//     ;
//   TCC0->CCBUF[tcChannel].reg = (uint32_t)(0x7F);
//   while (TCC0->SYNCBUSY.bit.CC0 || TCC0->SYNCBUSY.bit.CC1)
//     ;
//   TCC0->CTRLBCLR.bit.LUPD = 1;
//   while (TCC0->SYNCBUSY.bit.CTRLB)
//     ;
// }

// void setupDMA()
// {
//   // Enable the DMA controller
//   MCLK->AHBMASK.bit.DMAC_ = 1;
//   //MCLK->APBBMASK.bit.DMAC_ = 1;

//   // Reset DMA controller
//   DMAC->CTRL.bit.SWRST = 1;
//   while (DMAC->CTRL.bit.SWRST)
//     ;

//   // Enable DMA controller
//   DMAC->CTRL.bit.DMAENABLE = 1;
//   DMAC->CTRL.bit.LVLEN0 = 1;
//   DMAC->CTRL.bit.LVLEN1 = 1;
//   DMAC->CTRL.bit.LVLEN2 = 1;
//   DMAC->CTRL.bit.LVLEN3 = 1;

//   // Configure DMA channel 0
//   DMAC->Channel[0].CHCTRLA.bit.ENABLE = 0;
//   while (DMAC->Channel[0].CHCTRLA.bit.ENABLE)
//     ;

//   DMAC->Channel[0].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(TCC0_DMAC_ID_OVF) | DMAC_CHCTRLA_TRIGACT_BEAT;
//   DMAC->Channel[0].CHCTRLB.reg = DMAC_CHCTRLB_LVL_LVL0 | DMAC_CHCTRLB_TRIGACT_BEAT;

//   // Configure DMA descriptor
//   static DmacDescriptor descriptor __attribute__((aligned(16)));
//   descriptor.BTCTRL.bit.VALID = 1;
//   descriptor.BTCTRL.bit.EVOSEL = DMAC_BTCTRL_EVOSEL_DISABLE_Val;
//   descriptor.BTCTRL.bit.BLOCKACT = DMAC_BTCTRL_BLOCKACT_NOACT_Val;
//   descriptor.BTCTRL.bit.BEATSIZE = DMAC_BTCTRL_BEATSIZE_BYTE_Val;
//   descriptor.BTCTRL.bit.SRCINC = 1;
//   descriptor.BTCTRL.bit.DSTINC = 0;
//   descriptor.BTCTRL.bit.STEPSEL = DMAC_BTCTRL_STEPSEL_SRC_Val;
//   descriptor.BTCTRL.bit.STEPSIZE = DMAC_BTCTRL_STEPSIZE_X1_Val;
//   descriptor.BTCNT.reg = 1;
//   descriptor.SRCADDR.reg = (uint32_t)&value + 1;
//   descriptor.DSTADDR.reg = (uint32_t)&(PORT->Group[g_APinDescription[STAT_LED].ulPort].OUTSET.reg);
//   descriptor.DESCADDR.reg = 0;

//   // Set descriptor address
//   DMAC->Channel[0].CHID.reg = 0;
//   DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;
//   DMAC->Channel[0].CHCTRLB.bit.EVIE = 1;
//   DMAC->Channel[0].CHCTRLB.bit.EVOE = 1;
//   DMAC->Channel[0].CHCTRLB.bit.LVL = 0;
//   DMAC->Channel[0].CHCTRLB.bit.TRIGSRC = TCC0_DMAC_ID_OVF;
//   DMAC->Channel[0].CHCTRLB.bit.TRIGACT = DMAC_CHCTRLB_TRIGACT_BEAT_Val;
//   DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;
// }
void TC0_Handler() {
  if (TC0->COUNT16.INTFLAG.bit.OVF) {
    // Toggle the state of STAT_LED
    PORT->Group[g_APinDescription[STAT_LED].ulPort].OUTTGL.reg = (1 << g_APinDescription[STAT_LED].ulPin);
    // Clear the overflow interrupt flag
    TC0->COUNT16.INTFLAG.bit.OVF = 1;
    Serial.println("overflow");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(STAT_LED, OUTPUT);
  digitalWrite(STAT_LED, LOW);
  delay(1000);
  
  // Enable the TC0 module
  MCLK->APBAMASK.bit.TC0_ = 1;

  // Configure the Generic Clock Generator 0 (GCLK0) for TC0
  GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
  while (GCLK->PCHCTRL[TC0_GCLK_ID].bit.CHEN == 0);

  // Reset TC0
  TC0->COUNT16.CTRLA.bit.SWRST = 1;
  while (TC0->COUNT16.SYNCBUSY.bit.SWRST);

  // Configure TC0 in 16-bit mode, with a prescaler of 1024
  TC0->COUNT16.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT16;
  TC0->COUNT16.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV1024_Val;
  TC0->COUNT16.WAVE.bit.WAVEGEN = TC_WAVE_WAVEGEN_NFRQ_Val;
  while (TC0->COUNT16.SYNCBUSY.bit.ENABLE);
  // Set the period (TOP value) for the timer
  TC0->COUNT16.CC[0].reg = 0xFFFF; // 46875 * (1024 / 48000000) = 1 second
  while (TC0->COUNT16.SYNCBUSY.bit.CC0);

  // Enable the overflow interrupt
  TC0->COUNT16.INTENSET.bit.OVF = 1;

  // Enable the TC0 interrupt in the NVIC
  NVIC_EnableIRQ(TC0_IRQn);
  NVIC_SetPriority(TC0_IRQn, 1);

  // Enable TC0
  TC0->COUNT16.CTRLA.bit.ENABLE = 1;
  while (TC0->COUNT16.SYNCBUSY.bit.ENABLE);
}

void loop()
{
//  digitalWrite(STAT_LED, HIGH);
//   delay(1000);
//   digitalWrite(STAT_LED, LOW);
//   delay(1000);
}
