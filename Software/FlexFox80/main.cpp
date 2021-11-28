#include "atmel_start.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
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


/* ADC Defines */
#define BATTERY_READING 0
#define PA_VOLTAGE_READING 1
#define V12V_VOLTAGE_READING 2
#define BAND_80M_ANTENNA 3
#define BAND_2M_ANTENNA 4

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


/***********************************************************************
 * Global Variables & String Constants
 *
 * Identify each global with a "g_" prefix
 * Whenever possible limit globals' scope to this file using "static"
 * Use "volatile" for globals shared between ISRs and foreground
 ************************************************************************/
static char g_tempStr[21] = { '\0' };
static volatile EC g_last_error_code = ERROR_CODE_NO_ERROR;
static volatile SC g_last_status_code = STATUS_CODE_IDLE;

static volatile BOOL g_powering_off = FALSE;

static volatile uint16_t g_util_tick_countdown = 0;
static volatile BOOL g_battery_measurements_active = FALSE;
static volatile uint16_t g_maximum_battery = 0;
volatile BatteryType g_battery_type = BATTERY_UNKNOWN;

static volatile BOOL g_antenna_connection_changed = TRUE;
volatile AntConnType g_antenna_connect_state = ANT_CONNECTION_UNDETERMINED;

static volatile int32_t g_on_the_air = 0;
static volatile int g_sendID_seconds_countdown = 0;
static volatile uint16_t g_code_throttle = 50;
static volatile uint8_t g_WiFi_shutdown_seconds = 120;
static volatile BOOL g_report_seconds = FALSE;
static volatile BOOL g_wifi_active = TRUE;
static volatile uint8_t g_wifi_enable_delay = 0;
static volatile BOOL g_shutting_down_wifi = FALSE;
static volatile SleepType g_sleepType = NOT_SLEEPING;

char g_messages_text[2][MAX_PATTERN_TEXT_LENGTH + 1] = { "\0", "\0" };
volatile uint8_t g_id_codespeed = EEPROM_ID_CODE_SPEED_DEFAULT;
volatile uint8_t g_pattern_codespeed = EEPROM_PATTERN_CODE_SPEED_DEFAULT;
volatile uint16_t g_time_needed_for_ID = 0;
volatile int16_t g_on_air_seconds = EEPROM_ON_AIR_TIME_DEFAULT;                      /* amount of time to spend on the air */
volatile int16_t g_off_air_seconds = EEPROM_OFF_AIR_TIME_DEFAULT;                    /* amount of time to wait before returning to the air */
volatile int16_t g_intra_cycle_delay_time = EEPROM_INTRA_CYCLE_DELAY_TIME_DEFAULT;   /* offset time into a repeating transmit cycle */
volatile int16_t g_ID_period_seconds = EEPROM_ID_TIME_INTERVAL_DEFAULT;              /* amount of time between ID/callsign transmissions */
volatile time_t g_event_start_time = EEPROM_START_TIME_DEFAULT;
volatile time_t g_event_finish_time = EEPROM_FINISH_TIME_DEFAULT;
volatile BOOL g_event_enabled = EEPROM_EVENT_ENABLED_DEFAULT;                        /* indicates that the conditions for executing the event are set */
volatile BOOL g_event_commenced = FALSE;
volatile BOOL g_check_for_next_event = FALSE;
volatile BOOL g_waiting_for_next_event = FALSE;
volatile int g_update_timeout_seconds = 90;
volatile uint16_t g_battery_empty_mV = EEPROM_BATTERY_EMPTY_MV;

extern volatile uint16_t g_i2c0_timeout_ticks;
extern volatile uint16_t g_i2c1_timeout_ticks;
static volatile BOOL g_sufficient_power_detected = FALSE;
static volatile BOOL g_enableHardwareWDResets = FALSE;
extern volatile BOOL g_tx_power_is_zero;

static volatile BOOL g_go_to_sleep = FALSE;
static volatile BOOL g_sleeping = FALSE;
static volatile time_t g_seconds_left_to_sleep = 0;
static volatile time_t g_seconds_to_sleep = MAX_TIME;

#define NUMBER_OF_POLLED_ADC_CHANNELS 5
static const uint16_t g_adcChannelConversionPeriod_ticks[NUMBER_OF_POLLED_ADC_CHANNELS] = { TIMER2_0_5HZ, TIMER2_0_5HZ, TIMER2_0_5HZ, TIMER2_5_8HZ, TIMER2_5_8HZ };
static volatile uint16_t g_tickCountdownADCFlag[NUMBER_OF_POLLED_ADC_CHANNELS] = { TIMER2_0_5HZ, TIMER2_0_5HZ, TIMER2_0_5HZ, TIMER2_5_8HZ, TIMER2_5_8HZ };
//static uint16_t g_filterADCValue[NUMBER_OF_POLLED_ADC_CHANNELS] = { 500, 500, 500, 500, 500 };
//static volatile BOOL g_adcUpdated[NUMBER_OF_POLLED_ADC_CHANNELS] = { FALSE, FALSE, FALSE, FALSE, FALSE };
static volatile uint16_t g_lastConversionResult[NUMBER_OF_POLLED_ADC_CHANNELS];

extern Goertzel g_goertzel;

EepromManager g_ee_mgr;

Fox_t g_fox = FOX_1;
time_t g_event_start_epoch;
time_t g_event_finish_epoch;
int8_t g_utc_offset;
uint8_t g_unlockCode[8];

/***********************************************************************
 * Private Function Prototypes
 *
 * These functions are available only within this file
 ************************************************************************/
BOOL eventEnabled(void);
void handleLinkBusMsgs(void);
void handleSerialBusMsgs(void);
void wdt_init(WDReset resetType);
uint16_t throttleValue(uint8_t speed);
EC activateEventUsingCurrentSettings(SC* statusCode);
EC launchEvent(SC* statusCode);
EC hw_init(void);
EC rtc_init(void);
void set_ports(SleepType initType);
BOOL antennaIsConnected(void);
void initializeAllEventSettings(BOOL disableEvent);
void suspendEvent(void);

