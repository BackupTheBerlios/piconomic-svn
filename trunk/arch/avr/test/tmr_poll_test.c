#include "tmr_poll.h"

void tmr_poll_test(void)
{
    // Initialise PIO
    BIT_SET_HI(PORT_LED_O, BIT_LED_O);
    BIT_SET_HI(DDR_LED_O,  BIT_LED_O);
 
    // Initialise module
    tmr_poll_init();
  
    // Start timer with a 1s timeout
    tmr_poll_start(TMR_POLL_MS_TO_START_VAL(1000));
    
    for(;;)
    {
        // Wait until timer has expired
        while(!tmr_poll_has_exipred())
        {
            ;
        }

        // Restart timer
        tmr_poll_start(TMR_POLL_MS_TO_START_VAL(1000));
        
        // Toggle LED
        LED_TOGGLE();
    }
}
