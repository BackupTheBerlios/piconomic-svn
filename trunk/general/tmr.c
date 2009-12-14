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
    
    Title:          Polled software timers
    Author(s):      Pieter Conradie
    Creation Date:  2008/02/11
    Revision Info:  $Id$

============================================================================= */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "tmr.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void tmr_start(tmr_t *tmr, const tmr_ticks_t delay_in_ticks)
{
    // Save delay in case timer is restarted
    tmr->delay_in_ticks = delay_in_ticks;

    // Store start tick
    tmr->start_tick = TMR_GLUE_GET_COUNTER();

    // Set state to indicate that timer has started
    tmr->state = TMR_STARTED;
}

bool_t tmr_has_expired(tmr_t* tmr)
{
    tmr_ticks_t tick;

    // See if timer has been stopped
    if (tmr->state == TMR_STOPPED) return FALSE;

    // See if timer has already expired
    if (tmr->state == TMR_EXPIRED) return TRUE;

    // Fetch current time
    tick = TMR_GLUE_GET_COUNTER();

    // Timer expire test
    if( (tick - tmr->start_tick) < tmr->delay_in_ticks )
    {
        return FALSE;
    }

    // Set state to indicate that timer has expired
    tmr->state = TMR_EXPIRED;

    return TRUE;
}

void tmr_stop(tmr_t *tmr)
{
    // Stop timer
    tmr->state = TMR_STOPPED;
}

void tmr_restart(tmr_t *tmr)
{
    // Store start tick
    tmr->start_tick = TMR_GLUE_GET_COUNTER();

    // Set state to indicate that timer has started
    tmr->state = TMR_STARTED;
}

void tmr_reset(tmr_t *tmr)
{
    // Calculate and store new start tick
    tmr->start_tick += tmr->delay_in_ticks;

    // Set state to indicate that timer has started
    tmr->state = TMR_STARTED;
}

void tmr_wait(const tmr_ticks_t delay_in_ticks)
{
    tmr_t tmr;

    tmr_start(&tmr, delay_in_ticks);

    while (!tmr_has_expired(&tmr))
    {
        ;
    }
}

/* _____LOG__________________________________________________________________ */
/*

 2008-02-11 : Pieter.Conradie
 - Creation
   
*/
