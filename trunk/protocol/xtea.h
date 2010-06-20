#ifndef __XTEA_H__
#define __XTEA_H__
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

/** 
 *  @ingroup PROTOCOL
 *  @defgroup XTEA xtea.h : XTEA (eXtended Tiny Encryption Algorithm)
 *
 *  Block cipher encryption/decryption algorithm.
 *  
 *  Files: xtea.h & xtea.c
 *  
 *  @see http://en.wikipedia.org/wiki/XTEA
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 * Initialise 128-bit key for encryption/decryption.
 * 
 * @param key   4 x 32-bit values
 */
extern void xtea_init(const u32_t key[4]);

/**
 * Encrypt 64-bit data with 128-bit key.
 * 
 * @param data  2 x 32-bit values
 */
extern void xtea_encrypt(u32_t data[2]);

/**
 * Decrypt 64-bit data with 128-bit key. 
 *  
 * @param data  2 x 32-bit values 
 */
extern void xtea_decrypt(u32_t data[2]);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
