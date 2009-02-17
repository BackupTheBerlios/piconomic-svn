/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
/// \dir
/// !Purpose
/// 
/// Definition and functions for using AT91SAM7S-related features, such
/// has PIO pins, memories, etc.
/// 
/// !Usage
/// -# The code for booting the board is provided by board_cstartup.S and
///    board_lowlevel.c.
/// -# For using board PIOs, board characteristics (clock, etc.) and external
///    components, see board.h.
/// -# For manipulating memories (remapping, SDRAM, etc.), see board_memories.h.
//------------------------------------------------------------------------------
 
//------------------------------------------------------------------------------
/// \unit
/// !Purpose
/// 
/// Definition of AT91SAM7S-EK characteristics, AT91SAM7S-dependant PIOs and
/// external components interfacing.
/// 
/// !Usage
/// -# For operating frequency information, see "SAM7S-EK - Operating frequencies".
/// -# For using portable PIO definitions, see "SAM7S-EK - PIO definitions".
/// -# Several USB definitions are included here (see "SAM7S-EK - USB device").
//------------------------------------------------------------------------------

#ifndef BOARD_H 
#define BOARD_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#if defined(at91sam7s32)
    #include "at91sam7s32/at91sam7s32.h"
#elif defined(at91sam7s321)
    #include "at91sam7s321/at91sam7s321.h"
#elif defined(at91sam7s64)
    #include "at91sam7s64/at91sam7s64.h"
#elif defined(at91sam7s128)
    #include "at91sam7s128/at91sam7s128.h"
#elif defined(at91sam7s256)
    #include "at91sam7s256/at91sam7s256.h"
#elif defined(at91sam7s512)
    #include "at91sam7s512/at91sam7s512.h"
#else
    #error Board does not support the specified chip.
#endif

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7S-EK - Board Description"
/// This page lists several definition related to the board description.
///
/// !Definitions
/// - BOARD_NAME

/// Name of the board.
#define BOARD_NAME "AT91SAM7S-EK"
/// Board definition.
#define at91sam7sek
/// Family definition.
#define at91sam7s
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7S-EK - Operating frequencies"
/// This page lists several definition related to the board operating frequency
/// (when using the initialization done by board_lowlevel.c).
/// 
/// !Definitions
/// - BOARD_MAINOSC
/// - BOARD_MCK

/// Frequency of the board main oscillator.
#define BOARD_MAINOSC           18432000

/// Master clock frequency (when using board_lowlevel.c).
#define BOARD_MCK               48000000
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7S-EK - USB device"
/// This page lists constants describing several characteristics (controller
/// type, D+ pull-up type, etc.) of the USB device controller of the chip/board.
/// 
/// !Constants
/// - BOARD_USB_UDP
/// - BOARD_USB_PULLUP_EXTERNAL
/// - BOARD_USB_NUMENDPOINTS
/// - BOARD_USB_ENDPOINTS_MAXPACKETSIZE
/// - BOARD_USB_ENDPOINTS_BANKS

/// Chip has a UDP controller.
#define BOARD_USB_UDP

/// Indicates the D+ pull-up is external.
#define BOARD_USB_PULLUP_EXTERNAL

/// Number of endpoints in the USB controller.
#define BOARD_USB_NUMENDPOINTS                  4

/// Returns the maximum packet size of the given endpoint.
#define BOARD_USB_ENDPOINTS_MAXPACKETSIZE(i)    ((i == 0) ? 8 : 64)

/// Returns the number of FIFO banks for the given endpoint.
#define BOARD_USB_ENDPOINTS_BANKS(i)            (((i == 0) || (i == 3)) ? 1 : 2)

/// USB attributes configuration descriptor (bus or self powered, remote wakeup)
#define BOARD_USB_BMATTRIBUTES                  USBConfigurationDescriptor_SELFPOWERED_NORWAKEUP
//#define BOARD_USB_BMATTRIBUTES                  USBConfigurationDescriptor_BUSPOWERED_NORWAKEUP
//#define BOARD_USB_BMATTRIBUTES                  USBConfigurationDescriptor_SELFPOWERED_RWAKEUP
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7S-EK - PIO definitions"
/// This pages lists all the pio definitions contained in board.h. The constants
/// are named using the following convention: PIN_* for a constant which defines
/// a single Pin instance (but may include several PIOs sharing the same
/// controller), and PINS_* for a list of Pin instances.
///  
/// !DBGU
/// - PINS_DBGU
/// 
/// !LEDs
/// - PIN_LED_DS1
/// - PIN_LED_DS2
/// - PIN_LED_DS3
/// - PIN_LED_DS4
/// - PINS_LEDS
/// - LED_DS1
/// - LED_DS2
/// - LED_DS3
/// - LED_DS4
///
/// !Push buttons
/// - PIN_PUSHBUTTON_1
/// - PIN_PUSHBUTTON_2
/// - PIN_PUSHBUTTON_3
/// - PIN_PUSHBUTTON_4
/// - PINS_PUSHBUTTONS
/// - PUSHBUTTON_BP1
/// - PUSHBUTTON_BP2
/// - PUSHBUTTON_BP3
/// - PUSHBUTTON_BP4
///
/// !USART0
/// - PIN_USART0_RXD
/// - PIN_USART0_TXD
/// - PIN_USART0_SCK
/// 
/// !SPI0
/// - PIN_SPI_MISO
/// - PIN_SPI_MOSI
/// - PIN_SPI_SPCK
/// - PINS_SPI
/// - PIN_SPI_NPCS0
/// 
/// !PWMC
/// - PIN_PWMC_PWM0
/// - PIN_PWMC_PWM1
/// - PIN_PWMC_PWM2
///
/// !TWI
/// - PINS_TWI
///
/// !USB
/// - PIN_USB_VBUS
/// - PIN_USB_PULLUP

