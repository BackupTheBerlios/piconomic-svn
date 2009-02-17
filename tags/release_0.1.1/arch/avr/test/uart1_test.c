#include <avr/interrupt.h>

#include "uart1.h"

void uart1_test(void)
{
    u8_t data;
 
    // Initialise module
    uart1_init();

    // Enable global interrupts
    sei();
  
    for(;;)
    {
        // Wait until a byte is received
        if(uart1_get_rx_byte(&data))
        {
            // Send back received byte (loopback/echo mode)
            uart1_tx_byte(data);
        }
    }
}
