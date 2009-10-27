#include <avr/interrupt.h>

#include "uart0.h"
#include "printf.h"
#include "dbg.h"

// See if "dbg.h" is included
#ifdef  DBG

// Explicitely enable debug output for this file
#undef  DBG
#define DBG 1

// Change debug level to display progress, warnings and errors
#undef  DBG_LEVEL
#define DBG_LEVEL DBG_PROG

#endif

void dbg_test(void)
{
    u8_t counter;
 
    // Initialise modules
    uart0_init(115200,8,UART0_NO_PARITY,1);
    printf_init();

    // Enable global interrupts
    sei();

    PRINTF("\nDBG Test\n\n");
  
    DBG_ERR("This is an error msg\n");
    DBG_WARN("This is a warning msg\n");
    DBG_PROG("This is a progress msg\n");

    for(counter = 0; counter<16; counter++)
    {
        DBG_ASSERT(counter <= 7); // Counter may not exceed the value 7
        DBG_LOG(DBG_PROG, "Counter = %d\n", counter);
    }
}
