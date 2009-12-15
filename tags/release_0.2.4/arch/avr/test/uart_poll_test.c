#include "uart_poll.h"

void uart_poll_test(void)
{
    u8_t data;
 
    // Initialise module
    uart_poll_init();

    for(;;)
    {
        // Wait until a byte is received
        if(uart_poll_rx_byte(&data))            
        {
            // Send back received byte (loopback/echo mode)
            uart_poll_tx_byte(data);
        }
    }
}
