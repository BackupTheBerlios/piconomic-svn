#ifndef __BOARD_H__
#define __BOARD_H__
/* =============================================================================

    Copyright (c) 2006 Pieter Conradie <pieterconradie@users.berlios.de>
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
    
    * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
    
    * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
   
    Title:          Piconomic Design AB111-4 ATmega128 board
    Author(s):      Pieter Conradie
    Creation Date:  2007-04-27
    Revision Info:  $Id $

============================================================================= */
/** 
 *  @ingroup AVR_BOARDS
 *  @defgroup PICONOMIC_AB111_4 /piconomic_ab111-4 : Piconomic Design AB111-4 ATmega128 board
 * 
 *  Open hardware Atmel AVR development board.
 * 
 *  @image html piconomic_ab111-4.jpg
 * 
 *  Design documents and files located in /arch/avr/boards/piconomic_ab111-4:
 *  - <a href="file:///C:/piconomic_fwlib/arch/avr/boards/piconomic_ab111-4/piconomic_ab111-4_sch.pdf">Schematic</a>
 *  - <a href="file:///C:/piconomic_fwlib/arch/avr/boards/piconomic_ab111-4/piconomic_ab111-4_bom.pdf">Bill Of Materials</a>
 *  - <a href="file:///C:/piconomic_fwlib/arch/avr/boards/piconomic_ab111-4/piconomic_ab111-4_layout.pdf">Board Layout</a>
 *  - <a href="file:///C:/piconomic_fwlib/arch/avr/boards/piconomic_ab111-4/piconomic_ab111-4_errata.pdf">Errata for AB111-4 PCB</a>
 *  - <a href="file:///C:/piconomic_fwlib/arch/avr/boards/piconomic_ab111-4/piconomic_ab111-4_gerbers.7z">CAM files to manufacture PCB</a>
 * 
 *  Features:
 *  - ATmega128L-8AU microcontroller with 7.3728 MHz crystal oscillator
 *  - 32.768 kHz RTC
 *  - 250 kbps RS-232 Transceiver with hardware flow control (RTS/CTS)
 *  - 1/8 UL, slew rate limited, 250 kbps RS-485 Transceiver
 *  - 4 Mbit serial DataFlash with SPI interface
 *  - 100 mil pitch Headers: JTAG, ISP, GPIO, SPI, ADC, I2C, XMEM
 *  - User Interface: power and user LED, reset and user button, buzzer
 *  - +6V to +9V DC power socket (2.0mm female) with reverse-voltage protection diode
 *
 *  Reference documents:
 *  - Atmel <a href="http://www.atmel.com/dyn/products/product_card.asp?part_id=2018">ATmega128</a> datasheet
 *  - <a href="http://www.atmel.com/dyn/resources/prod_documents/doc0856.pdf">8-bit AVR Instruction set</a>
 *  - <a href="file:///D:/WinAVR/doc/avr-libc/avr-libc-user-manual/index.html">AVR Libc Reference Manual</a> 
 *
 *  Files:
 *  - arch/avr/boards/piconomic_ab111-4/board.h
 *  - arch/avr/boards/piconomic_ab111-4/board.c
 *
 *  @{
 */
/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
/** 
 *  @name Processor frequency in Hz
 */
//@{
#define F_CPU                   7372800ul
//@}

/** 
 *  @name External crystal frequency definition for "rtc_tmr0_32khz.h"
 *  See @ref RTC_TMR0_32KHZ
 */
//@{
#define F_RTC                   32768ul
//@}

/** 
 *  @name IO Pin Definition for "spi_master.h"
 *  See @ref SPI_MASTER
 */
//@{
#define PORT_SPI                PORTB
#define DDR_SPI                 DDRB
#define BIT_SPI_SS              0
#define BIT_SPI_SCK             1
#define BIT_SPI_MOSI            2
#define BIT_SPI_MISO            3
//@}

/** 
 *  @name IO Pin Definition for "sflash_AT45DB041B.h"
 *  See @ref AT45DB041B
 */
//@{
#define PORT_SFLASH_CS_O        PORTB
#define DDR_SFLASH_CS_O         DDRB
#define BIT_SFLASH_CS_O         4
//@}

/// @name Active low switch pin definition
//@{
#define PIN_SW_IP               PINB
#define BIT_SW_IP               5

/// Macro to detect if the switch is being pressed
#define SW_IS_PRESSED()         BIT_IS_LO(PIN_SW_IP,BIT_SW_IP)
//@}

/// @name User LED pin definition
//@{
#define PORT_LED_O              PORTB
#define DDR_LED_O               DDRB
#define BIT_LED_O               6

/// Macro to enable LED
#define LED_ON()                BIT_SET_HI(PORT_LED_O,BIT_LED_O)

/// Macro to disable LED
#define LED_OFF()               BIT_SET_LO(PORT_LED_O,BIT_LED_O)

/// Macro to toggle LED
#define LED_TOGGLE()            BIT_TOGGLE(PORT_LED_O,BIT_LED_O)
//@}

/// @name Buzzer pin definition
//@{
#define PORT_BUZZER_O           PORTB
#define BIT_BUZZER_O            7

/// Macro to enable buzzer
#define BUZZER_ON()             BIT_SET_LO(PORT_BUZZER_O,BIT_BUZZER_O)

/// Macro to disable buzzer
#define BUZZER_OFF()            BIT_SET_HI(PORT_BUZZER_O,BIT_BUZZER_O)
//@}

/// @name RS-485 direction control pin definitions
//@{
#define PORT_RS485_TX_EN_O      PORTD
#define DDR_RS485_TX_EN_O       DDRD
#define BIT_RS485_TX_EN_O       4

#define PORT_RS485_RX_EN_O      PORTD
#define DDR_RS485_RX_EN_O       DDRD
#define BIT_RS485_RX_EN_O       5
//@}

/// @name RS-232 Hardware flow control pin definitions
//@{
#define PIN_RTS_I               PIND
#define BIT_RTS_I               6

#define PORT_CTS_O              PORTD
#define BIT_CTS_O               7
//@}

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise the board hardware.
 */
extern void board_lowlevel_init(void);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
