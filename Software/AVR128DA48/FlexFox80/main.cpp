/*
 *  MIT License
 *
 *  Copyright (c) 2021-2026 DigitalConfections
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
 */

/*
 * Top-level FlexFox80 AVR runtime, interrupt, schedule, and command orchestration.
 */

#include "atmel_start.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <atomic.h>

#include "linkbus.h"
#include "serialbus.h"
#include "transmitter.h"
#include "ds3231.h"
#include "morse.h"
#include "adc.h"
#include "Goertzel.h"
#include "util.h"
#include "binio.h"
#include "eeprommanager.h"
#include "binio.h"
#include "leds.h"
#include "linkbus.h"
#include "huzzah.h"
#include "CircularStringBuff.h"
#include "event_schedule_state.h"
#include "event_time_state.h"
#include "rtc_sync_guard.h"
#include "avr_update_contract.h"
#include "wifi_power_lease.h"
#include "rstctrl.h"

#include <cpuint.h>
#include <ccp.h>
#include <atomic.h>


/***********************************************************************
 * Local Typedefs
 ************************************************************************/

typedef enum
{
	WD_SW_RESETS,
	WD_HW_RESETS,
	WD_FORCE_RESET,
	WD_DISABLE
} WDReset;

typedef enum
{
	AWAKENED_INIT,
	POWER_UP_START,
	AWAKENED_BY_CLOCK,
	AWAKENED_BY_ANTENNA,
	AWAKENED_BY_BUTTONPRESS
} Awakened_t;

typedef enum
{
	HARDWARE_OK,
	HARDWARE_NO_RTC = 0x01,
	HARDWARE_NO_SI5351 = 0x02,
	HARDWARE_NO_WIFI = 0x04,
	HARDWARE_NO_12V = 0x08,
	HARDWARE_NO_FET_BIAS = 0x10
} HardwareError_t;



/***********************************************************************
 * Global Variables & String Constants
 *
 * Identify each global with a "g_" prefix
 * Whenever possible limit globals' scope to this file using "static"
 * Use "volatile" for globals shared between ISRs and foreground
 ************************************************************************/
#define TEMPSTR_SIZE 100
#define RTC_SYNC_WAIT_TIMEOUT_MS 1500
static char g_tempStr[TEMPSTR_SIZE] = { '\0' };
static volatile EC g_last_error_code = ERROR_CODE_NO_ERROR;
static volatile SC g_last_status_code = STATUS_CODE_IDLE;

static volatile bool g_powering_off = false;

static volatile bool g_battery_measurements_active = false;
static volatile uint16_t g_maximum_battery = 0;

static volatile bool g_antenna_connection_changed = true;
volatile AntConnType g_antenna_connect_state = ANT_CONNECTION_UNDETERMINED;

static volatile bool g_start_event = false;
static volatile bool g_end_event = false;

static volatile int32_t g_on_the_air = 0;
static volatile int g_sendID_seconds_countdown = 0;
static volatile uint16_t g_code_throttle = 50;
static volatile uint16_t g_enunciation_code_throttle = 50;
static volatile uint8_t g_WiFi_shutdown_seconds = WIFI_NORMAL_SHUTDOWN_SECONDS;
static volatile uint16_t g_wifi_update_lease_seconds = 0;
static volatile bool g_report_seconds = false;
static volatile bool g_wifi_active = true;
static volatile bool g_clone_quiet = false;
static volatile bool g_clone_sync_report_armed = false;
static volatile bool g_clone_sync_report_ready = false;
static volatile time_t g_clone_sync_epoch = 0;
static volatile uint16_t g_clone_quiet_timeout_seconds = 0;
static volatile uint8_t g_wifi_enable_delay = 0;
static volatile bool g_shutting_down_wifi = false;
static volatile bool g_wifi_ready = false;
static volatile bool g_bootloader_reset_pending = false;
static uint8_t g_bootloader_protocol = 0;
static uint8_t g_bootloader_version_major = 0;
static uint8_t g_bootloader_version_minor = 0;
static volatile uint16_t g_hardware_error = (uint16_t)HARDWARE_OK;

static void renewWifiUpdateLease(void)
{
	ENTER_CRITICAL(wifi_update_lease_renew);
	g_wifi_update_lease_seconds = wifiUpdateLeaseRenew();
	EXIT_CRITICAL(wifi_update_lease_renew);
}

static void releaseWifiUpdateLease(void)
{
	ENTER_CRITICAL(wifi_update_lease_release);
	wifiUpdateLeaseRelease(&g_wifi_update_lease_seconds);
	EXIT_CRITICAL(wifi_update_lease_release);
}

static bool wifiUpdateLeaseActive(void)
{
	uint16_t seconds;
	ENTER_CRITICAL(wifi_update_lease_read);
	seconds = g_wifi_update_lease_seconds;
	EXIT_CRITICAL(wifi_update_lease_read);
	return seconds != 0;
}

extern Frequency_Hz g_tx_frequency;
char g_messages_text[STATION_ID+1][MAX_PATTERN_TEXT_LENGTH + 1];
volatile uint8_t g_id_codespeed = EEPROM_ID_CODE_SPEED_DEFAULT;
volatile uint8_t g_pattern_codespeed = EEPROM_PATTERN_CODE_SPEED_DEFAULT;
volatile uint8_t g_foxoring_pattern_codespeed = EEPROM_FOXORING_PATTERN_CODESPEED_DEFAULT;
volatile uint16_t g_time_needed_for_ID = 0;
volatile int16_t g_on_air_seconds = EEPROM_ON_AIR_TIME_DEFAULT;                      /* amount of time to spend on the air */
volatile int16_t g_off_air_seconds = EEPROM_OFF_AIR_TIME_DEFAULT;                    /* amount of time to wait before returning to the air */
volatile int16_t g_intra_cycle_delay_time = EEPROM_INTRA_CYCLE_DELAY_TIME_DEFAULT;   /* offset time into a repeating transmit cycle */
volatile int16_t g_ID_period_seconds = EEPROM_ID_TIME_INTERVAL_DEFAULT;              /* amount of time between ID/callsign transmissions */
volatile time_t g_event_start_epoch = EEPROM_START_TIME_DEFAULT;
volatile time_t g_event_finish_epoch = EEPROM_FINISH_TIME_DEFAULT;
volatile bool g_event_enabled = EEPROM_EVENT_ENABLED_DEFAULT;                        /* indicates that the conditions for executing the event are set */
volatile bool g_event_scheduled = false;											 /* Indicates that an event that starts in the future is scheduled */
volatile bool g_event_commenced = false;
volatile bool g_check_for_next_event = false;
volatile bool g_waiting_for_next_event = false;
volatile bool g_seconds_transition = false;

volatile bool g_sending_station_ID = false;											/* Allows a small extension of transmissions to ensure the ID is fully sent */

static volatile bool g_sufficient_power_detected = false;
static volatile bool g_enableHardwareWDResets = false;
extern volatile bool g_tx_power_is_zero;

static volatile bool g_go_to_sleep_now = false;
static volatile bool g_sleeping = false;
static volatile time_t g_time_to_wake_up = 0;
static volatile Awakened_t g_awakenedBy = POWER_UP_START;
static volatile SleepType g_sleepType = SLEEP_FOREVER;

// #define NUMBER_OF_POLLED_ADC_CHANNELS 4
// static const uint16_t g_adcChannelConversionPeriod_ticks[NUMBER_OF_POLLED_ADC_CHANNELS] = { TIMER2_0_5HZ, TIMER2_0_5HZ, TIMER2_0_5HZ, TIMER2_5_8HZ };
// static volatile uint16_t g_adcCountdownCount[NUMBER_OF_POLLED_ADC_CHANNELS] = { TIMER2_0_5HZ, TIMER2_0_5HZ, TIMER2_0_5HZ, TIMER2_5_8HZ };
// static uint16_t g_ADCFilterThreshold[NUMBER_OF_POLLED_ADC_CHANNELS] = { 500, 500, 500, 500 };
// static volatile bool g_adcUpdated[NUMBER_OF_POLLED_ADC_CHANNELS] = { false, false, false, false };
// static volatile uint16_t g_lastConversionResult[NUMBER_OF_POLLED_ADC_CHANNELS];
#define NUMBER_OF_POLLED_ADC_CHANNELS 4
#define ADC_TEMPERATURE_PERIOD_TICKS 300U
static ADC_Active_Channel_t g_adcChannelOrder[NUMBER_OF_POLLED_ADC_CHANNELS] = { ADCExternalBatteryVoltage, ADC12VRegulatedVoltage, ADCTXAdjustableVoltage, ADCTemperature };
enum ADC_Result_Order { EXTERNAL_BATTERY_VOLTAGE, REGULATED_12V, PA_VOLTAGE, INTERNAL_TEMPERATURE };
static const uint16_t g_adcChannelConversionPeriod_ticks[NUMBER_OF_POLLED_ADC_CHANNELS] = { TIMER2_5_8HZ, TIMER2_0_5HZ, TIMER2_0_5HZ, ADC_TEMPERATURE_PERIOD_TICKS };
/* Make temperature the first conversion so it is available during startup. */
static volatile uint16_t g_adcCountdownCount[NUMBER_OF_POLLED_ADC_CHANNELS] = { 100, 1000, 2000, 10 };
//static uint16_t g_ADCFilterThreshold[NUMBER_OF_POLLED_ADC_CHANNELS] = { 500, 500, 500 };
static volatile bool g_adcUpdated[NUMBER_OF_POLLED_ADC_CHANNELS] = { false, false, false, false };
static volatile uint16_t g_lastConversionResult[NUMBER_OF_POLLED_ADC_CHANNELS];
static volatile bool g_timer_launched_new_event = false;

extern Goertzel g_goertzel;
volatile uint16_t g_switch_closed_time = 0;
volatile uint16_t g_handle_counted_presses = 0;
volatile uint16_t g_switch_presses_count = 0;
volatile bool g_long_button_press = false;
volatile uint16_t g_check_temperature = 0;

Enunciation_t g_enunciator = LED_ONLY;
static volatile bool g_do_powerup_xmsn = false;

uint16_t g_Event_Configuration_Check = 0;

leds LEDS = leds();
CircularStringBuff g_text_buff = CircularStringBuff(TEXT_BUFF_SIZE);

EepromManager g_ee_mgr;

Fox_t g_fox[] = {FOX_1, FOX_1, SPRINT_S1, FOXORING_FOX1, INVALID_FOX}; /* none, classic, sprint, foxoring, blind */
extern Event_t g_event;
extern Fox_t g_foxoring_fox;
extern Frequency_Hz g_foxoring_frequencyA;
extern Frequency_Hz g_foxoring_frequencyB;
extern Frequency_Hz g_foxoring_frequencyC;
extern Frequency_Hz g_tx_frequency;
Frequency_Hz g_frequency_low = EEPROM_FREQUENCY_LOW_DEFAULT;
Frequency_Hz g_frequency_med = EEPROM_FREQUENCY_MED_DEFAULT;
Frequency_Hz g_frequency_hi = EEPROM_FREQUENCY_HI_DEFAULT;
Frequency_Hz g_frequency_beacon = EEPROM_FREQUENCY_BEACON_DEFAULT;
int8_t g_utc_offset;
uint8_t g_unlockCode[UNLOCK_CODE_SIZE + 1];
bool g_use_rtc_for_startstop = false;

volatile bool g_enable_manual_transmissions = false;
volatile bool g_enable_LED_enunciations = true;
volatile uint16_t g_delay_before_powerup_xmsn = 0;

void setEventStartEpoch(time_t value)
{
	ENTER_CRITICAL(event_start_epoch_store);
	g_event_start_epoch = value;
	EXIT_CRITICAL(event_start_epoch_store);
}

void setEventFinishEpoch(time_t value)
{
	ENTER_CRITICAL(event_finish_epoch_store);
	g_event_finish_epoch = value;
	EXIT_CRITICAL(event_finish_epoch_store);
}

void setEventEpochs(time_t start, time_t finish)
{
	ENTER_CRITICAL(event_epochs_store);
	g_event_start_epoch = start;
	g_event_finish_epoch = finish;
	EXIT_CRITICAL(event_epochs_store);
}

static void setWakeTimeFromForeground(time_t value)
{
	ENTER_CRITICAL(wake_time_store);
	g_time_to_wake_up = value;
	EXIT_CRITICAL(wake_time_store);
}

static void setOnTheAirFromForeground(int32_t value)
{
	ENTER_CRITICAL(on_the_air_store);
	g_on_the_air = value;
	EXIT_CRITICAL(on_the_air_store);
}

/***********************************************************************
 * Private Function Prototypes
 *
 * These functions are available only within this file
 ************************************************************************/
void handle_1sec_tasks(void);
EC syncSystemTimeToRTC(void);
EC waitForRTCSecondTransition(void);
bool eventEnabled(void);
void handleLinkBusMsgs(void);
void enterCloneQuietMode(void);
void resumeNormalReports(void);
void armCloneSyncReport(void);
void serviceCloneSyncReport(void);
void wdt_init(WDReset resetType);
uint16_t throttleValue(uint8_t speed);
EC activateEventUsingCurrentSettings(SC* statusCode);
EC launchEvent(SC* statusCode);
EC hw_init(void);
EC rtc_init(void);
bool setAntennaStateEnunciator(void);
void powerDown3V3(void);
void powerUp3V3(void);
void setFan(bool on);
bool fanIsOn(void);

