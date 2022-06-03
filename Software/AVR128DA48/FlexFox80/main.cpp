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
	AWAKENED_BY_POWERUP,
	AWAKENED_BY_CLOCK,
	AWAKENED_BY_ANTENNA,
	AWAKENED_BY_BUTTONPRESS
} Awakened_t;


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
static bool g_powerUp_initialization_complete = false;
static bool g_perform_3V3_init = false;

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
static volatile SleepType g_sleepType = DO_NOT_SLEEP;

char g_messages_text[2][MAX_PATTERN_TEXT_LENGTH + 1] = { "\0", "\0" };
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
volatile bool g_event_commenced = false;
volatile bool g_check_for_next_event = false;
volatile bool g_waiting_for_next_event = false;
volatile uint16_t g_battery_empty_mV = EEPROM_BATTERY_EMPTY_MV;

static volatile bool g_sufficient_power_detected = false;
static volatile bool g_enableHardwareWDResets = false;
extern volatile bool g_tx_power_is_zero;

static volatile bool g_go_to_sleep_now = false;
static volatile bool g_sleeping = false;
static volatile time_t g_seconds_left_to_sleep = 0;
static volatile time_t g_seconds_to_sleep = MAX_TIME;
static volatile Awakened_t g_awakenedBy = AWAKENED_BY_POWERUP;

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
volatile bool g_long_button_press = false;

leds LEDS = leds();
#define TEXT_BUFF_SIZE 50
CircularStringBuff g_text_buff = CircularStringBuff(TEXT_BUFF_SIZE);

EepromManager g_ee_mgr;

Fox_t g_fox = FOX_1;
int8_t g_utc_offset;
uint8_t g_unlockCode[8];
bool g_use_rtc_for_startstop = false;

volatile bool g_enable_manual_transmissions = true;

/***********************************************************************
 * Private Function Prototypes
 *
 * These functions are available only within this file
 ************************************************************************/
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


/*******************************/
/* Hardcoded event support     */
/*******************************/
void initializeAllEventSettings(bool disableEvent);
void suspendEvent(void);
void stopEventNow(EventActionSource_t activationSource);
void startEventNow(EventActionSource_t activationSource);
void startEventUsingRTC(void);
void setupForFox(Fox_t* fox, EventAction_t action);
time_t validateTimeString(char* str, time_t* epochVar, int8_t offsetHours);
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
    if(PORTA.INTFLAGS & (1 << RTC_SQW)) /* Handle 1-second interrupt */
    {
		system_tick();

		if(g_sleeping)
		{
			if(g_seconds_left_to_sleep)
			{
				g_seconds_left_to_sleep--;
			}

			if(!g_seconds_left_to_sleep)
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
		else
		{
			time_t temp_time = 0;

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
						LEDS.setRed(OFF);
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
								}
							}


							if(!g_on_the_air) /* On-the-air time has just expired */
							{
								if(g_off_air_seconds) /* If there will be a pause in transmissions before resuming */
								{
									keyTransmitter(OFF);
									g_on_the_air -= g_off_air_seconds;
									repeat = true;
									makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);    /* Reset pattern to start */
									g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
									LEDS.setRed(OFF);

									/* Enable sleep during off-the-air periods */
									int32_t timeRemaining = 0;
									time(&temp_time);
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
											g_seconds_to_sleep = (time_t)(g_off_air_seconds - 10);
											g_sleepType = SLEEP_UNTIL_NEXT_XMSN;
											g_go_to_sleep_now = true;
											g_sendID_seconds_countdown = MAX(0, g_sendID_seconds_countdown - (int)g_seconds_to_sleep);
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
						else if(g_on_the_air < 0)   /* off the air - g_on_the_air = 0 means all transmissions are disabled */
						{
							g_on_the_air++;

							if(!g_on_the_air)       /* off-the-air time has expired */
							{
								g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
								g_on_the_air = g_on_air_seconds;
								g_code_throttle = throttleValue(g_pattern_codespeed);
								bool repeat = true;
								makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
							}
						}
					}
				}
				else if(g_event_start_epoch > 0) /* off the air - waiting for the start time to arrive */
				{
					time(&temp_time);

					if(temp_time >= g_event_start_epoch) /* Time for the event to start */
					{
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
							makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
						}

						g_event_commenced = true;
						LEDS.blink(LEDS_GREEN_BLINK_FAST);
						LEDS.blink(LEDS_RED_OFF);
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

						if(!g_WiFi_shutdown_seconds)
						{
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
    }

    PORTA.INTFLAGS = 0xFF; /* Clear all PORTA interrupt flags */
}

