/* 
 * References:
 *
 * [1] "RM0006 - Reference manual STR91xFA ARM9®-based microcontroller family", ST
 *     http://www.st.com/stonline/products/literature/rm/13742.htm
 *
 * [2] "ARM DDI0213E ARM966E-S r2p1 Technical Reference Manual", ARM
 *      http://infocenter.arm.com/help/topic/com.arm.doc.ddi0213e/ARM966E-S_TRM.pdf
 *
 * [3] "STR91xFA Errata sheet - STR91xFA limitations and corrections", ST
 *     www.st.com/stonline/products/literature/es/12944.pdf
 */

#include "board.h"

/*
 * Convenience macro for assembler NOP.
 */
#define ARM_NOP() \
    __asm__ __volatile__("nop \n\t" : : );

void LowLevelInit(void)
{
#ifdef DEBUG
    /**
     * If DEBUG is defined in "91xconf.h", the peripheral registers are declared as 
     * "extern" in "91x_map.h" and initialised to correct values in the function 
     * "debug()" in "91x_lib.c".
     *
     */
    debug();
#endif

    /* "Buffered" is defined in "91xconf.h" */
#ifdef Buffered
#warning "Write Buffer is enabled on AHB and the D-TCM"
    __asm__ __volatile__(
    "mrc		p15, 0, r0, c1, c0, 0 \n\t" /* Read CP15 c1, Control Register       */
    "orr		r0, r0, #0x8          \n\t" /* Enable BIU write buffer on AMBA AHB  */
	"mcr		p15, 0, r0, c1, c0, 0 \n\t" /* Write CP15 c1, Control Register      */
                         : : : "r0");
#endif

    /* 
     * Map Flash Bank 0 at address 0x0 and Bank 1 at address 0x80000,
     * when the bank 0 is the boot bank, then enable the Bank 1.
     */
    FMI->BBSR   = 4;             /* Set Boot bank size to 512KB                    */
    FMI->NBBSR  = 2;             /* Set Non boot bank size to 32KB                 */
    FMI->BBADR  = 0x00000000>>2; /* Set Boot bank base address to 0x00000000       */
    FMI->NBBADR = 0x00080000>>2; /* Set Non boot bank base address to 0x00080000   */
    FMI->CR     = 0x18;          /* Enable CS of Flash Boot Bank and Non Boot Bank */

    /*
     * Enable SRAM.
     *
     * Set System configuration register to 96K SRAM size, DTCM & AHB wait-states disabled.
     */
    SCU->SCR0  = 0x0191;

#if 0
    /* 
     * Write a Write Flash Configuration Register command (0x60) to any word
     * address in Bank 1.
     */
    *((volatile u16 *)0x00080000) = 0x60;

    /* 
     * Write a Write Flash Configuration Register Confirm command (0x03)
     * 2 Wait states in read, PWD, LVD enabled, High BUSCFG
     */
    *((volatile u16 *)0x00083040) = 0x03;

    /* Select OSC as clock source */
    SCU->CLKCNTR = 0x00020002;

    /* Wait for OSC to stabilise */
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();
    ARM_NOP();

    /* Enable PLL to 96 MHz */
    SCU->PLLCONF = 0x000ac019;

    /* Wait until PLL is Locked */
    while(!(SCU->SYSSTATUS & 0x01))
    {
        ;
    }

    /* Select PLL as clock source after PLL has locked and FMICLK=RCLK, PCLK=RCLK/2 */
    SCU->CLKCNTR = 0x00020080;
#endif
    
    /* 
     * See [4] § 2.1 "Flash memory status register bit 7"
     * See [2] § 2.3.8 "Configuration Control Register"
     * Set bits 17 (Data TCM order bit) and bit 18 (Instruction TCM order bit) 
     * in the Configuration Control Register.
     */
    __asm__ __volatile__(
    "mov		r0, #0x60000           \n\t"
    "mcr		p15, 1, r0, c15, c1, 0 \n\t"
                         : : : "r0");
}
