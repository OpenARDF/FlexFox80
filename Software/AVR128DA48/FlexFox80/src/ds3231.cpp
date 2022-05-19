/*
 *  MIT License
 *
 *  Copyright (c) 2022 DigitalConfections
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 * ds3231.c
 *
 */

#include "defs.h"

   #include "ds3231.h"
   #include <util/twi.h>
   #include <stdio.h>
   #include <time.h>
   #include "i2c.h"
   #include "util.h"

   #define DS3231_I2C_SLAVE_ADDR ((uint8_t)0xD0)   /* corresponds to slave address = 0b1101000x */

   #define RTC_SECONDS                     0x00
   #define RTC_MINUTES                     0x01
   #define RTC_HOURS                       0x02
   #define RTC_DAY                         0x03
   #define RTC_DATE                        0x04
   #define RTC_MONTH_CENT                  0x05
   #define RTC_YEAR                        0x06
   #define RTC_ALARM1_SECONDS              0x07
   #define RTC_ALARM1_MINUTES              0x08
   #define RTC_ALARM1_HOUR                 0x09
   #define RTC_ALARM1_DAY_DATE             0x0A
   #define RTC_ALARM2_MINUTES              0x0B
   #define RTC_ALARM2_HOUR                 0x0C
   #define RTC_ALARM2_DAY_DATE             0x0D
   #define RTC_CONTROL                     0x0E
   #define RTC_CONTROL_STATUS              0x0F
   #define RTC_AGING                       0x10
   #define RTC_TEMP_MSB                    0x11
   #define RTC_TEMP_LSB                    0x12
   
#if INCLUDE_DS3231_SUPPORT

#ifdef FOOBAR
const uint8_t wd(int year, int month, int day)
{
	static const uint8_t weekdayname[] = {2 /*Mon*/, 3 /*Tue*/, 4 /*Wed*/, 5 /*Thu*/, 6 /*Fri*/, 7 /*Sat*/, 1 /*Sun*/};
	size_t JND =
	day
	+ ((153 * (month + 12 * ((14 - month) / 12) - 3) + 2) / 5)
	+ (365 * (year + 4800 - ((14 - month) / 12)))
	+ ((year + 4800 - ((14 - month) / 12)) / 4)
	- ((year + 4800 - ((14 - month) / 12)) / 100)
	+ ((year + 4800 - ((14 - month) / 12)) / 400)
	- 32045;
	return weekdayname[JND % 7];
}
#endif

time_t epoch_from_ltm(tm *ltm);

bool ds3231_init()
{
	I2C_0_Init();
	return (!ds3231_responding());
}

uint8_t bcd2dec(uint8_t val)
{
	uint8_t result = 10 * (val >> 4) + (val & 0x0F);
	return( result);
}

uint8_t dec2bcd(uint8_t val)
{
	uint8_t result = val % 10;
	result |= (val / 10) << 4;
	return (result);
}

uint8_t char2bcd(char c[])
{
	uint8_t result = (c[1] - '0') + ((c[0] - '0') << 4);
	return( result);
}