uint16_t timeNeededForID(void);
Frequency_Hz getFrequencySetting(void);
char* getCurrentPatternText(void);
Fox_t getFoxSetting(void);
bool eventScheduled(void);
bool eventScheduledForNow(void);
bool eventScheduledForTheFuture(void);
bool noEventWillRun(void);

char* externBatString(bool volts);
int repChar(char *str, char orig, char rep);
char *trimwhitespace(char *str);

/*******************************/
/* Hardcoded event support     */
/*******************************/
void initializeAllEventSettings(bool disableEvent);
void suspendEvent(void);
void stopEventNow(EventActionSource_t activationSource);
void startEventNow(EventActionSource_t activationSource);
void startEventUsingRTC(void);
void setupForFox(Fox_t fox, EventAction_t action);
time_t validateTimeString(char* str, time_t* epochVar);
bool reportTimeTill(time_t from, time_t until, const char* prefix, const char* failMsg);
void makeTimeTillString(char* str, time_t from, time_t until, bool* fail);
ConfigurationState_t clockConfigurationCheck(void);
void reportConfigErrors(void);
/*******************************/
/* End hardcoded event support */
/*******************************/

/**
PORTA interrupts:
One-second counter based on RTC 1-second square wave output.
*/
ISR(PORTA_PORT_vect)
{
	uint8_t x = VPORTA.INTFLAGS;
	VPORTA.INTFLAGS = x; /* Clear captured flags before slow work so a later edge remains pending. */
	
    if(x & (1 << RTC_SQW)) /* Handle 1-second interrupt */
    {
		uint8_t elapsed_seconds = rtcElapsedEdges();

		do
		{
			system_tick();

			/* If edges were recovered, report the newest recovered second. */
			if(g_clone_sync_report_armed && (elapsed_seconds == 1))
			{
				g_clone_sync_epoch = time(NULL);
				g_clone_sync_report_armed = false;
				g_clone_sync_report_ready = true;
			}

			if(g_clone_quiet_timeout_seconds)
			{
				g_clone_quiet_timeout_seconds--;
				if(!g_clone_quiet_timeout_seconds)
				{
					g_clone_quiet = false;
					g_clone_sync_report_armed = false;
					g_clone_sync_report_ready = false;
				}
			}

			if(g_on_the_air < 0)
			{
				g_on_the_air++;
			}
			else if(g_on_the_air > 0)
			{
				g_on_the_air--;
			}
		
			g_seconds_transition = true;

			if(g_sleeping)
			{
				if(g_sleepType == SLEEP_UNTIL_NEXT_XMSN)
				{
					if((g_on_the_air > -6) || (g_time_to_wake_up <= time(null))) /* Always wake up at least 5 seconds before showtime */
					{
						g_go_to_sleep_now = false;
						g_sleeping = false;
						g_awakenedBy = AWAKENED_BY_CLOCK;
					}
				}
				else
				{
					if(g_time_to_wake_up <= time(null))
					{
						g_go_to_sleep_now = false;
						g_sleeping = false;
						g_awakenedBy = AWAKENED_BY_CLOCK;
						/* The pre-start deadline has been consumed. A later WiFi shutdown
						 * must not re-enter this already-expired sleep mode. */
						g_sleepType = DO_NOT_SLEEP;
						g_on_the_air = 0; /* This will cause this variable to be properly initialized in the fast interrupt */
						g_timer_launched_new_event = true;
					}
				}
			}
			else
			{
				handle_1sec_tasks();
			}
		} while(--elapsed_seconds);
	}
}


/**
1-Second Interrupts:
One-second counter based on RTC.
*/
void handle_1sec_tasks(void)
{
	time_t temp_time = 0;

	if(g_check_temperature) g_check_temperature--;
	
	if(g_event_commenced)
	{
		if(g_event_finish_epoch && !g_check_for_next_event && !g_shutting_down_wifi)
		{
			temp_time = time(null);

			if(temp_time >= g_event_finish_epoch)
			{
				g_last_status_code = STATUS_CODE_EVENT_FINISHED;
				g_on_the_air = 0;
				keyTransmitter(OFF);
				g_event_enabled = false;
				g_event_commenced = false;
				g_check_for_next_event = true;
				g_wifi_enable_delay = 2;
				g_sleepType = SLEEP_FOREVER;
					
				if(g_hardware_error & (uint16_t)HARDWARE_NO_WIFI)
				{
					g_check_for_next_event = false;
					g_go_to_sleep_now = true;
				}
				else
				{
					LEDS.init();
				}
			}
		}
	}

	if(g_event_enabled)
	{
		if(g_event_commenced) /* an event is in progress */
		{
			if(g_sendID_seconds_countdown)
			{
				g_sendID_seconds_countdown--;
			}
		}
		else /* waiting for the start time to arrive */
		{
			if(g_event_start_epoch > MINIMUM_VALID_EPOCH) /* a start time has been set */
			{
				temp_time = time(null);

				if(temp_time >= g_event_start_epoch) /* Time for the event to start */
				{
					/* WiFi clients may intentionally keep the ESP powered across the
					 * start. Retire the pre-start sleep mode before that delayed WiFi
					 * shutdown can request sleep during the running event. */
					g_sleepType = DO_NOT_SLEEP;
					powerToTransmitter(ON);
				
// 					if(g_intra_cycle_delay_time)
// 					{
// 						g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
// //						g_on_the_air = -g_intra_cycle_delay_time;
// 						g_sendID_seconds_countdown = g_intra_cycle_delay_time + g_on_air_seconds - g_time_needed_for_ID;
// 					}
// 					else
// 					{
// 						g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
// //						g_on_the_air = g_on_air_seconds;
// 						g_sendID_seconds_countdown = g_on_air_seconds - g_time_needed_for_ID;
// 						g_code_throttle = throttleValue(g_pattern_codespeed);
// 						bool repeat = true;
// 						makeMorse(getCurrentPatternText(), &repeat, NULL, CALLER_AUTOMATED_EVENT);
// 					}
					
					g_on_the_air = 0; /* This will cause this variable to be properly initialized in the fast interrupt */

					g_event_commenced = true;
					g_timer_launched_new_event = true;
					LEDS.init();
				}
			}
		}
	}


	/**************************************
	* Delay before re-enabling linkbus receive
	***************************************/
	if(g_wifi_enable_delay)
	{
		g_wifi_enable_delay--;

		if(!g_wifi_enable_delay)
		{
			wifi_power(ON);     /* power on WiFi */
			wifi_reset(OFF);    /* bring WiFi out of reset */
			g_wifi_update_lease_seconds = 0;
			g_WiFi_shutdown_seconds = WIFI_NORMAL_SHUTDOWN_SECONDS;
		}
	}
	else
	{
		bool wifi_shutdown_due = false;
		if(g_wifi_update_lease_seconds)
		{
			/* The maintenance lease is independent of event/sleep scheduling.
			 * Even if those state flags become stale, a stopped updater cannot
			 * leave the battery-powered WiFi regulator enabled indefinitely. */
			wifi_shutdown_due = wifiUpdateLeaseTick(&g_wifi_update_lease_seconds);
		}
		else if(g_shutting_down_wifi || (!g_check_for_next_event && !g_waiting_for_next_event))
		{
			if(g_WiFi_shutdown_seconds)
			{
				g_WiFi_shutdown_seconds--;
				wifi_shutdown_due = !g_WiFi_shutdown_seconds && !g_enable_manual_transmissions;
			}
		}

		if(wifi_shutdown_due)
		{
			g_wifi_ready = false;
			wifi_reset(ON);     /* put WiFi into reset */
			wifi_power(OFF);    /* power off WiFi */
			g_shutting_down_wifi = false;
			g_wifi_active = false;

			if(g_sleepType != DO_NOT_SLEEP)
			{
				g_go_to_sleep_now = true;
			}
		}

		if(g_wifi_active)
		{
			g_report_seconds = true;
		}
	}
}

/**
PORTD interrupts:
Antenna connection interrupt.
*/
ISR(PORTD_PORT_vect)
{
	uint8_t x = VPORTD.INTFLAGS;
	
	if(x & (1 << X80M_ANTENNA_DETECT)) /* Handle antenna change */
	{
		if(g_sleeping)
		{
			g_go_to_sleep_now = false;
			g_sleeping = false;
			g_waiting_for_next_event = false; /* Ensure the wifi module does not get shut off prematurely */
		}
		
		g_awakenedBy = AWAKENED_BY_ANTENNA;	/* Flag that this interrupt has occurred regardless whether CPU was sleeping */
	}

    VPORTD.INTFLAGS = 0xFF; /* Clear all PORTD interrupt flags */
}



