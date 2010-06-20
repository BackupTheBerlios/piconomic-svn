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
    
    Title:          XMODEM-CRC receive module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: xmodem.c 81 2009-12-14 13:15:09Z pieterconradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "xmodem.h"
#include "tmr.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// \name XMODEM protocol definitions
//@{
#define XMODEM_PACKET_SIZE       133
#define XMODEM_DATA_SIZE         128
#define XMODEM_TIMEOUT_MS        1000
#define XMODEM_MAX_RETRIES       4
#define XMODEM_MAX_RETRIES_START 1
//@}

/// \name XMODEM flow control characters
//@{
#define XMODEM_SOH               0x01 // Start of Header
#define XMODEM_EOT               0x04 // End of Transmission 
#define XMODEM_ACK               0x06 // Acknowledge 
#define XMODEM_NAK               0x15 // Not Acknowledge 
#define XMODEM_C                 0x43 // ASCII “C”
//@}

/// XMODEM error list
typedef enum
{
    XMODEM_NO_ERROR,
    XMODEM_RECEIVED_EOT,
    XMODEM_ERR_RECEIVED_NOTHING,
    XMODEM_ERR_RECEIVE_TIMEOUT,
    XMODEM_ERR_INCORRECT_HEADER,
    XMODEM_ERR_INCORRECT_PACKET_NUMBER,
    XMODEM_ERR_DUPLICATE_PACKET_NUMBER,
    XMODEM_ERR_INCORRECT_CRC
} xmodem_error_t;

/* _____LOCAL VARIABLES______________________________________________________ */
/// Buffer
static u8_t xmodem_packet_buffer[XMODEM_PACKET_SIZE];

/// Variable to keep track of current packet number
static u8_t xmodem_packet_number;

static xmodem_rx_char_t     xmodem_rx_char_fn;
static xmodem_tx_char_t     xmodem_tx_char_fn;
static xmodem_on_rx_data_t  xmodem_on_rx_data_fn;
static xmodem_on_tx_data_t  xmodem_on_tx_data_fn;

static tmr_t xmodem_tmr;

/* _____PRIVATE FUNCTIONS____________________________________________________ */
static bool_t xmodem_rx_char(char* data)
{
    return (*xmodem_rx_char_fn)(data);
}

static void xmodem_tx_char(char data)
{
    (*xmodem_tx_char_fn)(data);
}

static void xmodem_on_rx_data(u8_t *data, u8_t bytes_received)
{
    (*xmodem_on_rx_data_fn)(data, bytes_received);
}

static bool_t xmodem_on_tx_data(u8_t *data, u8_t bytes_to_send)
{
    return (*xmodem_on_tx_data_fn)(data, bytes_to_send);
}

