;* ==========================================================================
;
;  Copyright (c) 2006 Pieter Conradie <pieterconradie@users.berlios.de>
;  All rights reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions are met:
;
;  * Redistributions of source code must retain the above copyright
;    notice, this list of conditions and the following disclaimer.
;
;  * Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in
;    the documentation and/or other materials provided with the
;    distribution.
;
;  * Neither the name of the copyright holders nor the names of
;    contributors may be used to endorse or promote products derived
;    from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
;  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;  POSSIBILITY OF SUCH DAMAGE.
;
;========================================================================= */

; Includes
.include "m128def.inc"

; Definitions
.def REG_TMP 	= R16
.def REG_ZERO 	= R1
.def REG_SREG 	= R0

.equ PORT_LED_O	= PORTB
.equ DDR_LED_O	= DDRB
.equ BIT_LED_O	= 6

; Reset vector
	rjmp	MAIN

; Output Compare 1A interrupt handler
.org OC1Aaddr
	; Save status register
	in	REG_SREG,SREG
	
	; Toggle output
	sbic	PORT_LED_O,BIT_LED_O
	rjmp	LED_OFF
LED_ON:
	sbi	PORT_LED_O,BIT_LED_O
	rjmp	LED_END
LED_OFF:
	cbi	PORT_LED_O,BIT_LED_O
LED_END:

	; Restore status register
	out	SREG,REG_SREG
	reti

; Initialise timer function
TIMER_INIT:
	; Start timer 1 with clock prescaler CLK/1024 and CTC Mode ("Clear Timer on Compare")
	; Resolution is 139 us
	; Maximum time is 9.1 s
	ldi	REG_TMP,(0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(0<<COM1C1)|(0<<COM1C0)|(0<<WGM11)|(0<<WGM10)
	out	TCCR1A,REG_TMP
	ldi	REG_TMP,(0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10);
	out	TCCR1B,REG_TMP

	; Reset time
	out	TCNT1L,REG_ZERO
	out	TCNT1H,REG_ZERO

	; Calculate and set delay
	ldi	REG_TMP,low(1799)		; ((F_CPU/1024)*PERIOD_MS)/1000 - 1
	out	OCR1AL,REG_TMP
	ldi	REG_TMP,high(1799)
	out	OCR1AH,REG_TMP

	; Enable interrupt on compare match
   	in 	REG_TMP,TIMSK
	ori	REG_TMP,(1<<OCIE1A)
	out	TIMSK,REG_TMP
	ret

; Main function loop
MAIN:
	; Initialise stack pointer to end of RAM
	ldi 	REG_TMP,low(RAMEND)
	out	SPL,REG_TMP
	ldi 	REG_TMP,high(RAMEND)
	out 	SPH,REG_TMP

	; Initialise register(s)
	clr	REG_ZERO

	; Set pin as output
	sbi	DDR_LED_O,BIT_LED_O
	cbi	PORT_LED_O,BIT_LED_O

	; Initialise timer
	rcall	TIMER_INIT

	; Enable global interrupts
	sei;

LOOP:
	rjmp	LOOP				;Loop indefinitely