/**
Periodic tasks not requiring precise timing. Rate = 300 Hz
*/
ISR(TCB0_INT_vect)
{
	static uint8_t fiftyMS = 6;
	static bool on_air_finished = false;
	static bool transitionPrepped = false;
	static bool intra_cycle_delay_counting = false;
	
	uint8_t x = TCB0.INTFLAGS;
	
    if(x & TCB_CAPT_bm)
    {
		static bool conversionInProcess = false;
		static int8_t indexConversionInProcess = 0;
		static uint8_t conversionTimeoutTicks = 0;
		static uint16_t codeInc = 0;
		bool repeat, finished;
		static uint16_t switch_closures_count_period = 0;
		uint8_t holdSwitch = 0;
		static uint8_t buttonReleased = false;
		static uint8_t longPressEnabled = true;
		static bool muteAfterID = false;				/* Inhibit any transmissions immediately after the ID has been sent */
		static bool hold_event_commenced = false;
		
		if(g_delay_before_powerup_xmsn)
		{
			g_delay_before_powerup_xmsn--;
		}

		fiftyMS++;
		if(!(fiftyMS % 6))
		{
			holdSwitch = portCdebouncedVals() & (1 << SWITCH);
			debounce();
			
			if(holdSwitch != (portCdebouncedVals() & (1 << SWITCH))) /* Change detected */
			{
				if(holdSwitch) /* Switch was open, so now it must be closed */
				{
					if(LEDS.active())
					{
						g_switch_presses_count++;
						buttonReleased = false;
					}
					else
					{
						longPressEnabled = false;
					}
				}
				else /* Switch is now open */
				{
					if(!LEDS.active())
					{
						LEDS.init();
						serialbus_init(SB_BAUD, SERIALBUS_USART);
						setAntennaStateEnunciator(); /* Set green LED appropriately */
					}
					else
					{
						g_switch_closed_time = 0;
						buttonReleased = true;
					}
					
					longPressEnabled = true;
				}
			}
			else if(!holdSwitch && LEDS.active()) /* Switch closed, LEDs operating */
			{
				if(!g_long_button_press && longPressEnabled)
				{
					if(++g_switch_closed_time >= 200)
					{
						g_long_button_press = true;
						g_switch_closed_time = 0;
						g_switch_presses_count = 0;
						longPressEnabled = false;
					}
				}
			}
		
			if(switch_closures_count_period)
			{
				switch_closures_count_period--;
				
				if(!switch_closures_count_period)
				{
					if(g_switch_presses_count && (g_switch_presses_count < 3))
					{
						g_handle_counted_presses = g_switch_presses_count;
					}
					
					g_switch_presses_count = 0;
				}
			}
			else if(g_switch_presses_count == 1 && buttonReleased)
			{
				switch_closures_count_period = 50;
			}
			else if(g_switch_presses_count > 2)
			{
				g_switch_presses_count = 0;
			}
			
			if(portDdebouncedVals() & (1 << X80M_ANTENNA_DETECT)) 
			{
				if(g_antenna_connect_state != ANT_DISCONNECTED)
				{
					g_antenna_connect_state = ANT_DISCONNECTED;
					g_antenna_connection_changed = true;
				}
			}
			else 
			{
				if(g_antenna_connect_state != ANT_CONNECTED)
				{
					g_antenna_connect_state = ANT_CONNECTED;
					g_antenna_connection_changed = true;
				}
			}
		}
							
		static bool key = false;
		
		/* Ensure that whenever an event commences, makeMorse() immediately gets initialized with the correct pattern text */
		if(hold_event_commenced != g_event_commenced)
		{
			hold_event_commenced = g_event_commenced;
			
			if(g_event_commenced)
			{
				g_code_throttle = throttleValue(g_pattern_codespeed);
				bool repeat = true;
				makeMorse(getCurrentPatternText(), &repeat, NULL, CALLER_AUTOMATED_EVENT);
			}
		}

		if(g_event_enabled && g_event_commenced) /* Handle cycling transmissions */
		{
			if((g_on_the_air > 0) || (g_sending_station_ID) || (!g_off_air_seconds && g_on_the_air))
			{
				on_air_finished = true;
				transitionPrepped = false;
				intra_cycle_delay_counting = false;
				
				if(!g_sending_station_ID && (!g_off_air_seconds || (g_on_the_air <= g_time_needed_for_ID)) && !g_sendID_seconds_countdown && g_time_needed_for_ID)
				{
					g_last_status_code = STATUS_CODE_SENDING_ID;
					g_code_throttle = throttleValue(g_id_codespeed);
					repeat = false;
					makeMorse(g_messages_text[STATION_ID], &repeat, NULL, CALLER_AUTOMATED_EVENT);  /* Send only once */
					g_sending_station_ID = true;
					g_sendID_seconds_countdown = g_ID_period_seconds;
				}
				
				if(codeInc)
				{
					codeInc--;

					if(!codeInc)
					{
						key = makeMorse(NULL, &repeat, &finished, CALLER_AUTOMATED_EVENT);
						
						if(!repeat && finished) /* ID has completed, so resume pattern */
						{
							g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
							g_code_throttle = throttleValue(g_pattern_codespeed);
							repeat = true;
							makeMorse(getCurrentPatternText(), &repeat, NULL, CALLER_AUTOMATED_EVENT);
							muteAfterID = g_sending_station_ID && g_off_air_seconds;
							g_sending_station_ID = false;
							if(!g_off_air_seconds)
							{
								g_on_the_air = g_on_air_seconds;
							}
						}
						
						codeInc = g_code_throttle;
					}
				}
				else
				{
					codeInc = g_code_throttle;
				}
				
				if(muteAfterID)
				{
					key = OFF;
				}
				
				keyTransmitter(key);
				LEDS.setRed(key);
			}
			else if(g_on_the_air < 0)
			{
				transitionPrepped = false;
			}
			else if(!g_on_the_air)
			{
				if(g_timer_launched_new_event)
				{
					g_timer_launched_new_event = false;	
					
					if(g_intra_cycle_delay_time)
					{
						g_on_the_air = -g_intra_cycle_delay_time; /* wait the delay time */
						intra_cycle_delay_counting = true;	
						transitionPrepped = true;				
					}
					else
					{
						transitionPrepped = false;
					}
					
					on_air_finished = false;
				}
				else if (intra_cycle_delay_counting)
				{
					intra_cycle_delay_counting = false;
					transitionPrepped = false;
					on_air_finished = false;
				}

				if(!transitionPrepped)
				{
					transitionPrepped = true;
										
					if(on_air_finished) /* A transmission just finished */
					{
						on_air_finished = false;
						
						key = OFF;
						keyTransmitter(OFF);
						
						if(g_off_air_seconds)
						{
							g_on_the_air = -g_off_air_seconds;

							/* Enable sleep during off-the-air periods */
							int32_t timeRemaining = 0;
							time_t temp_time = time(null);
							if(temp_time < g_event_finish_epoch)
							{
								timeRemaining = timeDif(g_event_finish_epoch, temp_time);
								g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
							}

							/* Don't sleep for the last cycle to ensure that the event doesn't end while
							* the transmitter is sleeping - which can cause problems with loading the next event */
							if(timeRemaining > (g_off_air_seconds + g_on_air_seconds + 15))
							{
								if((g_off_air_seconds > 15) && !g_WiFi_shutdown_seconds)
								{
									time_t seconds_to_sleep = (time_t)(g_off_air_seconds - 10);
									g_time_to_wake_up = temp_time + seconds_to_sleep;
									g_sleepType = SLEEP_UNTIL_NEXT_XMSN;
									g_go_to_sleep_now = true;
									g_sendID_seconds_countdown = MAX(0, g_ID_period_seconds - (int)seconds_to_sleep);
								}
							}
	
							muteAfterID = false;
							g_sending_station_ID = false;
							LEDS.setRed(OFF);
						}
						else
						{
							g_on_the_air = g_on_air_seconds;
						}
					}
					else /* Off-the-air period just finished */
					{
						g_on_the_air = g_on_air_seconds;
					
						muteAfterID = false;
						g_sending_station_ID = false;
						LEDS.setRed(OFF);
						
						g_code_throttle = throttleValue(g_pattern_codespeed);
						repeat = true;
						makeMorse(getCurrentPatternText(), &repeat, NULL, CALLER_AUTOMATED_EVENT);
						codeInc = g_code_throttle;
					}
				}
			}
		}
		else if(g_enable_manual_transmissions || g_enable_LED_enunciations) /* Handle enunciations & character string transmissions */
		{
			static bool charFinished = true;
			static bool idle = true;
			bool sendBuffEmpty = g_text_buff.empty();
			repeat = false;
			
			if(lastMorseCaller() != CALLER_MANUAL_TRANSMISSIONS)
			{
				g_text_buff.reset();
				sendBuffEmpty = true;
				charFinished = true;
				makeMorse((char*)"\0", &repeat, null, CALLER_MANUAL_TRANSMISSIONS); 
			}
			
			if(sendBuffEmpty && charFinished)
			{
				if(!idle)
				{
					if(key)
					{
						key = OFF;
						keyTransmitter(OFF);
						LEDS.setRed(OFF);
					}

					if(g_enunciator == LED_AND_RF)
					{
						powerToTransmitter(OFF);
					}
				
					g_text_buff.setBusy(false); /* free the text buffer for other users */
					codeInc = g_enunciation_code_throttle;
					idle = true;
				}
			}
			else 
			{
				idle = false;
				
				if(codeInc)
				{
					codeInc--;

					if(!codeInc)
					{
						key = makeMorse(null, &repeat, &charFinished, CALLER_MANUAL_TRANSMISSIONS);

						if(charFinished) /* Completed, send next char */
						{
							if(!g_text_buff.empty())
							{
								static char cc[2]; /* Must be static because makeMorse saves only a pointer to the character array */
								g_enunciation_code_throttle = MIN(g_enunciation_code_throttle, throttleValue(g_pattern_codespeed));
								cc[0] = g_text_buff.get();
								cc[1] = '\0';
								makeMorse(cc, &repeat, null, CALLER_MANUAL_TRANSMISSIONS);
								key = makeMorse(null, &repeat, &charFinished, CALLER_MANUAL_TRANSMISSIONS);
								g_text_buff.setBusy(true); /* ensure other buffer users don't interfere with the last character being sent */
							}
						}

						if(g_enunciator == LED_AND_RF) keyTransmitter(key);
						LEDS.setRed(key);
						codeInc = g_enunciation_code_throttle;
					}
				}
				else
				{
					if(g_enunciator == LED_AND_RF) keyTransmitter(key);
					LEDS.setRed(key);
					codeInc = g_enunciation_code_throttle;
				}
			}
		}

		/**
		 * Handle Periodic ADC Readings
		 * The following algorithm allows multiple ADC channel readings to be performed at different polling intervals. */
 		if(!conversionInProcess)
 		{
			/* Note: countdowns will pause while a conversion is in process. Conversions are so fast that this should not be an issue though. */
			indexConversionInProcess = -1;

			for(uint8_t i = 0; i < NUMBER_OF_POLLED_ADC_CHANNELS; i++)
			{
				if(g_adcCountdownCount[i])
				{
					g_adcCountdownCount[i]--;
				}

				if(g_adcCountdownCount[i] == 0)
				{
					indexConversionInProcess = (int8_t)i;
				}
			}

			if(indexConversionInProcess >= 0)
			{
				g_adcCountdownCount[indexConversionInProcess] = g_adcChannelConversionPeriod_ticks[indexConversionInProcess];    /* reset the tick countdown */
				ADC0_setADCChannel(g_adcChannelOrder[indexConversionInProcess]);
				ADC0_startConversion();
				conversionInProcess = true;
				conversionTimeoutTicks = 3; /* 10 ms at the 300 Hz task rate */
			}
		}
		else if(ADC0_conversionDone())   /* wait for conversion to complete */
		{
			static uint16_t holdConversionResult;
			uint16_t hold = ADC0_read(); //ADC;
			
			if(g_adcChannelOrder[indexConversionInProcess] == ADCTemperature)
			{
				ADC0_recordTemperatureResult(hold);
			}
			else if((hold > 10) && (hold < 4090))
			{
				holdConversionResult = hold; // (uint16_t)(((uint32_t)hold * ADC_REF_VOLTAGE_mV) >> 10);    /* millivolts at ADC pin */
				uint16_t lastResult = g_lastConversionResult[indexConversionInProcess];

				g_adcUpdated[indexConversionInProcess] = true;

	// 			if(g_adcChannelOrder[indexConversionInProcess] == ADCExternalBatteryVoltage)
	// 			{
	// 				bool directionUP = holdConversionResult > lastResult;
	// 				uint16_t delta = directionUP ? holdConversionResult - lastResult : lastResult - holdConversionResult;
	// 
	// 				if(delta > g_ADCFilterThreshold[indexConversionInProcess])
	// 				{
	// 					lastResult = holdConversionResult;
	// 					g_adcCountdownCount[indexConversionInProcess] = 100; /* speed up next conversion */
	// 				}
	// 				else
	// 				{
	// 					if(directionUP)
	// 					{
	// 						lastResult++;
	// 					}
	// 					else if(delta)
	// 					{
	// 						lastResult--;
	// 					}
	// 
	// 					g_battery_measurements_active = true;
	// 				}
	// 			}
	// 			else
	// 			{
 					lastResult = holdConversionResult;
	// 			}

				g_lastConversionResult[indexConversionInProcess] = lastResult;
			}
			else
			{
				hold = g_lastConversionResult[indexConversionInProcess];
			}

			conversionInProcess = false;
		}
		else if(conversionTimeoutTicks && !(--conversionTimeoutTicks))
		{
			/* Recover rather than leaving ADC sampling wedged indefinitely. */
			if(g_adcChannelOrder[indexConversionInProcess] == ADCTemperature)
			{
				ADC0_markTemperatureUnavailable();
			}
			ADC0_setADCChannel(ADCShutdown);
			conversionInProcess = false;
		}
    }

    TCB0.INTFLAGS = (TCB_CAPT_bm | TCB_OVF_bm); /* clear all interrupt flags */
}

/**
Handle switch closure interrupts
*/
ISR(PORTC_PORT_vect)
{
	uint8_t x = VPORTC.INTFLAGS;
	
	if(x & (1 << SWITCH))
	{
		if(g_sleeping)
		{
			g_go_to_sleep_now = false;
			g_sleeping = false;
			g_awakenedBy = AWAKENED_BY_BUTTONPRESS;	
			g_waiting_for_next_event = false; /* Ensure the wifi module does not get shut off prematurely */
		}
	}
	
	VPORTC.INTFLAGS = 0xFF; /* Clear all flags */
}


void powerDown3V3(void)
{
	powerToTransmitter(OFF); /* Turn off power to final FET */
	PORTA_set_pin_level(V3V3_PWR_ENABLE, LOW);	
	PORTB_set_pin_level(MAIN_PWR_ENABLE, LOW);
}

void powerUp3V3(void)
{
	powerToTransmitter(OFF); /* Turn off power to final FET */
	PORTB_set_pin_level(MAIN_PWR_ENABLE, HIGH);  /* Turn on 12V booster circuit */
	PORTA_set_pin_level(V3V3_PWR_ENABLE, HIGH);  /* Enable 3V3 power regulator */
}

void setFan(bool on)
{
	if(on)
	{
		PORTA_set_pin_level(FAN_ENABLE, HIGH);  /* Turn fan on */
	}
	else
	{
		PORTA_set_pin_level(FAN_ENABLE, LOW);  /* Turn fan off */
	}
}

bool fanIsOn(void)
{
	return(PORTA_get_pin_level(FAN_ENABLE) > 0);
}

#include "dac0.h"

