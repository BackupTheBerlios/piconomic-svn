.text                       /* Specify that code goes in text segment */
.code 32                    /* Select ARM instruction set */

.global _start              /* Specify global symbol */
_start:

/* Exception handling vector table */

        b   handler_rst     /* Reset */
        b   handler_und     /* Undefined Instruction */
        b   handler_swi     /* Software Interrupt */
        b   handler_pref    /* Prefetch Abort */
        b   handler_data    /* Data Abort */
        nop                 /* Reserved */
        b   handler_irq     /* Interrupt Request */
        b   handler_fiq     /* Fast Interrupt Request */

handler_rst:
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
handler_und:
        b       handler_und
handler_swi:
        b       handler_swi
handler_pref:
        b       handler_pref
handler_data:
        b       handler_data
handler_irq:
        b       handler_irq
handler_fiq:
        b       handler_fiq

/* Constants */
AT91C_PIOA_PER:   .word 0xFFFFF400
AT91C_PIOA_OER:   .word 0xFFFFF410
AT91C_PIOA_SODR:  .word 0xFFFFF430
AT91C_PIOA_CODR:  .word 0xFFFFF434

.end
