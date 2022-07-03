
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
**********************************************************************************************/

#ifndef DEFS_H
#define DEFS_H

#ifndef F_CPU
 #define F_CPU 24000000UL /* gets declared in makefile */
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

/******************************************************
 * Set the text that gets displayed to the user */
#define SW_REVISION "0.3"

//#define TRANQUILIZE_WATCHDOG

#define PRODUCT_NAME_SHORT "FlexFox ARDF Tx"
#define PRODUCT_NAME_LONG "FlexFox 80m ARDF Transmitter"

/*******************************************************/

/******************************************************
 * Include only the necessary hardware support */
   #define INCLUDE_SI5351_SUPPORT true // Silicon Labs Programmable Clock
   #define INCLUDE_DS3231_SUPPORT true // Maxim RTC
//   #define INCLUDE_DAC081C085_SUPPORT
//   #define ENABLE_PIN_CHANGE_INTERRUPT_0
//   #define ENABLE_PIN_CHANGE_INTERRUPT_1
//   #define ENABLE_PIN_CHANGE_INTERRUPT_2

// #ifdef INCLUDE_DAC081C085_SUPPORT
//    #define PA_DAC DAC081C_I2C_SLAVE_ADDR_A0
// #endif

//   #define AM_DAC DAC081C_I2C_SLAVE_ADDR_A1
//   #define BIAS_POT MCP4552_I2C_SLAVE_ADDR_A0

	#define DATE_STRING_SUPPORT_ENABLED

/*******************************************************/
/* Error Codes                                                                   */
/*******************************************************/
typedef enum {
	ERROR_CODE_NO_ERROR = 0x00,
	ERROR_CODE_REPORT_NO_ERROR = 0x01,
//	ERROR_CODE_2M_BIAS_SM_NOT_READY = 0xC6,
	ERROR_CODE_EVENT_STATION_ID_ERROR = 0xC7,
	ERROR_CODE_EVENT_PATTERN_CODE_SPEED_NOT_SPECIFIED = 0xC8,
	ERROR_CODE_EVENT_PATTERN_NOT_SPECIFIED = 0xC9,
	ERROR_CODE_EVENT_TIMING_ERROR = 0xCA,
	ERROR_CODE_EVENT_MISSING_TRANSMIT_DURATION = 0xCB,
	ERROR_CODE_EVENT_MISSING_START_TIME = 0xCC,
	ERROR_CODE_EVENT_NOT_CONFIGURED = 0xCD,
    ERROR_CODE_ILLEGAL_COMMAND_RCVD = 0xCE,
    ERROR_CODE_SW_LOGIC_ERROR = 0xCF,
	ERROR_CODE_EVENT_ENDED_IN_PAST = 0xD0,
	ERROR_CODE_ATMEGA_NOT_RESPONDING = 0xD1,
	ERROR_CODE_RF_OSCILLATOR_ERROR = 0xD2,
	ERROR_CODE_POWER_LEVEL_NOT_SUPPORTED = 0xF5,
	ERROR_CODE_NO_ANTENNA_PREVENTS_POWER_SETTING = 0xF6,
	ERROR_CODE_NO_ANTENNA_FOR_BAND = 0xF7,
	ERROR_CODE_WD_TIMEOUT = 0xF8,
	ERROR_CODE_SUPPLY_VOLTAGE_ERROR = 0xF9,
	ERROR_CODE_BUCK_REG_OUTOFSPEC = 0xFA,
	ERROR_CODE_CLKGEN_NONRESPONSIVE = 0xFB,
	ERROR_CODE_RTC_NONRESPONSIVE = 0xFC,
	ERROR_CODE_DAC3_NONRESPONSIVE = 0xFD,
	ERROR_CODE_DAC2_NONRESPONSIVE = 0xFE,
	ERROR_CODE_DAC1_NONRESPONSIVE = 0xFF
	} EC;