int main(void)
{
	time_t holdTime = 0;
	if((GPR.GPR1 & 0xF0U) == FLEXFOX_AVR_BOOT_HANDOFF_INFO_MAGIC)
	{
		g_bootloader_protocol = GPR.GPR1 & FLEXFOX_AVR_BOOT_HANDOFF_INFO_PROTOCOL_MASK;
		g_bootloader_version_major = GPR.GPR2;
		g_bootloader_version_minor = GPR.GPR3;
	}
	GPR.GPR1 = 0;
	GPR.GPR2 = 0;
	GPR.GPR3 = 0;
	
	atmel_start_init();
	LEDS.blink(LEDS_OFF, true);
	powerUp3V3();
	setFan(ON);
	
	g_ee_mgr.initializeEEPROMVars();
	g_ee_mgr.readNonVols();
	
	wifi_reset(ON);
	wifi_power(ON);
	util_delay_ms(0);
	while(util_delay_ms(1000) && !wifiPresent());
	
	if(wifiPresent())
	{
		wifi_power(OFF);
	}
	else
	{
		wifi_power(OFF);
		wifi_reset(ON);
		g_wifi_enable_delay = 0;
		g_WiFi_shutdown_seconds = 0;
		g_hardware_error |= (uint16_t)HARDWARE_NO_WIFI;
//		sb_send_string(TEXT_WIFI_NOT_DETECTED_TXT);	
	}
	
	if(init_transmitter() != ERROR_CODE_NO_ERROR)
	{
		if(!txIsInitialized())
		{
			g_hardware_error |= (uint16_t)HARDWARE_NO_SI5351;
//			sb_send_string(TEXT_TX_NOT_RESPONDING_TXT);	
		}
	}
	
	util_delay_ms(0);
	while(util_delay_ms(1000));
	
	EC code = rtc_init();
	
	util_delay_ms(0);
	while(util_delay_ms(7000) && (code == ERROR_CODE_RTC_NONRESPONSIVE))
	{
		code = rtc_init();
	}
	
	if(code == ERROR_CODE_RTC_NONRESPONSIVE)
	{
		g_hardware_error |= (uint16_t)HARDWARE_NO_RTC;
	}
	else
	{
		syncSystemTimeToRTC();
		g_event_scheduled = eventScheduled();
	}
	
	g_wifi_enable_delay = 3;
	g_start_event = eventEnabled(); /* Set flag to start any event stored in EEPROM */
	
	while (1) {
		handleLinkBusMsgs();
		if(g_bootloader_reset_pending)
		{
			/* Allow the final Linkbus ACK to leave the UART before reset removes ESP power. */
			while(linkbusTxInProgress()) { }
			util_delay_ms(0);
			while(util_delay_ms(25)) { }
			GPR.GPR1 = FLEXFOX_AVR_BOOT_APP_UPDATE_REQUEST;
			RSTCTRL_reset();
			while(1) { }
		}
		serviceCloneSyncReport();
		
		if(g_handle_counted_presses)
		{
			if(g_handle_counted_presses == 1)
			{
				startEventNow(PROGRAMMATIC);
			}
			else if (g_handle_counted_presses == 2)
			{
				suspendEvent();
			}
			
			g_handle_counted_presses = 0;
		}
		
		if(g_switch_closed_time >= 1000)
		{
			LEDS.blink(LEDS_GREEN_ON_CONSTANT);
			LEDS.blink(LEDS_RED_ON_CONSTANT);
		}
		else if(g_text_buff.empty())
		{
			if(!g_text_buff.isBusy())
			{
				g_enunciator = LED_ONLY;
			
				if(g_hardware_error != HARDWARE_OK)
				{
					LEDS.blink(LEDS_RED_AND_GREEN_BLINK_FAST);
				}
				else if(g_do_powerup_xmsn)
				{
					if(!g_delay_before_powerup_xmsn)
					{
						time_t now = time(null);
						ConfigurationState_t state = clockConfigurationCheck();

						if((state != CONFIGURATION_ERROR) && (now < g_event_start_epoch))
						{
							char str[50];
					
							g_WiFi_shutdown_seconds = MAX(60U, g_WiFi_shutdown_seconds);

							// TEMPSTR_SIZE must be > 44 to hold start time and battery voltage string
							sprintf(g_tempStr, "Start %sZ = %sV = ", convertEpochToTimeString(g_event_start_epoch, str, 50), externBatString(true));
							repChar(g_tempStr, '.', 'r'); // replace decimal points with 'r' for sending in Morse
							repChar(g_tempStr, '-', ' '); // replace hyphens with spaces
							repChar(g_tempStr, ':', 'r'); // replace colons with 'r'
						
							// TEMPSTR_SIZE must be > 44 + MAX_PATTERN_TEXT_LENGTH to hold pattern
							if(strlen(g_tempStr) < TEMPSTR_SIZE - MAX_PATTERN_TEXT_LENGTH)
							{
								if(g_messages_text[PATTERN_TEXT][0])
								{
									strncat(g_tempStr, (const char*)g_messages_text[PATTERN_TEXT], MAX_PATTERN_TEXT_LENGTH);
								}							
							}	
				
							// TEMPSTR_SIZE must be > 49 + 2xMAX_PATTERN_TEXT_LENGTH to hold ID
							if(strlen(g_tempStr) < (TEMPSTR_SIZE - MAX_PATTERN_TEXT_LENGTH + 5))
							{
								if(g_messages_text[STATION_ID][0])
								{
									strcat(g_tempStr, " = ");
									strncat(g_tempStr, (const char*)g_messages_text[STATION_ID], MAX_PATTERN_TEXT_LENGTH);
									strcat(g_tempStr, " ~");
								}
							}
						
							g_enunciation_code_throttle = throttleValue(15);
							LEDS.sendCode(g_tempStr);
							g_enunciator = LED_AND_RF;
						}
				
						g_do_powerup_xmsn = false;
					}
				}
				else 
				{
					/*
					 * Slow red promises that the event will run without another user
					 * action. Include the pending automatic boot launch, but do not
					 * advertise a suspended event merely because its EEPROM window
					 * remains current.
					 */
					if(g_start_event || !noEventWillRun())
					{
						g_enunciation_code_throttle = throttleValue(8);
						LEDS.sendCode((char*)"E  ");
					}
					else
					{
						if(g_event_enabled)
						{
							LEDS.blink(LEDS_RED_OFF);
						}
						else
						{
							LEDS.blink(LEDS_RED_BLINK_FAST);
						}
					}
				}
			}
		}
		else /* Make sure the text buffer is being emptied */
		{
			g_enable_LED_enunciations = true; /* There is only one consumer of g_text_buff so it is always OK to enable manual transmissions */
		}

		
		if(!g_check_temperature)
		{
			int16_t temp;
			
			if(temperatureC(&temp))
			{
				if(fanIsOn())
				{
					if(temp <= 30)
					{
						setFan(OFF);
					}
				}
				else
				{
					if(temp >= 35)
					{
						setFan(ON);
					}
				}			
			}
			else
			{
				/* Unknown temperature is not evidence that cooling is unnecessary. */
				setFan(ON);
			}
				
			g_check_temperature = 10;
		}
		
		if(g_long_button_press)
		{
			g_long_button_press = false;
			if(g_event_enabled)
			{
				suspendEvent(); /* disable a running event */
			}
			else
			{
				g_check_for_next_event = true; /* Request next scheduled event */
			}
			LEDS.init(LEDS_GREEN_ON_CONSTANT);
		}
		
		if(g_start_event)
		{
			g_start_event = false;	
			
			SC status = STATUS_CODE_IDLE;
			g_last_error_code = launchEvent(&status);
						
			if(g_WiFi_shutdown_seconds)
			{
				g_WiFi_shutdown_seconds = MAX(g_WiFi_shutdown_seconds, 10U);
			}
		}
		
		if(g_end_event)
		{
			g_end_event = false;		
			suspendEvent();	
		}
		
		if(g_report_seconds && !g_clone_quiet)
		{
			if(holdTime && (holdTime != time(null))) /* Synchronize time updates to second transitions */
			{
				g_report_seconds = false;
				sprintf(g_tempStr, "%lu", time(null));
				lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
				holdTime = 0;
			}
			else
			{
				holdTime = time(null);
			}
		}

		if(g_last_error_code && !g_clone_quiet)
		{
			sprintf(g_tempStr, "%u", g_last_error_code);
			lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_ERRORCODE_LABEL, g_tempStr);
			g_last_error_code = ERROR_CODE_NO_ERROR;
		}

		if(g_last_status_code && !g_clone_quiet)
		{
			sprintf(g_tempStr, "%u", g_last_status_code);
			lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_STATUSCODE_LABEL, g_tempStr);
			g_last_status_code = STATUS_CODE_IDLE;
		}
		
		if(g_check_for_next_event && !g_clone_quiet)
		{
			if(g_wifi_ready)
			{
				g_check_for_next_event = false;
				g_waiting_for_next_event = true;
				sprintf(g_tempStr, "%u", g_last_status_code);
				lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_ESP_LABEL, (char*)"1");
				g_sleepType = SLEEP_FOREVER;	
			}
			else if(!g_WiFi_shutdown_seconds && !g_wifi_enable_delay)
			{
				g_wifi_enable_delay = 1;
			}
		}
		
		if(g_antenna_connection_changed)
		{
			g_antenna_connection_changed = false;
			
			/* Take appropriate action here */
			if(g_antenna_connect_state == ANT_DISCONNECTED)
			{
				inhibitRFOutput(true);
				g_last_error_code = ERROR_CODE_NO_ANTENNA_FOR_BAND;
			}
			else if(g_antenna_connect_state == ANT_CONNECTED)
			{
				inhibitRFOutput(false);
				LEDS.init();				
				eventEnabled();

				if((g_sleepType == SLEEP_UNTIL_START_TIME) && (g_awakenedBy != POWER_UP_START))
				{
					g_do_powerup_xmsn = true;
					g_delay_before_powerup_xmsn = 1000;
				}
			}
			
			LEDS.init();
			setAntennaStateEnunciator(); /* update green LED setting */
		}
		
		/********************************
		 * Handle sleep
		 ******************************/
		/* A scheduler sleep request cannot cut short an active maintenance
		 * transfer. The independent ISR countdown still expires absolutely and
		 * reasserts this request after at most five minutes without renewal. */
		if(g_go_to_sleep_now && !wifiUpdateLeaseActive())
		{
			LEDS.deactivate();
 			linkbus_disable();	
			shutdown_transmitter();	
			wifi_reset(ON);
			wifi_power(OFF);
			powerDown3V3();
			setFan(OFF);
			system_sleep_settings();
			
			g_waiting_for_next_event = false;

			SLPCTRL_set_sleep_mode(SLPCTRL_SMODE_STDBY_gc);		
			g_sleeping = true;
			g_awakenedBy = AWAKENED_INIT;
			
			/* Disable BOD? */
			
 			while(g_go_to_sleep_now)
 			{
				set_sleep_mode(SLEEP_MODE_STANDBY);
//					set_sleep_mode(SLEEP_MODE_PWR_DOWN);
				DISABLE_INTERRUPTS();
				sleep_enable();
				ENABLE_INTERRUPTS();
				sleep_cpu();  /* Sleep occurs here */
				sleep_disable();
 			}
 
			/* Re-enable BOD? */
			
			g_sleeping = false;
			atmel_start_init();
			powerUp3V3();
			init_transmitter();
			
			if((g_awakenedBy == AWAKENED_BY_BUTTONPRESS) || (g_awakenedBy == AWAKENED_BY_ANTENNA) || (g_awakenedBy == POWER_UP_START))
			{	
				setAntennaStateEnunciator(); /* Set green LED appropriately */
 				linkbus_enable();
				g_wifi_enable_delay = 2; /* Ensure WiFi is enabled and countdown is reset */
			}
			else if(g_awakenedBy == AWAKENED_BY_CLOCK)
			{
				powerToTransmitter(ON);
				
				if(!g_event_enabled)
				{
					g_start_event = eventEnabled(); /* Start any event stored in EEPROM */
				
					if(!g_start_event)
					{
 						linkbus_enable();
						g_wifi_enable_delay = 2; /* Ensure WiFi is enabled and countdown is reset */
					}
				}
			}

			g_event_scheduled = eventScheduled();
 			g_last_status_code = STATUS_CODE_RETURNED_FROM_SLEEP;
		}
	}
}


