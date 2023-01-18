#include "atmel_start.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/sleep.h>
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
static char g_tempStr[50] = { '\0' };
static volatile EC g_last_error_code = ERROR_CODE_NO_ERROR;
static volatile SC g_last_status_code = STATUS_CODE_IDLE;

static volatile bool g_powering_off = false;

static volatile uint16_t g_util_tick_countdown = 0;
static volatile bool g_battery_measurements_active = false;
static volatile uint16_t g_maximum_battery = 0;

static volatile bool g_antenna_connection_changed = true;
volatile AntConnType g_antenna_connect_state = ANT_CONNECTION_UNDETERMINED;

static volatile bool g_start_event = false;
static volatile bool g_end_event = false;

static volatile int32_t g_on_the_air = 0;
static volatile int g_sendID_seconds_countdown = 0;
static volatile uint16_t g_code_throttle = 50;
static volatile uint8_t g_WiFi_shutdown_seconds = 120;
static volatile bool g_report_seconds = false;
static volatile bool g_wifi_active = true;
static volatile uint8_t g_wifi_enable_delay = 0;
static volatile bool g_shutting_down_wifi = false;
static volatile bool g_wifi_ready = false;
static volatile int g_hardware_error = (int)HARDWARE_OK;

extern Frequency_Hz g_80m_frequency;
char g_messages_text[STATION_ID+1][MAX_PATTERN_TEXT_LENGTH + 1];
volatile uint8_t g_id_codespeed = EEPROM_ID_CODE_SPEED_DEFAULT;
volatile uint8_t g_pattern_codespeed = EEPROM_PATTERN_CODE_SPEED_DEFAULT;
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

volatile bool g_sending_station_ID = false;											/* Allows a small extension of transmissions to ensure the ID is fully sent */
volatile bool g_muteAfterID = false;												/* Inhibit any transmissions after the ID has been sent */

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
#define NUMBER_OF_POLLED_ADC_CHANNELS 3
static ADC_Active_Channel_t g_adcChannelOrder[NUMBER_OF_POLLED_ADC_CHANNELS] = { ADCExternalBatteryVoltage, ADC12VRegulatedVoltage, ADCTXAdjustableVoltage };
enum ADC_Result_Order { EXTERNAL_BATTERY_VOLTAGE, REGULATED_12V, PA_VOLTAGE };
static const uint16_t g_adcChannelConversionPeriod_ticks[NUMBER_OF_POLLED_ADC_CHANNELS] = { TIMER2_5_8HZ, TIMER2_0_5HZ, TIMER2_0_5HZ };
static volatile uint16_t g_adcCountdownCount[NUMBER_OF_POLLED_ADC_CHANNELS] = { 100, 1000, 2000 };
//static uint16_t g_ADCFilterThreshold[NUMBER_OF_POLLED_ADC_CHANNELS] = { 500, 500, 500 };
static volatile bool g_adcUpdated[NUMBER_OF_POLLED_ADC_CHANNELS] = { false, false, false };
static volatile uint16_t g_lastConversionResult[NUMBER_OF_POLLED_ADC_CHANNELS];

extern Goertzel g_goertzel;
volatile uint16_t g_switch_closed_count = 0;
volatile uint16_t g_handle_counted_presses = 0;
volatile uint16_t g_switch_presses_count = 0;
volatile bool g_long_button_press = false;
volatile uint16_t g_check_temperature = 0;

Enunciation_t g_enunciator = LED_ONLY;

uint16_t g_Event_Configuration_Check = 0;

leds LEDS = leds();
CircularStringBuff g_text_buff = CircularStringBuff(TEXT_BUFF_SIZE);

EepromManager g_ee_mgr;

Fox_t g_fox = FOX_1;
extern Event_t g_event;
extern Fox_t g_foxoring_fox;
extern Frequency_Hz g_foxoring_frequencyA;
extern Frequency_Hz g_foxoring_frequencyB;
extern Frequency_Hz g_foxoring_frequencyC;
int8_t g_utc_offset;
uint8_t g_unlockCode[UNLOCK_CODE_SIZE + 1];
bool g_use_rtc_for_startstop = false;

volatile bool g_enable_manual_transmissions = false;
volatile bool g_enable_LED_enunciations = true;

/***********************************************************************
 * Private Function Prototypes
 *
 * These functions are available only within this file
 ************************************************************************/
void handle_1sec_tasks(void);
bool eventEnabled(void);
void handleLinkBusMsgs(void);
void handleSerialBusMsgs(void);
void wdt_init(WDReset resetType);
uint16_t throttleValue(uint8_t speed);
EC activateEventUsingCurrentSettings(SC* statusCode);
EC launchEvent(SC* statusCode);
EC hw_init(void);
EC rtc_init(void);
bool antennaIsConnected(void);
void updateAntennaStatus(void);
void powerDown3V3(void);
void powerUp3V3(void);
void setFan(bool on);
bool fanIsOn(void);

uint16_t timeNeededForID(void);
Frequency_Hz getFrequencySetting(void);
char* getPatternText(void);
Fox_t getFoxSetting(void);
bool eventScheduled(void);

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
	
    if(x & (1 << RTC_SQW)) /* Handle 1-second interrupt */
    {
		system_tick();

		if(g_sleeping)
		{
			if(g_sleepType == SLEEP_UNTIL_NEXT_XMSN)
			{
				if(g_on_the_air < 0) 
				{
					g_on_the_air++;
				}
				
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
				}
				else if(g_antenna_connection_changed)
				{
					g_go_to_sleep_now = false;
					g_sleeping = false;
					g_awakenedBy = AWAKENED_BY_ANTENNA;
				}
			}
		}
		else
		{
			handle_1sec_tasks();
		}
	}

    VPORTA.INTFLAGS = 0xFF; /* Clear all PORTA interrupt flags */
}


