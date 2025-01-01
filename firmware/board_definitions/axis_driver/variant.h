/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _VARIANT_FEATHER_M4_
#define _VARIANT_FEATHER_M4_

// The definitions here needs a SAMD core >=1.6.10
#define ARDUINO_SAMD_VARIANT_COMPLIANCE 10610

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC		(32768ul)

/** Master clock frequency */
#define VARIANT_MCK        (F_CPU)

#define VARIANT_GCLK0_FREQ (F_CPU)
#define VARIANT_GCLK1_FREQ (48000000UL)
#define VARIANT_GCLK2_FREQ (100000000UL)

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
#include "SERCOM.h"
#include "Uart.h"
#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// Number of pins defined in PinDescription array
#define PINS_COUNT           (28u)
#define NUM_DIGITAL_PINS     (28u)
#define NUM_ANALOG_INPUTS    (0u)
#define NUM_ANALOG_OUTPUTS   (0u)
#define analogInputToDigitalPin(p)  ((p < 6u) ? (p) + 14u : -1)

#define digitalPinToPort(P)        ( &(PORT->Group[g_APinDescription[P].ulPort]) )
#define digitalPinToBitMask(P)     ( 1 << g_APinDescription[P].ulPin )
//#define analogInPinToBit(P)        ( )
#define portOutputRegister(port)   ( &(port->OUT.reg) )
#define portInputRegister(port)    ( &(port->IN.reg) )
#define portModeRegister(port)     ( &(port->DIR.reg) )
#define digitalPinHasPWM(P)        ( g_APinDescription[P].ulPWMChannel != NOT_ON_PWM || g_APinDescription[P].ulTCChannel != NOT_ON_TIMER )

/*
 * digitalPinToTimer(..) is AVR-specific and is not defined for SAMD
 * architecture. If you need to check if a pin supports PWM you must
 * use digitalPinHasPWM(..).
 *
 * https://github.com/arduino/Arduino/issues/1833
 */
// #define digitalPinToTimer(P)

//Axis Basic 

//Auxiliary Port
#define AUX3 (0)
#define AUX4 (1)
#define AUX5 (2)
#define AUX6 (3)
#define AUX1 (4)
#define AUX2 (5)

//LEDS
#define NEOPIX (6)
#define STAT_LED (7)
#define LED_BUILTIN STAT_LED

//INPUT
#define USR_INPUT (30)

//Motor Control
#define MOTOR_EN (8)
#define MOTOR_DIR (9)
#define MOTOR_STEP (10)
#define MOTOR_M1 (11)
#define MOTOR_M0 (12)
#define MOTOR_INDEX (13)
#define MOTOR_DIAG (14)
#define MOTOR_SPREAD (15)

//Connectivity
#define SDA2 (18)
#define SCL2 (19)
#define SDA1 (20)
#define SCL1 (21)
#define SYNC_PIN (22)

#define EEPROM_CS (23)
#define ETH_CS (24)
#define MOSI (25)
#define MISO (26)
#define SCK (27)

#define PIN_DAC0             (50)
#define PIN_DAC1             (50)
/*
 * Analog pins
 */

#define PIN_VUSB (28ul)
#define PIN_VBUS (29ul)


static const uint8_t A0  = PIN_VUSB;
static const uint8_t A1  = PIN_VBUS;
// static const uint8_t A2  = PIN_A2;
// static const uint8_t A3  = PIN_A3;
// static const uint8_t A4  = PIN_A4;
// static const uint8_t A5  = PIN_A5;
// static const uint8_t A6  = PIN_A6 ;

//static const uint8_t DAC0 = PIN_DAC0;
//static const uint8_t DAC1 = PIN_DAC1;


#define ADC_RESOLUTION		12

// Other pins
#define PIN_ATN              (31ul)
static const uint8_t ATN = PIN_ATN;

// External Serial?: S0
// Internal Serial: 
// Motor UART: S0
// Internal I2C: needs alternative
// External I2C: S2, works
// SPI: S5 MOSI:3, MISO:0, SCK:1

/*
 * Serial interfaces
 */

// Serial1
#define PIN_SERIAL1_RX       (17)
#define PIN_SERIAL1_TX       (16)
#define PAD_SERIAL1_RX       (SERCOM_RX_PAD_3)
#define PAD_SERIAL1_TX       (UART_TX_PAD_0)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (MISO)
#define PIN_SPI_MOSI         (MOSI)
#define PIN_SPI_SCK          (SCK)
#define PERIPH_SPI           sercom5
#define PAD_SPI_TX           SPI_PAD_0_SCK_1
#define PAD_SPI_RX           SERCOM_RX_PAD_2

// static const uint8_t SS	  = 9 ;	// SERCOM1 last PAD is present on d9 but HW SS isn't used. Set here only for reference.
// static const uint8_t MOSI = PIN_SPI_MOSI ;
// static const uint8_t MISO = PIN_SPI_MISO ;
// static const uint8_t SCK  = PIN_SPI_SCK ;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 2

/*
SDA should be on pad 0
SCL should be on pad 1
*/


//External I2C
#define PIN_WIRE_SDA         (SDA2)
#define PIN_WIRE_SCL         (SCL2)
#define PERIPH_WIRE          sercom2
#define WIRE_IT_HANDLER      SERCOM2_Handler
#define WIRE_IT_HANDLER_0    SERCOM2_0_Handler
#define WIRE_IT_HANDLER_1    SERCOM2_1_Handler
#define WIRE_IT_HANDLER_2    SERCOM2_2_Handler
#define WIRE_IT_HANDLER_3    SERCOM2_3_Handler

//Internal I2C
#define PIN_WIRE1_SDA         (SDA1)
#define PIN_WIRE1_SCL         (SCL1)
#define PERIPH_WIRE1          sercom3
#define WIRE1_IT_HANDLER      SERCOM3_Handler
#define WIRE1_IT_HANDLER_0    SERCOM3_0_Handler
#define WIRE1_IT_HANDLER_1    SERCOM3_1_Handler
#define WIRE1_IT_HANDLER_2    SERCOM3_2_Handler
#define WIRE1_IT_HANDLER_3    SERCOM3_3_Handler


/*
 * USB
 */
#define PIN_USB_HOST_ENABLE (28ul)
#define PIN_USB_DM          (29ul)
#define PIN_USB_DP          (30ul)

// On-board QSPI Flash
// #define EXTERNAL_FLASH_DEVICES   GD25Q16C
// #define EXTERNAL_FLASH_USE_QSPI

#if !defined(VARIANT_QSPI_BAUD_DEFAULT)
  // TODO: meaningful value for this
  #define VARIANT_QSPI_BAUD_DEFAULT 5000000
#endif

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

/*	=========================
 *	===== SERCOM DEFINITION
 *	=========================
*/
extern SERCOM sercom0;
extern SERCOM sercom1;
extern SERCOM sercom2;
extern SERCOM sercom3;
extern SERCOM sercom4;
extern SERCOM sercom5;

extern Uart Serial1;

#endif

// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
#define SERIAL_PORT_USBVIRTUAL      Serial
#define SERIAL_PORT_MONITOR         Serial
// Serial has no physical pins broken out, so it's not listed as HARDWARE port
#define SERIAL_PORT_HARDWARE        Serial1

#endif /* _VARIANT_FEATHER_M4_ */