/* Returns the UNIX epoch value for the character string passed in datetime. If datetime is null then it returns
the UNIX epoch for the time held in the DS3231 RTC. If error is not null then it holds 1 if an error occurred */
	time_t RTC_String2Epoch(bool *error, char *datetime)
	{
		time_t epoch = 0;
		uint8_t data[7] = { 0, 0, 0, 0, 0, 0, 0 };

		struct tm ltm = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		int16_t year = 100;                 /* start at 100 years past 1900 */
		uint8_t month;
		uint8_t date;
		uint8_t hours;
		uint8_t minutes;
		uint8_t seconds;

		if(datetime)                            /* String format "YYMMDDhhmmss" */
		{
			data[0] = char2bcd(&datetime[10]);  /* seconds in BCD */
			data[1] = char2bcd(&datetime[8]);   /* minutes in BCD */
			data[2] = char2bcd(&datetime[6]);   /* hours in BCD */
			/* data[3] =  not used */
			data[4] = char2bcd(&datetime[4]);   /* day of month in BCD */
			data[5] = char2bcd(&datetime[2]);   /* month in BCD */
			data[6] = char2bcd(&datetime[0]);   /* 2-digit year in BCD */

			hours = bcd2dec(data[2]); /* Must be calculated here */

			year += (int16_t)bcd2dec(data[6]);
			ltm.tm_year = year;                         /* year since 1900 */

			year += 1900;                               /* adjust year to calendar year */

			month = bcd2dec(data[5]);
			ltm.tm_mon = month - 1;                     /* mon 0 to 11 */

			date = bcd2dec(data[4]);
			ltm.tm_mday = date;                         /* month day 1 to 31 */

			ltm.tm_yday = 0;
			for(uint8_t mon = 1; mon < month; mon++)    /* months from 1 to 11 (excludes partial month) */
			{
				ltm.tm_yday += month_length(year, mon);;
			}

			ltm.tm_yday += (ltm.tm_mday - 1);

			seconds = bcd2dec(data[0]);
			minutes = bcd2dec(data[1]);

			ltm.tm_hour = hours;
			ltm.tm_min = minutes;
			ltm.tm_sec = seconds;

			epoch = epoch_from_ltm(&ltm);
		}
		else
		{
			epoch = ds3231_get_epoch(NULL);
		}

		if(error)
		{
			*error = (epoch == 0);
		}

		return(epoch);
	}

	time_t epoch_from_ltm(tm *ltm)
	{
		time_t epoch = ltm->tm_sec + ltm->tm_min * 60 + ltm->tm_hour * 3600L + ltm->tm_yday * 86400L +
		(ltm->tm_year - 70) * 31536000L + ((ltm->tm_year - 69) / 4) * 86400L -
		((ltm->tm_year - 1) / 100) * 86400L + ((ltm->tm_year + 299) / 400) * 86400L;

		return(epoch);
	}

time_t ds3231_get_epoch(EC *result)
{
	uint8_t tries = 10; /* try several times in case of transient bus issues */
	time_t epoch = 0;
	uint8_t data[7] = { 0, 0, 0, 0, 0, 0, 0 };
	bool res;

	while(tries-- && (res = (I2C_0_GetData(DS3231_I2C_SLAVE_ADDR, RTC_SECONDS, data, 7) != 7)));

	if(!res)
	{
		struct tm ltm = {0};
		int16_t year = 100; // start at 100 years past 1900
		uint8_t month;
		uint8_t date;
		uint8_t hours;
		uint8_t minutes;
		uint8_t seconds;
		bool am_pm;
		bool twelvehour;

		year += data[6] & 0x0f;
		year += 10*((data[6] & 0xf0) >> 4);
		ltm.tm_year = year; // year since 1900

		year += 1900; // adjust year to calendar year

		month = data[5] & 0x0f;
		month += 10*((data[5] & 0xf0) >> 4);
		ltm.tm_mon = month - 1; // mon 0 to 11

		date = data[4] & 0x0f;
		date += 10*((data[4] & 0xf0) >> 4);

		ltm.tm_mday = date; // month day 1 to 31

		ltm.tm_yday = 0;
		for(uint8_t mon=1; mon<month; mon++) // months from 1 to 11 (excludes partial month)
		{
		  ltm.tm_yday += month_length(year, mon);;
		}

		ltm.tm_yday += (ltm.tm_mday - 1);

		seconds = 10 * ((data[0] & 0xf0) >> 4);
		seconds += (data[0] & 0x0f);

		minutes = 10 * ((data[1] & 0xf0) >> 4);
		minutes += (data[1] & 0x0f);

		am_pm = ((data[2] >> 5) & 0x01);

		hours = 10 * ((data[2] >> 4) & 0x01);
		hours += (data[2] & 0x0f);

		twelvehour = ((data[2] >> 6) & 0x01);

		if(twelvehour)
		{
			if(am_pm) hours += 12;
		}
		else // am_pm holds 20 hours flag
		{
			if(am_pm) hours += 20;
		}

		ltm.tm_hour = hours;
		ltm.tm_min = minutes;
		ltm.tm_sec = seconds;

		epoch = ltm.tm_sec + ltm.tm_min*60 + ltm.tm_hour*3600L + ltm.tm_yday*86400L +
		(ltm.tm_year-70)*31536000L + ((ltm.tm_year-69)/4)*86400L -
		((ltm.tm_year-1)/100)*86400L + ((ltm.tm_year+299)/400)*86400L;
	}

	if(result) *result = res ? ERROR_CODE_RTC_NONRESPONSIVE : ERROR_CODE_NO_ERROR;
	return epoch;
}

	bool ds3231_get_temp(int16_t * val)
	{
		uint8_t tries = 10; /* try several times in case of transient bus issues */
		uint8_t data[2] = { 0, 0 };
		bool result;
		
		while(tries-- && (result = (I2C_0_GetData(DS3231_I2C_SLAVE_ADDR, RTC_TEMP_MSB, data, 2) != 2)));

		if(!result)
		{
			*val = data[0];
			*val = *val << 8;
			*val |= data[1];
		}

		return(result);
	}