/**
1-Second Interrupts:
One-second counter based on RTC.
*/
void handle_1sec_tasks(void)
{
	static uint16_t extendedOnAirTime = 0;
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
				antennaIsConnected(); /* Set green LED appropriately */
				g_sleepType = SLEEP_FOREVER;
					
				if(g_hardware_error & (int)HARDWARE_NO_WIFI)
				{
					g_check_for_next_event = false;
					g_go_to_sleep_now = true;
				}
			}
		}
	}

	if(g_event_enabled)
	{
		if(g_event_commenced)
		{
			bool repeat;

			if(g_sendID_seconds_countdown)
			{
				g_sendID_seconds_countdown--;
			}

			if(g_on_the_air)
			{
				if(g_on_the_air > 0)    /* on the air */
				{
					g_on_the_air--;

					if(!g_sendID_seconds_countdown && g_time_needed_for_ID)
					{
						if(g_on_the_air == g_time_needed_for_ID)    /* wait until the end of a transmission */
						{
							g_last_status_code = STATUS_CODE_SENDING_ID;
							g_sendID_seconds_countdown = g_ID_period_seconds;
							g_code_throttle = throttleValue(g_id_codespeed);
							repeat = false;
							makeMorse(g_messages_text[STATION_ID], &repeat, NULL);  /* Send only once */
							g_sending_station_ID = true;
							extendedOnAirTime=0;
						}
					}


					if(!g_on_the_air) /* On-the-air time has just expired */
					{
						if(g_sending_station_ID)
						{
							g_on_the_air++; /* Continue sending ID */
							extendedOnAirTime++; /* Remove this time from the off-the-air period */
						}
						else
						{
							if(g_off_air_seconds) /* If there will be a pause in transmissions before resuming */
							{
								keyTransmitter(OFF);
								g_on_the_air -= g_off_air_seconds;
								g_on_the_air += extendedOnAirTime;
								extendedOnAirTime = 0;
							
								repeat = true;
								makeMorse(getPatternText(), &repeat, NULL);    /* Reset pattern to start */
								g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
								LEDS.setRed(OFF);

								/* Enable sleep during off-the-air periods */
								int32_t timeRemaining = 0;
								temp_time = time(null);
								if(temp_time < g_event_finish_epoch)
								{
									timeRemaining = timeDif(g_event_finish_epoch, temp_time);
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
										g_sendID_seconds_countdown = MAX(0, g_sendID_seconds_countdown - (int)seconds_to_sleep);
									}
								}
							}
							else /* Transmissions are continuous */
							{
								g_on_the_air = g_on_air_seconds;
								g_code_throttle = throttleValue(g_pattern_codespeed);
							}
						}
					}
				}
				else if(g_on_the_air < 0)   /* off the air - g_on_the_air = 0 means all transmissions are disabled */
				{
					g_on_the_air++;

					if(!g_on_the_air)       /* off-the-air time has expired */
					{
						g_muteAfterID = false;
						g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
						g_on_the_air = g_on_air_seconds;
						g_code_throttle = throttleValue(g_pattern_codespeed);
						bool repeat = true;
						makeMorse(getPatternText(), &repeat, NULL);
					}
				}
			}
		}
		else if(g_event_start_epoch > MINIMUM_VALID_EPOCH) /* off the air - waiting for the start time to arrive */
		{
			temp_time = time(null);

			if(temp_time >= g_event_start_epoch) /* Time for the event to start */
			{
				powerToTransmitter(ON);
				
				if(g_intra_cycle_delay_time)
				{
					g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
					g_on_the_air = -g_intra_cycle_delay_time;
					g_sendID_seconds_countdown = g_intra_cycle_delay_time + g_on_air_seconds - g_time_needed_for_ID;
				}
				else
				{
					g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
					g_on_the_air = g_on_air_seconds;
					g_sendID_seconds_countdown = g_on_air_seconds - g_time_needed_for_ID;
					g_code_throttle = throttleValue(g_pattern_codespeed);
					bool repeat = true;
					makeMorse(getPatternText(), &repeat, NULL);
				}

				g_event_commenced = true;
				LEDS.init();
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
			g_WiFi_shutdown_seconds = 120;
		}
	}
	else
	{
		if(g_shutting_down_wifi || (!g_check_for_next_event && !g_waiting_for_next_event))
		{
			if(g_WiFi_shutdown_seconds)
			{
				g_WiFi_shutdown_seconds--;

				if(!g_WiFi_shutdown_seconds && !g_enable_manual_transmissions)
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
Antenna disconnect interrupt.
*/
ISR(PORTD_PORT_vect)
{
	uint8_t x = VPORTD.INTFLAGS;
	
	if(x & (1 << X80M_ANTENNA_DETECT)) /* Handle antenna change */
	{
		bool ant = antennaIsConnected();

		if(!ant)    /* immediately detect disconnection */
		{
			if(g_antenna_connect_state != ANT_DISCONNECTED)
			{
				g_antenna_connect_state = ANT_DISCONNECTED;
				inhibitRFOutput(true);
			}
		}
		
		g_antenna_connection_changed = true;
	}

    VPORTD.INTFLAGS = 0xFF; /* Clear all PORTD interrupt flags */
}



/**
Periodic tasks not requiring precise timing. Rate = 300 Hz
*/
ISR(TCB0_INT_vect)
{
	static bool initializeManualTransmissions = true;
	uint8_t x = TCB0.INTFLAGS;
	
    if(x & TCB_CAPT_bm)
    {
		static bool conversionInProcess = false;
		static int8_t indexConversionInProcess = 0;
		static uint16_t codeInc = 0;
		bool repeat, finished;
		static uint16_t switch_closures_count_period = 0;
		
		if(g_util_tick_countdown)
		{
			g_util_tick_countdown--;
		}
		
		if(switch_closures_count_period)
		{
			switch_closures_count_period--;
				
			if(!switch_closures_count_period)
			{
				if(g_switch_presses_count && (g_switch_presses_count<=3))
				{
					g_handle_counted_presses = g_switch_presses_count;
				}
					
				g_switch_presses_count = 0;
			}
		}
		else if(g_switch_presses_count == 1)
		{
			switch_closures_count_period = 300;
		}
		else if(g_switch_presses_count > 2)
		{
			g_switch_presses_count = 0;
		}

		// Check switch state		
		if(!PORTC_get_pin_level(SWITCH))
		{
			if(g_switch_closed_count<1000)
			{
				g_switch_closed_count++;
				if(g_switch_closed_count >= 1000)
				{
					g_long_button_press = true;
				}
			}			
		}		
							
		updateAntennaStatus();

		static bool key = false;

		if(g_event_enabled && g_event_commenced) /* Handle cycling transmissions */
		{
			initializeManualTransmissions = true;
			
			if(g_on_the_air > 0)
			{
				if(codeInc)
				{
					codeInc--;

					if(!codeInc)
					{
						key = makeMorse(NULL, &repeat, &finished);

						if(!repeat && finished) /* ID has completed, so resume pattern */
						{
							g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
							g_code_throttle = throttleValue(g_pattern_codespeed);
							repeat = true;
							makeMorse(getPatternText(), &repeat, NULL);
							key = makeMorse(NULL, &repeat, &finished);
							g_muteAfterID = g_sending_station_ID && g_off_air_seconds;
							g_sending_station_ID = false;
						}
					}
				}
				else
				{
					if(g_muteAfterID) 
					{
						key = OFF;
					}
					
					keyTransmitter(key);
					LEDS.setRed(key);
					codeInc = g_code_throttle;
				}
			}
			else if(!g_on_the_air)
			{
				g_muteAfterID = false;
				
				if(key)
				{
					key = OFF;
					keyTransmitter(OFF);
					LEDS.setRed(OFF);
					powerToTransmitter(OFF);
					g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
				}
			}
		}
		else if(g_enable_manual_transmissions || g_enable_LED_enunciations) /* Handle enunciations & character string transmissions */
		{
			static bool charFinished = true;
			static bool idle = true;
			bool sendBuffEmpty = g_text_buff.empty();
			repeat = false;
			
			if(initializeManualTransmissions)
			{
				initializeManualTransmissions = false;
				g_text_buff.reset();
				makeMorse((char*)"\0", &repeat, null); /* reset makeMorse */
				sendBuffEmpty = true;
				charFinished = true;
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
						powerToTransmitter(OFF);
					}
				
					codeInc = g_code_throttle;
					makeMorse((char*)"\0", &repeat, null); /* reset makeMorse */
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
						key = makeMorse(null, &repeat, &charFinished);

						if(charFinished) /* Completed, send next char */
						{
							if(!g_text_buff.empty())
							{
								static char cc[2]; /* Must be static because makeMorse saves only a pointer to the character array */
								g_code_throttle = throttleValue(g_pattern_codespeed);
								cc[0] = g_text_buff.get();
								cc[1] = '\0';
								makeMorse(cc, &repeat, null);
								key = makeMorse(null, &repeat, &charFinished);
							}
						}

						if(g_enunciator == LED_AND_RF) keyTransmitter(key);
						LEDS.setRed(key);
						codeInc = g_code_throttle;
					}
				}
				else
				{
					if(g_enunciator == LED_AND_RF) keyTransmitter(key);
					LEDS.setRed(key);
					codeInc = g_code_throttle;
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
			}
		}
		else if(ADC0_conversionDone())   /* wait for conversion to complete */
		{
			static uint16_t holdConversionResult;
			uint16_t hold = ADC0_read(); //ADC;
			
			if((hold > 10) && (hold < 4090))
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
    }

    TCB0.INTFLAGS = TCB_CAPT_bm; /* clear interrupt flag */
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
			antennaIsConnected(); /* Set green LED appropriately */
			g_awakenedBy = AWAKENED_BY_BUTTONPRESS;	
			g_waiting_for_next_event = false; /* Ensure the wifi module does not get shut off prematurely */
		}
		else if(g_switch_closed_count > 5) /* debounce */
		{
			g_hardware_error = HARDWARE_OK; // Clear any detected hardware failures
			
			if(!LEDS.active())
			{
				antennaIsConnected(); /* Set green LED appropriately */
			}
			else
			{
				if(PORTC_get_pin_level(SWITCH)) g_switch_presses_count++;		
				g_switch_closed_count = 0;

				if(wifiEnabled())
				{
					sprintf(g_tempStr, "$ESP,X;"); /* send shutdown message to allow WiFi to prepare for shutdown */
					lb_send_msg(LINKBUS_MSG_COMMAND, LB_MESSAGE_ESP_LABEL, g_tempStr);
					g_WiFi_shutdown_seconds = 5;
				}
				else
				{
					g_wifi_enable_delay = 2;
				}					
			}
		}
		
		g_switch_closed_count = 0;
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
		g_hardware_error |= (int)HARDWARE_NO_WIFI;
//		sb_send_string(TEXT_WIFI_NOT_DETECTED_TXT);	
	}
	
	if(init_transmitter() != ERROR_CODE_NO_ERROR)
	{
		if(!txIsInitialized())
		{
			g_hardware_error |= (int)HARDWARE_NO_SI5351;
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
		g_hardware_error |= (int)HARDWARE_NO_RTC;
	}
	else
	{
		EC result;
		time_t current_epoch = ds3231_get_epoch(&result);
		if(result == ERROR_CODE_NO_ERROR)
		{
			set_system_time(current_epoch);
		}
		g_event_scheduled = eventScheduled();
	}
	
	g_wifi_enable_delay = 3;
	g_start_event = eventEnabled(); /* Set flag to start any event stored in EEPROM */

	while (1) {
		handleLinkBusMsgs();
//		handleSerialBusMsgs(); /* Uncomment when serial port comms are supported */
		
		if(g_handle_counted_presses)
		{
			if(g_handle_counted_presses == 1)
			{
//				sb_send_string((char*)"\n1 press\n");
				startEventNow(PROGRAMMATIC);
			}
			else if (g_handle_counted_presses == 2)
			{
//				sb_send_string((char*)"\n2 presses\n");
				suspendEvent();
			}
			
			g_handle_counted_presses = 0;
		}
		
		if(g_switch_closed_count >= 1000)
		{
			LEDS.blink(LEDS_GREEN_ON_CONSTANT);
			LEDS.blink(LEDS_RED_ON_CONSTANT);
		}
		else if(g_text_buff.empty())
		{
			if(g_hardware_error != HARDWARE_OK)
			{
				LEDS.blink(LEDS_RED_AND_GREEN_BLINK_FAST);
			}
			else 
			{
				antennaIsConnected(); /* Set green LED appropriately */

				if(g_event_scheduled)
				{
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
		
		if(!g_check_temperature)
		{
			int16_t temp = temperatureC();
			
			if((temp > -200) && (temp < 150)) // sanity check
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
				g_WiFi_shutdown_seconds = MAX(g_WiFi_shutdown_seconds, 10);
			}
		}
		
		if(g_end_event)
		{
			g_end_event = false;		
			suspendEvent();	
		}
		
		if(g_report_seconds)
		{
			g_report_seconds = false;
			sprintf(g_tempStr, "%lu", time(null));
			lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
		}

		if(g_last_error_code)
		{
			sprintf(g_tempStr, "%u", g_last_error_code);
			lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_ERRORCODE_LABEL, g_tempStr);
			g_last_error_code = ERROR_CODE_NO_ERROR;
		}

		if(g_last_status_code)
		{
			sprintf(g_tempStr, "%u", g_last_status_code);
			lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_STATUSCODE_LABEL, g_tempStr);
			g_last_status_code = STATUS_CODE_IDLE;
		}
		
		if(g_check_for_next_event)
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
			
			g_antenna_connect_state = antennaIsConnected() ? ANT_CONNECTED:ANT_DISCONNECTED;
			/* Take appropriate action here */
			if(g_antenna_connect_state == ANT_DISCONNECTED)
			{
				inhibitRFOutput(true);
				/* Green LED OFF */
			}
			else
			{
				inhibitRFOutput(false);
				/* Green LED ON */
			}
		}
		
		/********************************
		 * Handle sleep
		 ******************************/
		if(g_go_to_sleep_now)
		{
			LEDS.blink(LEDS_OFF);
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
				antennaIsConnected(); /* Set green LED appropriately */
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

void __attribute__((optimize("O0"))) handleSerialBusMsgs()
//void handleSerialBusMsgs()
{
	SerialbusRxBuffer* sb_buff;

	while((sb_buff = nextFullSBRxBuffer()))
	{
		if(!g_WiFi_shutdown_seconds)
		{
			g_wifi_enable_delay = 2;
		}
		else
		{
			g_WiFi_shutdown_seconds = 240;
		}
		
		SBMessageID msg_id = sb_buff->id;

		switch(msg_id)
		{
			case SB_MESSAGE_SET_FOX:
			{
				int c = (int)(sb_buff->fields[SB_FIELD1][0]);
				
				if(c)
				{
					if(g_event == EVENT_FOXORING)
					{
						if((c == 'A') || (c == '1'))
						{
							c = FOXORING_EVENT_FOXA;
						}
						else if((c == 'B') || (c == '2'))
						{
							c = FOXORING_EVENT_FOXB;
						}
						else if((c == 'C') || (c == '3'))
						{
							c = FOXORING_EVENT_FOXC;
						}
						else
						{
							sb_send_string((char*)"Use: FOX A, B, or C\n");
						}
					}
					else
					{
						if(c == 'B')
						{
							c = BEACON;
						}
						else if(c == 'F')
						{
							c = FOXORING;
						}
						else if(c == 'C')
						{
							char t = sb_buff->fields[SB_FIELD2][0];
							sb_buff->fields[SB_FIELD2][1] = '\0';

							if(t == 'B')
							{
								t = '0';
							}

							if(isdigit(t))
							{
								c = CLAMP(BEACON, atoi(sb_buff->fields[SB_FIELD2]), FOX_5);
							}
						}
						else if(c == 'S')
						{
							char x = 0;
							char t = sb_buff->fields[SB_FIELD2][0];
							char u = sb_buff->fields[SB_FIELD2][1];
							sb_buff->fields[SB_FIELD2][2] = '\0';

							if(t == 'B')
							{
								x = BEACON;
							}
							else if(t == 'F')
							{
								if((u > '0') && (u < '6'))
								{
									x = SPRINT_F1 + (u - '1');
								}
							}
							else if(t == 'S')
							{
								if((u > '0') && (u < '6'))
								{
									x = SPRINT_S1 + (u - '1');
								}
								else
								{
									x = SPECTATOR;
								}
							}
							else if(u == 'F')
							{
								if((t > '0') && (t < '6'))
								{
									x = SPRINT_F1 + (t - '1');
								}
							}
							else if(u == 'S')
							{
								if((t > '0') && (t < '6'))
								{
									x = SPRINT_S1 + (t - '1');
								}
							}

							if((x >= SPECTATOR) && (x <= SPRINT_F5))
							{
								c = x;
							}
							else
							{
								c = BEACON;
							}
						}
						else
						{
							c = atoi(sb_buff->fields[SB_FIELD1]);
						}
					}

					if((c >= BEACON) && (c < INVALID_FOX))
					{
 						Fox_t holdFox = (Fox_t)c;
 						g_ee_mgr.updateEEPROMVar(Fox_setting, (void*)&holdFox);
						 
 						if(holdFox != getFoxSetting())
 						{
							 if(g_event == EVENT_FOXORING)
							 {
								 if((c >= FOXORING_EVENT_FOXA) && (c <= FOXORING_EVENT_FOXC))
								 {
									 g_foxoring_fox = holdFox;
									 setupForFox(holdFox, START_EVENT_WITH_STARTFINISH_TIMES);
								 }
							 }
							 else
							 {
								 g_fox = holdFox;
								 setupForFox(holdFox, START_EVENT_WITH_STARTFINISH_TIMES);
							 }
 						}
					}
				} /* if(c) */

//				reportSettings();
			}
			break;
			
			
			case SB_MESSAGE_SLP:
			{
				if(sb_buff->fields[SB_FIELD1][0])
				{
					if(sb_buff->fields[SB_FIELD1][0] == '0')    
					{
						g_sleepType = DO_NOT_SLEEP;
					}
					else if (sb_buff->fields[SB_FIELD1][0] == '1')
					{
						g_event_enabled = eventEnabled(); // Set sleep type based on current event settings
					}
					else
					{
						g_go_to_sleep_now = true;
					}
				}
				else
				{
					g_go_to_sleep_now = true;
				}
			}
			break;
				
			case SB_MESSAGE_TX_FREQ:
			{
// 				if(sb_buff->fields[SB_FIELD1][0])
// 				{
// 					Frequency_Hz f;
// 					if(g_cloningInProgress)
// 					{
// 						if(!frequencyVal(sb_buff->fields[SB_FIELD2], &f))
// 						{
// 							if(sb_buff->fields[SB_FIELD1][0] == 'A')
// 							{
// 								g_foxoring_frequencyA = f;
// 								g_ee_mgr.updateEEPROMVar(Foxoring_FrequencyA, (void*)&f);
// 							}
// 							else if(sb_buff->fields[SB_FIELD1][0] == 'B')
// 							{
// 								g_foxoring_frequencyB = f;
// 								g_ee_mgr.updateEEPROMVar(Foxoring_FrequencyB, (void*)&f);
// 							}
// 							else if(sb_buff->fields[SB_FIELD1][0] == 'C')
// 							{
// 								g_foxoring_frequencyC = f;
// 								g_ee_mgr.updateEEPROMVar(Foxoring_FrequencyC, (void*)&f);
// 							}
// 							
// 							g_event_checksum += f;
// 					
// 							sb_send_string((char*)"FRE\r");
// 							g_programming_countdown = PROGRAMMING_MESSAGE_TIMEOUT_PERIOD;
// 						}
// 					}
// 					else if(!frequencyVal(sb_buff->fields[SB_FIELD1], &f))
// 					{
// 						if(g_event == EVENT_FOXORING)
// 						{
// 							if(g_foxoring_fox == FOXORING_EVENT_FOXA)
// 							{
// 								g_foxoring_frequencyA = f;
// 							}
// 							else if(g_foxoring_fox == FOXORING_EVENT_FOXB)
// 							{
// 								g_foxoring_frequencyB = f;
// 							}
// 							else if(g_foxoring_fox == FOXORING_EVENT_FOXC)
// 							{
// 								g_foxoring_frequencyC = f;
// 							}
// 						}
// 						else
// 						{
// 							g_frequency = f;
// 						}
// 						
// 						if(!txSetFrequency(&f, true))
// 						{
// 							g_ee_mgr.updateEEPROMVar(Frequency, (void*)&f);
// 						}
// 						else
// 						{
// 							sb_send_string(TEXT_TX_NOT_RESPONDING_TXT);
// 						}
// 					}
// 					else
// 					{
// 						sb_send_string((char*)"\n3500 kHz < Freq < 4000 kHz\n");
// 					}
// 				}
// 				
// 				if(!(g_cloningInProgress)) reportSettings();
			}
			break;
				
			case SB_MESSAGE_PATTERN:
			{
// 				if(g_cloningInProgress)
// 				{
// 					if(sb_buff->fields[SB_FIELD1][0] && sb_buff->fields[SB_FIELD2][0])
// 					{
// 						int len = MIN(MAX_PATTERN_TEXT_LENGTH, strlen(sb_buff->fields[SB_FIELD2]));
// 						
// 						if(sb_buff->fields[SB_FIELD1][0] == 'A')
// 						{
// 							strncpy((char*)g_messages_text[FOXA_PATTERN_TEXT], sb_buff->fields[SB_FIELD2], len);
// 							g_ee_mgr.updateEEPROMVar(FoxA_pattern_text, g_messages_text[FOXA_PATTERN_TEXT]);
// 						}
// 						else if(sb_buff->fields[SB_FIELD1][0] == 'B')
// 						{
// 							strncpy((char*)g_messages_text[FOXB_PATTERN_TEXT], sb_buff->fields[SB_FIELD2], len);
// 							g_ee_mgr.updateEEPROMVar(FoxB_pattern_text, g_messages_text[FOXB_PATTERN_TEXT]);
// 						}
// 						else if(sb_buff->fields[SB_FIELD1][0] == 'C')
// 						{
// 							strncpy((char*)g_messages_text[FOXC_PATTERN_TEXT], sb_buff->fields[SB_FIELD2], len);
// 							g_ee_mgr.updateEEPROMVar(FoxC_pattern_text, g_messages_text[FOXC_PATTERN_TEXT]);
// 						}
// 						
// 						for(int i=0; i<len; i++)
// 						{
// 							g_event_checksum += sb_buff->fields[SB_FIELD2][i];
// 						}
// 					
// 						sb_send_string((char*)"PAT\r");
// 						g_programming_countdown = PROGRAMMING_MESSAGE_TIMEOUT_PERIOD;
// 					}
// 				}
// 				else
// 				{
// 					if(sb_buff->fields[SB_FIELD1][0])
// 					{
// 						if(strlen(sb_buff->fields[SB_FIELD1]) <= MAX_PATTERN_TEXT_LENGTH)
// 						{
// 							strcpy(g_tempStr, sb_buff->fields[SB_FIELD1]);
// 						
// 							if(g_event == EVENT_FOXORING)
// 							{
// 								if(g_foxoring_fox == FOXORING_EVENT_FOXA)
// 								{
// 									strcpy(g_messages_text[FOXA_PATTERN_TEXT], g_tempStr);
// 									g_ee_mgr.updateEEPROMVar(FoxA_pattern_text, g_messages_text[FOXA_PATTERN_TEXT]);
// 								}
// 								else if(g_foxoring_fox == FOXORING_EVENT_FOXB)
// 								{
// 									strcpy(g_messages_text[FOXB_PATTERN_TEXT], g_tempStr);
// 									g_ee_mgr.updateEEPROMVar(FoxB_pattern_text, g_messages_text[FOXB_PATTERN_TEXT]);
// 								}
// 								else if(g_foxoring_fox == FOXORING_EVENT_FOXC)
// 								{
// 									strcpy(g_messages_text[FOXC_PATTERN_TEXT], g_tempStr);
// 									g_ee_mgr.updateEEPROMVar(FoxC_pattern_text, g_messages_text[FOXC_PATTERN_TEXT]);
// 								}
// 							}
// 							else
// 							{
// 								strcpy(g_messages_text[PATTERN_TEXT], g_tempStr);
// 							}
// 						}
// 						else
// 						{
// 							sb_send_string((char*)"Illegal pattern\n");
// 						}
// 					}
// 				}
// 				
// 				if(!(g_cloningInProgress)) reportSettings();
			}
			break;
			
			case SB_MESSAGE_KEY:
			{
				if(sb_buff->fields[SB_FIELD1][0])
				{
					if(sb_buff->fields[SB_FIELD1][0] == '0')    
					{
 						stopEventNow(PROGRAMMATIC);
						LEDS.setRed(OFF);
 						keyTransmitter(OFF);
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '1')  
					{
 						stopEventNow(PROGRAMMATIC);
						LEDS.setRed(ON);
 						keyTransmitter(ON);
					}
					else
					{
						sb_send_string((char*)"err\n");
					}
				}
				else
				{
					LEDS.setRed(OFF);
					antennaIsConnected(); /* Set green LED appropriately */
 					keyTransmitter(OFF);
					startEventNow(PROGRAMMATIC);
				}
			}
			break;

			case SB_MESSAGE_SYNC:
			{
				if(sb_buff->fields[SB_FIELD1][0])
				{
					if(sb_buff->fields[SB_FIELD1][0] == '0')       /* Stop the event. Re-sync will occur on next start */
					{
 						stopEventNow(PROGRAMMATIC);
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '1')  /* Start the event, re-syncing to a start time of now - same as a button press */
					{
						LEDS.setRed(OFF);
 						startEventNow(PROGRAMMATIC);
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '2')  /* Start the event at the programmed start time */
					{
 						g_event_enabled = false;					/* Disable an event currently underway */
 						startEventUsingRTC();
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '3')  /* Start the event immediately with transmissions starting now */
					{
 						setupForFox(INVALID_FOX, START_TRANSMISSIONS_NOW);
					}
					else
					{
						sb_send_string((char*)"err\n");
					}
				}
				else
				{
					sb_send_string((char*)"err\n");
				}
			}
			break;

			case SB_MESSAGE_SET_STATION_ID:
			{
// 				if(sb_buff->fields[SB_FIELD1][0])
// 				{
// 					int len = 0;
// 					
// 					if((sb_buff->fields[SB_FIELD1][0] == '\"') && (sb_buff->fields[SB_FIELD1][1] == '\"')) /* "" */
// 					{
// 						g_messages_text[STATION_ID][0] = '\0';
// 					}
// 					else
// 					{
// 						strcpy(g_tempStr, " "); /* Add a Space before ID gets sent */
// 						strcat(g_tempStr, sb_buff->fields[SB_FIELD1]);
// 
// 						if(sb_buff->fields[SB_FIELD2][0])
// 						{
// 							strcat(g_tempStr, " ");
// 							strcat(g_tempStr, sb_buff->fields[SB_FIELD2]);
// 						}
// 						
// 						len = MIN(MAX_PATTERN_TEXT_LENGTH, strlen(g_tempStr));
// 						strncpy((char*)g_messages_text[STATION_ID], g_tempStr, len);
// 					}
// 					
// 					if(g_cloningInProgress)
// 					{				
// 						for(int i=0; i<len; i++)
// 						{
// 							g_event_checksum += g_messages_text[STATION_ID][i];
// 						}
// 
// 						sb_send_string((char*)"ID\r");
// 						g_programming_countdown = PROGRAMMING_MESSAGE_TIMEOUT_PERIOD;
// 					}
// 				}
// 
// 				g_ee_mgr.updateEEPROMVar(StationID_text, g_messages_text[STATION_ID]);
// 				
// 				if(g_messages_text[STATION_ID][0])
// 				{
// 					g_time_needed_for_ID = timeNeededForID();
// 				}
// 				
// 				if(!(g_cloningInProgress)) reportSettings();
			}
			break;


			case SB_MESSAGE_CODE_SETTINGS:
			{
// 				if(sb_buff->fields[SB_FIELD1][0] == 'S')
// 				{
// 					char x = sb_buff->fields[SB_FIELD2][0];
// 
// 					if(x)
// 					{
//  						uint8_t speed = atol(sb_buff->fields[SB_FIELD2]);
//  						g_id_codespeed = CLAMP(MIN_CODE_SPEED_WPM, speed, MAX_CODE_SPEED_WPM);
// 						g_ee_mgr.updateEEPROMVar(Id_codespeed, (void*)&g_id_codespeed);
// 
// 						if(g_messages_text[STATION_ID][0])
// 						{
// 							g_time_needed_for_ID = timeNeededForID();
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sprintf(g_tempStr, "err\n");
// 				}
// 
// 				if(!(g_cloningInProgress)) reportSettings();
			}
			break;

			case SB_MESSAGE_MASTER:
			{
// 				if(sb_buff->fields[SB_FIELD1][0] == 'P')
// 				{
// 					if(!g_send_clone_success_countdown)
// 					{
// 						g_cloningInProgress = true;
// 						sb_send_string((char*)"MAS\r");
// 						g_programming_countdown = PROGRAMMING_MESSAGE_TIMEOUT_PERIOD;
// 						suspendEvent();
// 						g_event_checksum = 0;
// 					}
// 				}
// 				else if((sb_buff->fields[SB_FIELD1][0] == 'Q') && g_cloningInProgress)
// 				{
// 					uint32_t sum = atol(sb_buff->fields[SB_FIELD2]);
// 					g_cloningInProgress = false;
// 					g_programming_countdown = 0;
// 					if(sum == g_event_checksum)
// 					{
// 						sb_send_string((char*)"MAS ACK\r");
// 						g_send_clone_success_countdown = 18000;
// 						setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);   /* Start the event if one is configured */
// 						g_start_event = true;
// 					}
// 					else
// 					{
// 						sb_send_string((char*)"MAS NAK\r");
// 					}
// 				}
// 				else
// 				{
// 					if(sb_buff->fields[SB_FIELD1][0])
// 					{
// 						if((sb_buff->fields[SB_FIELD1][0] == 'M') || (sb_buff->fields[SB_FIELD1][0] == '1'))
// 						{
//  							g_isMaster = true;
// 							isMasterCountdownSeconds = 600; /* Remain Master for 10 minutes */
// 						}
// // 						else
// // 						{
// // 							g_isMaster = false;
// // 							isMasterCountdownSeconds = 0;
// // 							g_start_event = true;
// // 							reportSettings();
// // 						}
// 					}
// 				}
			}
			break;
			
			case SB_MESSAGE_EVENT:
			{
// 				if(g_cloningInProgress)
// 				{
// 					sb_send_string((char*)"EVT F\r");
// 					g_event = EVENT_FOXORING;
// 					g_ee_mgr.updateEEPROMVar(Event_setting, (void*)&g_event);
// 					g_programming_countdown = PROGRAMMING_MESSAGE_TIMEOUT_PERIOD;
// 					g_event_checksum += 'F';
// 				}
// 				else
// 				{
// 					if(sb_buff->fields[SB_FIELD1][0] == 'F')
// 					{
// 						g_event = EVENT_FOXORING;
// 						g_ee_mgr.updateEEPROMVar(Event_setting, (void*)&g_event);
// 						init_transmitter(getFrequencySetting());
// 						setupForFox(getFoxSetting(), START_NOTHING);
// 					}
// 					else if(sb_buff->fields[SB_FIELD1][0])
// 					{
// 						g_event = EVENT_NONE;
// 						g_ee_mgr.updateEEPROMVar(Event_setting, (void*)&g_event);
// 						init_transmitter(getFrequencySetting());
// 						setupForFox(getFoxSetting(), START_NOTHING);
// 					}
// 				}
// 				
// 				if(!(g_cloningInProgress)) reportSettings();
			}
			break;

			case SB_MESSAGE_CLOCK:
			{
// 				if(!sb_buff->fields[SB_FIELD1][0] || sb_buff->fields[SB_FIELD1][0] == 'T')   /* Current time format "YYMMDDhhmmss" */
// 				{		 
// 					if(sb_buff->fields[SB_FIELD2][0])
// 					{
// 						strncpy(g_tempStr, sb_buff->fields[SB_FIELD2], 12);
//   						time_t t;
// 						  
// 						if(g_cloningInProgress)
// 						{
// 							t = atol(g_tempStr);
// 						}
// 						else
// 						{	
// 							g_tempStr[12] = '\0';               /* truncate to no more than 12 characters */
// 							t = validateTimeString(g_tempStr, null);
// 						}
//   
//   						if(t)
//   						{
//  							set_system_time(t);
// 							 
// 							if(g_cloningInProgress)
// 							{
// 								sprintf(g_tempStr, "CLK T %lu\r", t);
// 								sb_send_string(g_tempStr);
// 								g_programming_countdown = PROGRAMMING_MESSAGE_TIMEOUT_PERIOD;
// 								g_event_checksum += t;
// 							}
// 							else
// 							{
//   								setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);   /* Start the event if one is configured */
// 							}
//  						}
// 					}
// 				}
// 				else if(sb_buff->fields[SB_FIELD1][0] == 'S')  /* Event start time */
// 				{
// 					strncpy(g_tempStr, sb_buff->fields[SB_FIELD2], 12);
// 					time_t s;
// 						
// 					if(g_cloningInProgress)
// 					{
// 						s = atol(g_tempStr);
// 					}
// 					else
// 					{
// 						g_tempStr[12] = '\0';               /* truncate to no more than 12 characters */
// 						s = validateTimeString(g_tempStr, null);
// 					}
//  
//  					if(s)
//  					{
// 						if(g_cloningInProgress)
// 						{
// 							g_event_start_epoch = atol(g_tempStr);
// 					
// 							g_ee_mgr.updateEEPROMVar(Event_start_epoch, (void*)&g_event_start_epoch);
// 							sb_send_string((char*)"CLK\r");
// 							g_programming_countdown = PROGRAMMING_MESSAGE_TIMEOUT_PERIOD;
// 							g_event_checksum += s;
// 						}
// 						else
// 						{
//  							g_event_start_epoch = s;
//  							g_event_finish_epoch = MAX(g_event_finish_epoch, (g_event_start_epoch + SECONDS_24H));
// 							g_ee_mgr.updateEEPROMVar(Event_start_epoch, (void*)&g_event_start_epoch);
//  							setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);
// 							if(eventScheduled())
// 							{
// 								startEventUsingRTC();
// 							}
//  						}
// 					 }
// 				}
// 				else if(sb_buff->fields[SB_FIELD1][0] == 'F')  /* Event finish time */
// 				{
// 					strncpy(g_tempStr, sb_buff->fields[SB_FIELD2], 12);
// 					time_t f;
// 					
// 					if(g_cloningInProgress)
// 					{
// 						f = atol(g_tempStr);
// 					}
// 					else
// 					{
// 						g_tempStr[12] = '\0';               /* truncate to no more than 12 characters */
// 						f = validateTimeString(g_tempStr, null);
// 					}					
//  
//  					if(f)
//  					{
// 						if(g_cloningInProgress)
// 						{
// 							g_event_finish_epoch = atol(g_tempStr);
// 							
// 							g_ee_mgr.updateEEPROMVar(Event_finish_epoch, (void*)&g_event_finish_epoch);
// 							sb_send_string((char*)"CLK\r");
// 							g_programming_countdown = PROGRAMMING_MESSAGE_TIMEOUT_PERIOD;
// 							g_event_checksum += f;
// 						}
// 						else
// 						{
//  							g_event_finish_epoch = f;
//   							g_ee_mgr.updateEEPROMVar(Event_finish_epoch, (void*)&g_event_finish_epoch);
//  							setupForFox(INVALID_FOX, START_EVENT_WITH_STARTFINISH_TIMES);
// 							if(eventScheduled())
// 							{
// 								startEventUsingRTC();
// 							}
// 						}
//  					}
// 				}
// 				else if(sb_buff->fields[SB_FIELD1][0] == 'C' && !g_cloningInProgress)  /* Clock calibration */
// 				{
// 					strncpy(g_tempStr, sb_buff->fields[SB_FIELD2], 12);
// 					uint16_t cal;
// 					
// 					cal = atoi(g_tempStr);
// 					RTC_set_calibration(cal);
// 				}
// //  				else if(sb_buff->fields[SB_FIELD1][0] == '*')  /* Sync seconds to zero */
// //  				{
// //  				}
// 
// 				if(!(g_cloningInProgress)) reportSettings();
			}
			break;

			case SB_MESSAGE_VOLTS:
			{
// 				char txt[6];
// 
// 				if(sb_buff->fields[SB_FIELD1][0])
// 				{
// 					float v = atof(sb_buff->fields[SB_FIELD1]);
// 
// 					if((v >= 0.1) && (v <= 15.))
// 					{
//  						g_voltage_threshold = v;
// 						g_ee_mgr.updateEEPROMVar(Voltage_threshold, (void*)&g_voltage_threshold);
// 					}
// 					else
// 					{
// 						sb_send_string((char*)"Err: 0.1 V < thresh < 15.0 V\n");
// 					}
// 
// 				}
// 
// 				dtostrf(g_battery_voltage, 4, 1, txt);
// 				txt[5] = '\0';
//   				sprintf(g_tempStr, "Bat =%s Volts\n", txt);
//  				sb_send_string(g_tempStr);
// 
// 				dtostrf(g_voltage_threshold, 4, 1, txt);
// 				txt[5] = '\0';
//  				sprintf(g_tempStr, "thresh =%s Volts\n", txt);
//  				sb_send_string(g_tempStr);
			}
			break;

			default:
			{
				sb_send_string(HELP_TEXT_TXT);
			}
			break;
		}

		sb_buff->id = SB_MESSAGE_EMPTY;
		sb_send_NewPrompt();

// 		g_LED_timeout_countdown = LED_TIMEOUT_SECONDS;
// 		g_config_error = NULL_CONFIG;   /* Trigger a new configuration enunciation */

	}
}

/* The compiler does not seem to optimize large switch statements correctly */
void __attribute__((optimize("O0"))) handleLinkBusMsgs()
{
	LinkbusRxBuffer* lb_buff;
	static uint8_t new_event_parameter_count = 0;
	bool send_ack = true;

	while((lb_buff = nextFullLBRxBuffer()))
	{
		LBMessageID msg_id = lb_buff->id;

		switch(msg_id)
		{
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
					
					if((lenstr == 2) && (str[0] == '\\') && (str[1] == 'B')) /* backspace */
					{
						g_enable_manual_transmissions = false; /* simple thread collision avoidance */
						g_text_buff.pop();
						g_enable_manual_transmissions = hold;
					}
					else if(lenstr > 1)
					{
						int i = 0;
						
						g_enable_manual_transmissions = false; /* simple thread collision avoidance */
						while(!g_text_buff.full() && i<lenstr && i<LINKBUS_MAX_MSG_FIELD_LENGTH)
						{
							g_text_buff.put(lb_buff->fields[LB_MSG_FIELD1][i++]);
						}
						g_enable_manual_transmissions = hold;
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
							g_enable_manual_transmissions = false; /* simple thread collision avoidance */
							g_text_buff.put(c);
							g_enable_manual_transmissions = hold;
						}
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
					g_WiFi_shutdown_seconds = 120;                                  /* wait 2 more minutes before shutting down WiFi */
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
						if(!g_event_enabled) g_start_event = true; /* Attempt to launch any event that is already set */
						antennaIsConnected(); /* Set green LED appropriately */
					}
					else if(f1 == '3')                      /* ESP is ready for power off" */
					{			
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
					
					if(pwr_mW != txGetPowerMw())
					{
						ec = txSetParameters(&pwr_mW, NULL);
						if(ec)
						{
							g_last_error_code = ec;
						}
						else
						{
							new_event_parameter_count++;
						}

						sprintf(g_tempStr, "M,%u", pwr_mW);
						lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_TX_POWER_LABEL, g_tempStr);
					}
					
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
					if((g_antenna_connect_state != ANT_CONNECTED) && !g_tx_power_is_zero)
					{
						g_last_error_code = ERROR_CODE_NO_ANTENNA_FOR_BAND;
					}
					else
					{
						if(f1 == '1')   /* Xmit immediately using current settings */
						{
							if((g_antenna_connect_state == ANT_CONNECTED) || g_tx_power_is_zero)
							{
								/* Set the Morse code pattern and speed */
								g_event_enabled = false; // prevent interrupts from affecting the settings
								powerToTransmitter(ON);
								bool repeat = true;
								makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
								g_code_throttle = throttleValue(g_pattern_codespeed);
								g_event_start_epoch = 1;                     /* have it start a long time ago */
								g_event_finish_epoch = MAX_TIME;             /* run for a long long time */
								g_on_air_seconds = 9999;                    /* on period is very long */
								g_off_air_seconds = 0;                      /* off period is very short */
								g_on_the_air = 9999;                        /*  start out transmitting */
								g_sendID_seconds_countdown = MAX_UINT16;    /* wait a long time to send the ID */
								g_event_commenced = true;                   /* get things running immediately */
								g_event_enabled = true;                     /* get things running immediately */
								g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
								antennaIsConnected(); /* Set green LED appropriately */
							}
							else
							{
								g_last_error_code = ERROR_CODE_NO_ANTENNA_FOR_BAND;
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
				}
				else if(f1 == '0')  /* Prepare to receive new event data */
				{
//					suspendEvent();
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
						g_event_start_epoch = mtime;
						set_system_time(ds3231_get_epoch(NULL));    /* update system clock */
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
							g_event_finish_epoch = mtime;
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
						ds3231_set_date_time(g_tempStr, RTC_CLOCK);
						set_system_time(ds3231_get_epoch(NULL));    /* update system clock */
					}
					else
					{
						sprintf(g_tempStr, "%lu", time(NULL));
						lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
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

							uint32_t temp_time = ds3231_get_epoch(NULL);
							set_system_time(temp_time);

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
				float bat = (float)g_lastConversionResult[EXTERNAL_BATTERY_VOLTAGE];
				bat *= 172.;
				bat *= 0.0005;
				bat += 1.;
				
				dtostrf(bat, 4, 0, g_tempStr);
				g_tempStr[5] = '\0';

				lb_broadcast_str(g_tempStr, "!BAT");

				/* The system clock gets re-initialized whenever a battery message is received. This
				 * is just to ensure the two stay closely in sync while the user interface is active */
				set_system_time(ds3231_get_epoch(NULL));    /* update system clock */
			}
			break;

			case LB_MESSAGE_TEMP:
			{
// 				int16_t v;
// 				if(!ds3231_get_temp(&v))
// 				{
// 					lb_broadcast_num(v, "!TEM");
// 				}
				
				dtostrf(temperatureC(), 4, 1, g_tempStr);
				g_tempStr[5] = '\0';

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


/***********************************************************************
 * Private Function Prototypes
 *
 * These functions are available only within this file
 ************************************************************************/
void updateAntennaStatus(void)
{
	static bool lastAntennaConnectionState = false;
	static uint8_t antennaReadCount = 3;
	bool ant = antennaIsConnected();

	if(!ant)    /* immediately detect disconnection */
	{
		if(g_antenna_connect_state != ANT_DISCONNECTED)
		{
			g_antenna_connect_state = ANT_DISCONNECTED;
			g_antenna_connection_changed = true;
		}
		
		antennaReadCount = 3;
	}
	else if(g_antenna_connect_state != ANT_CONNECTED)
	{
		if(ant == lastAntennaConnectionState)
		{
			if(antennaReadCount)
			{
				antennaReadCount--;

				if(!antennaReadCount)
				{
					g_antenna_connect_state = ANT_CONNECTED;
					g_antenna_connection_changed = true;
					antennaReadCount = 3;
				}
			}
		}
		else
		{
			antennaReadCount = 3;
		}
	}

	lastAntennaConnectionState = ant;
}

bool __attribute__((optimize("O0"))) eventEnabled()
{
	time_t now;
	int32_t dif;

	dif = timeDif(g_event_finish_epoch, g_event_start_epoch);

	now = time(null);
	dif = timeDif(now, g_event_finish_epoch);
	g_go_to_sleep_now = false;

	if(dif >= 0) /* Event has already finished */
	{
		g_sleepType = SLEEP_FOREVER;
		g_time_to_wake_up = MAX_TIME;
		g_wifi_enable_delay = 2;
		return(false); /* completed events are never enabled */
	}
	
	powerToTransmitter(ON);
	dif = timeDif(now, g_event_start_epoch);

	if(dif >= -30)  /* Don't sleep if the event starts in 30 seconds or less, or has already started */
	{
		g_sleepType = DO_NOT_SLEEP;
		g_time_to_wake_up = g_event_start_epoch - 5;
		return( true);
	}

	/* If we reach here, we have an event that will not start for at least 30 seconds. It needs to be enabled, and a sleep time needs to be calculated
	 * while allowing time for power-up (coming out of sleep) prior to the event start */
	g_time_to_wake_up = g_event_start_epoch - 5;
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

EC __attribute__((optimize("O0"))) launchEvent(SC* statusCode)
{
	set_system_time(ds3231_get_epoch(null));
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

	if(g_event_start_epoch >= g_event_finish_epoch)   /* Finish must be later than start */
	{
		return( ERROR_CODE_EVENT_NOT_CONFIGURED);
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

	if(!g_pattern_codespeed)
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
			int cyclePeriod = g_on_air_seconds + g_off_air_seconds;
			int secondsIntoCycle = dif % cyclePeriod;
			int timeTillTransmit = g_intra_cycle_delay_time - secondsIntoCycle;

			if(timeTillTransmit <= 0)                       /* we should have started transmitting already in this cycle */
			{
				if(g_on_air_seconds <= -timeTillTransmit)   /* we should have finished transmitting in this cycle */
				{
					g_on_the_air = -(cyclePeriod + timeTillTransmit);
					if(statusCode)
					{
						*statusCode = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
					}

					if(!g_event_enabled)
					{
						g_sendID_seconds_countdown = (g_on_air_seconds - g_on_the_air) - g_time_needed_for_ID;
					}
				}
				else    /* we should be transmitting right now */
				{
					g_on_the_air = g_on_air_seconds + timeTillTransmit;
					turnOnTransmitter = true;
					if(statusCode)
					{
						*statusCode = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
					}

					if(!g_event_enabled)
					{
						if(g_time_needed_for_ID < g_on_the_air)
						{
							g_sendID_seconds_countdown = g_on_the_air - g_time_needed_for_ID;
						}
					}
				}
			}
			else    /* it is not yet time to transmit in this cycle */
			{
				g_on_the_air = -timeTillTransmit;
				if(statusCode)
				{
					*statusCode = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
				}

				if(!g_event_enabled)
				{
					g_sendID_seconds_countdown = timeTillTransmit + g_on_air_seconds - g_time_needed_for_ID;
				}
			}

			if(turnOnTransmitter)
			{
				bool hold = g_event_enabled;
				g_event_enabled = false; // prevent interrupts from affecting the settings
				bool repeat = true;
				makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
				g_code_throttle = throttleValue(g_pattern_codespeed);
				g_event_enabled = hold;
			}
			else
			{
				keyTransmitter(OFF);
			}

			g_event_commenced = true;
			antennaIsConnected(); /* Set green LED appropriately */
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

bool antennaIsConnected()
{
	bool val = PORTD_get_pin_level(X80M_ANTENNA_DETECT);

	if(g_hardware_error == HARDWARE_OK)
	{
		if(val)
		{
			LEDS.blink(LEDS_GREEN_OFF);		
		}
		else
		{
			LEDS.blink(LEDS_GREEN_ON_CONSTANT);
		}
	}

	return(!val);
}

void initializeAllEventSettings(bool disableEvent)
{
	
}

void suspendEvent()
{
	g_event_enabled = false;    /* get things stopped immediately */
	g_on_the_air = 0;           /*  stop transmitting */
	g_event_commenced = false;  /* get things stopped immediately */
	keyTransmitter(OFF);
	powerToTransmitter(OFF);
	bool repeat = false;
	makeMorse((char*)"\0", &repeat, null);  /* reset makeMorse */
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
			setupForFox(INVALID_FOX, START_EVENT_NOW);
		}
		else if((conf == WAITING_FOR_START) || (conf == SCHEDULED_EVENT_WILL_NEVER_RUN) || (conf == SCHEDULED_EVENT_DID_NOT_START)) /* Start immediately */
		{
			setupForFox(INVALID_FOX, START_EVENT_NOW);
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
			setupForFox(INVALID_FOX, START_EVENT_NOW);
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

// 	g_LED_enunciating = false;
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
	set_system_time(ds3231_get_epoch(null));
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
	bool patternNotSet = true;
	
	if(fox == INVALID_FOX)
	{
		fox = getFoxSetting();
	}

	g_event_enabled = false;
	g_event_commenced = false;
	LEDS.setRed(OFF);
 	set_system_time(ds3231_get_epoch(null));

	switch(fox)
	{
		case FOX_1:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MOE");
				patternNotSet = false;
				g_intra_cycle_delay_time = 0;
			}
		}
		case FOX_2:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MOI");
				patternNotSet = false;
				g_intra_cycle_delay_time = 60;
			}
		}
		case FOX_3:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MOS");
				patternNotSet = false;
				g_intra_cycle_delay_time = 120;
			}
		}
		case FOX_4:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MOH");
				patternNotSet = false;
				g_intra_cycle_delay_time = 180;
			}
		}
		case FOX_5:
		{
			/* Set the Morse code pattern and speed */
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MO5");
				g_intra_cycle_delay_time = 240;
			}
			
//			bool repeat = true;
//			makeMorse(getPatternText(), &repeat, NULL);
			g_code_throttle = throttleValue(g_pattern_codespeed);

			g_sendID_seconds_countdown = 60;			/* wait 1 minute to send the ID */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 240;                    /* off period is very short */
		}
		break;

		case SPRINT_S1:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "ME");
				patternNotSet = false;
				g_intra_cycle_delay_time = 0;
			}
		}
		case SPRINT_S2:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MI");
				patternNotSet = false;
				g_intra_cycle_delay_time = 12;
			}
		}
		case SPRINT_S3:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MS");
				patternNotSet = false;
				g_intra_cycle_delay_time = 24;
			}
		}
		case SPRINT_S4:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MH");
				patternNotSet = false;
				g_intra_cycle_delay_time = 36;
			}
		}
		case SPRINT_S5:
		{
			{
				if(patternNotSet)
				{
					sprintf(g_messages_text[PATTERN_TEXT], "M5");
					g_intra_cycle_delay_time = 48;
				}
			}
			
//			bool repeat = true;
//			makeMorse(getPatternText(), &repeat, NULL);
			g_pattern_codespeed = 8;
			g_code_throttle = throttleValue(g_pattern_codespeed);

			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 12;						/* on period is very long */
			g_off_air_seconds = 48;						/* off period is very short */
		}
		break;

		case SPRINT_F1:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "OE");
				patternNotSet = false;
				g_intra_cycle_delay_time = 0;
			}
		}
		case SPRINT_F2:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "OI");
				patternNotSet = false;
				g_intra_cycle_delay_time = 12;
			}
		}
		case SPRINT_F3:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "OS");
				patternNotSet = false;
				g_intra_cycle_delay_time = 24;
			}
		}
		case SPRINT_F4:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "OH");
				patternNotSet = false;
				g_intra_cycle_delay_time = 36;
			}
		}
		case SPRINT_F5:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "O5");
				g_intra_cycle_delay_time = 48;
			}
			