void setupForFox(Fox_t* fox, EventAction_t action);

/**
One-second counter based on CPU clock.
*/
ISR(TCB3_INT_vect)
{
 	static uint16_t cnt = 0;
	
    /* Insert your TCB interrupt handling code */
    /**
     * The interrupt flag is cleared by writing 1 to it, or when the Capture register
     * is read in Capture mode
     */

    if(TCB3.INTFLAGS & TCB_CAPT_bm)
    {
		if(cnt++ == 299) /* check for 1-second interval */
		{
			cnt = 0;
			if(!g_event_enabled) LED_toggle_level();
					
			static BOOL lastAntennaConnectionState = FALSE;
			static uint8_t antennaReadCount = 3;
			BOOL ant = antennaIsConnected();

			if(!ant)    /* immediately detect disconnection */
			{
				if(g_antenna_connect_state != ANT_ALL_DISCONNECTED)
				{
					g_antenna_connect_state = ANT_ALL_DISCONNECTED;
					g_antenna_connection_changed = TRUE;
				}
			}
			else if(g_antenna_connect_state == ANT_ALL_DISCONNECTED)
			{
				if(ant == lastAntennaConnectionState)
				{
					if(antennaReadCount)
					{
						antennaReadCount--;

						if(!antennaReadCount)
						{
							g_antenna_connect_state = ANT_CONNECTION_UNDETERMINED;
							g_antenna_connection_changed = TRUE;
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

			system_tick();

			if(g_sleeping)
			{
				if(g_seconds_left_to_sleep)
				{
					g_seconds_left_to_sleep--;
				}

				if(!g_seconds_left_to_sleep || g_antenna_connection_changed)
				{
					g_go_to_sleep = FALSE;
					g_sleeping = FALSE;
				}
			}
			else
			{
				time_t temp_time = 0;

				if(g_update_timeout_seconds)
				{
					g_update_timeout_seconds--;
				}

				if(g_event_commenced)
				{
					if(g_event_finish_time && !g_check_for_next_event && !g_shutting_down_wifi)
					{
						time(&temp_time);

						if(temp_time >= g_event_finish_time)
						{
							g_last_status_code = STATUS_CODE_EVENT_FINISHED;
							g_on_the_air = 0;
							keyTransmitter(OFF);
							g_event_enabled = FALSE;
							g_event_commenced = FALSE;
							g_check_for_next_event = TRUE;
							g_update_timeout_seconds = 90;
							if(g_wifi_active)
							{
								g_WiFi_shutdown_seconds = 60;
							}
						}
					}
				}

				if(g_event_enabled)
				{
					if(g_event_commenced)
					{
						BOOL repeat;

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
										repeat = FALSE;
										makeMorse(g_messages_text[STATION_ID], &repeat, NULL);  /* Send only once */
									}
								}


								if(!g_on_the_air)
								{
									if(g_off_air_seconds)
									{
										keyTransmitter(OFF);
										g_on_the_air -= g_off_air_seconds;
										repeat = TRUE;
										makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);    /* Reset pattern to start */
										g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;


										/* Enable sleep during off-the-air periods */
										int32_t timeRemaining = 0;
										time(&temp_time);
										if(temp_time < g_event_finish_time)
										{
											timeRemaining = timeDif(g_event_finish_time, temp_time);
										}

										/* Don't sleep for the last cycle to ensure that the event doesn't end while
										 * the transmitter is sleeping - which can cause problems with loading the next event */
										if(timeRemaining > (g_off_air_seconds + g_on_air_seconds + 15))
										{
											if((g_off_air_seconds > 15) && !g_WiFi_shutdown_seconds)
											{
												g_seconds_to_sleep = (time_t)(g_off_air_seconds - 10);
												g_sleepType = SLEEP_UNTIL_NEXT_XMSN;
												g_go_to_sleep = TRUE;
												g_sendID_seconds_countdown = MAX(0, g_sendID_seconds_countdown - (int)g_seconds_to_sleep);
											}
										}
									}
									else
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
									BOOL repeat = TRUE;
									makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
								}
							}
						}
					}
					else if(g_event_start_time > 0) /* off the air - waiting for the start time to arrive */
					{
						time(&temp_time);

						if(temp_time >= g_event_start_time)
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
								BOOL repeat = TRUE;
								makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
							}

							g_event_commenced = TRUE;
						}
					}
				}


				/**************************************
				 * Delay before re-enabling linkbus receive
				 ***************************************/
				if(g_wifi_enable_delay)
				{
					g_wifi_enable_delay--;

					if(g_wifi_enable_delay == (LINKBUS_POWERUP_DELAY_SECONDS - 1))
					{
						wifi_power(ON);     /* power on WiFi */
						wifi_reset(OFF);    /* bring WiFi out of reset */
					}
					else if(!g_wifi_enable_delay)
					{
						linkbus_init(LB_BAUD, USART_DO_NOT_CHANGE); /* For development board */
					}
				}
				else
				{
					if(!g_update_timeout_seconds || g_shutting_down_wifi || (!g_check_for_next_event && !g_waiting_for_next_event))
					{
						if(g_WiFi_shutdown_seconds)
						{
							g_WiFi_shutdown_seconds--;

							if(!g_WiFi_shutdown_seconds)
							{
								wifi_reset(ON);     /* put WiFi into reset */
								wifi_power(OFF);    /* power off WiFi */
								g_shutting_down_wifi = FALSE;

								/* If an event hasn't been enabled by the time that WiFi shuts
								 *  down, then the transmitter will never run. Just sleep indefinitely
								 */
								if(!g_event_enabled)
								{
									g_sleepType = SLEEP_FOREVER;
									g_go_to_sleep = TRUE;
									g_seconds_to_sleep = MAX_TIME;
								}
								else if(g_sleepType == SLEEP_AFTER_WIFI_GOES_OFF)
								{
									eventEnabled(); /* Sets sleep time appropriately */
								}

								g_wifi_active = FALSE;
							}
						}
					}

					if(g_wifi_active)
					{
						g_report_seconds = TRUE;
					}
				}
			}

		}

        TCB3.INTFLAGS = TCB_CAPT_bm; /* clear interrupt flag */
    }
}



