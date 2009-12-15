#include <avr/interrupt.h>

#include "uart0.h"
#include "printf.h"
#include "rtc_tmr0_32khz.h"

void rtc_test(void)
{
    u8_t sec;

    rtc_time_t rtc_time = 
    {
        .sec   = 0,
        .min   = 0,
        .hour  = 12,
        .day   = 1,
        .month = 4,
        .year = 2007
    };
 
    // Initialise modules
    uart0_init();
    printf_init();
    rtc_init();

    // Enable global interrupts
    sei();

    // Set time and date
    rtc_set_time(&rtc_time);
  
    for(;;)
    {
        // Remember current seconds value
        sec = rtc_time.sec;

        // Fetch new time
        rtc_get_time(&rtc_time);

        // See if a second has elapsed
        if(rtc_time.sec != sec)
        {
            // Display the new date and time
            PRINTF("%04d/%02d/%02d %02dh%02d:%02d\n",
                   rtc_time.year,
                   rtc_time.month,
                   rtc_time.day,
                   rtc_time.hour,
                   rtc_time.min,
                   rtc_time.sec);
        }
    }
}
