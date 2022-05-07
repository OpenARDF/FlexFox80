/**********************************************************************************************
 * Copyright © 2017 Digital Confections LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 **********************************************************************************************
 *
 * esp8266.h
 *
 * Created: 6/18/2017 2:08:00 PM
 * Author: Digital Confections LLC
 */ 


#ifndef ESP8266_H_
#define ESP8266_H_

#define WIFI_SW_VERSION ("0.1.1")
#define USE_UDP_FOR_TIME_RETRIEVAL

#ifndef SecondsFromHours
  #define SecondsFromHours(hours) ((hours) * 3600)
#endif

#ifndef SecondsFromMinutes
  #define SecondsFromMinutes(min) ((min) * 60)
#endif

#ifndef BOOL
  typedef uint8_t BOOL;
#endif

#ifndef FALSE
  #define FALSE (0)
#endif

#ifndef TRUE
  #define TRUE !FALSE
#endif

#ifndef min
  #define min(x, y)  ((x) < (y) ? (x) : (y))
#endif

#define HOTSPOT_SSID_DEFAULT "MyRouter"
#define HOTSPOT_PW_DEFAULT "password"
#define TIME_HOST_DEFAULT "time.nist.gov"
#define TIME_HTTP_PORT_DEFAULT String("13")
#define BRIDGE_IP_ADDR_DEFAULT "10.10.10.150"
#define BRIDGE_SSID_DEFAULT "Fox_"
#define BRIDGE_PW_DEFAULT "password"
#define BRIDGE_TCP_PORT_DEFAULT String("73")
#define LEDS_ENABLE_DEFAULT TRUE
#define DEBUG_PRINTS_ENABLE_DEFAULT TRUE


typedef enum
{
	HOTSPOT_SSID,
	HOTSPOT_PW,
	TIME_HOST,
	TIME_HTTP_PORT,
  BRIDGE_IP_ADDR,
	BRIDGE_SSID,
	BRIDGE_PW,
	BRIDGE_TCP_PORT,
  LEDS_ENABLE,
  DEBUG_PRINTS_ENABLE,
	NUMBER_OF_SETTABLE_VARIABLES
} WiFiMemory;

#endif /* ESP8266_H_ */
