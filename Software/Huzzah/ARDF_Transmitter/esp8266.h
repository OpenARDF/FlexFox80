/**********************************************************************************************
    Copyright © 2022 Digital Confections LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in the
    Software without restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the
    following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
    FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

 **********************************************************************************************

    esp8266.h

    Created: 6/18/2017 2:08:00 PM
    Author: Digital Confections LLC
*/


#ifndef ESP8266_H_
#define ESP8266_H_

#define WIFI_SW_VERSION ("2.0")
#define USE_UDP_FOR_TIME_RETRIEVAL
#define MULTI_ACCESS_POINT_SUPPORT
#define SERIAL_BAUD_RATE 9600
//#define SERIAL_BAUD_RATE 19200

#define MAX_NUMBER_OF_WEB_CLIENTS WEBSOCKETS_SERVER_CLIENT_MAX
#define MAX_TIME ((unsigned long)4294967294)
#define FEB_7_2106 (MAX_TIME)
#define JAN_1_2020 ((unsigned long)1577836800)

/*
  #define HOTSPOT_SSID1_DEFAULT ("myRouter1")
  #define HOTSPOT_PW1_DEFAULT ("router1PW")
  #define HOTSPOT_SSID2_DEFAULT ("myRouter2")
  #define HOTSPOT_PW2_DEFAULT ("router2PW")
  #define HOTSPOT_SSID3_DEFAULT ("myRouter3")
  #define HOTSPOT_PW3_DEFAULT ("router3PW")
  #define MDNS_RESPONDER_DEFAULT ("fox")
  #define TIME_HOST_DEFAULT "time.nist.gov"
  #define TIME_HTTP_PORT_DEFAULT String("13")
  #define BRIDGE_IP_ADDR_DEFAULT "73.73.73.74"
  #define BRIDGE_SSID_DEFAULT "Fox_"
  #define BRIDGE_TCP_PORT_DEFAULT String("73") */
#define BRIDGE_PW_DEFAULT ""
#define SOFT_AP_IP_ADDR_DEFAULT "73.73.73.73"
#define LEDS_ENABLE_DEFAULT true
#define DEBUG_PRINTS_ENABLE_DEFAULT true
#define WIFI_DEBUG_PRINTS_ENABLED false
#define MASTER_SSID String("Tx_Master")
#define MASTER_ENABLE_DEFAULT false


typedef enum
{
  MASTER_ENABLE,
  LEDS_ENABLE,
  DEBUG_PRINTS_ENABLE,
  LAST_EVENT_FILE_RUN,
  NUMBER_OF_SETTABLE_VARIABLES
} WiFiMemory;

#endif  /* ESP8266_H_ */
