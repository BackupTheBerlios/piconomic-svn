#ifndef __VT100_H__
#define __VT100_H__
/* =============================================================================

    Copyright (c) 2008 Pieter Conradie [www.piconomic.co.za]
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
    
    Title:          VT100 Terminal Support
    Author(s):      Pieter Conradie
    Creation Date:  2008/08/04
    Revision Info:  $Id$

============================================================================= */

/** 
 *  @ingroup PROTOCOL
 *  @defgroup VT100 vt100.h : VT100 terminal helper module
 *
 *  Implements a command line parser using the ANSI/VT100 terminal protocol.
 *  
 *  Files: vt100.h & vt100.c
 *  
 *  @see
 *  - http://en.wikipedia.org/wiki/ANSI_escape_code
 *  - http://www.termsys.demon.co.uk/vtansi.htm
 *  
 *  Example:
 *  @include test/cmd_line_test.c
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/*
 * Definition for a pointer to a function that will be called to 
 * send a character
 */
typedef void (*vt100_put_char_t)(char data);

/// @name Special ASCII values
//@{
#define VT100_NONE          0
#define VT100_BEL           0x07
#define VT100_BS            0x08
#define VT100_TAB           0x09
#define VT100_CR            0x0D
#define VT100_LF            0x0A
#define VT100_ESC           0x1B
#define VT100_DEL           0x7F
#define VT100_ARROW_UP      0x80
#define VT100_ARROW_DN      0x81
#define VT100_ARROW_LEFT    0x82
#define VT100_ARROW_RIGHT   0x83
//@}

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 * Initialise VT100 module.
 * 
 * @param put_char Pointer to a function that will be called to send a character
 */
extern void vt100_init(vt100_put_char_t put_char);

/**
 * Process a received character byte. 
 *  
 * If the special VT100 command sequence is detected, VT100_NONE will be 
 * returned to indicate that received character should be ignored, otherwise 
 * the received character is returned. 
 *  
 * If up, down, left or right arrow sequence is received, it is indicated with 
 * the special ASCII sequence. 
 * 
 * @param data      Received character
 * 
 * @return char     VT100_NONE if character should be ignored, otherwise the 
 *                  received character.
 *  
 */
extern char vt100_process_rx_char(char data);

/**
 * Send 'clear screen' command to terminal
 */
extern void vt100_clear_screen(void);

/**
 * Send 'erase line' command to terminal
 */
extern void vt100_erase_line(void);

/* _____MACROS_______________________________________________________________ */

/**
 * @}
 */
#endif