bool ds3231_set_date_time_arducon(char *datetime, ClockSetting setting) /* String format "YYMMDDhhmmss" */
{
	uint8_t tries = 10; /* try several times in case of transient bus issues */
	bool failure = true;
	uint8_t data[7] = { 0, 0, 0, 0, 0, 0, 0 };

	if(datetime)                            
	{
		data[0] = char2bcd(&datetime[10]);  /* seconds in BCD */
		data[1] = char2bcd(&datetime[8]);   /* minutes in BCD */
		data[2] = char2bcd(&datetime[6]);   /* hours in BCD 24-hour format */
		/* data[3] =  not used */
		data[4] = char2bcd(&datetime[4]);   /* day of month in BCD */
		data[5] = char2bcd(&datetime[2]);   /* month in BCD */
		data[6] = char2bcd(&datetime[0]);   /* 2-digit year in BCD */

		while(tries-- && (failure = (I2C_0_SendData(DS3231_I2C_SLAVE_ADDR, RTC_SECONDS+(setting*7), data, 7) != 7)));
	}

	return(failure);
}


void ds3231_set_date_time(char * dateString, ClockSetting setting) /* "2018-03-23T18:00:00Z" */
{
	uint8_t tries = 10; /* try several times in case of transient bus issues */
	uint8_t data[7] = { 0, 0, 0, 1, 0, 0, 0 };
	int temp, year=2000, month, date;

	data[0] = dateString[18] - '0'; /* seconds */
	data[0] |= ((dateString[17] - '0') << 4); /*10s of seconds */
	data[1] = dateString[15] - '0'; /* minutes */
	data[1] |= ((dateString[14] - '0') << 4); /* 10s of minutes */
	data[2] = dateString[12] - '0'; /* hours */
	data[2] |= ((dateString[11] - '0') << 4); /* 10s of hours - sets 24-hour format (not AM/PM) */
	//data[3] = Skip day of week
	data[4] = dateString[9] - '0'; /* day of month digit 1 */
	date = data[5];
	temp = dateString[8] - '0';
	date += 10*temp;
	data[4] |= (temp << 4); /* day of month digit 10 */
	data[5] = dateString[6] - '0'; /* month digit 1 */
	month = data[6];
	temp = dateString[5] - '0';
	month += 10*temp;
	data[5] |= (temp << 4 ); /* month digit 10; century=0 */
	data[6] = dateString[3] - '0'; /* year digit 1 */
	year += data[7];
	temp = dateString[2] - '0';
	year += 10*temp;
	data[6] |= (temp << 4); /* year digit 10 */

	while(tries-- && ((I2C_0_SendData(DS3231_I2C_SLAVE_ADDR, RTC_SECONDS+(setting*7), data, 7)) != 7));
}


