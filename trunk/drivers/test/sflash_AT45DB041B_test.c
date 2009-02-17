#include <avr/interrupt.h>

#include "uart0.h"
#include "printf.h"
#include "spi_master.h"
#include "sflash_AT45DB041B.h"

u8_t page[AT45DB041B_PAGE_SIZE];

void at45db041b_test(void)
{
    u8_t  status;
    u16_t counter;

    // Initialise PIO
    BIT_SET_HI(PORT_SFLASH_CS_O, BIT_SFLASH_CS_O);
    BIT_SET_HI(DDR_SFLASH_CS_O,  BIT_SFLASH_CS_O);
 
    // Initialise modules
    spi_init();
    uart0_init();
    printf_init();
    at45db041b_init();

    // Enable global interrupts
    sei();
  
    // See if Serial FLASH is present
    status = at45db041b_get_status();
    if((status & AT45DB041B_DENSITY_MASK) != AT45DB041B_DENSITY)
    {
        PRINTF("Error! AT45DB041B not present. Status = 0x%02X\n", status);
        return;
    }

    // Write a page of Serial FLASH
    for(counter = 0; counter<AT45DB041B_PAGE_SIZE; counter++)
    {
        page[counter] = (u8_t)(counter&0xff);
    }
    at45db041b_write_page(AT45DB041B_PAGES-1, page);

    // Read a page of Serial FLASH
    at45db041b_read_page(AT45DB041B_PAGES-1, page);

    // Report content of page
    for(counter = 0; counter<AT45DB041B_PAGE_SIZE; counter++)
    {
        PRINTF("%02X ", page[counter]);
        if((counter&0x0F) == 0x0F)
        {
            PRINTF("\n");
        }
    }
}
