/* =============================================================================

    Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>
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
    
    Title:          Atmel AVR XMODEM bootloader
    Author(s):      Pieter.Conradie
    Creation Date:  2007-04-27
    Revision Info:  $Id: $
 
============================================================================= */

/**
 *  This is a simple bootloader that accepts a new binary application file 
 *  that is transferred over the serial port using the XMODEM-CRC protocol.
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
#include "board.h"
#include "tmr_poll.h"
#include "uart_poll.h"
#include "xmodem.h"
#include "flash.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#define FLASH_BOOT_SIZE          (FLASHEND+1-FLASH_BOOT_START)
#define FLASH_BOOT_PAGE_START    ((FLASHEND+1-FLASH_BOOT_SIZE)/FLASH_PAGE_SIZE)
#define FLASH_START_PAGE         (0)

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static u8_t  flash_buffer[FLASH_PAGE_SIZE];
static u16_t flash_page;
static u16_t index;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// Handler function that is called when an XMODEM packet is received
static void on_rx_data(u8_t* data, u8_t bytes_received)
{
    u16_t i = index;

    // Concatenate received data
    while(bytes_received--)
    {
        flash_buffer[i++] = *data++;
    }
    index = i;

    // See if enough data has been received for one flash page
    if(index < FLASH_PAGE_SIZE) return;

    LED_ON();

    // Reset receive index
    index = 0;

    // Write flash page
    flash_write_page(flash_page, flash_buffer);

    // Next page
    flash_page++;

    // Wrap if in bootloader section
    if(flash_page >= FLASH_BOOT_PAGE_START)
    {
        flash_page = 0;
    }

    LED_OFF();
}

// Initialise Programmable IO pins
static void pio_init(void)
{
    PORTB =  (1<<BIT_SFLASH_CS_O)|(1<<BIT_SW_IP)
            |(0<<BIT_LED_O)|(1<<BIT_BUZZER_O);
    DDRB  =  (1<<BIT_SFLASH_CS_O)|(0<<BIT_SW_IP)
            |(1<<BIT_LED_O)|(1<<BIT_BUZZER_O);

    PORTD =  (0<<BIT_RS485_TX_EN_O)|(1<<BIT_RS485_RX_EN_O)
            |(0<<BIT_RTS_I)|(0<<BIT_CTS_O);
    DDRD  =  (1<<BIT_RS485_TX_EN_O)|(1<<BIT_RS485_RX_EN_O)
            |(0<<BIT_RTS_I)|(1<<BIT_CTS_O);
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialise IO pins
    pio_init();

    // Enable LED
    LED_ON();

    // Initialise modules
    tmr_poll_init();
    uart_poll_init();

    // Wait 50 ms for RS232 driver to charge capacitors
    tmr_poll_wait(TMR_POLL_MS_TO_START_VAL(50));

    // Receive new flash content via XMODEM-CRC protocol
    flash_page = FLASH_START_PAGE;
    index      = 0;

    if(xmodem_rx_file(&on_rx_data))
    {
        // Commit last received page
        if(index != 0)
        {

#ifdef BOOT_CLEAR_REST_OF_FLASH
            // Clear rest of flash page
            u16_t i = index;
            while(i < FLASH_PAGE_SIZE)
            {
                flash_buffer[i++] = 0xFF;
            }
#endif
            // Write flash page
            flash_write_page(flash_page,flash_buffer);

#ifdef BOOT_CLEAR_REST_OF_FLASH
            // Next page
            flash_page++;
#endif
        }

#ifdef BOOT_CLEAR_REST_OF_FLASH
        // Clear rest of flash pages
        while(flash_page < FLASH_BOOT_PAGE_START)
        {
            flash_clear_page(flash_page++);
        }
#endif
    }

    // Execute application
#if (FLASHEND < 8*1024)
    // Use relative jump for an AVR with 8k or less of FLASH
    __asm__ __volatile__ ("rjmp RESET_ADDRESS"::);
#else
    // Use absolute jump for an AVR with more than 8k FLASH
    __asm__ __volatile__ ("jmp RESET_ADDRESS"::);
#endif

    for(;;)
    {
    }
}

/* _____LOG__________________________________________________________________ */
/*

 2007-04-27 : PJC
 - First release
   
*/