/**
PORTD interrupts:
Antenna disconnect interrupt.
*/
ISR(PORTD_PORT_vect)
{
	if(PORTD.INTFLAGS & (1 << X80M_ANTENNA_DETECT)) /* Handle antenna change */
	{
		bool ant = antennaIsConnected();

		if(!ant)    /* immediately detect disconnection */
		{
			if(g_antenna_connect_state != ANT_DISCONNECTED)
			{
				g_antenna_connect_state = ANT_DISCONNECTED;
				g_antenna_connection_changed = true;
				inhibitRFOutput(true);
			}
		}
	}

    PORTD.INTFLAGS = 0xFF; /* Clear all PORTD interrupt flags */
}



/**
Periodic tasks not requiring precise timing. Rate = 300 Hz
*/
ISR(TCB0_INT_vect)
{
    if(TCB0.INTFLAGS & TCB_CAPT_bm)
    {
		static bool conversionInProcess = false;
		static int8_t indexConversionInProcess = 0;
		static uint16_t codeInc = 0;
		bool repeat, finished;
		
		if(g_util_tick_countdown)
		{
			g_util_tick_countdown--;
		}
		
		// Check switch state		
		if(!PORTC_get_pin_level(SWITCH))
		{
			if(g_switch_closed_count<1000)
			{
				g_switch_closed_count++;
			}
		}		
							
		updateAntennaStatus();

		static bool key = false;

		if(g_event_enabled && g_event_commenced) /* Handle cycling transmissions */
		{
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
							makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
							key = makeMorse(NULL, &repeat, &finished);
						}

						if(key)
						{
							powerToTransmitter(ON);
							LEDS.setRed(ON);
						}
					}
				}
				else
				{
					keyTransmitter(key);
					LEDS.setRed(key);
					codeInc = g_code_throttle;
				}
			}
			else if(!g_on_the_air)
			{
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
		else if(g_enable_manual_transmissions) /* Handle single-character transmissions */
		{
			static bool charFinished = true;
			static bool idle = true;
			bool sendBuffEmpty = g_text_buff.empty();
			repeat = false;
			
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

						powerToTransmitter(!charFinished);
						keyTransmitter(key);
						LEDS.setRed(key);
						codeInc = g_code_throttle;
					}
				}
				else
				{
					keyTransmitter(key);
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

			volatile uint8_t i; /* volatile to prevent optimization performing undefined behavior */
			indexConversionInProcess = -1;

			for(i = 0; i < NUMBER_OF_POLLED_ADC_CHANNELS; i++)
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
	if(PORTC.INTFLAGS & (1 << SWITCH))
	{
		if(g_sleeping)
		{
			g_go_to_sleep_now = false;
			g_sleeping = false;
			LEDS.resume();
			g_awakenedBy = AWAKENED_BY_BUTTONPRESS;	
			g_waiting_for_next_event = false; /* Ensure the wifi module does not get shut off prematurely */
		}
		else if(g_switch_closed_count >= 1000)
		{
			g_long_button_press = true;
		}
		else if(g_switch_closed_count > 10) /* debounce */
		{
			if(!LEDS.active())
			{
				LEDS.resume();
			}
			else
			{
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
	
	PORTC.INTFLAGS = 0xFF; /* Clear all flags */
}


void powerDown3V3(void)
{
	PORTA_set_pin_level(V3V3_PWR_ENABLE, LOW);	
	PORTB_set_pin_level(MAIN_PWR_ENABLE, LOW);
	g_powerUp_initialization_complete = false;
}

void powerUp3V3(void)
{
	powerToTransmitter(OFF); /* Turn off power to FET driver */
	PORTB_set_pin_level(MAIN_PWR_ENABLE, HIGH);  /* Turn on 12V booster circuit */
	PORTA_set_pin_level(V3V3_PWR_ENABLE, HIGH);  /* Enable 3V3 power regulator */
	g_perform_3V3_init = true;
}

#include "dac0.h"

int main(void)
{
	atmel_start_init();
	LEDS.blink(LEDS_OFF);
	PORTB_set_pin_level(MAIN_PWR_ENABLE, ON);	
	powerUp3V3();
	
	g_ee_mgr.initializeEEPROMVars();
	g_ee_mgr.readNonVols();
					
	if(rtc_init() == ERROR_CODE_RTC_NONRESPONSIVE)
	{
		LEDS.blink(LEDS_RED_AND_GREEN_BLINK_FAST); /* LEDs blink an error signal */
	}
	else
	{
		EC result;
		 time_t current_epoch = ds3231_get_epoch(&result);
		if(result == ERROR_CODE_NO_ERROR)
		{
			set_system_time(current_epoch);
		}
	}
	
	wifi_reset(ON);
	wifi_power(ON);
	while(util_delay_ms(250));
	
	if(wifiPresent())
	{
		wifi_power(OFF);
		g_wifi_enable_delay = 3;
	}
	else
	{
		wifi_power(OFF);
		wifi_reset(ON);
		g_wifi_enable_delay = 0;
		g_WiFi_shutdown_seconds = 0;
		LEDS.blink(LEDS_RED_AND_GREEN_BLINK_SLOW); /* LEDs blink an error signal */
	}
	
	while (1) {
		handleLinkBusMsgs();
		handleSerialBusMsgs();
		
		if(g_perform_3V3_init)
		{			
			g_perform_3V3_init = false;
			
			if(init_transmitter() != ERROR_CODE_RF_OSCILLATOR_ERROR)
			{
				g_powerUp_initialization_complete = true;
			}
		}
		
		if(g_powerUp_initialization_complete)
		{
			g_powerUp_initialization_complete = false;
			g_start_event = eventEnabled(); /* Start any event loaded from EEPROM */
			if(!g_start_event)
			{
				LEDS.blink(LEDS_RED_OFF);
				LEDS.blink(LEDS_GREEN_ON_CONSTANT);
			}
		}
		
		if(g_switch_closed_count >= 1000)
		{
			LEDS.blink(LEDS_GREEN_ON_CONSTANT);
			LEDS.blink(LEDS_RED_ON_CONSTANT);
		}
		
		if(g_long_button_press)
		{
			if(g_event_enabled)
			{
				g_end_event = true;
				g_last_status_code = STATUS_CODE_REPORT_IDLE;
			}
			else
			{
				g_start_event = true;
			}
			
			g_long_button_press = false;
		}
		
		if(g_start_event)
		{
			g_start_event = false;	
			
			SC status = STATUS_CODE_IDLE;
			g_last_error_code = launchEvent(&status);
			
			LEDS.blink(LEDS_RED_OFF);

			if(g_event_enabled)
			{
				LEDS.blink(LEDS_GREEN_BLINK_FAST);
			}
			else
			{
				LEDS.blink(LEDS_GREEN_ON_CONSTANT);
			}
			
			if(g_WiFi_shutdown_seconds)
			{
				g_WiFi_shutdown_seconds = MAX(g_WiFi_shutdown_seconds, 10);
			}
		}
		
		if(g_end_event)
		{
			g_end_event = false;		
			suspendEvent();	
			LEDS.blink(LEDS_GREEN_ON_CONSTANT);
			LEDS.blink(LEDS_RED_OFF);
		}
		
		if(g_report_seconds)
		{
			g_report_seconds = false;
			sprintf(g_tempStr, "%lu", time(NULL));
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
			g_waiting_for_next_event = true;
			g_check_for_next_event = false;
			sprintf(g_tempStr, "%u", g_last_status_code);
			lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_ESP_LABEL, (char*)"1");
			g_sleepType = SLEEP_FOREVER;	
		}
		
		if(g_antenna_connection_changed)
		{
			g_antenna_connection_changed = false;
			/* Take appropriate action here */
			if(g_antenna_connect_state == ANT_DISCONNECTED)
			{
				inhibitRFOutput(true);
			}
			else if(g_event_enabled)
			{
				inhibitRFOutput(false);
			}
		}
		
		/********************************
		 * Handle sleep
		 ******************************/
		if(g_go_to_sleep_now)
		{
// 			g_sufficient_power_detected = false;    /* init hardware on return from sleep */

			LEDS.blink(LEDS_OFF);
 			linkbus_disable();	
			shutdown_transmitter();	
			wifi_reset(ON);
			wifi_power(OFF);
			powerDown3V3();
			
			g_waiting_for_next_event = false;

			SLPCTRL_set_sleep_mode(SLPCTRL_SMODE_STDBY_gc);		
			g_sleeping = true;
 			g_seconds_left_to_sleep = g_seconds_to_sleep;
			
 			while(g_go_to_sleep_now)
 			{
				DISABLE_INTERRUPTS();
				sleep_enable();
				ENABLE_INTERRUPTS();
				sleep_cpu();  /* Sleep occurs here */
				sleep_disable();
 			}
 
			g_sleeping = false;
			atmel_start_init();
			powerUp3V3();
 			g_perform_3V3_init = true;
 			linkbus_enable();
			
// 			wdt_init(WD_HW_RESETS);         /* enable hardware WD interrupts */
// 			wdt_reset();                    /* HW watchdog */
// 
			g_wifi_enable_delay = 2; /* Ensure WiFi is enabled and countdown is reset */

//  			if(g_sleepType == SLEEP_FOREVER)
//  			{
// 				g_check_for_next_event = true;
//  			}
			
			if(g_awakenedBy == AWAKENED_BY_BUTTONPRESS)
			{	
				if(g_event_enabled)
				{
					LEDS.blink(LEDS_GREEN_BLINK_FAST);
					LEDS.blink(LEDS_RED_OFF);
				}
				else
				{
					LEDS.blink(LEDS_GREEN_ON_CONSTANT);
					LEDS.blink(LEDS_RED_OFF);
				}
			}

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
		SBMessageID msg_id = sb_buff->id;

		switch(msg_id)
		{
			case SB_MESSAGE_SET_FOX:
			{
				int c = (int)(sb_buff->fields[SB_FIELD1][0]);

				if(c)
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

					if((c >= BEACON) && (c < INVALID_FOX))
					{
 						Fox_t holdFox = (Fox_t)c;
 						g_ee_mgr.updateEEPROMVar(Fox_setting, (void*)&holdFox);
 						if(holdFox != g_fox)
 						{
 							setupForFox(&holdFox, START_NOTHING);
 						}
					}
				}

				if(!fox2Text(g_tempStr, g_fox))
				{
					char str1[25];
					strcpy(str1, g_tempStr);
					sprintf(g_tempStr, "Fox=%s\n", str1);
				}
				else
				{
					sprintf(g_tempStr, "Fox=%u\n", (uint16_t)g_fox);
				}
				
				sb_send_string(g_tempStr);
			}
			break;
			
			case SB_MESSAGE_TX_POWER:
			{
				static uint16_t pwr_mW;

				if(sb_buff->fields[SB_FIELD1][0])
				{
					EC ec;

					pwr_mW = (uint16_t)atoi(sb_buff->fields[SB_FIELD1]);
					
					if(pwr_mW <= MAX_RF_POWER_MW)
					{
						ec = txSetParameters(&pwr_mW, NULL);
						if(ec)
						{
							g_last_error_code = ec;
						}
					}
			
					sprintf(g_tempStr, "PWR=%u mW\n", txGetPowerMw());
					sb_send_string(g_tempStr);
				}
				else
				{
					sprintf(g_tempStr, "PWR=%u mW\n", txGetPowerMw());
					sb_send_string(g_tempStr);
				}
			}
			break;
				
			case SB_MESSAGE_TX_FREQ:
			{
				Frequency_Hz transmitter_freq = 0;

				if(sb_buff->fields[SB_FIELD1][0])
				{
					Frequency_Hz f;
					frequencyVal(sb_buff->fields[SB_FIELD1], &f);

					if(!txSetFrequency(&f, true))
					{
						transmitter_freq = f;
						g_ee_mgr.saveAllEEPROM();
					}
					else
					{
						sb_send_string(TEXT_TX_NOT_RESPONDING_TXT);						
					}
				}

				transmitter_freq = txGetFrequency();

				if(transmitter_freq)
				{
					char result[20];
					if(!frequencyString(result, transmitter_freq))
					{
						sprintf(g_tempStr, "FRE=%s\n", result);						
					}
					else
					{
						sprintf(g_tempStr, "FRE=%lu\n", transmitter_freq);
					}
					
					sb_send_string(g_tempStr);
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
 						startEventNow(PROGRAMMATIC);
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '2')  /* Start the event at the programmed start time */
					{
 						g_event_enabled = false;					/* Disable an event currently underway */
 						startEventUsingRTC();
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '3')  /* Start the event immediately with a transmissions starting now */
					{
 						setupForFox(NULL, START_TRANSMISSIONS_NOW);
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
				if(sb_buff->fields[SB_FIELD1][0])
				{
					strcpy(g_tempStr, " "); /* Space before ID gets sent */
					strcat(g_tempStr, sb_buff->fields[SB_FIELD1]);

					if(sb_buff->fields[SB_FIELD2][0])
					{
						strcat(g_tempStr, " ");
						strcat(g_tempStr, sb_buff->fields[SB_FIELD2]);
					}

					if(strlen(g_tempStr) <= MAX_PATTERN_TEXT_LENGTH)
					{
						strcpy((char*)g_messages_text[STATION_ID], g_tempStr);
						g_ee_mgr.updateEEPROMVar(StationID_text, (void*)g_tempStr);
					}
				}

				if(g_messages_text[STATION_ID][0])
				{
					g_time_needed_for_ID = (600 + timeRequiredToSendStrAtWPM((char*)g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
				}

				sprintf(g_tempStr, "ID:%s\n", g_messages_text[STATION_ID]);
				sb_send_string(g_tempStr);
			}
			break;


			case SB_MESSAGE_CODE_SETTINGS:
			{
				if(sb_buff->fields[SB_FIELD1][0] == 'S')
				{
					char x = sb_buff->fields[SB_FIELD2][0];

					if(x)
					{
 						uint8_t speed = atol(sb_buff->fields[SB_FIELD2]);
 						g_id_codespeed = CLAMP(MIN_CODE_SPEED_WPM, speed, MAX_CODE_SPEED_WPM);
						g_ee_mgr.updateEEPROMVar(Id_codespeed, (void*)&g_id_codespeed);

						if(g_messages_text[STATION_ID][0])
						{
							g_time_needed_for_ID = (600 + timeRequiredToSendStrAtWPM((char*)g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
						}
					}

					sprintf(g_tempStr, "ID Speed: %d wpm\n", g_id_codespeed);
				}
				else
				{
					sprintf(g_tempStr, "err\n");
				}

				sb_send_string(g_tempStr);
			}
			break;

			case SB_MESSAGE_PASSWORD:
			{
				if(sb_buff->fields[SB_FIELD1][0])
				{
 					strncpy(g_tempStr, sb_buff->fields[SB_FIELD1], MAX_UNLOCK_CODE_LENGTH);
 					g_tempStr[MAX_UNLOCK_CODE_LENGTH] = '\0';   /* truncate to no more than max characters */

 					if(only_digits(g_tempStr) && (strlen(g_tempStr) >= MIN_UNLOCK_CODE_LENGTH))
 					{
 						strcpy((char*)g_unlockCode, g_tempStr);
						g_ee_mgr.updateEEPROMVar(UnlockCode, (void*)g_unlockCode);
 					}
				}

				sprintf(g_tempStr, "PWD=%s\n", g_unlockCode);
				sb_send_string(g_tempStr);
			}
			break;

			case SB_MESSAGE_CLOCK:
			{
				bool doprint = false;
 				time_t now = time(null);

				if(!sb_buff->fields[SB_FIELD1][0] || sb_buff->fields[SB_FIELD1][0] == 'T')   /* Current time format "YYMMDDhhmmss" */
				{		 
					if(sb_buff->fields[SB_FIELD2][0])
					{
						strncpy(g_tempStr, sb_buff->fields[SB_FIELD2], 12);
						g_tempStr[12] = '\0';               /* truncate to no more than 12 characters */
						time_t now = time(null);
 						time_t t = validateTimeString(g_tempStr, &now, -g_utc_offset);
 
 						if(t)
 						{
							bool result = ds3231_set_date_time_arducon(g_tempStr, RTC_CLOCK);
							
							if(result)
							{
								sprintf(g_tempStr, TEXT_RTC_NOT_RESPONDING_TXT);
							}
							else
							{
								char t[50];
								set_system_time(ds3231_get_epoch(null));
 								sprintf(g_tempStr, "Time: %s\n", convertEpochToTimeString(now, t, 50));
 								setupForFox(NULL, START_NOTHING);   /* Avoid timing problems if an event is already active */
							}
 						}
					}
					else
					{
							EC result;
							ds3231_get_epoch(&result);
							 
							if(result)
							{
								sprintf(g_tempStr, TEXT_RTC_NOT_RESPONDING_TXT);
							}
							else
							{
								char t[50];
								sprintf(g_tempStr, "Time:   %s\n", convertEpochToTimeString(now, t, 50));
								sb_send_string(g_tempStr);
								sprintf(g_tempStr, "Start:  %s\n", convertEpochToTimeString(g_event_start_epoch, t, 50));
								sb_send_string(g_tempStr);
								sprintf(g_tempStr, "Finish: %s\n", convertEpochToTimeString(g_event_finish_epoch, t, 50));
								sb_send_string(g_tempStr);

 								ConfigurationState_t cfg = clockConfigurationCheck();
 
 								if((cfg != WAITING_FOR_START) && (cfg != EVENT_IN_PROGRESS))
 								{
 									reportConfigErrors();
 								}
 								else
 								{
 									reportTimeTill(now, g_event_start_epoch, "Starts in: ", "In progress\n");
 									reportTimeTill(g_event_start_epoch, g_event_finish_epoch, "Lasts: ", NULL);
 									if(g_event_start_epoch < now)
 									{
 										reportTimeTill(now, g_event_finish_epoch, "Time Remaining: ", NULL);
 									}
 								}
							}
					}

					doprint = true;
				}
				else if(sb_buff->fields[SB_FIELD1][0] == 'S')  /* Event start time */
				{
					strcpy(g_tempStr, sb_buff->fields[SB_FIELD2]);
 					time_t s = validateTimeString(g_tempStr, (time_t*)&g_event_start_epoch, -g_utc_offset);
 
 					if(s)
 					{
 						g_event_start_epoch = s;
  						g_ee_mgr.updateEEPROMVar(Event_start_epoch, (void*)&g_event_start_epoch);
 						g_event_finish_epoch = MAX(g_event_finish_epoch, (g_event_start_epoch + SECONDS_24H));
  						g_ee_mgr.updateEEPROMVar(Event_finish_epoch, (void*)&g_event_finish_epoch);
 						setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);
 						if(g_event_start_epoch > time(null)) startEventUsingRTC();
 					}
 
					char t[50];
					sprintf(g_tempStr, "Start: %s\n", convertEpochToTimeString(g_event_start_epoch, t, 50));
  					doprint = true;
				}
				else if(sb_buff->fields[SB_FIELD1][0] == 'F')  /* Event finish time */
				{
 					strcpy(g_tempStr, sb_buff->fields[SB_FIELD2]);
 					time_t f = validateTimeString(g_tempStr, (time_t*)&g_event_finish_epoch, -g_utc_offset);
 
 					if(f)
 					{
 						g_event_finish_epoch = f;
  						g_ee_mgr.updateEEPROMVar(Event_finish_epoch, (void*)&g_event_finish_epoch);
 						setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);
 						if(g_event_start_epoch > now) startEventUsingRTC();
 					}
 
					char t[50];
					sprintf(g_tempStr, "Finish: %s\n", convertEpochToTimeString(g_event_finish_epoch, t, 50));
 					doprint = true;
				}
				else if(sb_buff->fields[SB_FIELD1][0] == '*')  /* Sync seconds to zero */
				{
 					ds3231_sync2nearestMinute();
				}

				if(doprint)
				{
					sb_send_string(g_tempStr);
				}
			}
			break;

			case SB_MESSAGE_UTIL:
			{
				if(sb_buff->fields[SB_FIELD1][0] == 'C')
				{
					if(sb_buff->fields[SB_FIELD2][0])
					{
						int16_t v = atoi(sb_buff->fields[SB_FIELD2]);

						if((v > -2000) && (v < 2000))
						{
// 							g_atmega_temp_calibration = v;
// 							ee_mgr.updateEEPROMVar(Atmega_temp_calibration, (void*)&g_atmega_temp_calibration);
						}
					}

// 					sprintf(g_tempStr, "T Cal= %d\n", g_atmega_temp_calibration);
// 					sb_send_string(g_tempStr);
				}

// 				sprintf(g_tempStr, "T=%dC\n", g_temperature);
// 				sb_send_string(g_tempStr);
// 
// 				sprintf(g_tempStr, "V=%d.%02dV\n", g_voltage / 100, g_voltage % 100);
// 				sb_send_string(g_tempStr);
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
	static uint8_t event_parameter_commands_rcvd_count = 0;
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
							LEDS.blink(LEDS_RED_ON_CONSTANT);
							txKeyDown(ON);
						}
						else if(c == ']')
						{
							txKeyDown(OFF);
							powerToTransmitter(OFF);
							LEDS.blink(LEDS_RED_OFF);
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
						/* Send WiFi the current time */
						sprintf(g_tempStr, "%lu", time(NULL));
						lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
						
						if(g_waiting_for_next_event)
						{
							lb_send_msg(LINKBUS_MSG_REPLY, (char *)LB_MESSAGE_ESP_LABEL, (char *)"1"); /* Request next scheduled event */
						}
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
					
					event_parameter_commands_rcvd_count++;
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
//								cli();
								g_event_enabled = false; // prevent interrupts from affecting the settings
								bool repeat = true;
								makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
								g_code_throttle = throttleValue(g_pattern_codespeed);
//								sei();
								g_event_start_epoch = 1;                     /* have it start a long time ago */
								g_event_finish_epoch = MAX_TIME;             /* run for a long long time */
								g_on_air_seconds = 9999;                    /* on period is very long */
								g_off_air_seconds = 0;                      /* off period is very short */
								g_on_the_air = 9999;                        /*  start out transmitting */
								g_sendID_seconds_countdown = MAX_UINT16;    /* wait a long time to send the ID */
								g_event_commenced = true;                   /* get things running immediately */
								g_event_enabled = true;                     /* get things running immediately */
								g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
							}
							else
							{
								g_last_error_code = ERROR_CODE_NO_ANTENNA_FOR_BAND;
							}
						}
						else if(f1 == '2')  /* enables a downloaded event stored in EEPROM */
						{
							/* This command configures the transmitter to launch an event at its scheduled start time */
							if(event_parameter_commands_rcvd_count && (event_parameter_commands_rcvd_count < NUMBER_OF_ESSENTIAL_EVENT_PARAMETERS))
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

								g_WiFi_shutdown_seconds = 60;
							
								new_event_parameter_count = 0;
								event_parameter_commands_rcvd_count = 0;
							}
						}
					}
				}
				else if(f1 == '0')  /* Stop continuous transmit (if enabled) and prepare to receive new event data */
				{
					suspendEvent();
					new_event_parameter_count = 0;
					event_parameter_commands_rcvd_count = 0;
					g_last_status_code = STATUS_CODE_RECEIVING_EVENT_DATA;
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
					
					event_parameter_commands_rcvd_count++;
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
						
						event_parameter_commands_rcvd_count++;
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
							g_time_needed_for_ID = (500 + timeRequiredToSendStrAtWPM(g_messages_text[STATION_ID], g_id_codespeed)) / 1000;				
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
					
					event_parameter_commands_rcvd_count++;
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
								g_time_needed_for_ID = (500 + timeRequiredToSendStrAtWPM(g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
							}
						}
						
						event_parameter_commands_rcvd_count++;
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
						
						event_parameter_commands_rcvd_count++;
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
						
						event_parameter_commands_rcvd_count++;
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
						
						event_parameter_commands_rcvd_count++;
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
						
						event_parameter_commands_rcvd_count++;
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
						
						event_parameter_commands_rcvd_count++;
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
					
					event_parameter_commands_rcvd_count++;
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
					
					event_parameter_commands_rcvd_count++;
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

		lb_buff->id = LB_MESSAGE_EMPTY;
		if(send_ack)
		{
			lb_send_text((char *)LB_MESSAGE_ACK);
		}
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
	bool runsFinite;

	dif = timeDif(g_event_finish_epoch, g_event_start_epoch);
	runsFinite = (dif > 0);

	now = time(null);
	dif = timeDif(now, g_event_finish_epoch);
	g_go_to_sleep_now = false;

	if((dif >= 0) && runsFinite) /* Event has already finished */
	{
		g_sleepType = SLEEP_FOREVER;
		g_seconds_to_sleep = MAX_TIME;
		g_wifi_enable_delay = 2;
		return( false); /* completed events are never enabled */
	}

	dif = timeDif(now, g_event_start_epoch);

	if(dif >= -15)  /* Don't sleep if the next transmission starts in 15 seconds or less */
	{
		g_sleepType = DO_NOT_SLEEP;
		g_seconds_to_sleep = 0;

		return( true);
	}

	/* If we reach here, we have an event that has not yet started but needs to be enabled, and a sleep time needs to be calculated
	 * while allowing time for power-up (coming out of sleep) prior to the event start */
	g_seconds_to_sleep = (-dif) - 5;   /* sleep until 5 seconds before the start time */
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

		g_time_needed_for_ID = (500 + timeRequiredToSendStrAtWPM(g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
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

			if(timeTillTransmit <= 0)                       /* we should have started transmitting already */
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
	PORTC_set_pin_pull_mode(3, PORT_PULL_UP);
	PORTC_set_pin_pull_mode(2, PORT_PULL_UP);
	
	ds3231_init();
		
	if(ds3231_1s_sqw(ON))
	{
		return ERROR_CODE_RTC_NONRESPONSIVE;
	}
	
	return ERROR_CODE_NO_ERROR;
}

bool antennaIsConnected()
{
	bool val = PORTD_get_pin_level(X80M_ANTENNA_DETECT);
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
			setupForFox(NULL, START_NOTHING);
		}
		else
		{
			setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);
		}
	}
	else if(activationSource == PROGRAMMATIC)
	{
		if(conf == CONFIGURATION_ERROR)                                                                                             /* Start immediately */
		{
			setupForFox(NULL, START_EVENT_NOW);
		}
		else if((conf == WAITING_FOR_START) || (conf == SCHEDULED_EVENT_WILL_NEVER_RUN) || (conf == SCHEDULED_EVENT_DID_NOT_START)) /* Start immediately */
		{
			setupForFox(NULL, START_EVENT_NOW);
		}
		else                                                                                                                        /*if((conf == EVENT_IN_PROGRESS) */
		{
			setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);                                                                  /* Let the RTC start the event */
		}
	}
	else                                                                                                                            /* PUSHBUTTON */
	{
		if(conf == CONFIGURATION_ERROR)                                                                                             /* No scheduled event */
		{
			setupForFox(NULL, START_EVENT_NOW);
		}
		else                                                                                                                        /* if(buttonActivated) */
		{
			if(conf == WAITING_FOR_START)
			{
				setupForFox(NULL, START_TRANSMISSIONS_NOW);                                                                         /* Start transmitting! */
			}
			else if(conf == SCHEDULED_EVENT_WILL_NEVER_RUN)
			{
				setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);                                                              /* rtc starts the event */
			}
			else                                                                                                                    /* Event should be running now */
			{
				setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);                                                              /* start the running event */
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
		setupForFox(NULL, START_NOTHING);
	}
	else    /* if(activationSource == PUSHBUTTON) */
	{
		if(conf == WAITING_FOR_START)
		{
			setupForFox(NULL, START_TRANSMISSIONS_NOW);
		}
		if(conf == SCHEDULED_EVENT_WILL_NEVER_RUN)
		{
			setupForFox(NULL, START_NOTHING);
		}
		else    /*if(conf == CONFIGURATION_ERROR) */
		{
			setupForFox(NULL, START_NOTHING);
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
		setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);
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



void setupForFox(Fox_t* fox, EventAction_t action)
{
	bool patternNotSet = true;
	
	if(fox)
	{
		if(*fox != INVALID_FOX)
		{
			g_fox = *fox;
		}
	}

	g_event_enabled = false;
	g_event_commenced = false;
	LEDS.setRed(OFF);
 	set_system_time(ds3231_get_epoch(null));

	switch(g_fox)
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
			
			bool repeat = true;
			makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
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
			
			bool repeat = true;
			makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
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
			
			bool repeat = true;
			makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
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
			bool repeat = true;
			makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
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
		bool repeat = true;
		makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
		g_code_throttle = throttleValue(g_pattern_codespeed);

// 		g_event_start_epoch = 1;                     /* have it start a long time ago */
// 		g_event_finish_epoch = MAX_TIME;             /* run for a long long time */
		g_on_the_air = g_on_air_seconds;			/* start out transmitting */
		g_event_commenced = true;                   /* get things running immediately */
		g_event_enabled = true;                     /* get things running immediately */
		g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
		
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

time_t validateTimeString(char* str, time_t* epochVar, int8_t offsetHours)
{
	time_t valid = 0;
	int len = strlen(str);
	time_t minimumEpoch = MINIMUM_EPOCH;
	uint8_t validationType = 0;
	time_t now = time(null);

	if(epochVar == &g_event_start_epoch)
	{
		minimumEpoch = MAX(now, MINIMUM_EPOCH);
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

		ep += (HOUR * offsetHours);

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
	
	if((g_event_finish_epoch < MINIMUM_EPOCH) || (g_event_start_epoch < MINIMUM_EPOCH) || (now < MINIMUM_EPOCH))
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

	if(now < MINIMUM_EPOCH) /* Current time is invalid */
	{
		sb_send_string(TEXT_SET_TIME_TXT);
	}

	if(g_event_finish_epoch < now)      /* Event has already finished */
	{
		if(g_event_start_epoch < now)   /* Event has already started */
		{
			sb_send_string(TEXT_SET_START_TXT);
		}

		sb_send_string(TEXT_SET_FINISH_TXT);
	}
	else if(g_event_start_epoch < now)  /* Event has already started */
	{
		if(g_event_start_epoch < MINIMUM_EPOCH)     /* Start in invalid */
		{
			sb_send_string(TEXT_SET_START_TXT);
		}
		else
		{
			sb_send_string((char*)"Event running...\n");
		}
	}
}




