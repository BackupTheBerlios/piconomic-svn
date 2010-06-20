/* =============================================================================

    Copyright (c) 2006 Pieter Conradie [www.piconomic.co.za]
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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "VT100.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#define VT100_ASCII_ESC 0x1B

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Function to call to output a character
static vt100_put_char_t vt100_put_char;

static u8_t vt100_esc_state;

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void vt100_send_array(const char* data, u8_t length)
{
    while(length != 0)
    {
        (*vt100_put_char)(*data++);
        length--;
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void vt100_init(vt100_put_char_t put_char)
{
    const char vt100_cmd_rst[]       = {VT100_ASCII_ESC,'c'};
    const char vt100_cmd_line_wrap[] = {VT100_ASCII_ESC,'[','7','h'};

    vt100_put_char  = put_char;
    vt100_esc_state = 0;

    // Reset
    vt100_send_array(vt100_cmd_rst,ARRAY_LENGTH(vt100_cmd_rst));

    // Enable line wrap
    vt100_send_array(vt100_cmd_line_wrap,ARRAY_LENGTH(vt100_cmd_line_wrap));
}

char vt100_process_rx_char(char data)
{
    switch(vt100_esc_state)
    {
    case 0:
        if(data == VT100_ASCII_ESC)
        {
            // Escape sequence detected
            vt100_esc_state++;
            // Indicate that received character should be ignored
            return VT100_NONE;
        }
        else
        {
            // Normal character received
            return data;
        }
    case 1:
        if(data == '[')
        {
            // Escape sequence detected
            vt100_esc_state++;
            // Indicate that received character should be ignored
            return VT100_NONE;
        }
        else
        {
            // Incorrect escape sequence
            vt100_esc_state = 0;
            // Indicate that received character should be ignored
            return VT100_NONE;
        }        
    case 2:
        if(data == 'A')
        {
            return VT100_ARROW_UP;
        }
        else if(data == 'B')
        {
            return VT100_ARROW_DN;
        }
        else if(data == 'C')
        {
            return VT100_ARROW_RIGHT;
        }
        else if(data == 'D')
        {
            return VT100_ARROW_LEFT;
        }
        // Fall through to default case...
    default:
        vt100_esc_state = 0;
        // Indicate that received character should be ignored
        return VT100_NONE;
    }
}

void vt100_clear_screen(void)
{
    const char vt100_cmd[] = {VT100_ASCII_ESC,'[','2','J'};

    vt100_send_array(vt100_cmd,ARRAY_LENGTH(vt100_cmd));
}

void vt100_erase_line(void)
{
    const char vt100_cmd[] = {VT100_ASCII_ESC,'[','2','K'};

    vt100_send_array(vt100_cmd,ARRAY_LENGTH(vt100_cmd));
}

/* _____LOG__________________________________________________________________ */
/*

 2008/08/04 : Pieter.Conradie
 - Created
   
*/