/**
Periodic tasks not requiring precise timing. Rate = 300 Hz
*/
ISR(TCB0_INT_vect)
{
    if(TCB0.INTFLAGS & TCB_CAPT_bm)
    {
//		static BOOL conversionInProcess = FALSE;
//		static int8_t indexConversionInProcess;
		static uint16_t codeInc = 0;
		BOOL repeat, finished;
		
		if(g_i2c0_timeout_ticks)
		{
			g_i2c0_timeout_ticks--;
		}

		if(g_i2c1_timeout_ticks)
		{
			g_i2c1_timeout_ticks--;
		}

		if(g_util_tick_countdown)
		{
			g_util_tick_countdown--;
		}

		static BOOL key = FALSE;

		if(g_event_enabled && g_event_commenced)
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
							repeat = TRUE;
							makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
							key = makeMorse(NULL, &repeat, &finished);
						}

						if(key)
						{
							powerToTransmitter(ON);
							LED_set_level(ON);
						}
					}
				}
				else
				{
					keyTransmitter(key);
					LED_set_level(key);
					codeInc = g_code_throttle;
				}
			}
			else if(!g_on_the_air)
			{
				if(key)
				{
					key = OFF;
					keyTransmitter(OFF);
					LED_set_level(OFF);
					powerToTransmitter(OFF);
					g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
				}
			}
		}

		/**
		 * Handle Periodic ADC Readings
		 * The following algorithm allows multiple ADC channel readings to be performed at different polling intervals. */
// 		if(!conversionInProcess)
// 		{
// 			/* Note: countdowns will pause while a conversion is in process. Conversions are so fast that this should not be an issue though. */
// 
// 			volatile uint8_t i; /* volatile to prevent optimization performing undefined behavior */
// 			indexConversionInProcess = -1;
// 
// 			for(i = 0; i < NUMBER_OF_POLLED_ADC_CHANNELS; i++)
// 			{
// 				if(g_tickCountdownADCFlag[i])
// 				{
// 					g_tickCountdownADCFlag[i]--;
// 				}
// 
// 				if(g_tickCountdownADCFlag[i] == 0)
// 				{
// 					indexConversionInProcess = (int8_t)i;
// 				}
// 			}
// 
// 			if(indexConversionInProcess >= 0)
// 			{
// 				g_tickCountdownADCFlag[indexConversionInProcess] = g_adcChannelConversionPeriod_ticks[indexConversionInProcess];    /* reset the tick countdown */
// //				ADMUX = (ADMUX & 0xF0) | activeADC[indexConversionInProcess];                                                       /* index through all active channels */
// //				ADCSRA |= (1 << ADSC);                                                                                              /*single conversion mode */
// 				conversionInProcess = TRUE;
// 			}
// 		}
// 		else if(1 /*!( ADCSRA & (1 << ADSC) ) */)   /* wait for conversion to complete */
// 		{
// 			uint16_t hold = 0; //ADC;
// 			static uint16_t holdConversionResult;
// 			holdConversionResult = (uint16_t)(((uint32_t)hold * ADC_REF_VOLTAGE_mV) >> 10);                                         /* millivolts at ADC pin */
// 			uint16_t lastResult = g_lastConversionResult[indexConversionInProcess];
// 
// 			g_adcUpdated[indexConversionInProcess] = TRUE;
// 
// 			if(indexConversionInProcess == BATTERY_READING)
// 			{
// 				BOOL directionUP = holdConversionResult > lastResult;
// 				uint16_t delta = directionUP ? holdConversionResult - lastResult : lastResult - holdConversionResult;
// 
// 				if(delta > g_filterADCValue[indexConversionInProcess])
// 				{
// 					lastResult = holdConversionResult;
// 					g_tickCountdownADCFlag[indexConversionInProcess] = 100; /* speed up next conversion */
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
// 					g_battery_measurements_active = TRUE;
// 
// 					if(lastResult > VOLTS_5)
// 					{
// 						g_battery_type = BATTERY_9V;
// 					}
// 					else if(lastResult > VOLTS_3_0)
// 					{
// 						g_battery_type = BATTERY_4r2V;
// 					}
// 				}
// 			}
// 			else if(indexConversionInProcess == V12V_VOLTAGE_READING)
// 			{
// 				lastResult = holdConversionResult;
// 			}
// 	/*		else if(indexConversionInProcess == PA_VOLTAGE_READING)
// 	 *		{
// 	 *			lastResult = holdConversionResult;
// 	 *			g_PA_voltage = holdConversionResult;
// 	 *		} */
// 			else
// 			{
// 				lastResult = holdConversionResult;
// 			}
// 
// 			g_lastConversionResult[indexConversionInProcess] = lastResult;
// 
// 			conversionInProcess = FALSE;
// 		}

        TCB0.INTFLAGS = TCB_CAPT_bm; /* clear interrupt flag */
    }
}