#ifdef DATE_STRING_SUPPORT_ENABLED
	bool ds3231_read_date_time(int32_t* val, char* buffer, TimeFormat format)
	{
		uint8_t tries = 10; /* try several times in case of transient bus issues */
		bool failure;
		uint8_t data[7] = { 0, 0, 0, 0, 0, 0, 0 };
		uint8_t month;
		uint8_t date;
		uint16_t year = 2000;
		uint8_t second10;
		uint8_t second;
		uint8_t minute10;
		uint8_t minute;
		uint8_t hour10;
		uint8_t hour;
		bool am_pm;
		bool twelvehour;
		
		while(tries-- && (failure = I2C_0_GetData(DS3231_I2C_SLAVE_ADDR, RTC_SECONDS, data, 7) != 7));

		if(!failure)
		{
			second10 = ((data[0] & 0xf0) >> 4);
			second = (data[0] & 0x0f);

			minute10 = ((data[1] & 0xf0) >> 4);
			minute = (data[1] & 0x0f);

			am_pm = ((data[2] >> 5) & 0x01);
			hour10 = ((data[2] >> 4) & 0x01);
			hour = (data[2] & 0x0f);

			twelvehour = ((data[2] >> 6) & 0x01);

			if(!twelvehour && am_pm)
			{
				hour10 = 2;
			}

			if(buffer)
			{
				switch(format)
				{
					case Minutes_Seconds:
					{
						sprintf(buffer, "%1d%1d:%1d%1d", minute10, minute, second10, second);
					}
					break;

					case Hours_Minutes_Seconds:
					{
						if(twelvehour)  /* 12-hour */
						{
							sprintf(buffer, "%1d%1d:%1d%1d:%1d%1d%s", hour10, hour, minute10, minute, second10, second, am_pm ? "AM" : "PM");
						}
						else            /* 24 hour */
						{
							sprintf(buffer, "%1d%1d:%1d%1d:%1d%1d", hour10, hour, minute10, minute, second10, second);
						}
					}
					break;

					default:    /* Day_Month_Year_Hours_Minutes_Seconds: */
					{
						date = data[4] & 0x0f;
						date += 10*((data[4] & 0xf0) >> 4);
						month = data[5] & 0x0f;
						month += 10*((data[5] & 0xf0) >> 4);
						year += data[6] & 0x0f;
						year += 10*((data[6] & 0xf0) >> 4);

						sprintf(buffer, "%4d-%02d-%02dT%1d%1d:%1d%1d:%1d%1d", year, month, date, hour10, hour, minute10, minute, second10, second);

						if(val)
						{
							*val = convertTimeStringToEpoch(buffer);
						}
					}
					break;
				}
			}

			if(val)
			{
				*val = second + 10 * second10 + 60 * (int32_t)minute + 600 * (int32_t)minute10 +  3600 * (int32_t)hour + 36000 * (int32_t)hour10;
			}
		}
		
		return(failure);
	}
