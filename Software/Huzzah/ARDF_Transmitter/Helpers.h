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

**********************************************************************************************/

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <ESP8266WiFi.h>

#ifndef SecondsFromHours
#define SecondsFromHours(hours) ((hours) * 3600)
#endif

#ifndef SecondsFromMinutes
#define SecondsFromMinutes(min) ((min) * 60)
#endif

#ifndef HoursFromSeconds
#define HoursFromSeconds(seconds) ((seconds) / 3600)
#endif

#ifndef MinutesFromSeconds
#define MinutesFromSeconds(seconds) ((seconds) / 60)
#endif

//#ifndef min
//#define min(x, y)  ((x) < (y) ? (x) : (y))
//#endif

//#ifndef max
//#define max(x, y)  ((x) > (y) ? (x) : (y))
//#endif

#define CLAMP(low, x, high) ({\
    __typeof__(x) __x = (x); \
    __typeof__(low) __low = (low);\
    __typeof__(high) __high = (high);\
    __x > __high ? __high : (__x < __low ? __low : __x);\
  })

typedef struct
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_yday;
  int tm_mon;
  int tm_year;
  int tm_ym4;
} Tyme;

const char * stringObjToConstCharString(String *val);
IPAddress stringToIP(String addr);
String formatBytes(size_t bytes);
String getContentType(String filename);
bool isLeapYear(int year);
unsigned long convertTimeStringToEpoch(String s);
bool mystrptime(String s, Tyme* tm);
String checksum(String str);
bool validateMessage(String str);
String convertEpochToTimeString(unsigned long epoch);

#endif  /*_HELPERS_H_ */
