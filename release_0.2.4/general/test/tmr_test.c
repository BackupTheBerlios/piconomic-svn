#include <avr/interrupt.h>

#include "pit_tmr1.h"
#include "tmr.h"

void tmr_test(void)
{
    // Create a timer object
    tmr_t tmr;

    // Initialise PIO
    BIT_SET_HI(PORT_LED_O, BIT_LED_O);
    BIT_SET_HI(DDR_LED_O,  BIT_LED_O);
 
    // Initialise module
    pit_init();

    // Enable global interrupts
    sei();
  
    // Start timer with a 1s timeout
    tmr_start(&tmr, TMR_MS_TO_TICKS(1000));
    
    for(;;)
    {
        // Wait until timer has expired
        while(!tmr_has_expired(&tmr))
        {
            ;
        }

        // Restart timer
        tmr_restart(&tmr);
        
        // Toggle LED
        LED_TOGGLE();
    }
}
