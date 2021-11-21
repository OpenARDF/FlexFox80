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
 * ds3231.h
 *
 * DS3231: Extremely Accurate I2C-Integrated RTC/TCXO/Crystal
 * http://datasheets.maximintegrated.com/en/ds/DS3231-DS3231S.pdf
 * The DS3231 is a low-cost, extremely accurate I2C real-time clock (RTC) with an integrated
 * temperature-compensated crystal oscillator (TCXO) and crystal. The device incorporates a
 * battery input, and maintains accurate timekeeping when main power to the device is interrupted.
 * The integration of the crystal resonator enhances the long-term accuracy of the device as well
 * as reduces the piece-part count in a manufacturing line. The DS3231 is available in commercial
 * and industrial temperature ranges, and is offered in a 16-pin, 300-mil SO package.
 *
 */

#include "defs.h"
#include <time.h>

#ifndef DS3231_H_
#define DS3231_H_

typedef enum {
	RTC_CLOCK,
	RTC_ALARM1,
	RTC_ALARM2
} ClockSetting;

/**
 *  Reads hours, minutes and seconds from the DS3231 and returns them in the memory location pointed to by
 *  the first two arguments.
 *  *val - if non-NULL will receive a dword value of the time in seconds since midnight.
 *  *char - if non-NULL will receive a string representation of the time
 *  format - specifies the string format to be used for the string time representation
 */
//	void ds3231_read_time(int32_t* val, char* buffer, TimeFormat format);
#ifdef DATE_STRING_SUPPORT_ENABLED
void ds3231_read_date_time(int32_t* val, char* buffer, TimeFormat format);
#endif // DATE_STRING_SUPPORT_ENABLED

/**
 *  Reads time from the DS3231 and returns the epoch
 */
time_t ds3231_get_epoch(EC *result);


/**
 *  Set year, month, date, day, hours, minutes and seconds of the DS3231 to the time passed in the argument.
 * dateString has the format 2018-03-23T18:00:00
 * ClockSetting setting = clock or alarm to be set
 */
	void ds3231_set_date_time(char * dateString, ClockSetting setting);

/**
 * Returns day of week given any date
 * year = 20xx
 * month = mm
 * day = dd
 * Returns 1=Sun, ... 7=Sat
 */
//const uint8_t wd(int year, int month, int day);


/**
 *
 */
	BOOL ds3231_get_temp(int16_t * val);

/**
 *  Turn on/off 1-second square wave on the INT/SQW pin.
 */
	void ds3231_1s_sqw(BOOL enable);


/**
 *
 */
	void ds3231_set_aging(int8_t* data);

/**
 *
 */
	int8_t ds3231_get_aging(void);



#endif  /* DS3231_H_ */