static bool_t xmodem_wait_rx_char(char* data, tmr_ticks_t ticks_to_wait)
{
    tmr_start(&xmodem_tmr, ticks_to_wait);
    while(!tmr_has_expired(&xmodem_tmr))
    {
        if(xmodem_rx_char(data))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/// Function that verifies that packet checksum is correct
static u16_t xmodem_calculate_checksum(void)
{
    u8_t  i;
    u8_t  j;
    u8_t  data;
    u16_t crc = 0x0000;

    // Repeat until all the data has been processed...
    for(j=3; j<(3+XMODEM_DATA_SIZE); j++)
    {
        data = xmodem_packet_buffer[j];

        // XOR high byte of CRC with 8-bit data
        crc = crc ^ (((u16_t)data)<<8); 

        // Repeat 8 times
        for(i=8; i!=0; i--)
        {
            // If highest bit is set, then shift left and XOR with 0x1021
            if((crc & 0x8000) != 0)
            {
                crc = (crc << 1) ^ 0x1021;
            }
            // else, just shift left
            else
            {
                crc = (crc << 1);
            }
        }
    }
	return crc;
}

static bool_t xmodem_verify_checksum(u16_t crc)
{
    // Compare received CRC with calculated value
    if(xmodem_packet_buffer[3+XMODEM_DATA_SIZE] != U16_HI8(crc))
    {    
        return FALSE;
    }
    if(xmodem_packet_buffer[4+XMODEM_DATA_SIZE] != U16_LO8(crc))
    {    
        return FALSE;
    }

    return TRUE;
}

/// Blocking function with a 1s timeout that tries to receive an XMODEM packet
static xmodem_error_t xmodem_rx_packet(void)
{
    u8_t  i = 0; 
    u8_t  data;   

    tmr_start(&xmodem_tmr, TMR_MS_TO_TICKS(XMODEM_TIMEOUT_MS));

    while(!tmr_has_expired(&xmodem_tmr))
    {
        // See if character has been received
        if(xmodem_rx_char((char *)&data))
        {
            // See if this is the first byte of a packet received
            if(i == 0)
            {
                // See if End Of Transmission has been received
                if(data == XMODEM_EOT)
                {    
                    return XMODEM_RECEIVED_EOT;
                }
                // Restart timer
                tmr_restart(&xmodem_tmr);
            }
            // Store received data in buffer
            xmodem_packet_buffer[i] = data;
            // Next byte in packet until whole packet has been received
            if(++i == XMODEM_PACKET_SIZE)
            {    
                break;
            }
        }
    }

    // See if anything was received
    if(i == 0)
    {
        return XMODEM_ERR_RECEIVED_NOTHING;
    }

    // See if correct header was received
    if(xmodem_packet_buffer[0] != XMODEM_SOH)
    {
        return XMODEM_ERR_INCORRECT_HEADER;
    }

    // See if whole packet was received
    if(i != XMODEM_PACKET_SIZE)
    {
        return XMODEM_ERR_RECEIVE_TIMEOUT;
    }

    // Check packet number checksum
    if((xmodem_packet_buffer[1]^xmodem_packet_buffer[2]) != 0xFF)
    {
        return XMODEM_ERR_INCORRECT_PACKET_NUMBER;
    }

    // See if duplicate packet was received
    if(xmodem_packet_buffer[1] == (xmodem_packet_number-1))
    {
        return XMODEM_ERR_DUPLICATE_PACKET_NUMBER;
    }

    // Make sure correct packet was received
    if(xmodem_packet_buffer[1] != xmodem_packet_number)
    {
        return XMODEM_ERR_INCORRECT_PACKET_NUMBER;
    }

    // Verify Checksum
    if(!xmodem_verify_checksum(xmodem_calculate_checksum()))
    {
        return XMODEM_ERR_INCORRECT_CRC;
    }

    return XMODEM_NO_ERROR;
}

static void xmodem_tx_packet(void)
{
    u8_t  i; 
    u8_t  data;
	u16_t crc;

    // See if correct header was received
    xmodem_tx_char(XMODEM_SOH);

    // packet number
    xmodem_tx_char(xmodem_packet_number);

    // packet number checksum
    xmodem_tx_char(xmodem_packet_number ^ 0xFF);

    // Send data
    for(i=3; i<(3+XMODEM_DATA_SIZE); i++)
    {
        data = xmodem_packet_buffer[i];
        xmodem_tx_char(data);
    }

    // Calculate checksum
    crc = xmodem_calculate_checksum();

    // Send checksum
    xmodem_tx_char(U16_HI8(crc));
    xmodem_tx_char(U16_LO8(crc));
    
}

/* _____FUNCTIONS_____________________________________________________ */
extern void xmodem_init(xmodem_rx_char_t    rx_char,
                        xmodem_tx_char_t    tx_char,
                        xmodem_on_rx_data_t on_rx_data,
                        xmodem_on_tx_data_t on_tx_data )
{
    xmodem_rx_char_fn      = rx_char;
    xmodem_tx_char_fn      = tx_char;
    xmodem_on_rx_data_fn   = on_rx_data;
    xmodem_on_tx_data_fn   = on_tx_data;
}

bool_t xmodem_rx_file(void)
{
    u8_t           retry_count    = XMODEM_MAX_RETRIES_START;
    bool_t         first_ack_sent = FALSE;
    u8_t           data;
    xmodem_error_t error;

    // Reset packet number
    xmodem_packet_number = 1;

    // Repeat until transfer is finished or error count is exceeded
    while(retry_count--)
    {
        if(!first_ack_sent)
        {
            // Send initial start character to start transfer (with CRC checking)
            xmodem_tx_char(XMODEM_C);
        }

        // Receive packet
        error = xmodem_rx_packet();
        
        // Handle state 
        switch(error)
        {
        case XMODEM_NO_ERROR:
            // Pass received data on to handler
            xmodem_on_rx_data(xmodem_packet_buffer+3,XMODEM_DATA_SIZE);
            // Acknowledge packet
            xmodem_tx_char(XMODEM_ACK);
            // Set flag to indicate that first packet has been correctly received
            first_ack_sent = TRUE;
            // Next packet
            xmodem_packet_number++;
            // Reset retry count
            retry_count = XMODEM_MAX_RETRIES;
            break;

        case XMODEM_RECEIVED_EOT:           
            // Acknowledge EOT and make sure sender has received ACK
            XMODEM_EOT_STATE:
            while(--retry_count)
            {
                xmodem_tx_char(XMODEM_ACK);
                tmr_start(&xmodem_tmr, TMR_MS_TO_TICKS(XMODEM_TIMEOUT_MS));
                while(!tmr_has_expired(&xmodem_tmr))
                {
                    if(xmodem_rx_char((char *)&data))
                        if(data == XMODEM_EOT)
                        {
                            // Unfortunate, but neccesary use of the "goto" keyword
                            goto XMODEM_EOT_STATE;
                        }
                }
                // File successfully transferred
                return TRUE;
            }
            // File not successfully transferred
            return FALSE;

        case XMODEM_ERR_DUPLICATE_PACKET_NUMBER:
            // Acknowledge packet
            xmodem_tx_char(XMODEM_ACK);
            break;

        case XMODEM_ERR_RECEIVED_NOTHING:
            // Fall through...
        case XMODEM_ERR_RECEIVE_TIMEOUT:
            // Fall through...
        case XMODEM_ERR_INCORRECT_HEADER:
            // Fall through...
        case XMODEM_ERR_INCORRECT_PACKET_NUMBER:
            // Fall through...
        case XMODEM_ERR_INCORRECT_CRC:
            // Fall through...
        default:
            if(first_ack_sent)
            {
                // Send NAK
                xmodem_tx_char(XMODEM_NAK);
            }
            break;
        }        
    }

    // File not successfully transferred
    return FALSE;
}

bool_t xmodem_tx_file(void)
{
    u8_t retry_count;
    u8_t data;

    // Reset packet number
    xmodem_packet_number = 1;

    // Wait for initial start character to start transfer (with CRC checking)
    if(!xmodem_wait_rx_char((char *)&data, 15*TMR_TICKS_PER_SEC))
    {
        return FALSE;
    }
    if(data != XMODEM_C)
    {
        return FALSE;
    }

    // Get data packet to send
    while(xmodem_on_tx_data(xmodem_packet_buffer+3, XMODEM_DATA_SIZE) == TRUE)
    {
        // Try sending error packet until error count is exceeded
        retry_count = XMODEM_MAX_RETRIES;
        while(retry_count)
        {
            retry_count--;

            // Send packet
            xmodem_tx_packet();
    
            // Wait for an ACK or NAK
            if(!xmodem_wait_rx_char((char *)&data, 3*TMR_TICKS_PER_SEC))
            {
                continue;
            }
            // Received a NAK. Resemd packet
            if(data == XMODEM_NAK)
            {
                continue;
            }
            // Received an ACK. Packet has been correctly received
            if(data == XMODEM_ACK)
            {
                break;
            }
        }
        // See if retry count was exceeded
        if(retry_count == 0)
        {
            return FALSE;
        }

        // Next packet index
        xmodem_packet_number++;
    }

    // Send "End Of Transfer"
    xmodem_tx_char(XMODEM_EOT);

    // File successfully transferred
    return TRUE;
}

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : Pieter.Conradie
 - First release
 
 2010-04-23 : Pieter.Conradie
 - Added xmodem_tx_file(...)
   
*/
