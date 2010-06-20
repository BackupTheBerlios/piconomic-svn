#include <avr/interrupt.h>

#include "uart0.h"
#include "printf.h"

void printf_test(void)
{
    u8_t counter;
 
    // Initialise modules
    uart0_init();
    printf_init();

    // Enable global interrupts
    sei();
  
    PRINTF("Starting counter...\n");
    for(counter = 0; counter<8; counter++)
    {
        PRINTF("Counter = %d\n", counter);
    }
}
