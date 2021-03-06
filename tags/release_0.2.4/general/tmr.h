#ifndef __TMR_H__
#define __TMR_H__
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

=========================================================================== */

/** 
 *  @ingroup GENERAL
 *  @defgroup TMR tmr.h : Polled software timers
 *
 *  Non-blocking software timers that have to be polled to determine 
 *  if they have expired.
 *  
 *  Files: tmr.h & tmr.c
 *  
 *  @par
 *  These timers are suitable for applications that are not timing critical.
 *  A counter is incremented with each timer tick and this value is 
 *  polled to determine when a timer has expired. Provision is made for counter 
 *  roll-over.
 *  
 *  @par
 *  An architecture specific timer support file must be provided (tmr_glue.h)
 *  that defines a macro to return the current counter value
 *  (#TMR_GLUE_GET_COUNTER) and the type returned (tmr_glue_t). In addition
 *  the number of ticks per second must also be defined
 *  (#TMR_GLUE_TICKS_PER_SEC).
 * 
 *  @see TMR_GLUE
 *  
 *  Example:
 *  @include test/tmr_test.c
 * 
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
#include "tmr_glue.h"

/* _____DEFINITIONS _________________________________________________________ */
#ifndef TMR_GLUE_GET_COUNTER
#error "A macro that returns the current counter value must be defined in 'timer_glue.h'"
#endif

#ifndef TMR_GLUE_TICKS_PER_SEC
#error "TMR_GLUE_TICKS_PER_SEC must be defined in 'timer_glue.h'"
#endif

/// The number of timer ticks per second
#define TMR_TICKS_PER_SEC   TMR_GLUE_TICKS_PER_SEC

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition of the tick counter
typedef tmr_glue_t tmr_ticks_t;

/// Timer state
typedef enum
{
    TMR_STOPPED = 0,
    TMR_STARTED,
    TMR_EXPIRED,
} tmr_state_t;

/// Structure to track state of a timer
typedef struct
{
    tmr_state_t state;           ///< State of timer: STOPPED, STARTED or EXPIRED
    tmr_ticks_t start_tick;      ///< Tick when timer started
    tmr_ticks_t delay_in_ticks;  ///< Timer delay
} tmr_t;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Start a timer.
 *
 *  @param[in,out]  tmr            Pointer to a timer object
 *  @param[in]      delay_in_ticks Delay in timer ticks
 */
extern void tmr_start(tmr_t *tmr,const tmr_ticks_t delay_in_ticks);


/** 
 *  See if a timer has expired.
 *  
 *  @param[in,out]  tmr   Pointer to a timer object
 *  
 *  @retval TRUE    timer expired
 *  @retval FALSE   timer not expired or timer stopped
 */
extern bool_t tmr_has_expired(tmr_t *tmr);

/** 
 *  Stop a running timer.
 *  
 *  @param[in,out] tmr    Pointer to a timer object
 */
extern void tmr_stop(tmr_t *tmr);

/** 
 *  Restart a timer with the delay set with tmr_start().
 * 
 *  The timer will expire from the current timer tick + tmr->delay_in_ticks.
 *  
 *  @param[in,out]  tmr   Pointer to a timer object
 */    
extern void tmr_restart(tmr_t *tmr);

/** 
 *  Reset a timer with the delay set with tmr_start().
 * 
 *  The timer will expire on tmr->start_tick + tmr->delay_in_ticks.
 * 
 *  Use this function in stead of tmr_restart() for periodic timers, because 
 *  the frequency will not drift over time. An error may accumulate if there
 *  is a delay between tmr_has_expired() and tmr_restart(). Thus the prefered
 *  usage for a periodic timer is:
 *  
 *  @code
 *  for(;;)
 *  {
 *      // Wait until timer has expired
 *      while(tmr_has_expired(&tmr) == FALSE)
 *      {
 *          ;
 *      }
 *      // Reset periodic timer
 *      tmr_reset(&tmr);
 *      // Do something...
 *  }
 *  @endcode
 *  
 *  @param[in,out]  tmr   Pointer to a timer object
 */    
extern void tmr_reset(tmr_t *tmr);

/** 
 *  Blocking wait for specified number of ticks.
 *  
 *  @param[in]  delay_in_ticks    Delay in timer ticks
 */ 
extern void tmr_wait(const tmr_ticks_t delay_in_ticks);

/* _____MACROS_______________________________________________________________ */
/** 
 *  Macro used to convert a timeout in milliseconds to timer ticks
 *  @param[in] delay_in_ms Delay in milliseconds
 *  @return    Delay in timer ticks
 */
#define TMR_MS_TO_TICKS(delay_in_ms)    \
     DIV_ROUND(delay_in_ms*TMR_TICKS_PER_SEC,1000ul)

/**
 *  @}
 */
#endif
