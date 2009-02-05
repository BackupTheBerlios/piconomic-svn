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

void DBG_vTest(void)
{
    u8_t counter;
 
    // Initialise modules
    uart0_init();
    printf_init();

    // Enable global interrupts
    sei();
  
    for(counter = 0; counter<16; counter++)
    {
        DBG_ASSERT(counter <= 7); // Counter may not exceed the value 7
        DBG_TRACE(DBG_PROG, "Counter = %d\n", counter);
    }
}
