/* =============================================================================

    Copyright (c) 2010 Pieter Conradie [www.piconomic.co.za]
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
    
    * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
    
    * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
   
    Title:          Unknown Microchip PIC24FGA002 board
    Author(s):      Pieter Conradie
    Creation Date:  2010/03/29
    Revision Info:  $Id $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

#include <PwrMgnt.h>
#include <PPS.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "board.h"
#ifndef BOARD_UNKNOWN_PIC24FGA002
#error "Wrong 'board.h' file included. Check include directories"
#endif
#include "uart1.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#warning "Unknown Microchip PIC24FGA002 Board Support Package"

#ifndef __PIC24FJ64GA002__
#warning "This board is supposed to use a PIC24FJ64GA002"
#endif

/* 
 * Set device configuration register 1 
 */ 
_CONFIG1(  JTAGEN_OFF       /* JTAG port is disabled */
         & GCP_OFF          /* Code protection is disabled */
         & GWRP_OFF         /* Writes to program memory are disabled */
         & BKBUG_OFF        /* Device resets into Operational mode (not debug) */
         & COE_OFF          /* Legacy bit; reserved now to be 1 */
         & ICS_PGx2         /* Emulator EMUC2/EMUD2 pins are shared with PGC2/PGD2 */
         & FWDTEN_OFF       /* Watchdog Timer is disabled */
         & WINDIS_OFF       /* Standard Watchdog Timer enabled (not Windowed) */
         & FWPSA_PR32       /* WDT Prescaler ratio of 1:32 */
         & WDTPS_PS32768);  /* Watchdog Timer Postscaler 1:32,768 */

/* 
 * Set device configuration register 2 
 */ 
_CONFIG2(  IESO_ON          /* IESO mode (Two-Speed Start-up) enabled */
         & FNOSC_FRCPLL     /* Fast RC Oscillator with postscaler and PLL module */
#if BOARD_CLK_SWITCHING_ENABLED
         & FCKSM_CSECME     /* Clock switching is enabled, Fail-Safe Clock Monitor is enabled */
#else
         & FCKSM_CSDCMD     /* Clock switching and Fail-Safe Clock Monitor are disabled */
#endif
         & OSCIOFNC_ON      /* OSCO/CLKO/RA3 functions as port I/O (RA3) */
         & IOL1WAY_OFF      /* The OSCCON<IOLOCK> bit can be set and cleared as needed, provided the unlock sequence has been completed */
         & I2C1SEL_PRI      /* Use default SCL1/SDA1 pins */
         & POSCMOD_NONE);   /* Primary oscillator disabled */

/* _____MACROS_______________________________________________________________ */
#ifdef __DEBUG
/** 
 *  Special pre-prologue assembler code to: 
 *  Store stack pointer (W15) in "board_w15"
 *  Store SR and return address (bits 22..16) in "board_sr_7_0_pc_22_16"
 *  Store return address (bits 22..16)in "board_pc_15_0"
 *  
 *  Note: this code does not attempt to save clobbered register W0, as interrupt
 *  does not return.
 */
#define DBG_ISR __attribute__((__interrupt__(__preprologue__( \
                "mov    w15, _board_w15 \n" \
                "mov    [w15-2], w0 \n" \
                "mov    w0, _board_sr_7_0_pc_22_16\n" \
                "mov    [w15-4], w0 \n" \
                "mov    w0, _board_pc_15_0\n")) \
                , no_auto_psv))
#endif

/* _____GLOBAL VARIABLES_____________________________________________________ */
#ifdef __DEBUG
volatile u16_t board_w15;
volatile u16_t board_sr_7_0_pc_22_16;
volatile u16_t board_pc_15_0;
#endif

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
#ifdef __DEBUG
void DBG_ISR _OscillatorFail(void)
{
    INTCON1bits.OSCFAIL = 0;
    while(1);
}

void DBG_ISR _AddressError(void)
{
    INTCON1bits.ADDRERR = 0;
    while(1);
}

void DBG_ISR _StackError(void)
{
    INTCON1bits.STKERR = 0;
    while(1);
}

void DBG_ISR _MathError(void)
{
    INTCON1bits.MATHERR = 0;
    while(1);
}
#endif

