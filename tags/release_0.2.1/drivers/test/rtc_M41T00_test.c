#include <avr/interrupt.h>

#include "uart0.h"
#include "printf.h"
#include "twi_master.h"
#include "rtc_M41T00.h"

void M41T00_vTest(void)
{
    u8_t u8Sec;

    m41t00_time_t xTime = 
    {
        .sec        = 0,
        .min        = 0,
        .hour       = 12,
        .day_of_week  = 1,
        .day_of_month = 1,
        .month      = 4,
        .year       = 7
    };

    // Initialise modules
    twi_init();
    UART0_vInit();
    printf_init();

    // Enable global interrupts
    sei();

    // Set time and date
    if(!m41t00_set_time(&xTime))
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
        u8Sec = xTime.sec;
        // Fetch new time
        if(!m41t00_get_time(&xTime))
        {
            PRINTF("Unable to set time!\n");
            for(;;)
            {
                ;
            }
        }
        // See if a second has elapsed
        if(xTime.sec != u8Sec)
        {
            // Display the new date and time
            PRINTF("%02d/%02d/%02d %02dh%02d:%02d\n",
                   xTime.year,
                   xTime.month,
                   xTime.day_of_month,
                   xTime.hour,
                   xTime.min,
                   xTime.sec);
        }
    }    
}
