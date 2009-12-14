#ifndef __RTC_TMR0_32KHZ_H__
#define __RTC_TMR0_32KHZ_H__
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

    Title:          Real Time Clock using 32.768 kHz crystal and Timer 0
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */
/** 
 *  @ingroup AVR
 *  @defgroup RTC_TMR0_32KHZ rtc_tmr0_32khz.h : Real Time Clock using 32.768 kHz
 *                                              crystal and Timer 0
 *
 *  This component provides basic time & calender functionality.
 *
 *  Files: rtc_tmr0_32khz.h & rtc_tmr0_32khz.c
 *
 *  Asynchronous 8-bit TMR0 operation is selected to use an external 32.768 kHz
 *  crystal. TMR0 is configured to generate an interrupt once a second, during 
 *  which the date and time is updated.
 *  
 *  @par Example:
 *  @include rtc_tmr0_32khz_test.c
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Structure to store time and date
typedef struct
{
   u8_t  sec;   ///< Seconds: 0 to 59
   u8_t  min;   ///< Minutes: 0 to 59
   u8_t  hour;  ///< Hours:   0 to 23
   u8_t  day;   ///< Days:    1 to 31 (depending on month)
   u8_t  month; ///< Months:  1 to 12
   u16_t year;  ///< Years:   1900 to 2099 (arbitrary)
} rtc_time_t;

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initalises Timer 0 to use an external 32.768 kHz crystal.
 * 
 *  Timer 0 is configured to generate an interrupt once a second,
 *  during which the date and time is updated.
 * 
 */
extern void rtc_init(void);

/**
 *  Get a copy of the time and date in an interrupt safe way.
 *
 *  @param[out] time Pointer to a structure that will hold the new date and time.
 * 
 */
extern void rtc_get_time(rtc_time_t *time);

/**
 *  Set the time and date in an interrupt safe way.
 *
 * @param[in] time Pointer to a structure that contains the new date and time.
 * 
 */
extern void rtc_set_time(const rtc_time_t *time);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
