#ifndef __RTC_M41T00_H__
#define __RTC_M41T00_H__
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

    Title:          Driver for an ST M41T00 Real Time Clock
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: rtc_M41T00.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */
/** 
 *  @ingroup DRIVERS
 *  @defgroup M41T00 rtc_M41T00.h : Driver for an ST M41T00 Real Time Clock
 *
 *  This driver interface with an ST M41T00 RTC using @ref TWI_MASTER
 *
 *  Files: rtc_m41t00.h & rtc_m41t00.c
 *
 *  @par Reference:
 *  ST M41T00M6E Serial real-time clock December 2006.pdf
 *  
 *  @par Example:
 *  @include rtc_M41T00_test.c
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Structure to store time and date
typedef struct
{
   u8_t sec;            ///< Seconds:       0  to 59
   u8_t min;            ///< Minutes:       0  to 59
   u8_t hour;           ///< Hours:         0  to 23
   u8_t day_of_week;    ///< Day of Week:   1  to 7
   u8_t day_of_month;   ///< Days of Month: 1  to 31 (depending on month)
   u8_t month;          ///< Months:        1  to 12
   u8_t year;           ///< Years:         00 to 99
} m41t00_time_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Get RTC time using TWI driver.
 * 
 *  @param[out] rtc_time   Pointer to a structure that will contain the new time.
 * 
 *  @retval TRUE    Time succesfully retrieved and copied into structure.
 *  @retval FALSE   Unable to get the time (TWI communication error) 
 *                  or timer has been stopped (new battery inserted)
 */
extern bool_t m41t00_get_time(m41t00_time_t *rtc_time);

/**
 *  Set RTC time using TWI driver.
 * 
 *  @param[in] rtc_time   Pointer to a structure containing the new time.
 * 
 *  @retval TRUE    Time succesfully set
 *  @retval FALSE   Unable to set the time (TWI communication error)
 */
extern bool_t m41t00_set_time(const m41t00_time_t* rtc_time);

/* _____MACROS_______________________________________________________________ */
/**
 *  @}
 */
#endif