#endif // DATE_STRING_SUPPORT_ENABLED
	
	bool ds3231_1s_sqw(bool enable)
	{
		uint8_t tries = 10; /* try several times in case of transient bus issues */
		bool failure;
		uint8_t data[1];
		
		data[0] = enable ? 0x00:0x04;	
		while(tries-- && (failure = (I2C_0_SendData(DS3231_I2C_SLAVE_ADDR, RTC_CONTROL, data, 1) != 1)));
		
		return(failure);
	}


	bool ds3231_set_aging(int8_t data_in)
	{
		uint8_t tries = 10; /* try several times in case of transient bus issues */
		bool failure;
		int8_t data[1];
		
		data[0] = data_in;
		while(tries-- && (failure = (I2C_0_SendData(DS3231_I2C_SLAVE_ADDR, RTC_AGING, (uint8_t *)data, 1) != 1)));
		return(failure);
	}


	int8_t ds3231_get_aging()
	{
		uint8_t tries = 10; /* try several times in case of transient bus issues */
		bool failure;
		int8_t data[1];
		
		while(tries-- && (failure = (I2C_0_GetData(DS3231_I2C_SLAVE_ADDR, RTC_AGING, (uint8_t *)data, 1) != 1)));
		return(data[0]);
	}

	bool ds3231_responding()
	{
		bool responseReceived = false;
		uint8_t tries = 10;
		int8_t data[1];		
		while(tries-- && !(responseReceived = (I2C_0_GetData(DS3231_I2C_SLAVE_ADDR, RTC_AGING, (uint8_t *)data, 1) == 1)));
		return(responseReceived);
	}


/* This simple synchronization approach works for all times except 12 midnight. If synchronization
results in the advancement to the next day, then one day would be lost. Instead of introducing that
error, this function merely fails to synchronize at midnight. */
bool ds3231_sync2nearestMinute()
{
	uint8_t tries = 10; /* try several times in case of transient bus issues */
	bool err = false;
	uint8_t data[8] = { 0, 0, 0 };
		
	while(tries-- && (err = (I2C_0_GetData(DS3231_I2C_SLAVE_ADDR, RTC_SECONDS, (uint8_t *)data, 3) != 3)));

	if(!err)
	{
		uint8_t hours;
		uint8_t minutes;
		uint8_t seconds;

		uint8_t hour10;
		uint8_t hour;
		bool am_pm;
		bool twelvehour;

		seconds = bcd2dec(data[0]);
		minutes = bcd2dec(data[1]);
		am_pm = ((data[2] >> 5) & 0x01);
		hour10 = ((data[2] >> 4) & 0x01);
		hour = (data[2] & 0x0f);

		twelvehour = ((data[2] >> 6) & 0x01);

		if(!twelvehour && am_pm)
		{
			hour10 = 2;
		}

		hours = 10 * hour10 + hour;

		if(seconds > 30)
		{
			minutes++;

			if(minutes > 59)
			{
				minutes = 0;
				hours++;

				if(hours > 23) /* Don't attempt to synchronize at midnight */
				{
					err = true;
				}
			}
		}

		if(!err)
		{
			data[0] = 0; /* seconds = 00 */
			data[1] = dec2bcd(minutes);
			data[2] = 0;

			if(twelvehour)
			{
				data[2] |= 0x40; /* set  12-hour bit */

				if(hours >= 12)
				{
					data[2] |= 0x20; /* set pm bit */
				}

				if(hours >= 10)
				{
					data[2] |= 0x10;
				}
			}
			else
			{
				if(hours >= 20)
				{
					data[2] |= 0x20; /* set 20 bit */
				}
				else if(hours >= 10)
				{
					data[2] |= 0x10; /* set 10 bit */
				}
			}

			data[2] |= hours % 10;
			
			while(tries-- && (err = (I2C_0_SendData(DS3231_I2C_SLAVE_ADDR, RTC_SECONDS, (uint8_t *)data, 4) != 4)));
		}
	}

	return err;
}

/**
 *   Converts an epoch (seconds since 1900)  into a string with format "ddd dd-mon-yyyy hh:mm:ss zzz"
 */
#define THIRTY_YEARS 946684800
char* convertEpochToTimeString(time_t epoch, char* buf, size_t size)
 {
   struct tm  ts;
	time_t t = epoch - THIRTY_YEARS;

    // Format time, "ddd dd-mon-yyyy hh:mm:ss zzz"
    ts = *localtime(&t);
    strftime(buf, size, "%a %d-%b-%Y %H:%M:%S", &ts);
   return buf;
 }


#endif  /* #ifdef INCLUDE_DS3231_SUPPORT */