/* The compiler does not seem to optimize large switch statements correctly */
void __attribute__((optimize("O0"))) handleLinkBusMsgs()
{
	LinkbusRxBuffer* lb_buff;
	static uint8_t new_event_parameter_count = 0;

	while((lb_buff = nextFullLBRxBuffer()))
	{
		bool send_ack = true;
		LBMessageID msg_id = lb_buff->id;

		switch(msg_id)
		{
			case LB_MESSAGE_UPDATE:
			{
				if(lb_buff->type == LINKBUS_MSG_COMMAND &&
				   strcmp(lb_buff->fields[LB_MSG_FIELD1], "LEASE") == 0 &&
				   !lb_buff->fields[LB_MSG_FIELD2][0])
				{
					/* PASS forwarding rejects every $UPD command, so only the
					 * sketch's internal maintenance path can request this lease.
					 * It remains an absolute countdown: if the ESP stalls or stops
					 * renewing it, AVR power removal occurs within five minutes. */
					g_wifi_active = true;
					renewWifiUpdateLease();
					break;
				}

				if(lb_buff->type == LINKBUS_MSG_QUERY)
				{
					if(g_bootloader_protocol)
					{
						sprintf(g_tempStr, "BL%u.%u,%u,0x4000,512",
						        g_bootloader_version_major,
						        g_bootloader_version_minor,
						        g_bootloader_protocol);
						lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_UPDATE_LABEL, g_tempStr);
					}
					else
					{
						send_ack = false;
						lb_send_text((char *)LB_MESSAGE_NACK);
					}
					break;
				}

				if(!g_bootloader_protocol || lb_buff->type != LINKBUS_MSG_COMMAND ||
				   strcmp(lb_buff->fields[LB_MSG_FIELD1], "START") != 0 ||
				   strcmp(lb_buff->fields[LB_MSG_FIELD2], "SSID") != 0)
				{
					send_ack = false;
					lb_send_text((char *)LB_MESSAGE_NACK);
				}
				else
				{
					/*
					 * The ESP accepts this command only after the operator has cancelled
					 * the active .event file and supplied this unit's SSID suffix. End
					 * the AVR's already-loaded EEPROM copy as part of the same handoff;
					 * otherwise a reset can restart a schedule which the ESP has already
					 * cancelled.
					 */
					suspendEvent();
					g_text_buff.reset();
					g_enable_manual_transmissions = false;
					setEventFinishEpoch(time(null));
					g_ee_mgr.updateEEPROMVar(Event_finish_epoch, (void *)&g_event_finish_epoch);
					while(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm) { }
					if(eeprom_read_dword(&(EepromManager::ee_vars.event_finish_epoch)) !=
					   (uint32_t)g_event_finish_epoch)
					{
						send_ack = false;
						lb_send_text((char *)LB_MESSAGE_NACK);
						break;
					}
					avr_eeprom_write_byte(FLEXFOX_AVR_UPDATE_EEPROM_MARKER_ADDRESS,
					                      FLEXFOX_AVR_UPDATE_EEPROM_MARKER);
					while(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm) { }
					if(eeprom_read_byte((const uint8_t *)FLEXFOX_AVR_UPDATE_EEPROM_MARKER_ADDRESS) !=
					   FLEXFOX_AVR_UPDATE_EEPROM_MARKER)
					{
						send_ack = false;
						lb_send_text((char *)LB_MESSAGE_NACK);
					}
					else
					{
						keyTransmitter(OFF);
						powerToTransmitter(OFF);
						g_WiFi_shutdown_seconds = 0;
						g_sleepType = DO_NOT_SLEEP;
						g_bootloader_reset_pending = true;
					}
				}
			}
			break;

			case LB_MESSAGE_WIFI:
			{
				bool result;

				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					result = atoi(lb_buff->fields[LB_MSG_FIELD1]);

					suspendEvent();
					linkbus_disable();
					g_WiFi_shutdown_seconds = 0;    /* disable sleep */
					g_sleepType = DO_NOT_SLEEP;

					if(result == 0)                 /* shut off power to WiFi */
					{
						wifi_power(OFF);
					}
				}
			}
			break;
			
			case LB_MESSAGE_KEY:
			{
				g_event_enabled = false; /* Ensure an ongoing event is interrupted */
				g_enable_manual_transmissions = true;
				
				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					char* str = lb_buff->fields[LB_MSG_FIELD1];
					int lenstr = strlen(str);
					bool hold = g_enable_manual_transmissions;
					bool queuedText = false;
					
					if((lenstr == 2) && (str[0] == '\\') && (str[1] == 'B')) /* backspace */
					{
						g_enable_manual_transmissions = false; /* simple thread collision avoidance */
						g_text_buff.pop();
						g_enable_manual_transmissions = hold;
					}
					else if(lenstr > 1)
					{
						int i = 0;

						/*
						 * A web Send command can interrupt automated Morse. Claim the
						 * generator before filling the manual buffer so the main loop
						 * does not discard the newly queued text while changing callers.
						 */
						if(lastMorseCaller() != CALLER_MANUAL_TRANSMISSIONS)
						{
							bool manualRepeat = false;
							g_text_buff.reset();
							makeMorse((char*)"\0", &manualRepeat, null, CALLER_MANUAL_TRANSMISSIONS);
						}
						
						g_enable_manual_transmissions = false; /* simple thread collision avoidance */
						while(!g_text_buff.full() && i<lenstr && i<LINKBUS_MAX_MSG_FIELD_LENGTH)
						{
							g_text_buff.put(lb_buff->fields[LB_MSG_FIELD1][i++]);
						}
						g_enable_manual_transmissions = hold;
						queuedText = (i > 0);
					}
					else
					{
						char c = lb_buff->fields[LB_MSG_FIELD1][0];
					
						if(c == '[')
						{
							powerToTransmitter(ON);
							LEDS.blink(LEDS_RED_ON_CONSTANT, true);
							txKeyDown(ON);
						}
						else if(c == ']')
						{
							txKeyDown(OFF);
							powerToTransmitter(OFF);
							LEDS.blink(LEDS_RED_OFF, true);
						}
						else if(c == '^') /* Prevent sleep shutdown */
						{
							suspendEvent();
							g_WiFi_shutdown_seconds = 0;    /* disable sleep */
							g_sleepType = DO_NOT_SLEEP;
						}
						else
						{
							if(lastMorseCaller() != CALLER_MANUAL_TRANSMISSIONS)
							{
								bool manualRepeat = false;
								g_text_buff.reset();
								makeMorse((char*)"\0", &manualRepeat, null, CALLER_MANUAL_TRANSMISSIONS);
							}

							g_enable_manual_transmissions = false; /* simple thread collision avoidance */
							g_text_buff.put(c);
							g_enable_manual_transmissions = hold;
							queuedText = true;
						}
					}

					if(queuedText)
					{
						g_enunciator = LED_AND_RF;
						powerToTransmitter(ON);
					}
				}
			}
			break;

			case LB_MESSAGE_RESET:
			{
#ifndef TRANQUILIZE_WATCHDOG
					wdt_init(WD_FORCE_RESET);
					while(1)
					{
						;
					}
#endif  /* TRANQUILIZE_WATCHDOG */
			}
			break;

			case LB_MESSAGE_ESP_COMM:
			{
				char f1 = lb_buff->fields[LB_MSG_FIELD1][0];

				g_wifi_active = true;

				if(f1 == 'Z')                                                       /* WiFi connected to browser - keep alive */
				{
					/* shut down WiFi after 2 minutes of inactivity */
					releaseWifiUpdateLease();
					g_WiFi_shutdown_seconds = WIFI_NORMAL_SHUTDOWN_SECONDS;          /* return to the ordinary 2-minute lease */
				}
				else if(f1 == 'C')                                                  /* ESP is beginning a clone session */
				{
					enterCloneQuietMode();
				}
				else if(f1 == 'S')                                                  /* Report time once, immediately after the next RTC edge */
				{
					armCloneSyncReport();
				}
				else if(f1 == 'R')                                                  /* ESP clone session ended */
				{
					resumeNormalReports();
				}
				else
				{
					if(f1 == '0')                                                   /* ESP says "I'm awake" */
					{
						g_wifi_ready = true;
						/* Send WiFi the current time */
						sprintf(g_tempStr, "%lu", time(NULL));
						lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
						
						if(g_waiting_for_next_event)
						{
							lb_send_msg(LINKBUS_MSG_REPLY, (char *)LB_MESSAGE_ESP_LABEL, (char *)"1"); /* Request next scheduled event */
						}
					}
					else if(f1 == '2') /* ESP has no web clients and no other business to conduct (e.g., At end of live radio session) */
					{
						g_wifi_enable_delay = 1; /* Start countdown to WiFi power off */
						if(!g_event_enabled) 
						{
							g_start_event = true; /* Attempt to launch any event that is already set */
						}
					}
					else if(f1 == '3')                      /* ESP is ready for power off" */
					{			
						releaseWifiUpdateLease();
						g_wifi_enable_delay = 0;
						g_WiFi_shutdown_seconds = 1;        /* Shut down WiFi in 1 seconds */
						g_waiting_for_next_event = false;   /* Prevents resetting shutdown settings */
						g_wifi_active = false;
						g_shutting_down_wifi = true;
					}
				}
			}
			break;

			case LB_MESSAGE_TX_POWER:
			{
				static uint16_t pwr_mW;

				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					EC ec;

					if((lb_buff->fields[LB_MSG_FIELD1][0] == 'M') && (lb_buff->fields[LB_MSG_FIELD2][0]))
					{
						pwr_mW = (uint16_t)atoi(lb_buff->fields[LB_MSG_FIELD2]);
					}
					else
					{
						pwr_mW = (uint16_t)atoi(lb_buff->fields[LB_MSG_FIELD1]);
					}
					
					bool powerChanged = (pwr_mW != txGetPowerMw());
					/*
					 * Reapply the DAC value even when the requested numeric power
					 * matches the cached value. Manual radio mode uses this to
					 * refresh the physical setting immediately before key-down.
					 */
					ec = txSetParameters(&pwr_mW, NULL);
					if(ec)
					{
						g_last_error_code = ec;
					}
					else if(powerChanged)
					{
						new_event_parameter_count++;
					}

					sprintf(g_tempStr, "M,%u", pwr_mW);
					lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_TX_POWER_LABEL, g_tempStr);
					
					g_Event_Configuration_Check |= TX_POWER_RECEIVED_B;
				}
			}
			break;

			case LB_MESSAGE_PERM:
			{
				g_ee_mgr.saveAllEEPROM();
			}
			break;

			case LB_MESSAGE_GO:
			{
				char f1 = lb_buff->fields[LB_MSG_FIELD1][0];

				if((f1 == '1') || (f1 == '2'))
				{					
					if(g_antenna_connect_state != ANT_CONNECTED)
					{
						g_last_error_code = ERROR_CODE_NO_ANTENNA_FOR_BAND;
					}

					if(f1 == '1')   /* Xmit immediately using current settings */
					{
						if((g_antenna_connect_state == ANT_CONNECTED) || g_tx_power_is_zero)
						{
							/* Set the Morse code pattern and speed */
							g_event_enabled = false; // prevent interrupts from affecting the settings
							powerToTransmitter(ON);
// 							bool repeat = true;
// 							makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
// 							g_code_throttle = throttleValue(g_pattern_codespeed);
							setEventEpochs(1, MAX_TIME);                 /* run from a long time ago to a long time from now */
							g_on_air_seconds = 9999;                    /* on period is very long */
							g_off_air_seconds = 0;                      /* off period is very short */
							setOnTheAirFromForeground(9999);            /* start out transmitting */
							g_sendID_seconds_countdown = MAX_UINT16;			/* wait a long time to send the ID */
							g_event_commenced = true;                   /* get things running immediately */
							g_event_enabled = true;                     /* get things running immediately */
							g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
						}
					}
					else if(f1 == '2')  /* enables a downloaded event stored in EEPROM */
					{
						g_event_scheduled = eventScheduled();
						/* This command configures the transmitter to launch an event at its scheduled start time */
						if(g_Event_Configuration_Check != FULLY_CONFIGURED_EVENT)
						{
							g_last_error_code = ERROR_CODE_EVENT_NOT_CONFIGURED;
						}
						else
						{
							if(new_event_parameter_count)
							{
								if(g_event_enabled)
								{
									suspendEvent();
								}
									
								g_ee_mgr.saveAllEEPROM();
							}
								
							if(!g_event_enabled)
							{
								SC status = STATUS_CODE_IDLE;
								g_last_error_code = launchEvent(&status);
								g_wifi_enable_delay = 2; /* Ensure WiFi is enabled and countdown is reset */
							}
							else
							{
								g_WiFi_shutdown_seconds = 60;
							}
							
							new_event_parameter_count = 0;
							g_Event_Configuration_Check = 0;
						}
					}
				}
				else if(f1 == '0')  /* Prepare to receive new event data */
				{
					new_event_parameter_count = 0;
					g_Event_Configuration_Check = 0;
					g_last_status_code = STATUS_CODE_RECEIVING_EVENT_DATA;
					g_enable_manual_transmissions = false;
				}
			}
			break;

			case LB_MESSAGE_STARTFINISH:
			{
				time_t mtime = 0;

				if(lb_buff->fields[LB_MSG_FIELD1][0] == 'S')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						mtime = atol(lb_buff->fields[LB_MSG_FIELD2]);
					}

					if(mtime != g_event_start_epoch)
					{
						setEventStartEpoch(mtime);
//						syncSystemTimeToRTC();    /* update system clock */
						new_event_parameter_count++;
					}
					
					g_Event_Configuration_Check |= START_TIME_RECEIVED_B;
				}
				else
				{
					if(lb_buff->fields[LB_MSG_FIELD1][0] == 'F')
					{
						if(lb_buff->fields[LB_MSG_FIELD2][0])
						{
							mtime = atol(lb_buff->fields[LB_MSG_FIELD2]);
						}

						if(mtime != g_event_finish_epoch)
						{
							setEventFinishEpoch(mtime);
							new_event_parameter_count++;
						}
						
						g_Event_Configuration_Check |= FINISH_TIME_RECEIVED_B;
					}
				}
			}
			break;

			case LB_MESSAGE_CLOCK:
			{
				g_wifi_active = true;

				if(lb_buff->type == LINKBUS_MSG_COMMAND)    /* ignore replies since, as the time source, we should never be sending queries anyway */
				{
					if(lb_buff->fields[LB_MSG_FIELD1][0])
					{
						strncpy(g_tempStr, lb_buff->fields[LB_MSG_FIELD1], 20);
						g_tempStr[20] = '\0';
						bool clock_set = ds3231_set_date_time(g_tempStr, RTC_CLOCK);
						EC clock_result = ERROR_CODE_RTC_NONRESPONSIVE;

						if(clock_set)
						{
							clock_result = syncSystemTimeToRTC();    /* update system clock */
						}

						if(clock_set && (clock_result == ERROR_CODE_NO_ERROR) && (lb_buff->fields[LB_MSG_FIELD2][0] == 'C'))
						{
							time_t rtc_epoch = ds3231_get_epoch(&clock_result);
							if(clock_result == ERROR_CODE_NO_ERROR)
							{
								sprintf(g_tempStr, "C,%lu", rtc_epoch);
								lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
							}
						}

						if(!clock_set || (clock_result != ERROR_CODE_NO_ERROR))
						{
							send_ack = false;
							lb_send_text((char *)LB_MESSAGE_NACK);
						}
					}
					else
					{
						g_report_seconds = true;
					}
				}
				else
				{
					if(lb_buff->type == LINKBUS_MSG_QUERY)
					{
						if(lb_buff->fields[LB_MSG_FIELD1][0] == 'X')
						{
							int8_t age = 0;

							if(lb_buff->fields[LB_MSG_FIELD2][0])
							{
								age = (int8_t)atoi(lb_buff->fields[LB_MSG_FIELD2]);
								ds3231_set_aging(age);
							}
							else
							{
								age = ds3231_get_aging();
								sprintf(g_tempStr, "X,%d", age);
								lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
							}
						}
						else
						{
							static uint32_t lastTime = 0;

							syncSystemTimeToRTC();    /* update system clock */
							uint32_t temp_time = time(null);

							if(temp_time != lastTime)
							{
								sprintf(g_tempStr, "%lu", temp_time);
								lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
								lastTime = temp_time;
							}
						}
					}
				}
			}
			break;

			case LB_MESSAGE_SET_STATION_ID:
			{
				if(lb_buff->type == LINKBUS_MSG_COMMAND)
				{
					if(lb_buff->fields[LB_MSG_FIELD1][0])
					{
						if(strcmp(g_messages_text[STATION_ID], lb_buff->fields[LB_MSG_FIELD1]))
						{
							strncpy(g_messages_text[STATION_ID], lb_buff->fields[LB_MSG_FIELD1], MAX_PATTERN_TEXT_LENGTH);
							g_time_needed_for_ID = timeNeededForID();		
							new_event_parameter_count++;    /* Any ID or no ID is acceptable */
						}
					}
					else /* No callsign */
					{
						if(g_messages_text[STATION_ID][0])
						{
							g_messages_text[STATION_ID][0] = '\0';
							g_time_needed_for_ID = 0;
							new_event_parameter_count++;    /* Any ID or no ID is acceptable */
						}
					}
					
					g_Event_Configuration_Check |= STATION_ID_RECEIVED_B;
				}
				else
				{
					if(g_messages_text[STATION_ID][0])
					{
						sprintf(g_tempStr, "!ID,%s;\n", g_messages_text[STATION_ID]);
						lb_send_text(g_tempStr);
						send_ack = false;
					}
				}
			}
			break;

			case LB_MESSAGE_CODE_SPEED:
			{
				uint8_t speed = g_pattern_codespeed;

				if(lb_buff->fields[LB_MSG_FIELD1][0] == 'I')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						speed = CLAMP(MIN_CODE_SPEED_WPM, atol(lb_buff->fields[LB_MSG_FIELD2]), MAX_CODE_SPEED_WPM);
						
						if(speed != g_id_codespeed)
						{
							g_id_codespeed = speed;
							new_event_parameter_count++;

							if(g_messages_text[STATION_ID][0])
							{
								g_time_needed_for_ID = timeNeededForID();
							}
						}
						
						g_Event_Configuration_Check |= ID_CODE_SPEED_RECEIVED_B;
					}
				}
				else if(lb_buff->fields[LB_MSG_FIELD1][0] == 'P')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						speed = CLAMP(MIN_CODE_SPEED_WPM, atol(lb_buff->fields[LB_MSG_FIELD2]), MAX_CODE_SPEED_WPM);
						
						if(speed != g_pattern_codespeed)
						{
							g_pattern_codespeed = speed;
							new_event_parameter_count++;
							g_code_throttle = throttleValue(g_pattern_codespeed);
						}
						
						g_Event_Configuration_Check |= PATTERN_CODE_SPEED_RECEIVED_B;
					}
				}
			}
			break;

			case LB_MESSAGE_TIME_INTERVAL:
			{
				int16_t time = 0;

				if(lb_buff->fields[LB_MSG_FIELD1][0] == '0')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						time = atol(lb_buff->fields[LB_MSG_FIELD2]);
						
						if(time != g_off_air_seconds)
						{
							g_off_air_seconds = time;
							new_event_parameter_count++;
						}
						
						g_Event_Configuration_Check |= OFF_TIME_RECEIVED_B;
					}
				}
				else if(lb_buff->fields[LB_MSG_FIELD1][0] == '1')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						time = atol(lb_buff->fields[LB_MSG_FIELD2]);
						
						if(time != g_on_air_seconds)
						{
							g_on_air_seconds = time;
							new_event_parameter_count++;
						}
						
						g_Event_Configuration_Check |= ON_TIME_RECEIVED_B;
					}
				}
				else if(lb_buff->fields[LB_MSG_FIELD1][0] == 'I')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						time = atol(lb_buff->fields[LB_MSG_FIELD2]);
						
						if(time != g_ID_period_seconds)
						{
							g_ID_period_seconds = time;
							new_event_parameter_count++;
						}
						
						g_Event_Configuration_Check |= ID_INTERVAL_RECEIVED_B;
					}
				}
				else if(lb_buff->fields[LB_MSG_FIELD1][0] == 'D')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						time = atol(lb_buff->fields[LB_MSG_FIELD2]);
						
						if(time != g_intra_cycle_delay_time)
						{
							g_intra_cycle_delay_time = time;
							new_event_parameter_count++;
						}
						
						g_Event_Configuration_Check |= OFFSET_TIME_RECEIVED_B;
					}
				}
			}
			break;

			case LB_MESSAGE_SET_PATTERN:
			{
				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					if(strcmp(g_messages_text[PATTERN_TEXT], lb_buff->fields[LB_MSG_FIELD1]))
					{
						strncpy(g_messages_text[PATTERN_TEXT], lb_buff->fields[LB_MSG_FIELD1], MAX_PATTERN_TEXT_LENGTH);
						new_event_parameter_count++;
					}
					
					g_Event_Configuration_Check |= MESSAGE_PATTERN_RECEIVED_B;
				}
			}
			break;

			case LB_MESSAGE_SET_FREQ:
			{
				Frequency_Hz transmitter_freq = 0;

				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					Frequency_Hz f = atol(lb_buff->fields[LB_MSG_FIELD1]);

					if(f != txGetFrequency())
					{
						if(!txSetFrequency(&f, true))
						{
							transmitter_freq = f;
							new_event_parameter_count++;
						}
					}
					
					g_Event_Configuration_Check |= FREQUENCY_RECEIVED_B;
				}
				else
				{
					transmitter_freq = txGetFrequency();
				}

				if(transmitter_freq)
				{
					sprintf(g_tempStr, "%ld,", transmitter_freq);
					lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_SET_FREQ_LABEL, g_tempStr);
				}
			}
			break;

			case LB_MESSAGE_BAT:
			{
				lb_broadcast_str(externBatString(false), "!BAT");
			}
			break;

			case LB_MESSAGE_TEMP:
			{
// 				int16_t v;
// 				if(!ds3231_get_temp(&v))
// 				{
// 					lb_broadcast_num(v, "!TEM");
// 				}
				
				int16_t temperature;
				if(temperatureC(&temperature))
				{
					dtostrf(temperature, 4, 1, g_tempStr);
					g_tempStr[5] = '\0';
				}
				else
				{
					strcpy(g_tempStr, TEMPERATURE_UNAVAILABLE_TEXT);
				}

				lb_broadcast_str(g_tempStr, "!TEM");
				
			}
			break;

			case LB_MESSAGE_VER:
			{
				lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_VER_LABEL, (char *)SW_REVISION);
			}
			break;
			
			/* Handle legacy messages for compatibility */
			case LB_MESSAGE_BAND: 
			case LB_MESSAGE_TX_MOD:
			{
			}
			break;

			default:
			{
// 				linkbus_reset_rx(); /* flush buffer */
// 				g_last_error_code = ERROR_CODE_ILLEGAL_COMMAND_RCVD;
				send_ack = false;
				lb_send_text((char *)LB_MESSAGE_NACK);
			}
			break;
		}

		if(send_ack)
		{
			lb_send_text((char *)LB_MESSAGE_ACK);
		}
		
		lb_buff->id = LB_MESSAGE_EMPTY;
	}
}