//			bool repeat = true;
//			makeMorse(getPatternText(), &repeat, NULL);
			g_pattern_codespeed = 15;
			g_code_throttle = throttleValue(g_pattern_codespeed);

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

		case FOXORING_EVENT_FOXA:
		{
			init_transmitter(getFrequencySetting());
			patternNotSet = false;
			g_intra_cycle_delay_time = 0;
//			bool repeat = true;
//			makeMorse(getPatternText(), &repeat, NULL);
			g_pattern_codespeed = 8;
			g_code_throttle = throttleValue(g_pattern_codespeed);

			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 0;						/* off period is very short */
		}
		break;
		
		case FOXORING_EVENT_FOXB:
		{
			init_transmitter(getFrequencySetting());
			patternNotSet = false;
			g_intra_cycle_delay_time = 0;
//			bool repeat = true;
//			makeMorse(getPatternText(), &repeat, NULL);
			g_pattern_codespeed = 8;
			g_code_throttle = throttleValue(g_pattern_codespeed);

			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 0;						/* off period is very short */
		}
		break;
		
		case FOXORING_EVENT_FOXC:
		{
			init_transmitter(getFrequencySetting());
			patternNotSet = false;
			g_intra_cycle_delay_time = 0;
//			bool repeat = true;
//			makeMorse(getPatternText(), &repeat, NULL);
			g_pattern_codespeed = 8;
			g_code_throttle = throttleValue(g_pattern_codespeed);

			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 0;						/* off period is very short */
		}
		break;
		
		case SPECTATOR:
		{
			sprintf(g_messages_text[PATTERN_TEXT], "S");
			patternNotSet = false;
			g_intra_cycle_delay_time = 0;
		}
		case BEACON:
		default:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MO");
			}
			
			g_intra_cycle_delay_time = 0;