int main(void)
{
// 	static uint16_t count = 0;
// 	static int16_t tempC = 0;

	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	LED_set_level(OFF);
	
	g_ee_mgr.initializeEEPROMVars();
	g_ee_mgr.readNonVols();
	
//	while(util_delay_ms(100)); // Allow USARTs to stabilize
// 	lb_send_text((char*)"Linkbus\n");
	sb_send_NewLine();
	sb_send_string((char *)PRODUCT_NAME_LONG);
	sprintf(g_tempStr, "\nSW Ver: %s\n", SW_REVISION);
	sb_send_string(g_tempStr);
	sb_send_string((char *)HELP_TEXT_TXT);
	sb_send_NewPrompt();
					
	ADC0_setADCChannel(ADCAudioInput);

	while (1) {
		while(util_delay_ms(250))
		{
// 			if(g_goertzel.SamplesReady())
// 			{
// 				ADC0.INTCTRL = 0x01; /* enable ADC interrupt */
// 			}

			handleLinkBusMsgs();
			handleSerialBusMsgs();
		}
		
		
//		tempC = temperatureC();
//		sprintf(g_tempStr, "Seconds: %d: %d\n", count++, tempC);
//		lb_send_text(g_tempStr);
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
// 						ee_mgr.updateEEPROMVar(Fox_setting, (void*)&holdFox);
 						if(holdFox != g_fox)
 						{
 							setupForFox(&holdFox, START_NOTHING);
 						}
					}
				}

				sprintf(g_tempStr, "Fox=%u\n", (uint16_t)g_fox);
				sb_send_string(g_tempStr);
			}
			break;

			case SB_MESSAGE_SYNC:
			{
				if(sb_buff->fields[SB_FIELD1][0])
				{
					if(sb_buff->fields[SB_FIELD1][0] == '0')       /* Stop the event. Re-sync will occur on next start */
					{
// 						stopEventNow(PROGRAMMATIC);
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '1')  /* Start the event, re-syncing to a start time of now - same as a button press */
					{
// 						startEventNow(PROGRAMMATIC);
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '2')  /* Start the event at the programmed start time */
					{
// 						g_transmissions_disabled = TRUE;        /* Disable an event currently underway */
// 						startEventUsingRTC();
					}
					else if(sb_buff->fields[SB_FIELD1][0] == '3')  /* Start the event at the programmed start time */
					{
// 						setupForFox(NULL, START_TRANSMISSIONS_NOW);
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
// 						uint8_t speed = atol(sb_buff->fields[SB_FIELD2]);
// 						g_id_codespeed = CLAMP(MIN_CODE_SPEED_WPM, speed, MAX_CODE_SPEED_WPM);
//						ee_mgr.updateEEPROMVar(Id_codespeed, (void*)&g_id_codespeed);

						if(g_messages_text[STATION_ID][0])
						{
							g_time_needed_for_ID = (600 + timeRequiredToSendStrAtWPM((char*)g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
						}
					}

					sprintf(g_tempStr, "ID: %d wpm\n", g_id_codespeed);
				}
				else if(sb_buff->fields[SB_FIELD1][0] == 'P')
				{
					char x = sb_buff->fields[SB_FIELD2][0];

					if(x)
					{
// 						g_ptt_periodic_reset_enabled = ((x == '1') || (x == 'T') || (x == 'Y'));
//						ee_mgr.updateEEPROMVar(Ptt_periodic_reset, (void*)&g_ptt_periodic_reset_enabled);
// 						g_use_ptt_periodic_reset = g_ptt_periodic_reset_enabled;
					}

					sprintf(g_tempStr, "DRP:%d\n", 99);
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
// 					strncpy(g_tempStr, sb_buff->fields[SB_FIELD1], MAX_UNLOCK_CODE_LENGTH);
// 					g_tempStr[MAX_UNLOCK_CODE_LENGTH] = '\0';   /* truncate to no more than max characters */

// 					if(only_digits(g_tempStr) && (strlen(g_tempStr) >= MIN_UNLOCK_CODE_LENGTH))
// 					{
// 						strcpy((char*)g_unlockCode, g_tempStr);
//						ee_mgr.updateEEPROMVar(UnlockCode, (void*)g_unlockCode);
// 					}
				}

				sprintf(g_tempStr, "PWD=%s\n", (char*)"code");
				sb_send_string(g_tempStr);
			}
			break;

			case SB_MESSAGE_CLOCK:
			{
				BOOL doprint = false;

				if(sb_buff->fields[SB_FIELD1][0] == 'T')   /* Current time format "YYMMDDhhmmss" */
				{
					strncpy(g_tempStr, sb_buff->fields[SB_FIELD2], 12);
					g_tempStr[12] = '\0';               /* truncate to no more than 12 characters */

// 					time_t t = validateTimeString(g_tempStr, (time_t*)&g_current_epoch, -g_utc_offset);
// 
// 					if(t)
// 					{
// 						#if INCLUDE_RV3028_SUPPORT
// 						ds3231_set_epoch(t);
// 						#else
// 						ds3231_set_date_time(g_tempStr, RTC_CLOCK);
// 						#endif
// 
// 						g_current_epoch = t;
// 						sprintf(g_tempStr, "Time:%lu\n", g_current_epoch);
// 						setupForFox(NULL, START_NOTHING);   /* Avoid timing problems if an event is already active */
// 					}
// 					else
// 					{
// 						g_current_epoch = ds3231_get_epoch();
// 						reportTimeTill(g_current_epoch, g_event_start_epoch, "Starts in: ", NULL);
// 						sprintf(g_tempStr, "Epoch:%lu\n", g_current_epoch);
// 					}

					doprint = true;
				}
				else if(sb_buff->fields[SB_FIELD1][0] == 'S')  /* Event start time */
				{
					strcpy(g_tempStr, sb_buff->fields[SB_FIELD2]);
// 					time_t s = validateTimeString(g_tempStr, (time_t*)&g_event_start_epoch, -g_utc_offset);
// 
// 					if(s)
// 					{
// 						g_event_start_epoch = s;
//  					ee_mgr.updateEEPROMVar(Event_start_epoch, (void*)&g_event_start_epoch);
// 						g_event_finish_epoch = MAX(g_event_finish_epoch, (g_event_start_epoch + SECONDS_24H));
//  					ee_mgr.updateEEPROMVar(Event_finish_epoch, (void*)&g_event_finish_epoch);
// 						setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);
// 						if(g_event_start_epoch > g_current_epoch) startEventUsingRTC();
// 					}
// 
// 					sprintf(g_tempStr, "Start:%lu\n", g_event_start_epoch);
// 					doprint = true;
				}
				else if(sb_buff->fields[SB_FIELD1][0] == 'F')  /* Event finish time */
				{
// 					strcpy(g_tempStr, sb_buff->fields[SB_FIELD2]);
// 					time_t f = validateTimeString(g_tempStr, (time_t*)&g_event_finish_epoch, -g_utc_offset);
// 
// 					if(f)
// 					{
// 						g_event_finish_epoch = f;
//  						ee_mgr.updateEEPROMVar(Event_finish_epoch, (void*)&g_event_finish_epoch);
// 						setupForFox(NULL, START_EVENT_WITH_STARTFINISH_TIMES);
// 						if(g_event_start_epoch > g_current_epoch) startEventUsingRTC();
// 					}
// 
// 					sprintf(g_tempStr, "Finish:%lu\n", g_event_finish_epoch);
// 					doprint = true;
				}
				else if(sb_buff->fields[SB_FIELD1][0] == '*')  /* Sync seconds to zero */
				{
// 					ds3231_sync2nearestMinute();
				}
				else
				{
// 					ConfigurationState_t cfg = clockConfigurationCheck();
// 
// 					if((cfg != WAITING_FOR_START) && (cfg != EVENT_IN_PROGRESS))
// 					{
// 						reportConfigErrors();
// 					}
// 					else
// 					{
// 						reportTimeTill(g_current_epoch, g_event_start_epoch, "Starts in: ", "In progress\n");
// 						reportTimeTill(g_event_start_epoch, g_event_finish_epoch, "Lasts: ", NULL);
// 						if(g_event_start_epoch < g_current_epoch)
// 						{
// 							reportTimeTill(g_current_epoch, g_event_finish_epoch, "Time Remaining: ", NULL);
// 						}
// 					}
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
				sb_send_string((char *)HELP_TEXT_TXT);
// 				ee_mgr.send_Help();
			}
			break;
		}

		sb_buff->id = SB_MESSAGE_EMPTY;
		sb_send_NewPrompt();

// 		g_LED_timeout_countdown = LED_TIMEOUT_SECONDS;
// 		g_config_error = NULL_CONFIG;   /* Trigger a new configuration enunciation */
	}}

