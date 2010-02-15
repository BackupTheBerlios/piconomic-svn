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
    Revision Info:  $Id$
 
============================================================================= */

/** 
 * @ingroup AVR_EXAMPLES
 * @defgroup AVR_XMODEM_BOOTLOADER /xmodem_bootloader
 * 
 * This is an example of how to implement a bootloader on the Atmel AVR. 
 *  
 * It is a simple bootloader that accepts a new binary application file 
 * transferred over the serial port using the XMODEM-CRC protocol. 
 * 
 * @par How to use the XMODEM-CRC bootloader
 * 
 * 1. Compile and link the firmware application and generate a @b binary programming
 * file (not Intel HEX!)
 * 
 * <em>Use "arch/avr/examples/led_blink/led_blink.bin" as a first test.</em>
 * 
 * 2. Create a new HyperTerminal serial port session, configured to 115200 BAUD,
 * 8 Data Bits, No Parity, 1 Stop Bit, No Flow Control.
 * 
 * <em>At this stage you should verify that the communication between HyperTerminal
 * and the board is OK, by powering and/or resetting the board and verifying that
 * HyperTerminal displays at least one received "C" character.</em>
 * 
 * 3. Select "Transfer > Send File ... ". Select "Protocol > Xmodem". Select your
 * application file. The screen should look similar to image:
 * 
 * @image html avr_hyperterminal.jpg "HyperTerminal"
 * 
 * 3. Select "Send".
 * 
 * <em>HyperTerminal will now wait for a 'C' character from the board to start the
 * transfer.</em>
 * 
 * 5. Power and/or reset the board to start the transfer.
 * 
 * <em>If the transfer is successful, the HyperTerminal dialog window will disappear.
 * The bootloader automatically jumps to the start of the application at address
 * 0x0000.</em>
 * 
 * @par Final Note:
 * 
 * The AVR fuse bits must be set so that execution starts from the boot vector
 * address. This means that the bootloader will always be executed first. 
 * The bootloader sends a 'C' character to start a transfer and waits for 1 
 * second for a valid XMODEM-CRC data packet. If the transfer is not successful,
 * it will jump to address 0x0000 and execute the application. 
 *  
 * @image html avr_fuse_bits.png "AVR Fuse Bits in AVR Studio" 
 * 
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
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

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialise board
    board_lowlevel_init();

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
