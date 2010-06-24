#include <avr/interrupt.h>

#include "uart0.h"
#include "printf.h"
#include "cmd_line.h"
#include "vt100.h"

static cmd_line_t cmd_line_led;
static cmd_line_t cmd_line_led_on;
static cmd_line_t cmd_line_led_off;

static const char* cmd_line_handler_led(int argc, char* argv[])
{
    if(BIT_IS_HI(PORT_LED_O,BIT_LED_O))
    {
        return "LED is on";
    }
    else
    {
        return "LED is off";
    }
}

static const char* cmd_line_handler_led_on(int argc, char* argv[])
{
    LED_ON();

    return "LED is on";
}

static const char* cmd_line_handler_led_off(int argc, char* argv[])
{
    LED_OFF();

    return "LED is off";
}

static void main_put_char(char data)
{
    while(!uart0_tx_byte((u8_t)data))
    {
        ;
    }
}

void cmd_line_test(void)
{
    // Initialise modules
    uart0_init(115200,8,UART0_NO_PARITY,1);
    printf_init();

    // Enable global interrupts
    sei();    

    // Initialise command line parser and VT100 terminal helper
	vt100_init(&main_put_char);
    cmd_line_init(&main_put_char);

    cmd_line_add      (&cmd_line_led,                  "led",&cmd_line_handler_led,    "display status of led");
    cmd_line_add_child(&cmd_line_led,&cmd_line_led_on, "on", &cmd_line_handler_led_on, "switch led on"        );
    cmd_line_add_child(&cmd_line_led,&cmd_line_led_off,"off",&cmd_line_handler_led_off,"switch led off"       );

    PRINTF("\nCommand Line test\n\n");

    for(;;)
    {
        // Pass all received data to command line parser
        if(uart0_get_rx_byte(&data))
        {
            cmd_line_process(data);
        }        
    }
}