/*******************************************************/
/* Status Codes                                                                 */
/*******************************************************/
typedef enum {
	STATUS_CODE_IDLE = 0x00,
	STATUS_CODE_REPORT_IDLE = 0x01,
	STATUS_CODE_NO_ANT_ATTACHED = 0xE9,
//	STATUS_CODE_2M_ANT_ATTACHED = 0xEA,
	STATUS_CODE_80M_ANT_ATTACHED = 0xEB,
	STATUS_CODE_RECEIVING_EVENT_DATA = 0xEC,
	STATUS_CODE_RETURNED_FROM_SLEEP = 0xED,
	STATUS_CODE_BEGINNING_XMSN_THIS_CYCLE = 0xEE,
	STATUS_CODE_SENDING_ID = 0xEF,
	STATUS_CODE_NO_EVENT_TO_RUN = 0xFA,
	STATUS_CODE_EVENT_NEVER_ENDS = 0xFB,
	STATUS_CODE_EVENT_FINISHED = 0xFC,
	STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING = 0xFD,
	STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT = 0xFE,
	STATUS_CODE_WAITING_FOR_EVENT_START = 0xFF
	} SC;

typedef enum {
	DO_NOT_SLEEP,
	SLEEP_UNTIL_START_TIME,
	SLEEP_UNTIL_NEXT_XMSN,
	SLEEP_FOREVER
	} SleepType;
	
	
/*******************************************************/

#ifndef uint16_t_defined
#define uint16_t_defined
typedef unsigned int uint16_t;
#endif

#ifndef uint32_t_defined
#define uint32_t_defined
typedef unsigned long uint32_t;
#endif

#ifndef unit8_t_defined
#define unit8_t_defined
typedef unsigned char uint8_t;
#endif

#ifndef null
#define null 0
#endif

#ifndef PI
#define PI 3.141592653589793
#endif

#ifndef MINUTE
#define MINUTE 60UL
#endif

#ifndef HOUR
#define HOUR 3600UL
#endif

#ifndef DAY
#define DAY 86400UL
#endif

#ifndef YEAR
#define YEAR 31536000UL
#endif

/*******************************************************/
/*******************************************************
* ADC Scale Factors */
/* Battery voltage should be read when +12V supply is enabled and all transmitters are fully powered off */
#define ADC_REF_VOLTAGE_mV 1100UL

#define ADC_MAX_VOLTAGE_MV 4200L /* maximum voltage the ADC can read */
#define BATTERY_VOLTAGE_MAX_MV 4200L /* voltage at which the battery is considered to be fully charged */
#define BATTERY_DROP 320L /* voltage drop between the battery terminals and the ADC input while powering the ESP8266 */
#define BATTERY_DROP_OFFSET (BATTERY_DROP * 1023L)
#define VBAT(x) (BATTERY_DROP + (x * ADC_MAX_VOLTAGE_MV) / 1023L)
#define BATTERY_PERCENTAGE(x, y) ( ( 100L * ((x * ADC_MAX_VOLTAGE_MV + BATTERY_DROP_OFFSET) - (1023L * y)) )  / ((BATTERY_VOLTAGE_MAX_MV - y) * 1023L))

#define SUPPLY_VOLTAGE_MAX_MV 14100L
#define VSUPPLY(x)((x * SUPPLY_VOLTAGE_MAX_MV) / 1023L)

#define PA_VOLTAGE_MAX_MV 14100L
#define VPA(x)((x * PA_VOLTAGE_MAX_MV) / 1023L)

typedef uint16_t BatteryLevel;  /* in milliVolts */

#define VOLTS_5 (((5000L - BATTERY_DROP) * 1023L) / BATTERY_VOLTAGE_MAX_MV)
#define VOLTS_3_19 (((3190L - BATTERY_DROP) * 1023L) / BATTERY_VOLTAGE_MAX_MV)
#define VOLTS_3_0 (((3000L - BATTERY_DROP) * 1023L) / BATTERY_VOLTAGE_MAX_MV)
#define VOLTS_2_4 (((2400L - BATTERY_DROP) * 1023L) / BATTERY_VOLTAGE_MAX_MV)

