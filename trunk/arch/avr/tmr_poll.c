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

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "tmr_poll.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void tmr_poll_init(void)
{
    // Initialise 16-bit timer 1 @ CK/1024
    TCCR1A = 0;
    TCCR1B = (1<<CS2)|(0<<CS1)|(1<<CS0);
}

void tmr_poll_start(u16_t start_val)
{
    TCNT1 = start_val;

    // Clear timer overflow flag
    BIT_SET_HI(TIFR, TOV1);
}

bool_t tmr_poll_has_exipred(void)
{
    // See if timer overflow flag is set
    if(BIT_IS_HI(TIFR, TOV1))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void tmr_poll_wait(u16_t start_val)
{
    TCNT1 = start_val;

    // Clear timer overflow flag
    BIT_SET_HI(TIFR, TOV1);

    // Wait until timer overflows
    LOOP_UNTIL_BIT_IS_HI(TIFR, TOV1);   
}

/**
 * @}
 */

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : Pieter Conradie
 - First release
   
*/
