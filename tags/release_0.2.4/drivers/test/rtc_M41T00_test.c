#include <avr/interrupt.h>

#include "uart0.h"
#include "printf.h"
#include "twi_master.h"
#include "rtc_M41T00.h"

void m41t00_test(void)
{
    u8_t sec;

    m41t00_time_t time = 
    {
        .sec          = 0,
        .min          = 0,
        .hour         = 12,
        .day_of_week  = 1,
        .day_of_month = 1,
        .month        = 4,
        .year         = 7
    };

    // Initialise modules
    twi_init();
    uart0_vInit();
    printf_init();

    // Enable global interrupts
    sei();

    // Set time and date
    if(!m41t00_set_time(&time))
    {
        PRINTF("Unable to set time!\n");
        for(;;)
        {
            ;
        }
    }
  
    for(;;)
    {
        // Remember current seconds value
        sec = time.sec;
        // Fetch new time
        if(!m41t00_get_time(&time))
        {
            PRINTF("Unable to set time!\n");
            for(;;)
            {
                ;
            }
        }
        // See if a second has elapsed
        if(time.sec != sec)
        {
            // Display the new date and time
            PRINTF("%02d/%02d/%02d %02dh%02d:%02d\n",
                   time.year,
                   time.month,
                   time.day_of_month,
                   time.hour,
                   time.min,
                   time.sec);
        }
    }    
}