/// DBGU pins (DTXD and DRXD) definitions.
#define PINS_DBGU  {0x00000600, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// LED #0 pin definition.
#define PIN_LED_DS1   {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
/// LED #1 pin definition.
#define PIN_LED_DS2   {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
/// LED #2 pin definition.
#define PIN_LED_DS3   {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
/// LED #3 pin definition.
#define PIN_LED_DS4   {1 << 3, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
/// List of the four LED pin definitions.
#define PINS_LEDS   PIN_LED_DS1, PIN_LED_DS2, PIN_LED_DS3, PIN_LED_DS4
/// LED DS1 index.
#define LED_DS1      0
/// LED DS2 index.
#define LED_DS2      1
/// LED DS3 index.
#define LED_DS3      2
/// LED DS4 index.
#define LED_DS4      3

/// Push button #0 definition.
#define PIN_PUSHBUTTON_1    {1 << 19, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEGLITCH | PIO_PULLUP}
/// Push button #1 definition.
#define PIN_PUSHBUTTON_2    {1 << 20, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEGLITCH | PIO_PULLUP}
/// Push button #2 definition
#define PIN_PUSHBUTTON_3    {1 << 15, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEGLITCH | PIO_PULLUP}
/// Push button #3 definition.
#define PIN_PUSHBUTTON_4    {1 << 14, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEGLITCH | PIO_PULLUP}
/// List of all push button definitions.
#define PINS_PUSHBUTTONS    PIN_PUSHBUTTON_1, PIN_PUSHBUTTON_2, PIN_PUSHBUTTON_3, PIN_PUSHBUTTON_4
/// Push button #1 index.
#define PUSHBUTTON_BP1   0
/// Push button #2 index.
#define PUSHBUTTON_BP2   1
/// Push button #3 index.
#define PUSHBUTTON_BP3   2
/// Push button #4 index.
#define PUSHBUTTON_BP4   3

/// USART0 TXD pin definition.
#define PIN_USART0_RXD  {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 RXD pin definition.
#define PIN_USART0_TXD  {1 << 6, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 RTS pin definition.
#define PIN_USART0_RTS  {1 << 7, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 CTS pin definition.
#define PIN_USART0_CTS  {1 << 8, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 SCK pin definition.
#define PIN_USART0_SCK  {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}

/// SPI MISO pin definition.
#define PIN_SPI_MISO   {1 << 12, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
/// SPI MOSI pin definition.
#define PIN_SPI_MOSI   {1 << 13, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// SPI SPCK pin definition.
#define PIN_SPI_SPCK   {1 << 14, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// List of SPI pin definitions (MISO, MOSI & SPCK).
#define PINS_SPI       PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SPCK
/// SPI chip select 0 pin definition.
#define PIN_SPI_NPCS0  {1 << 11, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// PWMC PWM0 pin definition.
#define PIN_PWMC_PWM0  {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// PWMC PWM0 pin definition.
#define PIN_PWMC_PWM1  {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// PWMC PWM0 pin definition.
#define PIN_PWMC_PWM2  {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// TWI pins definition.
#define PINS_TWI  {0x00000018, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// USB VBus monitoring pin definition.
#define PIN_USB_VBUS    {1 << 13, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
/// USB pull-up control pin definition.
#define PIN_USB_PULLUP  {1 << 16, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7S-EK - Memories"
/// This page lists definitions related to internal & external on-board memories.
/// 
/// !Embedded Flash
/// - BOARD_FLASH_EFC
/// - BOARD_FLASH_IAP_ADDRESS

/// Indicates chip has an EFC.
#define BOARD_FLASH_EFC
/// Address of the IAP function in ROM.
#define BOARD_FLASH_IAP_ADDRESS         0x300E08
//------------------------------------------------------------------------------

#endif //#ifndef BOARD_H

