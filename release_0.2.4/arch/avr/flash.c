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
    
    Title:          Internal FLASH write routines
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/boot.h>
#include <avr/interrupt.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "flash.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
BOOTLOADER_SECTION void flash_clear_page(const u16_t page)
{
    // Calculate 32-bit address from specified page
    u32_t adr = ((u32_t)page) * SPM_PAGESIZE;

    // Save status of global interrupts
    u8_t sreg = SREG;

    // Disable interrupts
    cli();

    boot_page_erase(adr);

    // Wait until SPM command has finished
    boot_spm_busy_wait();

    // Re-enable RWW-section
    boot_rww_enable();

    // Restore status of global interrupts
    SREG = sreg;
}

BOOTLOADER_SECTION void flash_write_page(const u16_t page, const u8_t* data)
{
    u16_t i;

    // Calculate 32-bit address from specified page
    u32_t adr = ((u32_t)page) * FLASH_PAGE_SIZE;

    // Save status of global interrupts
    u8_t sreg = SREG;

    // Disable interrupts
    cli();

    // Clear the page first
    flash_clear_page(page);

    // Fill write data with 16-bit data
    for(i=0; i<FLASH_PAGE_SIZE; i+=2)
    {
        // Fetch little endian word of data
        u16_t word;
        word  = *data++;
        word += (*data++) << 8;

        boot_page_fill(adr+i, word);
    }

    // Commit data to flash page
    boot_page_write(adr);   

    // Wait until SPM command has finished
    boot_spm_busy_wait();

    // Re-enable RWW-section
    boot_rww_enable();

    // Restore status of global interrupts
    SREG = sreg;
}

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : Pieter.Conradie
 - First release
 
 2009-06-11 Pieter.Conradie
 - Fixed bug in flash_clear_page() where address was hardcoded for a
   page size of 256 bytes.
   
*/
