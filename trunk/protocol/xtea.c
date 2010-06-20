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
    
    Title:          XTEA (eXtended Tiny Encryption Algorithm)
    Author(s):      Pieter.Conradie
    Creation Date:  2010/05/03
    Revision Info:  $Id: $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "xtea.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#ifndef XTEA_NUMBER_OF_ROUNDS
#define XTEA_NUMBER_OF_ROUNDS 16
#endif

#define XTEA_DELTA            0x9E3779B9

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static u32_t xtea_key[4];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void xtea_init(const u32_t key[4])
{
    u8_t i;

    for(i=0;i<4;i++)
    {
        xtea_key[i] = key[i];
    }
}

void xtea_encrypt(u32_t data[2])
{
    u8_t i;

    u32_t d0  = data[0];
    u32_t d1  = data[1];
    u32_t sum = 0;

    for (i=XTEA_NUMBER_OF_ROUNDS; i != 0; i--)
    {
        d0  += (((d1 << 4) ^ (d1 >> 5)) + d1) ^ (sum + xtea_key[sum & 3]);
        sum += XTEA_DELTA;
        d1  += (((d0 << 4) ^ (d0 >> 5)) + d0) ^ (sum + xtea_key[(sum>>11) & 3]);
    }
    data[0] = d0;
    data[1] = d1;
}

void xtea_decrypt(u32_t data[2])
{
    u8_t i;

    u32_t d0  = data[0];
    u32_t d1  = data[1];
    u32_t sum = XTEA_DELTA*XTEA_NUMBER_OF_ROUNDS;

    for (i=XTEA_NUMBER_OF_ROUNDS; i != 0; i--)
    {
        d1  -= (((d0 << 4) ^ (d0 >> 5)) + d0) ^ (sum + xtea_key[(sum>>11) & 3]);
        sum -= XTEA_DELTA;
        d0  -= (((d1 << 4) ^ (d1 >> 5)) + d1) ^ (sum + xtea_key[sum & 3]);
    }
    data[0] = d0;
    data[1] = d1;
}


/* _____LOG__________________________________________________________________ */
/*

 2010/05/03 : Pieter.Conradie
 - Created
   
*/