#define POWER_OFF_VOLT_THRESH_MV VOLTS_2_4 /* 2.4 V = 2400 mV */
#define POWER_ON_VOLT_THRESH_MV VOLTS_3_0  /* 3.0 V = 3000 mV */

#define ANTENNA_DETECT_THRESH 20
#define ANTENNA_DETECT_DEBOUNCE 50

#define NUMBER_OF_ESSENTIAL_EVENT_PARAMETERS 12


/*******************************************************/

#ifndef SELECTIVELY_DISABLE_OPTIMIZATION
	#define SELECTIVELY_DISABLE_OPTIMIZATION
#endif

/******************************************************
 * EEPROM definitions */
#define EEPROM_INITIALIZED_FLAG 0x00CE
#define EEPROM_UNINITIALIZED 0x00

#define EEPROM_STATION_ID_DEFAULT "FOXBOX"
#define EEPROM_PATTERN_TEXT_DEFAULT "PARIS|"

#define EEPROM_START_TIME_DEFAULT 0
#define EEPROM_FINISH_TIME_DEFAULT 0
#define EEPROM_EVENT_ENABLED_DEFAULT false
#define EEPROM_ID_CODE_SPEED_DEFAULT 20
#define EEPROM_PATTERN_CODE_SPEED_DEFAULT 8
#define EEPROM_ON_AIR_TIME_DEFAULT 60
#define EEPROM_OFF_AIR_TIME_DEFAULT 240
#define EEPROM_INTRA_CYCLE_DELAY_TIME_DEFAULT 0
#define EEPROM_ID_TIME_INTERVAL_DEFAULT 300

#define EEPROM_SI5351_CALIBRATION_DEFAULT 0x00
#define EEPROM_CLK0_OUT_DEFAULT 133000000
#define EEPROM_CLK1_OUT_DEFAULT 70000000
#define EEPROM_CLK2_OUT_DEFAULT 10700000
#define EEPROM_CLK0_ONOFF_DEFAULT OFF
#define EEPROM_CLK1_ONOFF_DEFAULT OFF
#define EEPROM_CLK2_ONOFF_DEFAULT OFF

#define EEPROM_BATTERY_EMPTY_MV 3430
#define MAX_UNLOCK_CODE_LENGTH 8
#define EEPROM_DTMF_UNLOCK_CODE_DEFAULT "1357"
#define MIN_UNLOCK_CODE_LENGTH 4
#define MIN_CODE_SPEED_WPM (uint8_t)5
#define MAX_CODE_SPEED_WPM (uint8_t)20
#define MAX_RF_POWER_MW (uint16_t)5000
#define MIN_RF_POWER_MW (uint16_t)0
#define EEPROM_START_EPOCH_DEFAULT 0
#define EEPROM_FINISH_EPOCH_DEFAULT 0
#define EEPROM_UTC_OFFSET_DEFAULT 0
#define EEPROM_FOX_SETTING_DEFAULT FOX_1
#define TEXT_SET_TIME_TXT (char*)"CLK T YYMMDDhhmmss <- Set current time\n"
#define TEXT_SET_START_TXT (char*)"CLK S YYMMDDhhmmss <- Set start time\n"
#define TEXT_SET_FINISH_TXT (char*)"CLK F YYMMDDhhmmss <- Set finish time\n"
#define TEXT_SET_ID_TXT (char*)"ID [\"callsign\"] <- Set callsign\n"
#define TEXT_ERR_FINISH_BEFORE_START_TXT (char*)"Err: Finish before start!\n"
#define TEXT_ERR_FINISH_IN_PAST_TXT (char*)"Err: Finish in past!\n"
#define TEXT_ERR_START_IN_PAST_TXT (char*)"Err: Start in past!\n"
#define TEXT_ERR_INVALID_TIME_TXT (char*)"Err: Invalid time!\n"
#define TEXT_ERR_TIME_IN_PAST_TXT (char*)"Err: Time in past!\n"
#define TEXT_RTC_NOT_RESPONDING_TXT (char*)"Error: No response from clock hardware\n"
#define TEXT_TX_NOT_RESPONDING_TXT (char*)"Error: No response from transmit hardware\n"
#define TEXT_WIFI_NOT_DETECTED_TXT (char*)"Warning: WiFi hardware not detected\n"
#define MINIMUM_EPOCH ((time_t)1609459200)  /* 1 Jan 2021 00:00:00 */
#define SECONDS_24H 86400