/* The compiler does not seem to optimize large switch statements correctly */
void __attribute__((optimize("O0"))) handleLinkBusMsgs()
{
	LinkbusRxBuffer* lb_buff;
	static uint8_t event_parameter_count = 0;
	BOOL send_ack = TRUE;

	while((lb_buff = nextFullLBRxBuffer()))
	{
		LBMessageID msg_id = lb_buff->id;

		switch(msg_id)
		{
			case LB_MESSAGE_WIFI:
			{
				BOOL result;

				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					result = atoi(lb_buff->fields[LB_MSG_FIELD1]);

					suspendEvent();
					linkbus_disable();
					g_WiFi_shutdown_seconds = 0;    /* disable sleep */

					if(result == 0)                 /* shut off power to WiFi */
					{
//						PORTD &= ~((1 << PORTD6) | (1 << PORTD7));
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

// 			case MESSAGE_OSC:
// 			{
// 				if(lb_buff->fields[FIELD1][0])
// 				{
// 					uint8_t result = 0;
// 					static uint8_t lastVal = 0;
// 					static uint8_t valCount = 0;
// 					static uint8_t bestResultCount = 0;
// 					int val = (uint16_t)atoi(lb_buff->fields[FIELD1]);
// 
// 					if(!val)
// 					{
// 						g_calibrate_baud = FALSE;
// 					}
// 					else if(val == 255)
// 					{
// 						eeprom_update_byte(&ee_clock_OSCCAL, 0xFF); /* erase any existing value */
// 					}
// 					else
// 					{
// 						if(abs(val - lastVal) > 10)                 /* Gap identified */
// 						{
// 							calcOSCCAL(255);
// 							valCount = 0;
// 						}
// 
// 						lastVal = val;
// 						valCount++;
// 						result = calcOSCCAL(val);
// 
// 						if(valCount > bestResultCount)
// 						{
// 							g_best_OSCCAL = result;
// 							bestResultCount = valCount;
// 						}
// 					}
// 
// 					send_ack = FALSE;
// 				}
// 			}
// 			break;

			case LB_MESSAGE_ESP_COMM:
			{
				char f1 = lb_buff->fields[LB_MSG_FIELD1][0];

				g_wifi_active = TRUE;

				if(f1 == 'Z')                                                       /* WiFi connected to browser - keep alive */
				{
					/* shut down WiFi after 2 minutes of inactivity */
					g_WiFi_shutdown_seconds = 120;                                  /* wait 2 more minutes before shutting down WiFi */
				}
				else
				{
					if(f1 == '0')                                                   /* ESP says "I'm awake" */
					{
						if(g_waiting_for_next_event)
						{
//							calibrateOscillator(0);                                 /* Abort baud calibration */
							lb_send_msg(LINKBUS_MSG_REPLY, (char *)LB_MESSAGE_ESP_LABEL, (char *)"1"); /* Request next scheduled event */
						}
						/* Send WiFi the current time */
						sprintf(g_tempStr, "%lu", time(NULL));
						lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_CLOCK_LABEL, g_tempStr);
					}
					else if(f1 == '3')                      /* ESP is ready for power off" */
					{
						cli();
						g_wifi_enable_delay = 0;
						g_WiFi_shutdown_seconds = 1;        /* Shut down WiFi in 1 seconds */
						g_waiting_for_next_event = FALSE;   /* Prevents resetting shutdown settings */
						g_check_for_next_event = FALSE;     /* Prevents resetting shutdown settings */
						g_wifi_active = FALSE;
						g_shutting_down_wifi = TRUE;
						sei();
					}
				}
			}
			break;

// 			case MESSAGE_TX_MOD:
// 			{
// 				if(lb_buff->fields[FIELD1][0] == 'A')   /* AM */
// 				{
// 					Modulation setModulation = MODE_AM;
// 					txSetParameters(NULL, NULL, &setModulation, NULL);
// 					event_parameter_count++;
// 				}
// 				else if(lb_buff->fields[FIELD1][0] == 'C')  /* CW */
// 				{
// 					Modulation setModulation = MODE_CW;
// 					txSetParameters(NULL, NULL, &setModulation, NULL);
// 					event_parameter_count++;
// 				}
// 				else if(lb_buff->fields[FIELD1][0] == 'F')  /* FM */
// 				{
// 					Modulation setModulation = MODE_FM;
// 					txSetParameters(NULL, NULL, &setModulation, NULL);
// 					event_parameter_count++;
// 				}
// 			}
// 			break;

			case LB_MESSAGE_TX_POWER:
			{
				static uint16_t pwr_mW;

				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					EC ec;

					if((lb_buff->fields[LB_MSG_FIELD1][0] == 'M') && (lb_buff->fields[LB_MSG_FIELD2][0]))
					{
						pwr_mW = (uint16_t)atoi(lb_buff->fields[LB_MSG_FIELD2]);
						event_parameter_count++;
					}
					else
					{
						pwr_mW = (uint16_t)atoi(lb_buff->fields[LB_MSG_FIELD1]);
					}

					ec = txSetParameters(&pwr_mW, NULL);
					if(ec)
					{
						g_last_error_code = ec;
					}

					sprintf(g_tempStr, "M,%u", pwr_mW);
					lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_TX_POWER_LABEL, g_tempStr);
				}
			}
			break;

			case LB_MESSAGE_PERM:
			{
				storeTransmitterValues();
//				saveAllEEPROM();
			}
			break;

			case LB_MESSAGE_GO:
			{
				char f1 = lb_buff->fields[LB_MSG_FIELD1][0];

				if((f1 == '1') || (f1 == '2'))
				{
					if(!txIsAntennaForBand() && !g_tx_power_is_zero)
					{
						g_last_error_code = ERROR_CODE_NO_ANTENNA_FOR_BAND;
					}
					else
					{
						if(f1 == '1')   /* Xmit immediately using current settings */
						{
							if(txIsAntennaForBand() || g_tx_power_is_zero)
							{
								/* Set the Morse code pattern and speed */
								cli();
								BOOL repeat = TRUE;
								makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
								g_code_throttle = throttleValue(g_pattern_codespeed);
								sei();
								g_event_start_time = 1;                     /* have it start a long time ago */
								g_event_finish_time = MAX_TIME;             /* run for a long long time */
								g_on_air_seconds = 9999;                    /* on period is very long */
								g_off_air_seconds = 0;                      /* off period is very short */
								g_on_the_air = 9999;                        /*  start out transmitting */
								g_sendID_seconds_countdown = MAX_UINT16;    /* wait a long time to send the ID */
								g_event_commenced = TRUE;                   /* get things running immediately */
								g_event_enabled = TRUE;                     /* get things running immediately */
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
							if(event_parameter_count < NUMBER_OF_ESSENTIAL_EVENT_PARAMETERS)
							{
								g_last_error_code = ERROR_CODE_EVENT_NOT_CONFIGURED;
							}
							else
							{
								SC status = STATUS_CODE_IDLE;
								static EC ec;
								ec = launchEvent(&status);
								if(g_go_to_sleep && g_sleepType)
								{
									g_sleepType = SLEEP_AFTER_WIFI_GOES_OFF;
									g_go_to_sleep = FALSE;
								}

								g_WiFi_shutdown_seconds = 60;

								if(!ec)
								{
//									saveAllEEPROM();    /* Make sure all  event values get saved */
									storeTransmitterValues();
								}
							}
						}
					}
				}
				else if(f1 == '0')  /* Stop continuous transmit (if enabled) and prepare to receive new event data */
				{
					suspendEvent();
					/* Restore saved event settings */
					event_parameter_count = 0;
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

					if(mtime)
					{
						g_event_start_time = mtime;
						cli();
						set_system_time(ds3231_get_epoch(NULL));    /* update system clock */
						sei();
						event_parameter_count++;
					}
				}
				else
				{
					if(lb_buff->fields[LB_MSG_FIELD1][0] == 'F')
					{
						if(lb_buff->fields[LB_MSG_FIELD2][0])
						{
							mtime = atol(lb_buff->fields[LB_MSG_FIELD2]);
						}

						if(mtime)
						{
							g_event_finish_time = mtime;
							event_parameter_count++;
						}
					}
				}
			}
			break;

			case LB_MESSAGE_CLOCK:
			{
				g_wifi_active = TRUE;

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
				event_parameter_count++;    /* Any ID or no ID is acceptable */

				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					strncpy(g_messages_text[STATION_ID], lb_buff->fields[LB_MSG_FIELD1], MAX_PATTERN_TEXT_LENGTH);

					if(g_messages_text[STATION_ID][0])
					{
						g_time_needed_for_ID = (500 + timeRequiredToSendStrAtWPM(g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
					}
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
						speed = atol(lb_buff->fields[LB_MSG_FIELD2]);
						g_id_codespeed = CLAMP(5, speed, 20);
						event_parameter_count++;

						if(g_messages_text[STATION_ID][0])
						{
							g_time_needed_for_ID = (500 + timeRequiredToSendStrAtWPM(g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
						}
					}
				}
				else if(lb_buff->fields[LB_MSG_FIELD1][0] == 'P')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						speed = atol(lb_buff->fields[LB_MSG_FIELD2]);
						g_pattern_codespeed = CLAMP(5, speed, 20);
						event_parameter_count++;
						g_code_throttle = throttleValue(g_pattern_codespeed);
					}
				}
			}
			break;

			case LB_MESSAGE_TIME_INTERVAL:
			{
				uint16_t time = 0;

				if(lb_buff->fields[LB_MSG_FIELD1][0] == '0')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						time = atol(lb_buff->fields[LB_MSG_FIELD2]);
						g_off_air_seconds = time;
						event_parameter_count++;
					}
				}
				else if(lb_buff->fields[LB_MSG_FIELD1][0] == '1')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						time = atol(lb_buff->fields[LB_MSG_FIELD2]);
						g_on_air_seconds = time;
						event_parameter_count++;
					}
				}
				else if(lb_buff->fields[LB_MSG_FIELD1][0] == 'I')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						time = atol(lb_buff->fields[LB_MSG_FIELD2]);
						g_ID_period_seconds = time;
						event_parameter_count++;
					}
				}
				else if(lb_buff->fields[LB_MSG_FIELD1][0] == 'D')
				{
					if(lb_buff->fields[LB_MSG_FIELD2][0])
					{
						time = atol(lb_buff->fields[LB_MSG_FIELD2]);
						g_intra_cycle_delay_time = time;
						event_parameter_count++;
					}
				}
			}
			break;

			case LB_MESSAGE_SET_PATTERN:
			{
				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					strncpy(g_messages_text[PATTERN_TEXT], lb_buff->fields[LB_MSG_FIELD1], MAX_PATTERN_TEXT_LENGTH);
					event_parameter_count++;
				}
			}
			break;

			case LB_MESSAGE_SET_FREQ:
			{
				Frequency_Hz transmitter_freq = 0;

				if(lb_buff->fields[LB_MSG_FIELD1][0])
				{
					static Frequency_Hz f;
					f = atol(lb_buff->fields[LB_MSG_FIELD1]);

					Frequency_Hz ff = f;
					if(txSetFrequency(&ff, TRUE))
					{
						transmitter_freq = ff;
						event_parameter_count++;
					}
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

// 			case MESSAGE_BAND:
// 			{
// 				RadioBand band;
// 
// 				if(lb_buff->fields[FIELD1][0])  /* band field */
// 				{
// 					EC ec = ERROR_CODE_ILLEGAL_COMMAND_RCVD;
// 					int b = atoi(lb_buff->fields[FIELD1]);
// 
// 					if(b == 80)
// 					{
// 						RadioBand b = BAND_80M;
// 						BOOL en = TRUE;
// 						ec = txSetParameters(NULL, &b, NULL, &en);
// 						event_parameter_count++;
// 					}
// 					else if(b == 2)
// 					{
// 						RadioBand b = BAND_2M;
// 						BOOL en = TRUE;
// 						ec = txSetParameters(NULL, &b, NULL, &en);
// 						event_parameter_count++;
// 					}
// 
// 					if(ec)
// 					{
// 						g_last_error_code = ec;
// 					}
// 				}
// 
// 				if(lb_buff->type == LINKBUS_MSG_QUERY)  /* Query */
// 				{
// 					/* Send a reply */
// 					sprintf(g_tempStr, "%i", band);
// 					lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_BAND_LABEL, g_tempStr);
// 				}
// 			}
// 			break;

			case LB_MESSAGE_BAT:
			{
				uint16_t bat;

				if(g_lastConversionResult[BATTERY_READING] > VOLTS_3_0) /* Send % of internal battery charge remaining */
				{
					bat = (uint16_t)CLAMP(0, BATTERY_PERCENTAGE(g_lastConversionResult[BATTERY_READING], (int32_t)g_battery_empty_mV), 100);
				}
				else                                                    /* Send the voltage of the external battery */
				{
					bat = VEXT(g_lastConversionResult[V12V_VOLTAGE_READING]);
				}

				lb_broadcast_num(bat, "!BAT");

				/* The system clock gets re-initialized whenever a battery message is received. This
				 * is just to ensure the two stay closely in sync while the user interface is active */
				set_system_time(ds3231_get_epoch(NULL));    /* update system clock */
			}
			break;

			case LB_MESSAGE_TEMP:
			{
				int16_t v;
				if(!ds3231_get_temp(&v))
				{
					lb_broadcast_num(v, "!TEM");
				}
			}
			break;

			case LB_MESSAGE_VER:
			{
				lb_send_msg(LINKBUS_MSG_REPLY, LB_MESSAGE_VER_LABEL, (char *)SW_REVISION);
			}
			break;


			default:
			{
				linkbus_reset_rx(); /* flush buffer */
				g_last_error_code = ERROR_CODE_ILLEGAL_COMMAND_RCVD;
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
BOOL eventEnabled()
{
	return TRUE;
}

void wdt_init(WDReset resetType)
{
	
}

uint16_t throttleValue(uint8_t speed)
{
	float temp;
	speed = CLAMP(5, (int8_t)speed, 20);
	temp = (4500L / (uint16_t)speed) / 10L;
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
	if(!g_event_start_time)
	{
		return( ERROR_CODE_EVENT_MISSING_START_TIME);
	}

	if(g_event_start_time >= g_event_finish_time)   /* Finish must be later than start */
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

	time_t now = time(NULL);
	if(g_event_finish_time < now)   /* the event has already finished */
	{
		if(statusCode)
		{
			*statusCode = STATUS_CODE_NO_EVENT_TO_RUN;
		}
	}
	else
	{
		int32_t dif = timeDif(now, g_event_start_time); /* returns arg1 - arg2 */

		if(dif >= 0)                                    /* start time is in the past */
		{
			BOOL turnOnTransmitter = FALSE;
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
					turnOnTransmitter = TRUE;
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
				cli();
				BOOL repeat = TRUE;
				makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
				g_code_throttle = throttleValue(g_pattern_codespeed);
				sei();
			}
			else
			{
				keyTransmitter(OFF);
			}

			g_event_commenced = TRUE;
		}
		else    /* start time is in the future */
		{
			if(statusCode)
			{
				*statusCode = STATUS_CODE_WAITING_FOR_EVENT_START;
			}
			keyTransmitter(OFF);
		}

		g_waiting_for_next_event = FALSE;
		g_update_timeout_seconds = 90;
	}

	return( ERROR_CODE_NO_ERROR);
}


EC hw_init()
{
	return ERROR_CODE_NO_ERROR;
}

EC rtc_init()
{
	return ERROR_CODE_NO_ERROR;
}

void set_ports(SleepType initType)
{
	
}

BOOL antennaIsConnected()
{
	return true;
}

void initializeAllEventSettings(BOOL disableEvent)
{
	
}

void suspendEvent()
{
	
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

// 	g_current_epoch = RTC_get_epoch();
// 	g_use_ptt_periodic_reset = FALSE;

	cli();

	switch(g_fox)
	{
		case FOX_1:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MOE");
				patternNotSet = false;
			}
		}
		case FOX_2:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MOI");
				patternNotSet = false;
			}
		}
		case FOX_3:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MOS");
				patternNotSet = false;
			}
		}
		case FOX_4:
		{
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MOH");
				patternNotSet = false;
			}
		}
		case FOX_5:
		{
			/* Set the Morse code pattern and speed */
			if(patternNotSet)
			{
				sprintf(g_messages_text[PATTERN_TEXT], "MO5");
				patternNotSet = false;
			}

			BOOL repeat = TRUE;
			makeMorse(g_messages_text[PATTERN_TEXT], &repeat, NULL);
			g_code_throttle = throttleValue(g_pattern_codespeed);

			g_event_start_time = 1;                     /* have it start a long time ago */
			g_event_finish_time = MAX_TIME;             /* run for a long long time */
			g_on_air_seconds = 60;						/* on period is very long */
			g_off_air_seconds = 240;                    /* off period is very short */
			g_on_the_air = 9999;                        /* start out transmitting */
			g_sendID_seconds_countdown = 60;			/* wait a long time to send the ID */
			g_event_commenced = TRUE;                   /* get things running immediately */
			g_event_enabled = TRUE;                     /* get things running immediately */
			g_last_status_code = STATUS_CODE_EVENT_STARTED_NOW_TRANSMITTING;
		}
		break;

		case SPRINT_S1:
		case SPRINT_S2:
		case SPRINT_S3:
		case SPRINT_S4:
		case SPRINT_S5:
		{
// 			g_on_air_interval_seconds = 12;
// 			g_cycle_period_seconds = 60;
// 			g_number_of_foxes = 5;
// 			g_pattern_codespeed = SPRINT_SLOW_CODE_SPEED;
// 			g_fox_id_offset = SPRINT_S1 - 1;
// 			g_id_interval_seconds = 600;
		}
		break;

		case SPRINT_F1:
		case SPRINT_F2:
		case SPRINT_F3:
		case SPRINT_F4:
		case SPRINT_F5:
		{
// 			g_on_air_interval_seconds = 12;
// 			g_cycle_period_seconds = 60;
// 			g_number_of_foxes = 5;
// 			g_pattern_codespeed = SPRINT_FAST_CODE_SPEED;
// 			g_fox_id_offset = SPRINT_F1 - 1;
// 			g_id_interval_seconds = 600;
		}
		break;

