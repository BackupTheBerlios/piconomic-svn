#ifndef __TMR_POLL_H__
#define __TMR_POLL_H__
/* =============================================================================

    Copyright (c) 2006 Pieter Conradie [www.piconomic.co.za]
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
    
    Title:          Polled timeout functionality using 16-bit TMR1
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup TMR_POLL tmr_poll.h : Polled timeout functionality using 16-bit TMR1
 *
 *  This driver provides polled timing facilities using 16-bit timer 1 @ CK/1024
 *  
 *  Files: tmr_poll.h & tmr_poll.c
 *  
 *  TMR1 is initialised to count up from 0. When the counter overflows 
 *  the TOV1 flag is set. This flag indicates that the timer has expired.
 *  TCNT1 is initialised with a calculated starting value to set the required
 *  delay. A macro #TMR_POLL_MS_TO_START_VAL is provided to calculate the value
 *  during compile time.
 *  
 *  @note   A 16-bit timer @ 7.3728MHz/1024 will expire in 9.1 seconds
 *
 *  @par Example:
 *  @include tmr_poll_test.c
 * 
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise TMR1 @@ CK/1024
 */
extern void tmr_poll_init(void);

/**
 *  Set counter value so that overflow flag is set after the specified delay.
 * 
 * @param start_val  starting value to set the required delay when counter overflows
 */
extern void tmr_poll_start(u16_t start_val);

/**
 *  See if the timer counter has rolled over to zero by testing overflow flag.
 * 
 *  @retval TRUE    timer has expired
 *  @retval FALSE   timer has not expired
 */
extern bool_t tmr_poll_has_exipred(void);

/**
 *  Block for a specified delay.
 * 
 * @param start_val  starting value to set the required delay when counter overflows
 * 
 */
extern void tmr_poll_wait(u16_t start_val);

/* _____MACROS_______________________________________________________________ */
/** 
 *  Macro used to convert a timeout in milliseconds to a 16-bit counter start value.
 *
 *  @param[in] time_ms  Delay in milliseconds
 *  @return    u16_t    16-bit counter start value
 */
#define TMR_POLL_MS_TO_START_VAL(time_ms) (u16_t)((u32_t)0x10000 - \
                                      ((u32_t)time_ms*(F_CPU/1024ul)/1000ul))

/**
 *  @}
 */

#endif
