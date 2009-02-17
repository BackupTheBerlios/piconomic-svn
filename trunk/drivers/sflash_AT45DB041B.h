#ifndef __AT45DB041B_H__
#define __AT45DB041B_H__
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

    Title:          Atmel AT45DB041B 4-megabit Serial DataFlash Driver
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: sflash_AT45DB041B.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */
/** 
 *  @ingroup DRIVERS
 *  @defgroup AT45DB041B sflash_AT45DB041B.h : Atmel AT45DB041B 4-megabit DataFlash Driver
 *
 *  Driver that communicates with an Atmel AT45DB041B 4-megabit DataFlash using 
 *  @ref SPI_MASTER.
 *
 *  Files: sflash_AT45DB041B.h & sflash_AT45DB041B.c
 *
 *  @par Reference:
 *  Atmel 4-megabit 2.5-volt or 2.7-volt DataFlash AT45DB041B Revision 3443C-DFLASH-5/05.pdf
 *  
 *  @note This driver is also compatible with the Atmel AT45DB041D.
 *  
 *  @par Example:
 *  @include "sflash_AT45DB041B_test.c"
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* __DEFINITIONS ________________________________________________________ */
/// Serial Flash number of pages
#define AT45DB041B_PAGES        2048

/// Serial Flash page size in bytes
#define AT45DB041B_PAGE_SIZE    264

// 
/** 
 *  \name Status register content
 */
//@{
#define AT45DB041B_STATUS_READY 7
#define AT45DB041B_STATUS_COMP  6
#define AT45DB041B_DENSITY_MASK 0x3C
#define AT45DB041B_DENSITY      0x1C
//@}

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise driver flags.
 */
extern void at45db041b_init(void);

/**
 *  Read a page from Serial Flash.
 *  
 *  This function reads a page of data from Serial Flash and stores it in the 
 *  specified buffer. The buffer must be at least #AT45DB041B_PAGE_SIZE bytes
 *  in size to accomodate a full page.
 * 
 *  The Serial Flash has #AT45DB041B_PAGES pages.
 * 
 *  @param[in]  page        0 to (#AT45DB041B_PAGES-1)
 *  @param[out] buffer      Buffer to store read data
 */
extern void at45db041b_read_page(u16_t page,
                                 u8_t  *buffer);

/**
 *  Partial read of data in a page of Serial Flash.
 * 
 *  This function reads part of a page of data from Serial Flash and stores it 
 *  in the specified buffer. The buffer must be at least @b number_of_bytes
 *  in size to the read data.
 * 
 *  @note Only read up to the end of the specified page. If the page boundary is
 *         exceeded, the index will wrap to the start of the page, i.e. only 
 *        the content of the specified page will be read.
 * 
 *  @param[in]  page                0 to (#AT45DB041B_PAGES-1)
 *  @param[in]  start_byte_in_page  Index of first byte to read (0 to
 *                                  #AT45DB041B_PAGE_SIZE - 1)
 *  @param[out] buffer              Buffer to store read data
 *  @param[in]  number_of_bytes     Number of bytes to read
 * 
 */
extern void at45db041b_read_data(u16_t page,
                                 u16_t start_byte_in_page,
                                 u8_t  *buffer,
                                 u16_t number_of_bytes);

/**
 *  Write a page from Serial Flash.
 *  
 *  This function writes a page of data to Serial Flash using the specified 
 *  buffer as the source. The buffer must contain at least #AT45DB041B_PAGE_SIZE bytes
 *  of data.
 * 
 *  The Serial Flash has #AT45DB041B_PAGES pages.
 * 
 *  @param[in]  page   0 to (#AT45DB041B_PAGES-1)
 *  @param[in] buffer  Buffer containing data to be written
 */
extern void at45db041b_write_page(u16_t      page,
                                  const u8_t *buffer);

/**
 *  Partial write of data in a page of Serial Flash.
 * 
 *  This function writes a part of a page of data to Serial Flash using the 
 *  specified buffer as source. The buffer must contain at least @b number_of_bytes
 *  of data.
 * 
 *  @note Only write up to the end of the specified page. If the page boundary
 *        is exceeded, the index will wrap to the start of the page, i.e. only 
 *        the content of the specified page will be written.
 * 
 *  @param[in]  page                0 to (#AT45DB041B_PAGES-1)
 *  @param[in]  start_byte_in_page  Index of first byte to write (0 to
 *                                  #AT45DB041B_PAGE_SIZE - 1)
 *  @param[in]  buffer              Buffer containing data to be written
 *  @param[in]  number_of_bytes     Number of bytes to write
 * 
 */
extern void at45db041b_write_data(u16_t      page,   
                                  u16_t      start_byte_in_page,
                                  const u8_t *buffer, 
                                  u16_t      number_of_bytes);

/**
 *  Check if Serial Flash is ready for the next read or write access.
 * 
 *  When data is written to Serial Flash, the microcontroller must wait
 *  until the operation is finished, before the next one is attempted. 
 *  This function allows the microcontroller to do something else while 
 *  waiting for the operation to finish.
 * 
 * @retval TRUE     Serial Flash is ready for next read or write access.
 * @retval FALSE    Serial Flash is busy writing.
 */
extern bool_t at45db041b_ready(void);

/**
 *  Read the status register of the Serial Flash.
 * 
 *  The status register contains flags (e.g. #AT45DB041B_STATUS_READY)
 *  and the density value (e.g. #AT45DB041B_DENSITY). This function can be used
 *  to check that a valid and working Serial Flash device is connected.
 * 
 *  @return u8_t Status register value
 */
extern u8_t at45db041b_get_status(void);

/* _____MACROS_______________________________________________________________ */
/**
 *  @}
 */
#endif