#if SUPPORT_TEMP_AND_VOLTAGE_REPORTING
		case REPORT_BATTERY:
		{
// 			g_on_air_interval_seconds = 30;
// 			g_cycle_period_seconds = 60;
// 			g_number_of_foxes = 2;
// 			g_pattern_codespeed = SPRINT_SLOW_CODE_SPEED;
// 			g_fox_id_offset = REPORT_BATTERY - 1;
// 			g_id_interval_seconds = 60;
		}
		break;
#endif // SUPPORT_TEMP_AND_VOLTAGE_REPORTING


		/* case BEACON:
		 * case SPECTATOR: */
		default:
		{
// 			g_use_ptt_periodic_reset = g_ptt_periodic_reset_enabled;
// 			g_number_of_foxes = 1;
// 			g_pattern_codespeed = 8;
// 			g_id_interval_seconds = g_ptt_periodic_reset_enabled ? 60 : 600;
// 			g_on_air_interval_seconds = g_id_interval_seconds;
// 			g_fox_id_offset = 0;
// 			g_cycle_period_seconds = g_id_interval_seconds;
		}
		break;
	}

	if(action == START_NOTHING)
	{
// 		g_use_rtc_for_startstop = FALSE;
// 		g_transmissions_disabled = TRUE;
	}
	else if(action == START_EVENT_NOW)
	{
// 		g_fox_counter = 1;
// 		g_seconds_since_sync = 0;                                               /* Total elapsed time since synchronization */
// 		g_use_rtc_for_startstop = FALSE;
// 		g_transmissions_disabled = FALSE;
	}
	else if(action == START_TRANSMISSIONS_NOW)                                  /* Immediately start transmitting, regardless RTC or time slot */
	{
// 		g_fox_counter = MAX(1, g_fox - g_fox_id_offset);
// 		g_seconds_since_sync = (g_fox_counter - 1) * g_on_air_interval_seconds; /* Total elapsed time since start of event */
// 		g_use_rtc_for_startstop = FALSE;
// 		g_transmissions_disabled = FALSE;
// 		g_initialize_fox_transmissions = INIT_EVENT_STARTING_NOW;
	}
	else                                                                    /* if(action == START_EVENT_WITH_STARTFINISH_TIMES) */
	{
// 		if(g_event_start_epoch < g_current_epoch)                           /* timed event in progress */
// 		{
// 			g_seconds_since_sync = g_current_epoch - g_event_start_epoch;   /* Total elapsed time counter: synced at event start time */
// 			g_fox_counter = CLAMP(1, 1 + ((g_seconds_since_sync % g_cycle_period_seconds) / g_on_air_interval_seconds), g_number_of_foxes);
// 			g_initialize_fox_transmissions = INIT_EVENT_IN_PROGRESS_WITH_STARTFINISH_TIMES;
// 		}
// 		else                                                                /* event starts in the future */
// 		{
// 			g_seconds_since_sync = 0;                                       /* Total elapsed time counter */
// 			g_fox_counter = 1;
// 			if(g_event_start_epoch > (g_current_epoch + 300))
// 			{
// 				digitalWrite(PIN_PWDN, OFF); /* Turn off the radio until close to start time */
// 			}
// 		}
// 
// 		g_use_rtc_for_startstop = TRUE;
// 		g_transmissions_disabled = TRUE;
	}

// 	sendMorseTone(OFF);
// 	g_code_throttle    = 0;                 /* Adjusts Morse code speed */
// 	g_on_the_air       = FALSE;             /* Controls transmitter Morse activity */

// 	g_config_error = NULL_CONFIG;           /* Trigger a new configuration enunciation */
// 	digitalWrite(PIN_LED, OFF);             /*  LED Off - in case it was left on */
// 
// 	digitalWrite(PIN_CW_KEY_LOGIC, OFF);    /* TX key line */
// 	g_sendAMmodulation = FALSE;
// 	g_LED_enunciating = FALSE;
// 	g_config_error = NULL_CONFIG;           /* Trigger a new configuration enunciation */

	sei();
}