typedef enum
{
	NULL_CONFIG,
	WAITING_FOR_START,
	CONFIGURATION_ERROR,
	SCHEDULED_EVENT_DID_NOT_START,
	SCHEDULED_EVENT_WILL_NEVER_RUN,
	EVENT_IN_PROGRESS
} ConfigurationState_t;



/******************************************************
 * General definitions for making the code easier to understand */

#ifndef Frequency_Hz
	typedef uint32_t Frequency_Hz;
#endif

#define ON              1
#define OFF             0
#define TOGGLE			2
#define HIGH			1
#define LOW				0
#define UNDETERMINED	3

#define MIN(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#define MAX(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })

#define CLAMP(low, x, high) ({\
  __typeof__(x) __x = (x); \
  __typeof__(low) __low = (low);\
  __typeof__(high) __high = (high);\
  __x > __high ? __high : (__x < __low ? __low : __x);\
  })

#define MAX_TIME 4294967295UL
#define MAX_UINT16 65535
#define MAX_INT16 32767

typedef enum
{
	DOWN = -1,
	NOCHANGE = 0,
	UP = 1,
	SETTOVALUE
} IncrType;

typedef enum
{
	ANT_CONNECTION_UNDETERMINED,
	ANT_DISCONNECTED,
	ANT_CONNECTED
} AntConnType;


typedef enum
{
	BEACON = 0,
	FOX_1,
	FOX_2,
	FOX_3,
	FOX_4,
	FOX_5,
	FOXORING,
	SPECTATOR,
	SPRINT_S1,
	SPRINT_S2,
	SPRINT_S3,
	SPRINT_S4,
	SPRINT_S5,
	SPRINT_F1,
	SPRINT_F2,
	SPRINT_F3,
	SPRINT_F4,
	SPRINT_F5,
	INVALID_FOX
	#if SUPPORT_TEMP_AND_VOLTAGE_REPORTING
	,
	REPORT_BATTERY
	#endif // SUPPORT_TEMP_AND_VOLTAGE_REPORTING
} Fox_t;

#define QUAD_MASK 0xC0
#define QUAD_A 7
#define QUAD_B 6

#define MAX_TONE_VOLUME_SETTING 15
#define TONE_POT_VAL(x) (255 - (x*17))
#define MAX_MAIN_VOLUME_SETTING 15

#define POWER_OFF_DELAY 5000
#define BACKLIGHT_OFF_DELAY 5000
#define BACKLIGHT_ALWAYS_ON 65535
#define HEADPHONE_REMOVED_DELAY 100
#define POWERUP_LOW_VOLTAGE_DELAY 900   /* A short delay at first power up before declaring battery is too low */
#define LOW_VOLTAGE_DELAY 9000          /* A longer delay if the receiver has been running and the battery starts to sag */
#define CURSOR_EXPIRATION_DELAY 5000    /* Keep cursor displayed this long without user action */
#define LONG_PRESS_TICK_COUNT 1200      /* Press a button for this many ticks in order to access a long-press function */

#define SEND_ID_DELAY 4100

/* Periodic TIMER2 interrupt timing definitions */
#define OCR2A_OVF_FREQ_300 0x0C
#define OCR2A_OVF_FREQ_600 0x06
#define OCR2A_OVF_FREQ_720 0x05
#define OCR2A_OVF_BASE_FREQ OCR2A_OVF_FREQ_720
#define TIMER2_57HZ (120/OCR2A_OVF_BASE_FREQ)
#define TIMER2_20HZ (588/OCR2A_OVF_BASE_FREQ)
#define TIMER2_5_8HZ (1200/OCR2A_OVF_BASE_FREQ)
#define TIMER2_0_5HZ (12000/OCR2A_OVF_BASE_FREQ)

