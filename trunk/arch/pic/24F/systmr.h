#ifndef __SYSTMR_H__
#define __SYSTMR_H__
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
    
    Title:          System Timer
    Author(s):      Pieter.Conradie
    Creation Date:  2010/04/12
    Revision Info:  $Id: $

============================================================================= */

/** 
 *  @ingroup PIC24F
 *  @defgroup PIC24F_SYSTMR systmr.h : System Timer
 *
 *  Driver to initalise a timer so that a periodic interrupt is generated.
 *
 *  Files: pic\24F\systmr.h & pic\24F\systmr.c
 *
 *  systmr_init() configures a timer peripheral to expire every
 *  1/SYSTMR_TICKS_PER_SEC second and generate an interrupt during which an
 *  internal counter is incremented. systmr_get_counter() must be called to
 *  fetch a copy in an interrupt safe way.
 * 
 *  See @ref TMR which builds on @ref PIC24F_SYSTMR to provide multiple software
 *  timers.
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
#ifndef SYSTMR_TICKS_PER_SEC
/// The number of ticks per second
#define SYSTMR_TICKS_PER_SEC 1000ul
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition of the tick counter
typedef u16_t systmr_ticks_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initalise timer to generate an interrupt every 1/SYSTMR_TICKS_PER_SEC second.
 */ 
extern void systmr_init(void);

/**
 *  Fetch counter value atomically (disable interrupt during copy if required).
 */
extern systmr_ticks_t systmr_get_tick_counter(void);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
