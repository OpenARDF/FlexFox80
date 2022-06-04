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

#include "Helpers.h"
#include <Arduino.h>
/*#include <GDBStub.h> */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
/*#include <ESP8266mDNS.h> */
#include <user_interface.h>
#include "esp8266.h"
/*#include <ArduinoOTA.h> */
#include <FS.h>
/*#include <WebSocketsClient.h> */
#include <Hash.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFiType.h>
#include <time.h>

/*
    Removes extraneous characters that tend to clutter the ends of String object contents. Then
    returns a const char * to the C string contained in the String.
*/
#define MAX_CONST_CHAR_STR_LEN (150)
const char * stringObjToConstCharString(String *val)
{
  char str[MAX_CONST_CHAR_STR_LEN];

  strncpy(str, (*val).c_str(), MAX_CONST_CHAR_STR_LEN);

  for (uint16_t i = 0; i < strlen(str); i++)
  {
    char c = str[i];
    if ((!(isalnum(c) || isprint(c))) || (c == 13))
    {
      str[i] = '\0';
      break;
    }
  }

  *val = str;

  return ((const char*)(*val).c_str());
}

String checksum(String str)
{
  char hex[4];
  int star = str.lastIndexOf("*");

  if (star > 0)
  {
    str = str.substring(0, star);
  }
  int checksum = 0;

  for (uint16_t i = 0; i < str.length(); i++)
  {
    checksum ^= str.charAt(i);
  }

  sprintf(hex, "*%02x", checksum);
  return ( String(hex));
}

/**
    Returns true if checksum calculation does not match the string's checksum,
    or if the string passed in the argument doesn't include a checksum
*/
bool validateMessage(String str)
{
  str.trim();
  int star = str.lastIndexOf("*");
  if (star < ((int)str.length() - 3))
  {
    return ( true);
  }
  String cs = str.substring(star);
  String check = checksum(str);
  return (!cs.equals(check));
}


/*
    Converts a String containing a valid IP address into an IPAddress.
    Returns the IPAddress contained in addr, or returns "-1,-1,-1,-1" if the address is invalid
*/
IPAddress stringToIP(String addr)
{
  IPAddress ipAddr = IPAddress(-1, -1, -1, -1);
  int dot, nextDot;
  /* 129.6.15.28 = NIST, Gaithersburg, Maryland */
  String o1 = "129", o2 = "6", o3 = "15", o4 = "28";  /* default to some reasonable IP address */

  dot = addr.indexOf(".", 0);

  if (dot > 0)
  {
    o1 = addr.substring(0, dot);

    if ((o1.toInt() >= 0) && (o1.toInt() <= 255))
    {
      dot++;
      nextDot = addr.indexOf(".", dot);

      if (nextDot > 0)
      {
        o2 = addr.substring(dot, nextDot);

        if ((o2.toInt() >= 0) && (o2.toInt() <= 255))
        {
          nextDot++;
          dot = addr.indexOf(".", nextDot);

          if (dot > 0)
          {
            o3 = addr.substring(nextDot, dot);
            dot++;
            o4 = addr.substring(dot);

            if (((o3.toInt() >= 0) && (o3.toInt() <= 255)) && ((o4.toInt() >= 0) && (o4.toInt() <= 255)))
            {

              ipAddr = IPAddress(o1.toInt(), o2.toInt(), o3.toInt(), o4.toInt());
            }
          }
        }
      }
    }
  }

  return (ipAddr);
}


String formatBytes(size_t bytes)
{ /* convert sizes in bytes to KB and MB */
  if (bytes < 1024)
  {
    return ( String(bytes) + "B");
  }
  else if (bytes < (1024 * 1024))
  {
    return ( String(bytes / 1024.0) + "KB");
  }
  /*  else if (bytes < (1024 * 1024 * 1024)) */
  return ( String(bytes / 1024.0 / 1024.0) + "MB");
}

String getContentType(String filename)
{ /* determine the filetype of a given filename, based on the extension */
  if (filename.endsWith(".html"))
  {
    return ( "text/html");
  }
  else if (filename.endsWith(".css"))
  {
    return ( "text/css");
  }
  else if (filename.endsWith(".js"))
  {
    return ( "application/javascript");
  }
  else if (filename.endsWith(".ico"))
  {
    return ( "image/x-icon");
  }
  else if (filename.endsWith(".gz"))
  {
    return ( "application/x-gzip");
  }
  return ( "text/plain");
}


