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
    
    Title:          NMEA parser
    Author(s):      Pieter.Conradie
    Creation Date:  2010/05/28
    Revision Info:  $Id: $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "nmea.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Receive and transmit buffer size
#define NMEA_BUFFER_SIZE    128

typedef enum
{
   NMEA_RX_STATE_START = 0,
   NMEA_RX_STATE_PAYLOAD,
   NMEA_RX_STATE_CHECKSUM1,
   NMEA_RX_STATE_CHECKSUM2,
   NMEA_RX_STATE_END_CR,
   NMEA_RX_STATE_END_LF,
} nmea_rx_state_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
nmea_data_t nmea_data;

/* _____LOCAL VARIABLES______________________________________________________ */
static nmea_tx_byte_t           nmea_tx_byte_fn;
static nmea_on_valid_str_t      nmea_on_valid_str_fn;
static nmea_on_valid_gps_data_t nmea_on_valid_gps_data_fn;

static u8_t                     nmea_rx_buffer[NMEA_BUFFER_SIZE];
static u16_t                    nmea_rx_index;
static u8_t                     nmea_rx_checksum;
static nmea_rx_state_t          nmea_rx_state;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void   nmea_tx_byte                  (u8_t data);

static bool_t nmea_cmp_nibble_with_hex_ascii(u8_t nibble, char ascii);
static char*  nmea_parse_str_to_u32         (char* buffer, u32_t* value);
static char*  nmea_parse_str_to_s16         (char* buffer, s16_t* value);
static char*  nmea_parse_str_to_u16         (char* buffer, u16_t* value);
static char*  nmea_parse_str_fraction_to_u16(char* buffer, u16_t* value, u8_t precision);
static char*  nmea_parse_str_to_u8          (char* buffer, u8_t* value);
static char*  nmea_parse_str_fraction_to_u8 (char* buffer, u8_t* value, u8_t precision);
static char*  nmea_find_next_param          (char* buffer);

static void   nmea_on_rx_frame              (char* buffer);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void nmea_tx_byte(u8_t data)
{
    if(nmea_tx_byte_fn == NULL)
    {
        return;
    }
    (*nmea_tx_byte_fn)(data);
}

static bool_t  nmea_cmp_nibble_with_hex_ascii(u8_t nibble, char ascii)
{
   if(nibble<=9)
   {
       nibble += '0';
   }
   else
   {
       nibble += ('A'-10);
   }

   if(nibble == ascii)
   {
       return TRUE;
   }
   else
   {
       return FALSE;
   }
}

static char* nmea_parse_str_to_u32(char* buffer, u32_t* value)
{
   u32_t u32Value = 0;
   while((*buffer != ',')&&(*buffer != '.')&&(*buffer != '\0'))
   {
      if(*buffer == ' ')
      {
         // Ignore whitespace
         buffer++;
         continue;
      }
      u32Value *= 10;
      u32Value += (*buffer++) - '0';
   }
   *value = u32Value;
   return buffer;
}

static char* nmea_parse_str_to_s16(char* buffer, s16_t* value)
{
   bool_t  bNeg     = FALSE;
   s16_t s16Value = 0;

   while((*buffer != ',')&&(*buffer != '.')&&(*buffer != '\0'))
   {
      if(*buffer == ' ')
      {
         // Ignore whitespace
         buffer++;
         continue;
      }
      else if(*buffer == '-')
      {
         // Remove sign
         bNeg = TRUE;
         buffer++;
         continue;
      }
      s16Value *= 10;
      s16Value += (*buffer++) - '0';
   }
   // Add sign if necessary
   if(bNeg) s16Value *= -1;

   *value = s16Value;
   return buffer;
}

static char* nmea_parse_str_to_u16(char* buffer, u16_t* value)
{
   u16_t u16Value = 0;
   while((*buffer != ',')&&(*buffer != '.')&&(*buffer != '\0'))
   {
      if(*buffer == ' ')
      {
         // Ignore whitespace
         buffer++;
         continue;
      }
      u16Value *= 10;
      u16Value += (*buffer++) - '0';
   }
   *value = u16Value;
   return buffer;
}

static char* nmea_parse_str_fraction_to_u16(char* buffer, u16_t* value, u8_t precision)
{
   u16_t u16Value = 0;
   while((*buffer != ',')&&(*buffer != '.')&&(*buffer != '\0')&&(precision != 0))
   {
      if(*buffer == ' ')
      {
         // Ignore whitespace
         buffer++;
         continue;
      }
      u16Value *= 10;
      u16Value += (*buffer++) - '0';
      precision--;
   }
   while(precision != 0)
   {
      precision--;
      u16Value *= 10;
   }
   *value = u16Value;
   return buffer;
}

static char* nmea_parse_str_to_u8(char* buffer, u8_t* value)
{
   u8_t u8Value = 0;
   while((*buffer != ',')&&(*buffer != '.')&&(*buffer != '\0'))
   {
      if(*buffer == ' ')
      {
         // Ignore whitespace
         buffer++;
         continue;
      }
      u8Value *= 10;
      u8Value += (*buffer++) - '0';
   }
   *value = u8Value;
   return buffer;
}