//			bool repeat = true;
//			makeMorse(getPatternText(), &repeat, NULL);
			g_pattern_codespeed = 8;
			g_code_throttle = throttleValue(g_pattern_codespeed);

			g_sendID_seconds_countdown = 600;			/* wait 10 minutes send the ID */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 0;						/* off period is very short */
		}
		break;
	}

	if(action == START_NOTHING)
	{
		g_event_commenced = false;                   /* get things running immediately */
		g_event_enabled = false;                     /* get things running immediately */

 		g_use_rtc_for_startstop = false;
 		g_event_enabled = false;
		keyTransmitter(OFF);
		LEDS.setRed(OFF);
		powerToTransmitter(OFF);
	}
	else if(action == START_EVENT_NOW)
	{
// 		g_seconds_since_sync = 0;                   /* Total elapsed time since synchronization */
//  		g_use_rtc_for_startstop = false;
//  		g_event_enabled = true;
// 		g_event_commenced = true;					/* get things running immediately */
// 		g_event_enabled = true;						/* get things running immediately */

		EC result;
		set_system_time(ds3231_get_epoch(&result));
		time_t now = time(null);
		
		if(result == ERROR_CODE_NO_ERROR)
		{
			g_event_start_epoch = now;
			if(g_event_start_epoch > g_event_finish_epoch)
			{
				g_event_finish_epoch = g_event_start_epoch + DAY;
			}
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
//		bool repeat = true;
//		makeMorse(getPatternText(), &repeat, NULL);
		g_code_throttle = throttleValue(g_pattern_codespeed);

// 		g_event_start_epoch = 1;                     /* have it start a long time ago */
// 		g_event_finish_epoch = MAX_TIME;             /* run for a long long time */
		g_on_the_air = g_on_air_seconds;			/* start out transmitting */
		g_event_commenced = true;                   /* get things running immediately */
		g_event_enabled = true;                     /* get things running immediately */
		g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
		antennaIsConnected(); /* Set green LED appropriately */
// 		g_seconds_since_sync = (g_fox_counter - 1) * g_on_air_interval_seconds; /* Total elapsed time since start of event */
 		g_use_rtc_for_startstop = false;
 		g_event_enabled = true;
// 		g_initialize_fox_transmissions = INIT_EVENT_STARTING_NOW;
	}
	else         /* if(action == START_EVENT_WITH_STARTFINISH_TIMES) */
	{
		SC sc;
 //		EC ec = 
		launchEvent(&sc);
	}

// 	sendMorseTone(OFF);
// 	g_code_throttle    = 0;                 /* Adjusts Morse code speed */
// 	g_on_the_air       = false;             /* Controls transmitter Morse activity */

// 	g_config_error = NULL_CONFIG;           /* Trigger a new configuration enunciation */
// 	digitalWrite(PIN_LED, OFF);             /*  LED Off - in case it was left on */
// 
// 	digitalWrite(PIN_CW_KEY_LOGIC, OFF);    /* TX key line */
// 	g_sendAMmodulation = false;
// 	g_LED_enunciating = false;
// 	g_config_error = NULL_CONFIG;           /* Trigger a new configuration enunciation */
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


bool reportTimeTill(time_t from, time_t until, const char* prefix, const char* failMsg)
{
	bool failure = false;

	if(from >= until)   /* Negative time */
	{
		failure = true;
		if(failMsg)
		{
			sb_send_string((char*)failMsg);
		}
	}
	else
	{
		if(prefix)
		{
			sb_send_string((char*)prefix);
		}
		time_t dif = until - from;
		uint16_t years = dif / YEAR;
		time_t hold = dif - (years * YEAR);
		uint16_t days = hold / DAY;
		hold -= (days * DAY);
		uint16_t hours = hold / HOUR;
		hold -= (hours * HOUR);
		uint16_t minutes = hold / MINUTE;
		uint16_t seconds = hold - (minutes * MINUTE);

		g_tempStr[0] = '\0';

		if(years)
		{
			sprintf(g_tempStr, "%d yrs ", years);
			sb_send_string(g_tempStr);
		}

		if(days)
		{
			sprintf(g_tempStr, "%d days ", days);
			sb_send_string(g_tempStr);
		}

		if(hours)
		{
			sprintf(g_tempStr, "%d hrs ", hours);
			sb_send_string(g_tempStr);
		}

		if(minutes)
		{
			sprintf(g_tempStr, "%d min ", minutes);
			sb_send_string(g_tempStr);
		}

		sprintf(g_tempStr, "%d sec", seconds);
		sb_send_string(g_tempStr);

		sb_send_NewLine();
		g_tempStr[0] = '\0';
	}

	return( failure);
}



ConfigurationState_t clockConfigurationCheck(void)
{
	time_t now = time(null);
	
	if((g_event_finish_epoch <= MINIMUM_VALID_EPOCH) || (g_event_start_epoch <= MINIMUM_VALID_EPOCH) || (now <= MINIMUM_VALID_EPOCH))
	{
		return(CONFIGURATION_ERROR);
	}

	if(g_event_finish_epoch <= g_event_start_epoch) /* Event configured to finish before it started */
	{
		return(CONFIGURATION_ERROR);
	}

	if(now > g_event_finish_epoch)  /* The scheduled event is over */
	{
		return(CONFIGURATION_ERROR);
	}

	if(now > g_event_start_epoch)       /* Event should be running */
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
	set_system_time(ds3231_get_epoch(null));
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
			sb_send_string((char*)"Start with > SYN 2\n");
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
	Fox_t fox;
	
	if(g_event == EVENT_FOXORING)
	{
		switch(g_foxoring_fox)
		{
			case FOXORING_EVENT_FOXA:
			{
				fox = FOXORING_EVENT_FOXA;
			}
			break;
			
			case FOXORING_EVENT_FOXB:
			{
				fox = FOXORING_EVENT_FOXB;
			}
			break;
			
			case FOXORING_EVENT_FOXC:
			{
				fox = FOXORING_EVENT_FOXC;
			}
			break;
			
			default:
			{
				fox = INVALID_FOX;
			}
			break;
		}
	}
	else
	{
		fox = g_fox;
	}
	
	return fox;
}

char* getPatternText(void)
{
	char* txt = NULL;
		
	if(g_event == EVENT_FOXORING)
	{
		switch(g_foxoring_fox)
		{
			case FOXORING_EVENT_FOXA:
			{
				txt = g_messages_text[FOXA_PATTERN_TEXT];
			}
			break;
				
			case FOXORING_EVENT_FOXB:
			{
				txt = g_messages_text[FOXB_PATTERN_TEXT];
			}
			break;
				
			case FOXORING_EVENT_FOXC:
			{
				txt = g_messages_text[FOXC_PATTERN_TEXT];
			}
			break;
				
			default:
			{
				txt = g_messages_text[FOXA_PATTERN_TEXT];
			}
			break;
		}
	}
	else
	{
		txt = g_messages_text[PATTERN_TEXT];
	}
	
	return txt;
}

Frequency_Hz getFrequencySetting(void)
{
	Frequency_Hz freq;
		
	if(g_event == EVENT_FOXORING)
	{
		switch(g_foxoring_fox)
		{
			case FOXORING_EVENT_FOXA:
			{
				freq = g_foxoring_frequencyA;
			}
			break;
				
			case FOXORING_EVENT_FOXB:
			{
				freq = g_foxoring_frequencyB;
			}
			break;
				
			case FOXORING_EVENT_FOXC:
			{
				freq = g_foxoring_frequencyC;
			}
			break;
				
			default:
			{
				freq = EEPROM_FOXORING_FREQUENCYA_DEFAULT;
			}
			break;
		}
	}
	else
	{
		freq = g_80m_frequency;
	}

	return(freq);
}

bool eventScheduled(void)
{
	set_system_time(ds3231_get_epoch(null));
	time_t now = time(null);	
	bool result = ((now > MINIMUM_VALID_EPOCH) && (g_event_start_epoch > now) && (g_event_finish_epoch > g_event_start_epoch));
	return(result);
}