void enterCloneQuietMode(void)
{
	ENTER_CRITICAL(clone_quiet_state);
	g_clone_quiet = true;
	g_clone_sync_report_armed = false;
	g_clone_sync_report_ready = false;
	g_clone_quiet_timeout_seconds = 900;
	g_report_seconds = false;
	EXIT_CRITICAL(clone_quiet_state);
}

void resumeNormalReports(void)
{
	ENTER_CRITICAL(clone_quiet_state);
	g_clone_quiet = false;
	g_clone_sync_report_armed = false;
	g_clone_sync_report_ready = false;
	g_clone_quiet_timeout_seconds = 0;
	g_report_seconds = false;
	EXIT_CRITICAL(clone_quiet_state);
}

void armCloneSyncReport(void)
{
	ENTER_CRITICAL(clone_sync_state);
	if(g_clone_quiet)
	{
		g_clone_sync_report_armed = true;
		g_clone_sync_report_ready = false;
		g_clone_quiet_timeout_seconds = 900;
	}
	EXIT_CRITICAL(clone_sync_state);
}

void serviceCloneSyncReport(void)
{
	time_t epoch = 0;

	ENTER_CRITICAL(clone_sync_report);
	if(g_clone_sync_report_ready)
	{
		epoch = g_clone_sync_epoch;
		g_clone_sync_report_ready = false;
	}
	EXIT_CRITICAL(clone_sync_report);

	if(epoch)
	{
		sprintf(g_tempStr, "%lu", epoch);
		lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
	}
}


/***********************************************************************
 * Private Function Prototypes
 *
 * These functions are available only within this file
 ************************************************************************/

/* Returns true if an event should be running now or is scheduled to run in the future. Sets g_sleepType, and g_time_to_wake_up, appropriately
   based on when the event should start. An event that is already finished returns false. */
bool __attribute__((optimize("O0"))) eventEnabled()
{
	time_t now;
	int32_t dif;

	now = time(null);
	g_go_to_sleep_now = false;
	if(!eventScheduledAt(
		now,
		g_event_start_epoch,
		g_event_finish_epoch,
		MINIMUM_VALID_EPOCH))
	{
		g_sleepType = SLEEP_FOREVER;
		setWakeTimeFromForeground(MAX_TIME);
		g_wifi_enable_delay = 2;
		return(false);
	}
	
	powerToTransmitter(ON);
	dif = timeDif(now, g_event_start_epoch);

	if(dif >= -120)  /* Don't sleep if the event starts in 120 seconds or less, or has already started */
	{
		g_sleepType = DO_NOT_SLEEP;
		setWakeTimeFromForeground(g_event_start_epoch - 10);
		return( true);
	}

	/* If we reach here, we have an event that will not start for at least 121 seconds. It needs to be enabled, and a sleep time needs to be calculated
	 * while allowing time for power-up (coming out of sleep) prior to the event start */
	setWakeTimeFromForeground(g_event_start_epoch - 10);
	g_sleepType = SLEEP_UNTIL_START_TIME;

	return( true);
}


void wdt_init(WDReset resetType)
{
	
}

uint16_t throttleValue(uint8_t speed)
{
	float temp;
	speed = CLAMP(5, (int8_t)speed, 20);
	temp = (3544L / (uint16_t)speed) / 10L; /* tune numerator to achieve "PARIS " sent 8 times in 60 seconds at 8 WPM */
	return( (uint16_t)temp);
}

EC syncSystemTimeToRTC(void)
{
	EC result = waitForRTCSecondTransition();
	if(result != ERROR_CODE_NO_ERROR)
	{
		return result;
	}

	uint8_t generation = rtcEdgeGeneration();
	time_t t = ds3231_get_epoch(&result);
	if(!rtcSyncReadCanCommit(
		result == ERROR_CODE_NO_ERROR,
		generation,
		rtcEdgeGeneration()))
	{
		return ERROR_CODE_RTC_NONRESPONSIVE;
	}

	set_system_time(t);
	return result;
}

EC waitForRTCSecondTransition(void)
{
	g_seconds_transition = false;
	util_delay_ms(0);

	RtcSyncWaitState wait_state;
	do
	{
		bool timer_running = util_delay_ms(RTC_SYNC_WAIT_TIMEOUT_MS);
		wait_state = rtcSyncWaitState(
			g_seconds_transition,
			timer_running);
	} while(wait_state == RTC_SYNC_WAITING);

	util_delay_ms(0);
	return wait_state == RTC_SYNC_EDGE_READY ? ERROR_CODE_NO_ERROR : ERROR_CODE_RTC_NONRESPONSIVE;
}

EC __attribute__((optimize("O0"))) launchEvent(SC* statusCode)
{
	EC ec = activateEventUsingCurrentSettings(statusCode);

	if(*statusCode)
	{
		g_last_status_code = *statusCode;
	}

	if(ec)
	{
		g_last_error_code = ec;
	}
	else
	{
		g_event_enabled = eventEnabled();
		/* Applying a valid event is an explicit user action. Re-arm the shared
		 * indicator timeout so the resulting slow/fast red state remains visible
		 * even when the prior display period expired before this transaction. */
		LEDS.init();
	}

	return( ec);
}

