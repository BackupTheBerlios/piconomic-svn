#ifndef __BOARD_H__
#define __BOARD_H__
/* =============================================================================

    Copyright (c) 2010 Pieter Conradie [www.piconomic.co.za]
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
   
    Title:          Unknown Microchip PIC24FGA002 board
    Author(s):      Pieter Conradie
    Creation Date:  2010/03/29
    Revision Info:  $Id $

============================================================================= */

/** 
 *  @ingroup PIC24F_BOARDS
 *  @defgroup UNKNOWN_PIC24FGA002 /unknown_pic24fga002 : Microchip PIC 24FGA002 board
 * 
 *  Microchip PIC 24FGA002 board.
 * 
 *  Files:
 *  - arch/pic/24F/boards/unknown_pic24fga002/board.h
 *  - arch/pic/24F/boards/unknown_pic24fga002/board.c
 *
 *  @{
 */
/* _____STANDARD INCLUDES____________________________________________________ */
#include <p24Fxxxx.h>									  

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
/// Specify which board is used
#define BOARD_UNKNOWN_PIC24FGA002   1

/// Frequency of primary oscillator in Hz
#define F_OSC               8000000ul

/// Fcy = Fosc/2 in Hz (8MHz Fast RC x 4 PLL / 2)
#define F_CY                ((F_OSC*4)/2ul)

/// Disable clock switching option
#define BOARD_CLK_SWITCHING_ENABLED 0

/// DataFlash options
#define AT45DB041D
#define AT45D_POWER_OF_TWO_PAGE_SIZE 1

/// UART1 options
#define UART1_BAUD           19200ul
#define UART1_TX_BUFFER_SIZE 512
#define UART1_RX_BUFFER_SIZE 64

/// UART2 options
#define UART2_BAUD           9600ul
#define UART2_TX_BUFFER_SIZE 64
#define UART2_RX_BUFFER_SIZE 256

/// Data FLASH Chip Select
#define FLASH_CS_LATCH      LATBbits.LATB5
#define FLASH_CS_LO()       {FLASH_CS_LATCH=0;}
#define FLASH_CS_HI()       {FLASH_CS_LATCH=1;}

/// RF Power Amplifier power enable
#define RF_PA_LATCH         LATAbits.LATA0
#define RF_PA_ON()          {RF_PA_LATCH=0;}
#define RF_PA_OFF()         {RF_PA_LATCH=1;}

/// RF Transceiver power enable
#define RF_PWR_ON()
#define RF_PWR_OFF()

/// GPS power enable
#define GPS_PWR_LATCH       LATAbits.LATA2
#define GPS_PWR_ON()        {GPS_PWR_LATCH=1;}
#define GPS_PWR_OFF()       {GPS_PWR_LATCH=0;}
#define GPS_PWR_IS_ON()     (GPS_PWR_LATCH!=0)

/// Accelerometer Chip Select
#define ACCEL_CS_LATCH      LATAbits.LATA3
#define ACCEL_CS_LO()       {FLASH_CS_LATCH=0;}
#define ACCEL_CS_HI()       {FLASH_CS_LATCH=1;}

/// Green LED
#define LED_GRN_ON()
#define LED_GRN_OFF()
#define LED_GRN_TOGGLE()

/// Red LED
#define LED_RED_LATCH       LATAbits.LATA1
#define LED_RED_ON()        {LED_RED_LATCH=1;}
#define LED_RED_OFF()       {LED_RED_LATCH=0;}
#define LED_RED_TOGGLE()    {if(LED_RED_LATCH==0)LED_RED_ON() else LED_RED_OFF()}

/// SPI1 Peripheral
#define SPI1_CLOCK          RP8
#define SPI1_DATA_OUTPUT    RP9
#define SPI1_DATA_INPUT     RP6

/// RF Interrupt 0
#define RF_INT0             RP7
/// RF Interrupt 1
#define RF_INT1             RP12

/// RF SS_DATA
#define RF_SS_DATA_LATCH    LATBbits.LATB13
#define RF_SS_DATA_LO()     {RF_SS_DATA_LATCH=0;}
#define RF_SS_DATA_HI()     {RF_SS_DATA_LATCH=1;}

/// RF nSS_CONFIG
#define RF_SS_CONFIG_LATCH  LATBbits.LATB14
#define RF_SS_CONFIG_LO()   {RF_SS_CONFIG_LATCH=0;}
#define RF_SS_CONFIG_HI()   {RF_SS_CONFIG_LATCH=1;}

/// RF Data pin
#define RF_DATA_PORT        PORTBbits.RB15
#define RF_DATA_IS_LO()     (RF_DATA_PORT==0)
#define RF_DATA_IS_HI()     (RF_DATA_PORT==1)

/// RF Clk output pin
#define RF_CLK_PORT          PORTAbits.RA4
#define RF_CLK_IS_LO()      (RF_CLK_PORT==0)
#define RF_CLK_IS_HI()      (RF_CLK_PORT==1)

/// Macro to insert a delay of one clock cycle
#define NOP() __asm__ __volatile__("nop\n\t"::);

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise the board hardware.
 */
extern void   board_lowlevel_init(void);

/**
 * Get received data from a serial stream.
 * 
 * @param data    Pointer to location where received data should be stored
 * 
 * @return bool_t TRUE  Data has been received 
 *                FALSE No data available
 */
extern bool_t board_get_char(char *data);
extern void   board_put_char(const char data);

extern void   board_flash_cs_lo(void);
extern void   board_flash_cs_hi(void);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
