#ifndef __PIT_TMR1_H__
#define __PIT_TMR1_H__
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
    
    Title:          Periodic Interval Timer using Timer 1
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

=========================================================================== */

/** 
 *  @ingroup AVR
 *  @defgroup PIT_TMR1 pit_tmr1.h : Periodic Interval Timer using Timer 1
 *
 *  Driver to initalise 16-bit TMR1 so that a periodic interrupt is generated.
 *
 *  Files: pit_tmr1.h & pit_tmr1.c
 *
 *  PIT_vInit() configures TMR1 to expire every 1/PIT_TICKS_PER_SEC second and
 *  generate an interrupt during which the internal counter PIT_tsTickCounter is 
 *  incremented. PIT_tsGetCounter() must be called to fetch a copy of 
 *  PIT_tsTickCounter in an interrupt safe way.
 * 
 *  See @ref TMR which builds on @ref PIT_TMR1 to provide multiple software
 *  timers.
 *  
 *  @par Example:
 *  @include tmr_test.c
 * 
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
/// The number of timer ticks per second
#define PIT_TICKS_PER_SEC 100ul

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition of the tick counter
typedef u16_t pit_ticks_t;


/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initalise TMR1 to generate an interrupt every 1/PIT_TICKS_PER_SEC second.
 */ 
extern void pit_init(void);

/**
 *  Fetch counter value atomically (disable interrupt during copy).
 */
extern pit_ticks_t pit_get_counter(void);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
