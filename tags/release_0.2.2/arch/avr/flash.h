#ifndef __FLASH_H__
#define __FLASH_H__
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
    Revision Info:  $Id: flash.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup FLASH flash.h : Internal FLASH write routines
 *
 *  This component can clear and write a page of internal FLASH.
 *
 *  Files: flash.h & flash.c
 *
 *  @note These routines must always reside in the bootloader section. 
 *        If mixing application code and bootloader code, the prefix 
 *        BOOTLOADER_SECTION can be used to place these functions in a 
 *        section called ".bootloader". See <avr/boot.h>
 * 
 * @par Example of setting ".bootloader" section address in Makefile:
 * @code
 * #LDFLAGS += -Wl,--section-start,.bootloader=0x1FF00
 * @endcode
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
/// FLASH Page size in bytes
#define FLASH_PAGE_SIZE  SPM_PAGESIZE

/// Total number of FLASH pages (application and bootloader section)
#define FLASH_NR_OF_PAGES ((FLASHEND+1)/FLASH_PAGE_SIZE)

/* _____FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Function to clear a page.
 *
 *  @param page  0 to (#FLASH_NR_OF_PAGES - 1)
 * 
 */
extern void flash_clear_page(const u16_t page);

/**
 *  Function to write a page.
 * 
 *  @param page      0 to (#FLASH_NR_OF_PAGES - 1)
 *  @param data      Pointer to buffer containing data to write
 * 
 */
extern void flash_write_page(const u16_t page, const  u8_t *data);

/**
 * @}
 */

#endif
