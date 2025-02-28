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

void dma_init() {
    configure_dma_descriptor();
    configure_dma();
    NVIC_EnableIRQ(DMAC_0_IRQn);
    NVIC_SetPriority(DMAC_0_IRQn, 1);
}



// // DMA interrupt handler for handling 50-byte chunk completion
// void DMAC_0_Handler(void) {
//     // Check if Transfer Complete interrupt occurred for channel 0
//     // if (DMAC->CHINTFLAG.bit.TCMPL) {
//     //     Clear the interrupt flag
//     //     DMAC->CHID.reg = DMAC_CHID_ID(0);  // Select Channel 0
//     //     DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;  // Clear Transfer Complete interrupt flag

//     //     Reset DMA descriptor for the next 50-byte block
//     //     dmaDescriptorArray[0].BTCNT.reg = DMA_BURST_SIZE;  // Set the next block transfer size to 50 bytes
//     //     dmaBufferPosition = (dmaBufferPosition % DMA_BUFFER_SIZE)  + DMA_BURST_SIZE; //Move position by 50 bytes
//     //     dmaDescriptorArray[0].DSTADDR.reg = (uint32_t)(circular_buffer) + dmaBufferPosition;// The destination address wraps within the 500-byte buffer
//     // }
// }
int start = 0;
void TC0_Handler() {
  if (TC0->COUNT16.INTFLAG.bit.OVF) {
    // Toggle the state of STAT_LED
    PORT->Group[g_APinDescription[STAT_LED].ulPort].OUTTGL.reg = (1 << g_APinDescription[STAT_LED].ulPin);
    // Clear the overflow interrupt flag
    TC0->COUNT16.INTFLAG.bit.OVF = 1;
    Serial.printf("DMA: %d\n", millis()-start);
    start = millis();
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  pinMode(STAT_LED, OUTPUT);
  digitalWrite(STAT_LED, LOW);
  delay(1000);
  
  // Enable the TC0 module
  MCLK->APBAMASK.bit.TC0_ = 1;

  // Configure the Generic Clock Generator 0 (GCLK0) for TC0
  GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;
  while (GCLK->PCHCTRL[TC0_GCLK_ID].bit.CHEN == 0);

  GCLK->GENCTRL[1].bit.DIV = 0x00;
  while(GCLK->SYNCBUSY.bit.GENCTRL1);

  GCLK->GENCTRL[1].bit.DIVSEL = 0;
  while(GCLK->SYNCBUSY.bit.GENCTRL1);

  Serial.printf("Prescaler: %d\n", GCLK->GENCTRL[0].bit.DIV);

  // Reset TC0
  TC0->COUNT16.CTRLA.bit.SWRST = 1;
  while (TC0->COUNT16.SYNCBUSY.bit.SWRST);
  
  TC0->COUNT16.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT16;
  TC0->COUNT16.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV1024_Val;
  TC0->COUNT16.WAVE.bit.WAVEGEN = TC_WAVE_WAVEGEN_NFRQ_Val;
  while (TC0->COUNT16.SYNCBUSY.bit.ENABLE);
  // Set the period (TOP value) for the timer
  TC0->COUNT16.CC[0].reg = 0x1000; // 46875 * (1024 / 48000000) = 1 second
  while (TC0->COUNT16.SYNCBUSY.bit.CC0);  

  // Enable the overflow interrupt
  TC0->COUNT16.INTENSET.bit.OVF = 1;

  // Enable the TC0 interrupt in the NVIC
  NVIC_EnableIRQ(TC0_IRQn);
  NVIC_SetPriority(TC0_IRQn, 1);

  // Enable TC0
  TC0->COUNT16.CTRLA.bit.ENABLE = 1;
  while (TC0->COUNT16.SYNCBUSY.bit.ENABLE);

  start = millis();
}

void loop()
{
//  digitalWrite(STAT_LED, HIGH);
//   delay(1000);
//   digitalWrite(STAT_LED, LOW);
//   delay(1000);
}
