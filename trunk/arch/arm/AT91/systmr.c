/* =============================================================================

    Copyright (c) 2008 Pieter Conradie [www.piconomic.co.za]
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
    
    Title:          System Timer using the PIT peripheral 
    Author(s):      Pieter Conradie
    Creation Date:  2008/02/11
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <pit/pit.h>
#include <aic/aic.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "systmr.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static volatile systmr_ticks_t systmr_tick_counter;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void systmr_interrupt(void)
{
    u32_t u32Pivr = 0;
    u32_t u32Pisr = 0;

    // Read the PISR
    u32Pisr = PIT_GetStatus() & AT91C_PITC_PITS;
    if (u32Pisr != 0) 
    {
        // Read the PIVR. It acknowledges the interrupt
        u32Pivr = PIT_GetPIVR();

        // Add to counter the number of occurrences of periodic intervals 
        // since the last read of PITD_PIVR
        systmr_tick_counter += (u32Pivr >> 20);
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void systmr_init(void)
{
    // Set counter to 0
    systmr_tick_counter  = 0;    

    // Initialize and enable the PIT
    AT91C_BASE_PITC->PITC_PIMR  = (  AT91C_PITC_PIV 
                                   & (DIV_ROUND(DIV_ROUND(BOARD_MCK,16),SYSTMR_TICKS_PER_SEC)-1));
    AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITEN;    

    // Disable the interrupt on the interrupt controller
    AIC_DisableIT(AT91C_ID_SYS);

    // Configure the AIC for PIT interrupts
    AIC_ConfigureIT(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST | AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, systmr_interrupt);

    // Enable the interrupt on the interrupt controller
    AIC_EnableIT(AT91C_ID_SYS);

    // Enable the interrupt on the pit
    PIT_EnableIT();

    // Enable the pit
    PIT_Enable();    
}

systmr_ticks_t systmr_get_counter(void)
{
    systmr_ticks_t counter;

    // Fetch current time
    counter = systmr_tick_counter;

    return counter;
}

/* _____LOG__________________________________________________________________ */
/*

 2008/02/06 : Pieter.Conradie
 - Created
 
 2010/04/21 : Pieter.Conradie
 - Renamed from pitd to systmr
   
*/