static void board_peripheral_pin_select()
{
    // Disable I/O Lock (Peripheral pins can be mapped to PIO pins)
    PPSUnLock;
    
    // Map RP3 to UART1 Transmit (U1TX)
    PPSOutput(PPS_RP3, PPS_U1TX);
    // Map RP2 to UART1 Receive (U1RX)
    PPSInput(PPS_U1RX, PPS_RP2);

    // Map RP1 to UART2 Transmit (U2TX)
    PPSOutput(PPS_RP1, PPS_U2TX);
    // Map RP0 to UART2 Receive (U2RX)
    PPSInput(PPS_U2RX, PPS_RP0);

    // Map RP6 to SPI1 Data Input
    PPSInput(PPS_SDI1, PPS_RP6);
    // Map RP8 to SPI1 Clock Output
    PPSOutput(PPS_RP8, PPS_SCK1OUT);
    // Map RP9 to SPI1 Data Output
    PPSOutput(PPS_RP9, PPS_SDO1);

    // Map RP12 to External Interrupt 1 (IRQ1)
    PPSInput(PPS_INT1, PPS_RP12);

    // Enable I/O Lock
    PPSLock;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void board_lowlevel_init(void)
{
#if __DEBUG
    // Reset "reasons for reset" flags
    RCONbits.POR   = 0; // Power-on Reset
    RCONbits.BOR   = 0; // Brown-out Reset
    RCONbits.WDTO  = 0; // Watchdog Timer Time-out
    RCONbits.SWR   = 0; // Software Reset
    RCONbits.EXTR  = 0; // External Reset
    RCONbits.CM    = 0; // Configuration Word Mismatch Reset
    RCONbits.TRAPR = 0; // Trap Conflict Reset
#endif

#if BOARD_CLK_SWITCHING_ENABLED
    /* 
     * Select Fast RC oscillator with PLL. 
     *  
     * During a /MCLR, WDTO or SWR reset, the oscillator selection is not reset 
     * to the default value as specified in Configuration Word 2 (_CONFIG2(...). 
     * Thus it must be explicitely reset in case the firmware changed the 
     * oscillator during execution.
     */
    PwrMgnt_OscSel(FRC_OSC_WITH_POSTSCALER_PLL);
#endif
	
    // Explicitely disable watchdog, because it may have been enabled in bootloader
    RCONbits.SWDTEN = 0;

    /* 
     * Explicitely enable interrupt nesting, because it may be disabled in
     * the bootloader.
     */
    INTCON1bits.NSTDIS = 0;

    /* 
     * Fix CPU peripheral clock ratio to 1:1
     * Set FRC Postscaler to 8 MHz (divide by 1)
     */
    CLKDIV = 0x0000;
    
    // Wait until PLL has lock
    while(OSCCONbits.LOCK != 1)
    {
        ;
    }

    /* 
     * Configure all analog pins to digital mode; I/O port read enabled
     * Disable Bandgap reference voltage
     */
    AD1PCFG = 0xffff;    

    // Set Port A initial state
    TRISA =0x0010;	  //0000 0000 0001 0000  1=input, 0=output
    LATA = 0x0009;	  //0000 0000 0000 1001

    // Set Port B initial state
    TRISB =0x90d5;    //1001 0000 1101 0101  1=input, 0=output
    LATB = 0x602a;	  //0110 0000 0010 1010

    // Map peripheral functions to PIO pins
    board_peripheral_pin_select();
}

void board_flash_cs_lo(void)
{
    FLASH_CS_LO();
    NOP(); /* Delay for minimum CS setup time */
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
}

void board_flash_cs_hi(void)
{
    NOP();  /* Delay for minimum CS hold time */
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    FLASH_CS_HI();
    NOP(); /* Delay for minimum CS high time */
    NOP();
}

bool_t board_get_char(char *data)
{
    // Direct to UART1
    return uart1_get_rx_byte((u8_t *)data);
}

void board_put_char(const char data)
{
    // Direct to UART1
    while(!uart1_tx_byte((u8_t)data))
    {
        ;
    }
}

int write(int handle, void *buffer, unsigned int len)
{
    // Redirect stdout stream to UART1
    char data;
    int  bytes_sent = 0;

	switch (handle)
	{
	case 0:
	case 1:
    case 2:
        while(len != 0)
        {
            data = *(char *)buffer++;

            // See if data contains a Line Feed character
            if(data == '\n')
            {
                // Inject a Carriage Return character
                board_put_char('\r');
            }
            board_put_char(data);

            bytes_sent++;            
            len--;
        }
		break;
	default:
		break;
	}

    return bytes_sent;
}

/* _____LOG__________________________________________________________________ */
/*

 2010/03/29 : Pieter.Conradie
 - Created
  
*/
