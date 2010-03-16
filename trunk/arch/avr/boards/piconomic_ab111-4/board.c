/* =============================================================================

    Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>
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
    
    Title:          Piconomic Design AB111-4 ATmega128 board
    Author(s):      Pieter.Conradie
    Creation Date:  2008/11/01
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "board.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void board_lowlevel_init(void)
{
    // Initialise Programmable IO pins
    PORTB =   (1<<BIT_SPI_SS) 
            | (1<<BIT_SPI_SCK)
            | (1<<BIT_SPI_MOSI)
            | (0<<BIT_SPI_MISO)
            | (1<<BIT_SFLASH_CS_O)
            | (1<<BIT_SW_IP)
            | (0<<BIT_LED_O)
            | (1<<BIT_BUZZER_O);

    DDRB  =   (0<<BIT_SPI_SS) 
            | (1<<BIT_SPI_SCK)
            | (1<<BIT_SPI_MOSI)
            | (0<<BIT_SPI_MISO)
            | (1<<BIT_SFLASH_CS_O)
            | (0<<BIT_SW_IP)
            | (1<<BIT_LED_O)
            | (1<<BIT_BUZZER_O);

    PORTD =   (0<<BIT_RS485_TX_EN_O)
            | (1<<BIT_RS485_RX_EN_O)
            | (0<<BIT_RTS_I)
            | (0<<BIT_CTS_O);

    DDRD  =   (1<<BIT_RS485_TX_EN_O)
            | (1<<BIT_RS485_RX_EN_O)
            | (0<<BIT_RTS_I)
            | (1<<BIT_CTS_O);    
}

/* _____LOG__________________________________________________________________ */
/*

 2009/02/17 : Pieter.Conradie
 - Created
 
 2010-03-16 : Pieter.Conradie
 - Initialise SPI pins too.
   
*/
