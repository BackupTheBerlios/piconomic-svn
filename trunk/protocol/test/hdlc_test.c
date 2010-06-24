#include <avr/interrupt.h>

#include "hdlc.h"
#include "printf.h"
#include "uart0.h"
#include "uart1.h"

static void rs485_put_char(char data)
{
    while(!uart1_tx_byte((u8_t)data))
    {
        ;
    }
}

void hdlc_on_rx_frame(u8_t* buffer, u16_t bytes_received)
{
    PRINTF("\nHDLC RX [%d]: ", bytes_received);
    while(bytes_received != 0)
    {
        PRINTF("%02X ", *buffer++);
        bytes_received--;
    }
    PRINTF("\n");
}

void hdlc_test(void)
{
    u8_t data;

    // Enable RS-485 PIO for loopback operation
    BIT_SET_HI(PORT_RS485_TX_EN_O, BIT_RS485_TX_EN_O);
    BIT_SET_HI(DDR_RS485_TX_EN_O, BIT_RS485_TX_EN_O);

    BIT_SET_LO(PORT_RS485_RX_EN_O, BIT_RS485_RX_EN_O);
    BIT_SET_HI(DDR_RS485_RX_EN_O, BIT_RS485_RX_EN_O);

    // Initialise modules
    uart0_init();
    uart1_init();
    printf_init();
    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);

    // Enable global interrupts
    sei();
        
    // Send an HDLC packet
    data = 0x55;
    hdlc_tx_frame(&data,1);

    // Process received data
    for(;;)
    {
        if(uart1_get_rx_byte(&data))
        {
            PRINTF("%02X ", data);
            // Feed received data to HDLC layer
            hdlc_on_rx_byte(data);
        }
    }
}