static char* nmea_parse_str_fraction_to_u8(char* buffer, u8_t* value, u8_t precision)
{
   u8_t u8Value = 0;
   while((*buffer != ',')&&(*buffer != '.')&&(*buffer != '\0')&&(precision != 0))
   {
      if(*buffer == ' ')
      {
         // Ignore whitespace
         buffer++;
         continue;
      }
      u8Value *= 10;
      u8Value += (*buffer++) - '0';
      precision--;
   }
   while(precision != 0)
   {
      precision--;
      u8Value *= 10;      
   }
   *value = u8Value;
   return buffer;
}

static char* nmea_find_next_param(char* buffer)
{
   while(*buffer != ',')
   {
      if(*buffer == '\0') return buffer;
      buffer++;
   }
   return (++buffer);
}

static void nmea_on_rx_frame(char* buffer)
{  
    // Notify handler with valid NMEA string
   (*nmea_on_valid_str_fn)(buffer);

   // Make sure that message ID starts with "GP"
   if(*buffer++ != 'G')
   {
       return;
   }
   if(*buffer++ != 'P')
   {
       return;
   }

   // Parse GGA (Global Positioning System Fixed Data) string
   if(strncmp(buffer,NMEA_GGA_STR,3) == 0)
   {
#if 0
      if(nmea_data.gga_valid_flag)
      {
         // Enable VTG message @ 1Hz update rate
         nmea_tx_frame("$PSRF103,05,00,01,01");
      }
#endif
      buffer += 4;
      nmea_data.gga_valid_flag     = FALSE;
      nmea_data.latitude_fraction  = 0;
      nmea_data.longitude_fraction = 0;
      // UTC Time
      buffer = nmea_parse_str_to_u32(buffer, &nmea_data.utc_time);
      if(*buffer == '.')
      {
         buffer++;
         buffer = nmea_parse_str_fraction_to_u16(buffer, &nmea_data.utc_time_fraction, 3);
      }
      buffer = nmea_find_next_param(buffer);

      // Latitude
      buffer = nmea_parse_str_to_u16(buffer, (u16_t*)&nmea_data.latitude);
      if(*buffer == '.')
      {
         buffer++;
         buffer = nmea_parse_str_fraction_to_u16(buffer, &nmea_data.latitude_fraction, 4);
      }
      buffer = nmea_find_next_param(buffer);
      if(*buffer == 'S')
      {
          nmea_data.latitude *= -1;
      }
      buffer = nmea_find_next_param(buffer);

      // Longitude
      buffer = nmea_parse_str_to_u16(buffer, (u16_t*)&nmea_data.longitude);
      if(*buffer == '.')
      {
         buffer++;
         buffer = nmea_parse_str_fraction_to_u16(buffer, &nmea_data.longitude_fraction, 4);
      }
      buffer = nmea_find_next_param(buffer);
      if(*buffer == 'W')
      {
          nmea_data.longitude *= -1;
      }
      buffer = nmea_find_next_param(buffer);
      // Fix valid
      if(*buffer == '1')
      {
          nmea_data.gga_valid_flag  = TRUE;
      }
      buffer = nmea_find_next_param(buffer);
      // Number of satelites
      buffer = nmea_parse_str_to_u8(buffer, &nmea_data.sattelites_used);
      buffer = nmea_find_next_param(buffer);
      // HDOP
      buffer = nmea_parse_str_to_u8(buffer, &nmea_data.hdop);
      if(*buffer == '.')
      {
         buffer++;
         buffer = nmea_parse_str_fraction_to_u8(buffer, &nmea_data.hdop_fraction, 1);
      }
      buffer = nmea_find_next_param(buffer);
      // Altitude
      buffer = nmea_parse_str_to_s16(buffer, &nmea_data.altitude);
      if(*buffer == '.')
      {
         buffer++;
         buffer = nmea_parse_str_fraction_to_u8(buffer, &nmea_data.altitude_fraction, 2);
      }      

      nmea_data.gga_valid_flag = TRUE;
      
      // See if all the fields were populated
      if(nmea_data.gga_valid_flag && nmea_data.vtg_valid_flag)
      {
         (*nmea_on_valid_gps_data_fn)();
         nmea_data.gga_valid_flag = FALSE;
         nmea_data.vtg_valid_flag = FALSE;
      }
   }
   // Parse VTG (Course Over Ground and Ground Speed) string
   else if(strncmp(buffer, NMEA_VTG_STR, 3) == 0)
   {
      buffer += 4;

      // Heading
      buffer = nmea_parse_str_to_u16(buffer,&nmea_data.heading);
      if(*buffer == '.')
      {
         buffer++;
         buffer = nmea_parse_str_fraction_to_u8(buffer,&nmea_data.heading_fraction,2);
      }
      buffer = nmea_find_next_param(buffer);
      buffer = nmea_find_next_param(buffer);
      buffer = nmea_find_next_param(buffer);
      buffer = nmea_find_next_param(buffer);
      buffer = nmea_find_next_param(buffer);
      buffer = nmea_find_next_param(buffer);

      // Speed
      buffer = nmea_parse_str_to_u8(buffer,&nmea_data.speed);
      if(*buffer == '.')
      {
         buffer++;
         buffer = nmea_parse_str_fraction_to_u8(buffer,&nmea_data.speed_fraction,2);
      }
      
      nmea_data.vtg_valid_flag = TRUE;
      
      // See if all the fields were populated
      if(nmea_data.gga_valid_flag && nmea_data.vtg_valid_flag)
      {
         (*nmea_on_valid_gps_data_fn)();
         nmea_data.gga_valid_flag = FALSE;
         nmea_data.vtg_valid_flag = FALSE;
      }
   }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void nmea_init(nmea_tx_byte_t           tx_byte,
               nmea_on_valid_str_t      on_valid_str,
               nmea_on_valid_gps_data_t on_valid_gps_data)
{
    // Save function pointers
    nmea_tx_byte_fn           = tx_byte;
    nmea_on_valid_str_fn      = on_valid_str;
    nmea_on_valid_gps_data_fn = on_valid_gps_data;

   // Reset state variables
   nmea_rx_state            = NMEA_RX_STATE_START;
   nmea_rx_index            = 0;
   nmea_data.gga_valid_flag = FALSE;
   nmea_data.vtg_valid_flag = FALSE;
}

void nmea_on_rx_byte(u8_t data)
{   
   switch(nmea_rx_state)
   {
   case NMEA_RX_STATE_START :
      {
         // Check start sequence
         if(data != '$')
         {
             break;
         }
         nmea_rx_state = NMEA_RX_STATE_PAYLOAD;
         // Reset index and checksum
         nmea_rx_checksum = 0;
         nmea_rx_index    = 0;
         return;
      }
   case NMEA_RX_STATE_PAYLOAD :
      {
         // Check for unexpected characters in payload
         if(  (data == '$' )
            ||(data == '\r')
            ||(data == '\n')
            ||(data >= 0x80)  )
         {
             break;
         }
         // Check for checksum marker
         if(data == '*')
         {
            nmea_rx_state = NMEA_RX_STATE_CHECKSUM1;
            return;
         }
         // Update checksum of payload
         nmea_rx_checksum ^= data;
         // Put received byte into buffer
         nmea_rx_buffer[nmea_rx_index] = data;
         // Check for buffer overflow
         if (++nmea_rx_index >= (NMEA_BUFFER_SIZE-1))
         {
             break;
         }
         return;
      }
   case NMEA_RX_STATE_CHECKSUM1 :
      {
         // Check high nibble of checksum
         if(!nmea_cmp_nibble_with_hex_ascii(((nmea_rx_checksum>>4)&0x0f), data))
         {
             break;
         }
         
         nmea_rx_state = NMEA_RX_STATE_CHECKSUM2;
         return;
      }
   case NMEA_RX_STATE_CHECKSUM2 :
      {
         // Check low nibble of checksum
         if(!nmea_cmp_nibble_with_hex_ascii((nmea_rx_checksum&0x0f),data))
         {
             break;
         }

         nmea_rx_state = NMEA_RX_STATE_END_CR;
         return;
      }
   case NMEA_RX_STATE_END_CR :
      {
         // Check Carriage Return
         if(data != '\r')
         {
             break;
         }
         nmea_rx_state = NMEA_RX_STATE_END_LF;
         return;
      }
    case NMEA_RX_STATE_END_LF:
      {
         // Check Line Feed
         if(data != '\n')
         {
             break;
         }
         // Append terminating zero
         nmea_rx_buffer[nmea_rx_index] = '\0';
         // String successfully received
         nmea_on_rx_frame((char*)nmea_rx_buffer);
         break;
      }   
   }
   // Error detected... reset receiver
   nmea_rx_state = NMEA_RX_STATE_START;
}

void nmea_tx_frame(char* frame)
{
   u8_t  data;
   u8_t  checksum = '$';

   // Send data and calculate checksum
   while(*frame)
   {
       data      = *frame++;
       checksum ^= data;
       nmea_tx_byte(data);
   }

   // Add checksum
   nmea_tx_byte('*');
   // Send high nibble
   if(checksum<0xA0)
   {
       nmea_tx_byte(((checksum>>4)&0x0f)+'0');
   }
   else
   {
       nmea_tx_byte(((checksum>>4)&0x0f)+('A'-10));
   }
   // Send low nibble
   checksum &=0x0f;
   if(checksum<0x0A)
   {
       nmea_tx_byte(checksum+'0');
   }
   else
   {
       nmea_tx_byte(checksum+('A'-10));
   }

   // Add end sequence
   nmea_tx_byte('\r');
   nmea_tx_byte('\n');
}

/* _____LOG__________________________________________________________________ */
/*

 2010/05/28 : Pieter.Conradie
 - Created
   
*/
