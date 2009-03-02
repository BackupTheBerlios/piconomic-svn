.text                       /* Specify that code goes in text segment */
.code 32                    /* Select ARM instruction set */

.global _start              /* Specify global symbol */
_start:

/* Exception handling vector table */

        b   arm_handler_rst     /* 0x00 : Reset */
        b   arm_handler_und     /* 0x04 : Undefined Instruction */
        b   arm_handler_swi     /* 0x08 : Software Interrupt (SWI) */
        b   arm_handler_pref    /* 0x0C : Prefetch Abort (instruction fetch memory abort) */
        b   arm_handler_data    /* 0x10 : Data Abort (data access memory abort) */
        nop                     /* 0x14 : Reserved */
        b   arm_handler_irq     /* 0x18 : IRQ (interrupt request) */
        b   arm_handler_fiq     /* 0x1C : FIQ (fast interrupt request) */

arm_handler_rst:
        /* Enable PA0 as a PIO pin */
        ldr     r0, AT91C_PIOA_PER
        mov     r1, #1
        str     r1,[r0]
        /* Enable PA0 as an output pin */
        ldr     r0, AT91C_PIOA_OER
        str     r1,[r0]
loop:
        /* Output 0; Enable LED on PA0 */
        ldr     r0, AT91C_PIOA_CODR
        str     r1,[r0]
        /* Output 1; Disable LED on PA0 */
        ldr     r0, AT91C_PIOA_SODR
        str     r1,[r0]
        /* Repeat forever */
        b       loop            

/* Unhandled exeption vectors */
arm_handler_und:
        b       arm_handler_und
arm_handler_swi:
        b       arm_handler_swi
arm_handler_pref:
        b       arm_handler_pref
arm_handler_data:
        b       arm_handler_data
arm_handler_irq:
        b       arm_handler_irq
arm_handler_fiq:
        b       arm_handler_fiq

/* Constants */
AT91C_PIOA_PER:   .word 0xFFFFF400
AT91C_PIOA_OER:   .word 0xFFFFF410
AT91C_PIOA_SODR:  .word 0xFFFFF430
AT91C_PIOA_CODR:  .word 0xFFFFF434

.end