EC activateEventUsingCurrentSettings(SC* statusCode)
{
	/* Make sure everything has been sanely initialized */
	if(!g_event_start_epoch)
	{
		return( ERROR_CODE_EVENT_MISSING_START_TIME);
	}

	if(g_event_start_epoch > g_event_finish_epoch)   /* A reversed window is malformed. */
	{
		return( ERROR_CODE_EVENT_NOT_CONFIGURED);
	}

	if(eventEpochsExplicitlyDisabled(g_event_start_epoch, g_event_finish_epoch))
	{
		/* Equal epochs are a valid request to persistently disable the event. */
		suspendEvent();
		g_waiting_for_next_event = false;
		if(statusCode)
		{
			*statusCode = STATUS_CODE_NO_EVENT_TO_RUN;
		}
		return( ERROR_CODE_NO_ERROR);
	}

	if(!g_on_air_seconds)
	{
		return( ERROR_CODE_EVENT_MISSING_TRANSMIT_DURATION);
	}

	if(g_intra_cycle_delay_time > (g_off_air_seconds + g_on_air_seconds))
	{
		return( ERROR_CODE_EVENT_TIMING_ERROR);
	}

	if(g_messages_text[PATTERN_TEXT][0] == '\0')
	{
		return( ERROR_CODE_EVENT_PATTERN_NOT_SPECIFIED);
	}

	if((g_pattern_codespeed < MIN_CODE_SPEED_WPM) || (g_pattern_codespeed > MAX_CODE_SPEED_WPM))
	{
		return( ERROR_CODE_EVENT_PATTERN_CODE_SPEED_NOT_SPECIFIED);
	}

	if(g_messages_text[STATION_ID][0] != '\0')
	{
		if((!g_id_codespeed || !g_ID_period_seconds))
		{
			return( ERROR_CODE_EVENT_STATION_ID_ERROR);
		}

		g_time_needed_for_ID = timeNeededForID();
	}
	else
	{
		g_time_needed_for_ID = 0;   /* ID will never be sent */
	}

	/* Take one edge-aligned snapshot without hanging forever if RTC SQW is lost. */
	EC sync_result = waitForRTCSecondTransition();
	if(sync_result != ERROR_CODE_NO_ERROR)
	{
		return sync_result;
	}

	time_t now = time(null);
	
	if(g_event_finish_epoch < now)   /* the event has already finished */
	{
		if(statusCode)
		{
			*statusCode = STATUS_CODE_NO_EVENT_TO_RUN;
		}
	}
	else
	{
		int32_t dif = timeDif(now, g_event_start_epoch); /* returns arg1 - arg2 */

		if(dif >= 0)                                    /* start time is in the past */
		{
			bool turnOnTransmitter = false;
			int32_t on_the_air = 0;
			int cyclePeriod = g_on_air_seconds + g_off_air_seconds;
			int secondsIntoCycle = dif % cyclePeriod;
			int timeTillTransmit = g_intra_cycle_delay_time - secondsIntoCycle;

			if(timeTillTransmit <= 0)                       /* we should have started transmitting already in this cycle */
			{
				if(g_on_air_seconds <= -timeTillTransmit)   /* we should have finished transmitting in this cycle */
				{
					on_the_air = -(cyclePeriod + timeTillTransmit);
					if(statusCode)
					{
						*statusCode = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
					}

					if(!g_event_enabled)
					{
						g_sendID_seconds_countdown = (g_on_air_seconds - on_the_air) - g_time_needed_for_ID;
					}
				}
				else    /* we should be transmitting right now */
				{
					on_the_air = g_on_air_seconds + timeTillTransmit;
					turnOnTransmitter = true;
					if(statusCode)
					{
						*statusCode = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
					}

					if(!g_event_enabled)
					{
						if(g_time_needed_for_ID < on_the_air)
						{
							g_sendID_seconds_countdown = on_the_air - g_time_needed_for_ID;
						}
					}
				}
			}
			else    /* it is not yet time to transmit in this cycle */
			{
				on_the_air = -timeTillTransmit;
				if(statusCode)
				{
					*statusCode = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
				}

				if(!g_event_enabled)
				{
					g_sendID_seconds_countdown = timeTillTransmit + g_on_air_seconds - g_time_needed_for_ID;
				}
			}

			setOnTheAirFromForeground(on_the_air);

			if(turnOnTransmitter)
			{
				powerToTransmitter(ON);
			}
			else
			{
				keyTransmitter(OFF);
			}

			g_event_commenced = true;
			LEDS.init();
		}
		else    /* start time is in the future */
		{
			if(statusCode)
			{
				*statusCode = STATUS_CODE_WAITING_FOR_EVENT_START;
			}
			keyTransmitter(OFF);
		}

		g_waiting_for_next_event = false;
	}

	return( ERROR_CODE_NO_ERROR);
}


EC hw_init()
{
	return ERROR_CODE_NO_ERROR;
}

EC rtc_init()
{	
	bool failure = true;
	
	util_delay_ms(0);
	while(util_delay_ms(1000) && failure)
	{
		failure = ds3231_init();
	}
	
	if(!failure)
	{
		failure = true;
		
		util_delay_ms(0);
		while(util_delay_ms(1000) && failure)
		{
			failure = ds3231_1s_sqw(ON);
		}
		
		if(failure)
		{
			return ERROR_CODE_RTC_NONRESPONSIVE;
		}
	}
	else
	{
		return ERROR_CODE_RTC_NONRESPONSIVE;
	}
	
	return ERROR_CODE_NO_ERROR;
}

bool __attribute__((optimize("O0"))) setAntennaStateEnunciator()
{
	bool val = (g_antenna_connect_state == ANT_CONNECTED);
	static Blink_t b = LEDS_OFF;

	if(g_hardware_error == (uint16_t)HARDWARE_OK)
	{
		if(val)
		{
			b = LEDS_GREEN_ON_CONSTANT;
		}
		else
		{
			b = LEDS_GREEN_OFF;
		}
		
		LEDS.blink(b);
	}

	return(val);
}

void initializeAllEventSettings(bool disableEvent)
{
	
}

void suspendEvent()
{
	g_event_enabled = false;    /* get things stopped immediately */
	setOnTheAirFromForeground(0); /* stop transmitting */
	g_event_commenced = false;  /* get things stopped immediately */
	keyTransmitter(OFF);
	powerToTransmitter(OFF);
	LEDS.init();
}


void startEventNow(EventActionSource_t activationSource)
{
	ConfigurationState_t conf = clockConfigurationCheck();

	if(activationSource == POWER_UP)
	{
		if(conf == CONFIGURATION_ERROR)
		{
			setupForFox(INVALID_FOX, START_NOTHING);
		}
		else
		{
			setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);
		}
	}
	else if(activationSource == PROGRAMMATIC)
	{
		if(conf == CONFIGURATION_ERROR)                                                                                             /* Start immediately */
		{
			setupForFox(INVALID_FOX, START_EVENT_NOW_AND_RUN_FOREVER);
		}
		else if((conf == WAITING_FOR_START) || (conf == SCHEDULED_EVENT_WILL_NEVER_RUN) || (conf == SCHEDULED_EVENT_DID_NOT_START)) /* Start immediately */
		{
			setupForFox(INVALID_FOX, START_EVENT_NOW_AND_RUN_FOREVER);
		}
		else                                                                                                                        /*if((conf == EVENT_IN_PROGRESS) */
		{
			setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);                                                                  /* Let the RTC start the event */
		}
	}
	else                                                                                                                            /* PUSHBUTTON */
	{
		if(conf == CONFIGURATION_ERROR)                                                                                             /* No scheduled event */
		{
			setupForFox(INVALID_FOX, START_EVENT_NOW_AND_RUN_FOREVER);
		}
		else                                                                                                                        /* if(buttonActivated) */
		{
			if(conf == WAITING_FOR_START)
			{
				setupForFox(INVALID_FOX, START_TRANSMISSIONS_NOW);                                                                         /* Start transmitting! */
			}
			else if(conf == SCHEDULED_EVENT_WILL_NEVER_RUN)
			{
				setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);                                                              /* rtc starts the event */
			}
			else                                                                                                                    /* Event should be running now */
			{
				setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);                                                              /* start the running event */
			}
		}
	}
}

void stopEventNow(EventActionSource_t activationSource)
{
	ConfigurationState_t conf = clockConfigurationCheck();

	if(activationSource == PROGRAMMATIC)
	{
		setupForFox(INVALID_FOX, START_NOTHING);
	}
	else    /* if(activationSource == PUSHBUTTON) */
	{
		if(conf == WAITING_FOR_START)
		{
			setupForFox(INVALID_FOX, START_TRANSMISSIONS_NOW);
		}
		if(conf == SCHEDULED_EVENT_WILL_NEVER_RUN)
		{
			setupForFox(INVALID_FOX, START_NOTHING);
		}
		else    /*if(conf == CONFIGURATION_ERROR) */
		{
			setupForFox(INVALID_FOX, START_NOTHING);
		}
	}

// 	if(g_sync_pin_stable == STABLE_LOW)
// 	{
// 		digitalWrite(PIN_LED, OFF); /*  LED Off */
// 	}
}

void startEventUsingRTC(void)
{
	syncSystemTimeToRTC();    /* update system clock */
	time_t now = time(null);
	ConfigurationState_t state = clockConfigurationCheck();

	if(state != CONFIGURATION_ERROR)
	{
		setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);
		reportTimeTill(now, g_event_start_epoch, "Starts in: ", "In progress\n");

		if(g_event_start_epoch < now)
		{
			reportTimeTill(now, g_event_finish_epoch, "Time Remaining: ", NULL);
		}
		else
		{
			reportTimeTill(g_event_start_epoch, g_event_finish_epoch, "Lasts: ", NULL);
		}
	}
	else
	{
		reportConfigErrors();
	}
}



void setupForFox(Fox_t fox, EventAction_t action)
{
	bool delayNotSet = true;
	
	if(fox == INVALID_FOX)
	{
		fox = getFoxSetting();
	}

	g_event_enabled = false;
	g_event_commenced = false;
	LEDS.setRed(OFF);
 	syncSystemTimeToRTC();    /* update system clock */

	switch(fox)
	{
		case FOX_1:
		{
			delayNotSet = false;
			g_intra_cycle_delay_time = 0;
		}
		case FOX_2:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 60;
			}
		}
		case FOX_3:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 120;
			}
		}
		case FOX_4:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 180;
			}
		}
		case FOX_5:
		{
			/* Set the Morse code pattern and speed */
			if(delayNotSet)
			{
				g_intra_cycle_delay_time = 240;
			}
			
			g_sendID_seconds_countdown = 60;			/* wait 1 minute to send the ID */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 240;                    /* off period is very short */
		}
		break;

		case SPRINT_S1:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 0;
			}
		}
		case SPRINT_S2:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 12;
			}
		}
		case SPRINT_S3:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 24;
			}
		}
		case SPRINT_S4:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 36;
			}
		}
		case SPRINT_S5:
		{
			{
				if(delayNotSet)
				{
					g_intra_cycle_delay_time = 48;
				}
			}

			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 12;						/* on period is very long */
			g_off_air_seconds = 48;						/* off period is very short */
		}
		break;

		case SPRINT_F1:
		{
			delayNotSet = false;
			g_intra_cycle_delay_time = 0;
		}
		case SPRINT_F2:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 12;
			}
		}
		case SPRINT_F3:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 24;
			}
		}
		case SPRINT_F4:
		{
			if(delayNotSet)
			{
				delayNotSet = false;
				g_intra_cycle_delay_time = 36;
			}
		}
		case SPRINT_F5:
		{
			if(delayNotSet)
			{
				g_intra_cycle_delay_time = 48;
			}

			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 12;						/* on period is very long */
			g_off_air_seconds = 48;						/* off period is very short */
		}
		break;

#if SUPPORT_TEMP_AND_VOLTAGE_REPORTING
		case REPORT_BATTERY:
		{
			g_intra_cycle_delay_time = 0;
// 			g_on_air_interval_seconds = 30;
// 			g_cycle_period_seconds = 60;
// 			g_number_of_foxes = 2;
// 			g_pattern_codespeed = SPRINT_SLOW_CODE_SPEED;
// 			g_fox_id_offset = REPORT_BATTERY - 1;
// 			g_id_interval_seconds = 60;
		}
		break;
