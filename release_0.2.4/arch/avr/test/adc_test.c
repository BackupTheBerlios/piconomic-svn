#include <avr/interrupt.h>

#include "adc.h"
#include "uart0.h"
#include "printf.h"

void adc_test(void)
{
    u16_t adc_val;
 
    // Initialise modules
    adc_init();
    uart0_init();
    printf_init();

    // Enable global interrupts
    sei();
  
    // Select ADC0
    adc_sel_channel(0);
   
    // Sample channel
    adc_val = adc_get_sample();

    PRINTF("\nADC Value = %d -> Voltage = %2.2f V\n", 
           adc_val,
           ((float)adc_val)*(3.3/1023.0));
}