#define BEEP_SHORT 100
#define BEEP_LONG 65535

/******************************************************
 * UI Hardware-related definitions */

typedef enum lcdRow
{
	ROW0,
	ROW1,
	NUMBER_OF_LCD_ROWS
} LcdRowType;

typedef enum lcdColumn
{
	COL0,
	COL1,
	COL2,
	COL3,
	COL4,
	COL5,
	COL6,
	COL7,
	COL8,
	COL9,
	COL10,
	COL11,
	COL12,
	COL13,
	COL14,
	COL15,
	COL16,
	COL17,
	COL18,
	COL19,
	NUMBER_OF_LCD_COLS,
	INVALID_LCD_COLUMN
} LcdColType;

typedef enum
{
	BUTTON1_COLUMN = COL0,
	BUTTON2_COLUMN = COL5,
	BUTTON3_COLUMN = COL10,
	BUTTON4_COLUMN = COL15
} ButtonColumn;

typedef enum
{
	FrequencyFormat,
	HourMinuteSecondFormat,
	HourMinuteSecondDateFormat
} TextFormat;

#define DISPLAY_WIDTH_STRING_SIZE (NUMBER_OF_LCD_COLS + 1)

typedef uint8_t BackLightSettingType;
#define BL_OFF 0xFF
#define BL_LOW 0xCF
#define BL_MED 0x8F
#define BL_HIGH 0x00

typedef uint8_t ContrastType;

typedef enum volumeSetting
{
	VOL_ZERO = 0,
	VOL_10,
	VOL_20,
	VOL_30,
	VOL_40,
	VOL_50,
	VOL_60,
	VOL_70,
	VOL_80,
	VOL_90,
	VOL_100,
	DECREMENT_VOL,
	INCREMENT_VOL,
	VOL_NOT_SPECIFIED
} VolumeSetting;

typedef enum volumeType
{
	TONE_VOLUME,
	MAIN_VOLUME
} VolumeType;

typedef enum batteryType
{
	BATTERY_9V,
	BATTERY_4r2V,
	BATTERY_EXTERNAL,
	BATTERY_UNKNOWN
} BatteryType;

typedef enum buttons
{
	BUTTON1,
	BUTTON2,
	BUTTON3,
	BUTTON4,
	NUMBER_OF_BUTTONS
} ButtonType;

typedef enum
{
	Minutes_Seconds,                        /* minutes up to 59 */
	Hours_Minutes_Seconds,                  /* hours up to 23 */
	Day_Month_Year_Hours_Minutes_Seconds,   /* Year up to 99 */
	Minutes_Seconds_Elapsed,                /* minutes up to 99 */
	Time_Format_Not_Specified
} TimeFormat;

#define NO_TIME_SPECIFIED (-1)

#define SecondsFromHours(hours) ((hours) * 3600)
#define SecondsFromMinutes(min) ((min) * 60)

typedef enum
{
	PATTERN_TEXT,
	STATION_ID
} TextIndex;

#define MAX_PATTERN_TEXT_LENGTH 20

typedef enum
{
	POWER_UP,
	PUSHBUTTON,
	PROGRAMMATIC,
	NO_ACTION
} EventActionSource_t;

typedef enum
{
	START_NOTHING,
	START_EVENT_NOW,
	START_TRANSMISSIONS_NOW,
	START_EVENT_WITH_STARTFINISH_TIMES
} EventAction_t;

typedef enum
{
	INIT_NOT_SPECIFIED,
	INIT_EVENT_STARTING_NOW,
	INIT_TRANSMISSIONS_STARTING_NOW,
	INIT_EVENT_IN_PROGRESS_WITH_STARTFINISH_TIMES
} InitializeAction_t;
// 
// typedef enum
// {
// 	AUDIO_SAMPLING,
// 	TEMPERATURE_SAMPLING,
// 	VOLTAGE_SAMPLING
// } ADCChannel_t;


#endif  /* DEFS_H */





