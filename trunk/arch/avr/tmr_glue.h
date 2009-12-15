#ifndef __TMR_GLUE_H__
#define __TMR_GLUE_H__
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
    
    Title:          Polled software timers glue functionality
    Author(s):      Pieter.Conradie
    Creation Date:  2008/11/17
    Revision Info:  $Id$

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup AVR_TMR_GLUE tmr_glue.h : Polled software timers glue functionality
 *
 *  Linkage between general/tmr.h and avr/pit_tmr1.h
 *  
 *  Files: avr/tmr_glue.h
 * 
 *  @see TMR
 * 
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
#include "pit_tmr1.h"

/* _____DEFINITIONS _________________________________________________________ */
/// The number of timer ticks per second
#define TMR_GLUE_TICKS_PER_SEC  PIT_TICKS_PER_SEC

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition of the tick counter
typedef pit_ticks_t tmr_glue_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
/// Macro to get the current counter value
#define TMR_GLUE_GET_COUNTER() (pit_get_counter())

/**
 *  @}
 */
#endif