#endif // SUPPORT_TEMP_AND_VOLTAGE_REPORTING

		case FOXORING_FOX1:
		case FOXORING_FOX2:
		case FOXORING_FOX3:
		{
			init_transmitter(getFrequencySetting());
			g_intra_cycle_delay_time = 0;
			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 0;						/* off period is very short */
		}
		break;
		
		case SPECTATOR:
		case BEACON:
		default:
		{
			g_intra_cycle_delay_time = 0;
			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 0;						/* off period is very short */
		}
		break;
	}

	if(action == START_NOTHING)
	{
		g_event_commenced = false;                   /* do not get things running yet */
		g_event_enabled = false;                     /* do not get things running yet */

 		g_use_rtc_for_startstop = false;
 		g_event_enabled = false;
		keyTransmitter(OFF);
		LEDS.setRed(OFF);
		powerToTransmitter(OFF);
	}
	else if(action == START_EVENT_NOW_AND_RUN_FOREVER)
	{
		EC result = syncSystemTimeToRTC();    /* update system clock */
		time_t now = time(null);
		
		if(result == ERROR_CODE_NO_ERROR)
		{
			time_t finish = g_event_finish_epoch;
			if(now > finish)
			{
				finish = now + DAY;
			}
			setEventEpochs(now, finish);
		}
		
		SC status = STATUS_CODE_IDLE;
		result = launchEvent(&status);
		g_wifi_enable_delay = 2; /* Ensure WiFi is enabled and countdown is reset */

		if(!result)
		{
			g_ee_mgr.saveAllEEPROM();
		}
	}
	else if(action == START_TRANSMISSIONS_NOW)                                  /* Immediately start transmitting, regardless RTC or time slot */
	{
		g_code_throttle = throttleValue(g_pattern_codespeed);

		setOnTheAirFromForeground(g_on_air_seconds); /* start out transmitting */
		g_event_commenced = true;                   /* get things running immediately */
		g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
 		g_use_rtc_for_startstop = false;
		g_event_enabled = true;                     /* get things running immediately */
	}
	else         /* if(action == START_EVENT_WITH_STARTFINISH_TIMES) */
	{
		launchEvent((SC*)&g_last_status_code);
	}
}

time_t validateTimeString(char* str, time_t* epochVar)
{
	time_t valid = 0;
	int len = strlen(str);
	time_t minimumEpoch = MINIMUM_VALID_EPOCH;
	uint8_t validationType = 0;
	time_t now = time(null);

	if(epochVar == &g_event_start_epoch)
	{
		minimumEpoch = MAX(now, MINIMUM_VALID_EPOCH);
		validationType = 1;
	}
	else if(epochVar == &g_event_finish_epoch)
	{
		minimumEpoch = MAX(g_event_start_epoch, now);
		validationType = 2;
	}
	
	if(len == 10)
	{
		str[10] = '0';
		str[11] = '0';
		str[12] = '\0';
		len = 12;
	}

	if((len == 12) && (only_digits(str)))
	{
		time_t ep = RTC_String2Epoch(NULL, str);    /* String format "YYMMDDhhmmss" */

		if(ep > minimumEpoch)
		{
			valid = ep;
		}
		else
		{
			if(validationType == 1)         /* start time validation */
			{
				sb_send_string(TEXT_ERR_START_IN_PAST_TXT);
			}
			else if(validationType == 2)    /* finish time validation */
			{
				if(ep < time(null))
				{
					sb_send_string(TEXT_ERR_FINISH_IN_PAST_TXT);
				}
				else
				{
					sb_send_string(TEXT_ERR_FINISH_BEFORE_START_TXT);
				}
			}
			else    /* current time validation */
			{
				sb_send_string(TEXT_ERR_TIME_IN_PAST_TXT);
			}
		}
	}
	else if(len)
	{
		sb_send_string(TEXT_ERR_INVALID_TIME_TXT);
	}

	return(valid);
}

void makeTimeTillString(char* str, time_t from, time_t until, bool* fail)
{
	if(from >= until)   /* Negative time */
	{
		if(fail)
		{
			*fail = true;
		}
		
		return;
	}
	
	if(str)
	{
		time_t dif = until - from;
		uint16_t years = dif / YEAR;
		time_t hold = dif - (years * YEAR);
		uint16_t days = hold / DAY;
		hold -= (days * DAY);
		uint16_t hours = hold / HOUR;
		hold -= (hours * HOUR);
		uint16_t minutes = hold / MINUTE;
		uint16_t seconds = hold - (minutes * MINUTE);
		char tmpstr[15];
		
		str[0] = '\0';

		if(years)
		{
			sprintf(tmpstr, "%d yrs ", years);
			strcat(str, tmpstr);
		}

		if(days)
		{
			sprintf(tmpstr, "%d days ", days);
			strcat(str, tmpstr);
		}

		if(hours)
		{
			sprintf(tmpstr, "%d hrs ", hours);
			strcat(str, tmpstr);
		}

		if(minutes)
		{
			sprintf(tmpstr, "%d min ", minutes);
			strcat(str, tmpstr);
		}

		if(seconds)
		{
			sprintf(tmpstr, "%d sec", seconds);
			strcat(str, tmpstr);
		}
	}
}


bool reportTimeTill(time_t from, time_t until, const char* prefix, const char* failMsg)
{
	bool failure = false;
	makeTimeTillString(g_tempStr, from, until, &failure);

	if(failure)
	{
		sb_send_string((char*)failMsg);
	}
	else
	{
		if(prefix)
		{
			sb_send_string((char*)prefix);
		}
		
		sb_send_string(g_tempStr);	
		sb_send_NewLine();
	}

	return( failure);
}



ConfigurationState_t clockConfigurationCheck(void)
{
	time_t now = time(null);
	EventSchedulePosition position = eventSchedulePosition(
		now,
		g_event_start_epoch,
		g_event_finish_epoch,
		MINIMUM_VALID_EPOCH);

	if((position == EVENT_SCHEDULE_INVALID) || (position == EVENT_SCHEDULE_FINISHED))
	{
		return(CONFIGURATION_ERROR);
	}

	if(position == EVENT_SCHEDULE_ACTIVE) /* Event should be running */
	{
		if(!g_event_enabled)
		{
			return(SCHEDULED_EVENT_DID_NOT_START);  /* Event scheduled to be running isn't */
		}
		else
		{
			return(EVENT_IN_PROGRESS);              /* Event is running, so clock settings don't matter */
		}
	}
	else if(!g_use_rtc_for_startstop)
	{
		return(SCHEDULED_EVENT_WILL_NEVER_RUN);
	}

	return(WAITING_FOR_START);  /* Future event hasn't started yet */
}

void reportConfigErrors(void)
{
	syncSystemTimeToRTC(); /* sync system clock to RTC */
	time_t now = time(null);

	if(g_messages_text[STATION_ID][0] == '\0')
	{
		sb_send_string(TEXT_SET_ID_TXT);
	}

	if(now <= MINIMUM_VALID_EPOCH) /* Current time is invalid */
	{
		sb_send_string(TEXT_SET_TIME_TXT);
	}
	
	if(g_event_finish_epoch <= MINIMUM_VALID_EPOCH)
	{
		sb_send_string(TEXT_SET_FINISH_TXT);
		
		if(g_event_start_epoch < MINIMUM_VALID_EPOCH)
		{
			sb_send_string(TEXT_SET_START_TXT);
		}
	}
	else if(g_event_finish_epoch <= now)      /* Event has already finished */
	{
		if(g_event_start_epoch < now)   /* Event has already started */
		{
			sb_send_string(TEXT_SET_START_TXT);
		}

		sb_send_string(TEXT_SET_FINISH_TXT);
	}
	else if(g_event_start_epoch < now)  /* Event has already started */
	{
		if(g_event_start_epoch < MINIMUM_VALID_EPOCH)     /* Start invalid */
		{
			sb_send_string(TEXT_SET_START_TXT);
		}
		else if(!g_event_enabled)
		{
			sb_send_string((char*)"Start with > GO 2\n");
		}
		else
		{
			sb_send_string((char*)"Event running...\n");
		}
	}
}

uint16_t timeNeededForID(void)
{
	return((uint16_t)(((float)timeRequiredToSendStrAtWPM((char*)g_messages_text[STATION_ID], g_id_codespeed)) / 1000.));
}

Fox_t getFoxSetting(void)
{
	return g_fox[g_event];
}

int getFoxCodeSpeed(void)
{
	if(g_fox[g_event] == BEACON)
	{
		return(g_pattern_codespeed);
	}
	else if(g_event == EVENT_FOXORING)
	{
		return(g_foxoring_pattern_codespeed);
	}
	
	return(g_pattern_codespeed);
}

int getPatternCodeSpeed(void)
{
	if(!g_event_commenced)
	{
		return ENUNCIATION_BLINK_WPM;
	}
	
	return(getFoxCodeSpeed());
}

char* getCurrentPatternText(void)
{
	return g_messages_text[PATTERN_TEXT];
// 	char* c;
// 	
// 	switch(g_fox[g_event])
// 	{
// 		case FOX_1:
// 		{
// 			c = (char*)"MOE";
// 		}
// 		break;
// 		
// 		case FOX_2:
// 		{
// 			c = (char*)"MOI";
// 		}
// 		break;
// 		
// 		case FOX_3:
// 		{
// 			c = (char*)"MOS";
// 		}
// 		break;
// 		
// 		case FOX_4:
// 		{
// 			c = (char*)"MOH";
// 		}
// 		break;
// 		
// 		case FOX_5:
// 		{
// 			c = (char*)"MO5";
// 		}
// 		break;
// 		
// 		case SPECTATOR:
// 		{
// 			c = (char*)"S";
// 		}
// 		break;
// 		
// 		case SPRINT_S1:
// 		{
// 			c = (char*)"ME";
// 		}
// 		break;
// 		
// 		case SPRINT_S2:
// 		{
// 			c = (char*)"MI";
// 		}
// 		break;
// 		
// 		case SPRINT_S3:
// 		{
// 			c = (char*)"MS";
// 		}
// 		break;
// 		
// 		case SPRINT_S4:
// 		{
// 			c = (char*)"MH";
// 		}
// 		break;
// 		
// 		case SPRINT_S5:
// 		{
// 			c = (char*)"M5";
// 		}
// 		break;
// 		
// 		case SPRINT_F1:
// 		{
// 			c = (char*)"OE";
// 		}
// 		break;
// 		
// 		case SPRINT_F2:
// 		{
// 			c = (char*)"OI";
// 		}
// 		break;
// 		
// 		case SPRINT_F3:
// 		{
// 			c = (char*)"OS";
// 		}
// 		break;
// 		
// 		case SPRINT_F4:
// 		{
// 			c = (char*)"OH";
// 		}
// 		break;
// 		
// 		case SPRINT_F5:
// 		{
// 			c = (char*)"O5";
// 		}
// 		break;
// 		
// 		case FOXORING_FOX1:
// 		case FOXORING_FOX2:
// 		case FOXORING_FOX3:
// 		{
// 			c = g_messages_text[FOXORING_PATTERN_TEXT];
// 		}
// 		break;
// 		
// 		case BEACON:
// 		{
// 			c = (char*)"MO";
// 		}
// 		break;
// 		
// 		default:
// 		{
// 			c = g_messages_text[PATTERN_TEXT];
// 		}
// 		break;
// 	}
// 	
// 	return c;
}

Frequency_Hz getFrequencySetting(void)
{
	return(txGetFrequency());
}


bool noEventWillRun(void)
{
	time_t now = time(null);
	bool eventWillRun = eventWillRunWithoutUserActionAt(
		now,
		g_event_start_epoch,
		g_event_finish_epoch,
		MINIMUM_VALID_EPOCH,
		g_event_enabled);

	return !eventWillRun ||
		(g_sleepType == SLEEP_USER_OVERRIDE) ||
		(g_sleepType == SLEEP_FOREVER);
}

bool eventScheduledForNow(void)
{
	time_t now = time(null);
	return eventScheduledForNowAt(
		now,
		g_event_start_epoch,
		g_event_finish_epoch,
		MINIMUM_VALID_EPOCH);
}

bool eventScheduledForTheFuture(void)
{
	time_t now = time(null);
	return eventScheduledForTheFutureAt(
		now,
		g_event_start_epoch,
		g_event_finish_epoch,
		MINIMUM_VALID_EPOCH);
}

bool eventScheduled(void)
{
	time_t now = time(null);
	return eventScheduledAt(
		now,
		g_event_start_epoch,
		g_event_finish_epoch,
		MINIMUM_VALID_EPOCH);
}


// Caller must provide a pointer to a string of length 6 or greater.
char* externBatString(bool volts)
{
	static char str[7] = "?";
	char* pstr = str;
	float bat = (float)g_lastConversionResult[EXTERNAL_BATTERY_VOLTAGE];
	bat *= 172.;
	bat *= 0.0005;
	bat += 1.;
	
	if((bat >= 0.) && (bat <= 180.))
	{
		if(volts)
		{
			dtostrf(bat/10., 5, 1, str);
			str[6] = '\0';
			pstr = trimwhitespace(str);
			return pstr;
		}
		else
		{
			dtostrf(bat, 4, 0, str);		
			str[5] = '\0';
			return str;
		}
	}
				
	return str;
}

/** 
The repChar() function replaces all occurences of \orig with \rep in the passed
character array str.

\returns The repChar() function returns the number of replaced characters. */
int repChar(char *str, char orig, char rep) 
{
	char *p = str;
	int n = 0;
	while((p = strchr(p, orig)) != NULL) 
	{
		*p++ = rep;
		n++;
	}
	return n;
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == '\0')  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
