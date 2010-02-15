#include <avr/interrupt.h>

#include "uart0.h"
#include "printf.h"
#include "dbg.h"

// Change debug level to display errors, warnings and progress
#ifdef  DBG_LEVEL
#undef  DBG_LEVEL
#define DBG_LEVEL (DBG_LEVEL_ERR|DBG_LEVEL_WARN|DBG_LEVEL_PROG)
#endif

void dbg_test(void)
{
    u8_t counter;
 
    // Initialise modules
    uart0_init(115200,8,UART0_NO_PARITY,1);
    printf_init();

    // Enable global interrupts
    sei();    

    DBG_TRACE("\nDBG Test\n\n");

    DBG_ERR("This is an error msg\n");
    DBG_WARN("This is a warning msg\n");
    DBG_PROG("This is a progress msg\n");

    for(counter = 0; counter<16; counter++)
    {
        DBG_ASSERT(counter <= 7); // Counter may not exceed the value 7
        DBG_LOG(DBG_LEVEL_PROG, "Counter = %d\n", counter);
    }
}