bool isLeapYear(int year)
{
  if (year % 4)   /* if not divisible by 4 it is not a leap year (e.g., 2001) */
  {
    return ( false);
  }

  if (year % 100) /* if divisible by 4, and not by 100, it is a leap year (e.g., 2004) */
  {
    return ( true);
  }

  if (year % 400) /* if divisible by 4, and by 100, but not by 400, it is not a leap year (e.g., 1900) */
  {
    return ( false);
  }

  return ( true); /* if divisible by 4, by 100, and by 400 it is a leap year (e.g., 2000) */
}



/**
    Returns parsed time structure from a string of format "yyyy-mm-ddThh:mm:ss"
*/
bool mystrptime(String s, Tyme* tm)
{
  const int month_days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  String temp;
  int index, hold;
  bool isleap;

  index = s.indexOf("-");
  temp = s.substring(0, index);
  hold = temp.toInt();
  isleap = isLeapYear(hold);
  hold -= 1900;
  if ((hold < 0) || (hold > 200))
  {
    return ( true);
  }
  tm->tm_year = hold;

  index = s.indexOf("-", index + 1);
  temp = s.substring(index - 2, index);
  hold = temp.toInt() - 1;
  if ((hold > 11) || (hold < 0))
  {
    return ( true);
  }
  tm->tm_mon = hold;

  index = s.indexOf("T", index);
  temp = s.substring(index - 2, index);
  hold = temp.toInt();
  if ((hold > 31) || (hold < 1))
  {
    return ( true);
  }
  tm->tm_mday = hold;

  tm->tm_yday = 0;
  for (int i = 0; i < tm->tm_mon; i++)
  {
    tm->tm_yday += month_days[i];
    if ((i == 1) && isleap)
    {
      tm->tm_yday++;
    }
  }

  tm->tm_yday += (tm->tm_mday - 1);

  index = s.indexOf(":", index);
  temp = s.substring(index - 2, index);
  hold = temp.toInt();
  if ((hold > 23) || (hold < 0))
  {
    return ( true);
  }
  tm->tm_hour = hold;

  if (s.length() < 19) /* time string is missing seconds */
  {
    temp = s.substring(index + 1, index + 3);
    hold = temp.toInt();
    if (hold > 59)
    {
      return ( true);
    }
    tm->tm_min = hold;
    tm->tm_sec = 0;
  }
  else
  {
    index = s.lastIndexOf(":");
    temp = s.substring(index - 2, index);
    hold = temp.toInt();
    if (hold > 59)
    {
      return ( true);
    }
    tm->tm_min = hold;

    temp = s.substring(index + 1, index + 3);
    hold = temp.toInt();
    if (hold > 59)
    {
      return ( true);
    }
    tm->tm_sec = hold;
  }

  return ( false);
}

/**
    Converts a string of format "yyyy-mm-ddThh:mm:ss" to seconds since 1900
*/
unsigned long convertTimeStringToEpoch(String s)
{
  unsigned long result = 0;

  Tyme tm;

  if (!mystrptime(s, &tm))
  {
    result = tm.tm_sec + tm.tm_min * 60 + tm.tm_hour * 3600L + tm.tm_yday * 86400L +
             (tm.tm_year - 70) * 31536000L + ((tm.tm_year - 69) / 4) * 86400L -
             ((tm.tm_year - 1) / 100) * 86400L + ((tm.tm_year + 299) / 400) * 86400L;
  }

  return ( result);
}

/**
    Converts an epoch (seconds since 1900) and converts it into a string of format "yyyy-mm-ddThh:mm:ssZ containing UTC"
*/
String convertEpochToTimeString(unsigned long epoch)
{
  String timeStr = String("err");
  struct tm  ts;
  char buf[80];

  if (epoch < JAN_1_2020)
  {
    return timeStr;
  }

  time_t e = (time_t)epoch;
  // Format time, "ddd yyyy-mm-ddThh:mm:ss"
  ts = *localtime(&e);
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &ts);
  timeStr = buf;

  return timeStr;
}
