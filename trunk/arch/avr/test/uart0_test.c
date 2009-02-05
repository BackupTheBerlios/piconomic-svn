#include <avr/interrupt.h>

#include "uart0.h"

void uart0_test(void)
{
    u8_t data;
 
    // Initialise module
    uart0_init();

    // Enable global interrupts
    sei();
  
    for(;;)
    {
        // Wait until a byte is received
        if(uart0_get_rx_byte(&data))
        {
            // Send back received byte (loopback/echo mode)
            uart0_tx_byte(data);
        }
    }
}
