/* =============================================================================

    Copyright (c) 2009 Pieter Conradie <pieterconradie@users.berlios.de>
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
    
    Title:          ARM AT91 LED Blink example
    Author(s):      Pieter.Conradie
    Creation Date:  2009-02-17
    Revision Info:  $Id$
 
============================================================================= */

/** 
 * @ingroup ARM_AT91_EXAMPLES
 * @defgroup ARM_AT91_LED_BLINK /led_blink
 * 
 * This example toggles the LED once a second.
 *  
 * It indirectly demonstrates the ARM startup assembly code, board 
 * specific low level initialisation and nested IRQ exception handler. 
 *  
 * The following modules are exercised:
 * - @ref AT91_PITD 
 * - @ref TMR 
 *  
 * The <a href="http://www.atmel.com/dyn/products/tools_card.asp?tool_id=4343"> 
 * AT91 Library Software Package 1.5</a> is needed to compile this example. The 
 * location of the library is specified in the Makefile and must be updated if 
 * neccesary: 
 * @code
 * AT91LIB = ../../../../../../atmel_at91lib
 * @endcode 
 *  
 * The suggested directory structure is the following:
 * @code 
 * |--atmel_at91lib
 * | |--boards
 * | |--components
 * | |--drivers
 * | |--memories
 * | |--peripherals
 * | |--usb
 * | |--utility
 * |--piconomic_fwlib 
 * | |--arch 
 * | | |--arm
 * | | | |--AT91
 * | | | | |--examples
 * | | | | | |--led_blink
 * @endcode 
 *  
 * C Files: 
 * - /piconomic_fwlib/arch/arm/AT91/examples/led_blink/main.c 
 * - /piconomic_fwlib/arch/arm/AT91/examples/led_blink/board.c
 * - /piconomic_fwlib/arch/arm/arm_crt0.S
 * - /piconomic_fwlib/arch/arm/arm.c
 * - /piconomic_fwlib/arch/arm/AT91/arm_irq.S
 * - /piconomic_fwlib/arch/arm/AT91/pitd.c
 * - /piconomic_fwlib/general/tmr.c
 * - /atmel_at91lib/boards/at91sam7s-ek/board_lowlevel.c
 * - /atmel_at91lib/boards/at91sam7s-ek/board_memories.c
 * - /atmel_at91lib/peripherals/aic/aic.c
 * - /atmel_at91lib/peripherals/pit/pit.c
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
#include "pitd.h"
#include "tmr.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// LED is on PIOA, pin 0
#define LED_BASE_PIO    AT91C_BASE_PIOA
#define LED_PIN_O       0

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void led_init  (void);
static void led_on    (void);
static void led_off   (void);
static void led_toggle(void);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void led_init(void)
{
    // Enable the PIO to control the corresponding pin
    LED_BASE_PIO->PIO_PER  = (1<<LED_PIN_O);

    // Enable the output on the I/O line
    LED_BASE_PIO->PIO_OER  = (1<<LED_PIN_O);

    // Set initial state of LED to on
    led_on();    
}

static void led_on(void)
{
    // Set the data to be driven on the I/O line
    LED_BASE_PIO->PIO_CODR = (1<<LED_PIN_O);
}

static void led_off(void)
{
    // Clear the data to be driven on the I/O line
    LED_BASE_PIO->PIO_SODR = (1<<LED_PIN_O);
}

static void led_toggle(void)
{
    // See if LED is off
    if(LED_BASE_PIO->PIO_ODSR & (1<<LED_PIN_O))
    {
        led_on();
    }
    else
    {
        led_off();
    }
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Create a timer object
    tmr_t tmr;

    // Initialise modules
    led_init();
    pit_init();

    // Initialise timer to expire once a second
    tmr_start(&tmr, TMR_TICKS_PER_SEC);

    for(;;)
    {
        // Wait until timer has expired
        while(!tmr_has_expired(&tmr))
        {
            ;
        }
        // Reset timer
        tmr_reset(&tmr);

        // Toggle LED pin
        led_toggle();
    }
}

/* _____LOG__________________________________________________________________ */
/*

 2009-02-17 : Pieter.Conradie
 - First release
   
*/
