#ifndef __NMEA_H__
#define __NMEA_H__
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

/** 
 *  @ingroup PROTOCOL
 *  @defgroup NMEA nmea.h : NMEA parser
 *
 *  GPS NMEA protocol parser.
 *  
 *  Files: nmea.h & nmea.c
 *  
 *  @see http://en.wikipedia.org/wiki/NMEA_0183
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
// NMEA output strings
#define NMEA_GGA_STR "GGA" /* Time, position and fix type data. */
#define NMEA_GLL_STR "GLL" /* Latitude, longitude, UTC time of position fix and status. */
#define NMEA_GSA_STR "GSA" /* GPS receiver operating mode, satellites used in the position solution and DOP values. */
#define NMEA_GSV_STR "GSV" /* The number of GPS satellites in view, satellite ID numbers, elevation, azimuth, and SNR values. */
#define NMEA_MSS_STR "MSS" /* Signal-to-noise ratio, signal strength, frequency, and bit rate from a radio-beacon receiver. */
#define NMEA_RMC_STR "RMC" /* Time, date, position, course and speed data. */
#define NMEA_VTG_STR "VTG" /* Course and speed information relative to the ground */
#define NMEA_ZDA_STR "ZDA" /* Date and time */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/**
 * Definition for a pointer to a function that will be called to 
 * send a character.
 */
typedef void (*nmea_tx_byte_t)(u8_t data);

/**
 * Definition for a pointer to a function that when a valid NMEA string is 
 * received.
 */
typedef void (*nmea_on_valid_str_t)(const char* data);

/**
 * Definition for a pointer to a function that will be called when GPS data 
 * is valid.
 */
typedef void (*nmea_on_valid_gps_data_t)(void);

/// Parsed time, position, quality data
typedef struct
{
   u32_t    utc_time;
   u16_t    utc_time_fraction;
   u8_t     delay_from_PPS;
   s16_t    latitude;
   u16_t    latitude_fraction;
   s16_t    longitude;
   u16_t    longitude_fraction;
   s16_t    altitude;
   u8_t     altitude_fraction;
   u16_t    heading;
   u8_t     heading_fraction;
   u8_t     speed;
   u8_t     speed_fraction;
   u8_t     sattelites_used;
   u8_t     hdop;
   u8_t     hdop_fraction;
   bool_t   gga_valid_flag;
   bool_t   vtg_valid_flag;
} nmea_data_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */
extern nmea_data_t nmea_data;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 * Initialise NMEA parser module
 * 
 * @param tx_byte           Pointer to a function that will be called to 
 *                          transmit a byte.
 * @param on_valid_str      Pointer to a function that will be called when a 
 *                          valid NMEA string has been received.
 * @param on_valid_gps_data Pointer to a function that will be called when the 
 *                          data structure has been completely polulated with
 *                          valid data.
 */
extern void nmea_init      (nmea_tx_byte_t           tx_byte,
                            nmea_on_valid_str_t      on_valid_str,
                            nmea_on_valid_gps_data_t on_valid_gps_data);

/**
 *  Function handler that is fed all raw received data.
 * 
 *  @param[in] data     received 8-bit data
 * 
 */
extern void nmea_on_rx_byte(u8_t data);

/**
 * Function that is called to send an NMEA frame with the checksum appended.
 * 
 * @param frame     Pointer to zero terminated string.
 */
extern void nmea_tx_frame(char* frame);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
