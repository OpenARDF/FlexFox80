#include <atmel_start.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "include/linkbus.h"
#include "include/transmitter.h"
#include "include/ds3231.h"
#include "include/morse.h"
#include "adc.h"
#include "Goertzel.h"


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
static volatile AntConnType g_antenna_connect_state = ANT_CONNECTION_UNDETERMINED;

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

extern volatile BOOL g_i2c_not_timed_out;
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
static uint16_t g_filterADCValue[NUMBER_OF_POLLED_ADC_CHANNELS] = { 500, 500, 500, 500, 500 };
static volatile BOOL g_adcUpdated[NUMBER_OF_POLLED_ADC_CHANNELS] = { FALSE, FALSE, FALSE, FALSE, FALSE };
static volatile uint16_t g_lastConversionResult[NUMBER_OF_POLLED_ADC_CHANNELS];

extern Goertzel g_goertzel;

/***********************************************************************
 * Private Function Prototypes
 *
 * These functions are available only within this file
 ************************************************************************/
BOOL eventEnabled(void);
void handleLinkBusMsgs(void);
void initializeEEPROMVars(void);
void saveAllEEPROM(void);
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
			LED_toggle_level();
		}

        TCB3.INTFLAGS = TCB_CAPT_bm; /* clear interrupt flag */
    }
}



/**
Periodic tasks not requiring precise timing. Rate = 300 Hz
*/
ISR(TCB0_INT_vect)
{
    /* Insert your TCB interrupt handling code */
    /**
     * The interrupt flag is cleared by writing 1 to it, or when the Capture register
     * is read in Capture mode
     */

    if(TCB0.INTFLAGS & TCB_CAPT_bm)
    {
		static BOOL conversionInProcess = FALSE;
		static int8_t indexConversionInProcess;
		static uint16_t codeInc = 0;
		BOOL repeat, finished;

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
						}
					}
				}
				else
				{
					keyTransmitter(key);
					codeInc = g_code_throttle;
				}
			}
			else if(!g_on_the_air)
			{
				if(key)
				{
					key = OFF;
					keyTransmitter(OFF);
					powerToTransmitter(OFF);
					g_last_status_code = STATUS_CODE_EVENT_STARTED_WAITING_FOR_TIME_SLOT;
				}
			}
		}

		/**
		 * Handle Periodic ADC Readings
		 * The following algorithm allows multipe ADC channel readings to be performed at different polling intervals. */
		if(!conversionInProcess)
		{
			/* Note: countdowns will pause while a conversion is in process. Conversions are so fast that this should not be an issue though. */

			volatile uint8_t i; /* volatile to prevent optimization performing undefined behavior */
			indexConversionInProcess = -1;

			for(i = 0; i < NUMBER_OF_POLLED_ADC_CHANNELS; i++)
			{
				if(g_tickCountdownADCFlag[i])
				{
					g_tickCountdownADCFlag[i]--;
				}

				if(g_tickCountdownADCFlag[i] == 0)
				{
					indexConversionInProcess = (int8_t)i;
				}
			}

			if(indexConversionInProcess >= 0)
			{
				g_tickCountdownADCFlag[indexConversionInProcess] = g_adcChannelConversionPeriod_ticks[indexConversionInProcess];    /* reset the tick countdown */
//				ADMUX = (ADMUX & 0xF0) | activeADC[indexConversionInProcess];                                                       /* index through all active channels */
//				ADCSRA |= (1 << ADSC);                                                                                              /*single conversion mode */
				conversionInProcess = TRUE;
			}
		}
		else if(1 /*!( ADCSRA & (1 << ADSC) ) */)                                                                                          /* wait for conversion to complete */
		{
			uint16_t hold = 0; //ADC;
			static uint16_t holdConversionResult;
			holdConversionResult = (uint16_t)(((uint32_t)hold * ADC_REF_VOLTAGE_mV) >> 10);                                         /* millivolts at ADC pin */
			uint16_t lastResult = g_lastConversionResult[indexConversionInProcess];

			g_adcUpdated[indexConversionInProcess] = TRUE;

			if(indexConversionInProcess == BATTERY_READING)
			{
				BOOL directionUP = holdConversionResult > lastResult;
				uint16_t delta = directionUP ? holdConversionResult - lastResult : lastResult - holdConversionResult;

				if(delta > g_filterADCValue[indexConversionInProcess])
				{
					lastResult = holdConversionResult;
					g_tickCountdownADCFlag[indexConversionInProcess] = 100; /* speed up next conversion */
				}
				else
				{
					if(directionUP)
					{
						lastResult++;
					}
					else if(delta)
					{
						lastResult--;
					}

					g_battery_measurements_active = TRUE;

					if(lastResult > VOLTS_5)
					{
						g_battery_type = BATTERY_9V;
					}
					else if(lastResult > VOLTS_3_0)
					{
						g_battery_type = BATTERY_4r2V;
					}
				}
			}
			else if(indexConversionInProcess == V12V_VOLTAGE_READING)
			{
				lastResult = holdConversionResult;
			}
	/*		else if(indexConversionInProcess == PA_VOLTAGE_READING)
	 *		{
	 *			lastResult = holdConversionResult;
	 *			g_PA_voltage = holdConversionResult;
	 *		} */
			else
			{
				lastResult = holdConversionResult;
			}

			g_lastConversionResult[indexConversionInProcess] = lastResult;

			conversionInProcess = FALSE;
		}

        TCB0.INTFLAGS = TCB_CAPT_bm; /* clear interrupt flag */
    }
}


int main(void)
{
	uint16_t count = 0;
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	linkbus_send_text("ABC...\n");
	
	ADC0_startConversions();

	while (1) {
		while(util_delay_ms(1000));
		sprintf(g_tempStr, "Seconds: %d\n", count++);
		linkbus_send_text(g_tempStr);
	}
}

/* The compiler does not seem to optimize large switch statements correctly */
void __attribute__((optimize("O0"))) handleLinkBusMsgs()
{
	LinkbusRxBuffer* lb_buff;
	static uint8_t event_parameter_count = 0;
	BOOL send_ack = TRUE;

	while((lb_buff = nextFullRxBuffer()))
	{
		LBMessageID msg_id = lb_buff->id;

		switch(msg_id)
		{
			case MESSAGE_WIFI:
			{
				BOOL result;

				if(lb_buff->fields[FIELD1][0])
				{
					result = atoi(lb_buff->fields[FIELD1]);

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

			case MESSAGE_RESET:
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

			case MESSAGE_ESP_COMM:
			{
				char f1 = lb_buff->fields[FIELD1][0];

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
							lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_ESP_LABEL, "1"); /* Request next scheduled event */
						}
						/* Send WiFi the current time */
						sprintf(g_tempStr, "%lu", time(NULL));
						lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_CLOCK_LABEL, g_tempStr);
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

			case MESSAGE_TX_POWER:
			{
				static uint16_t pwr_mW;

				if(lb_buff->fields[FIELD1][0])
				{
					EC ec;

					if((lb_buff->fields[FIELD1][0] == 'M') && (lb_buff->fields[FIELD2][0]))
					{
						pwr_mW = (uint16_t)atoi(lb_buff->fields[FIELD2]);
						event_parameter_count++;
					}
					else
					{
						pwr_mW = (uint16_t)atoi(lb_buff->fields[FIELD1]);
					}

					ec = txSetParameters(&pwr_mW, NULL);
					if(ec)
					{
						g_last_error_code = ec;
					}

					sprintf(g_tempStr, "M,%u", pwr_mW);
					lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_TX_POWER_LABEL, g_tempStr);
				}
			}
			break;

			case MESSAGE_PERM:
			{
				storeTransmitterValues();
				saveAllEEPROM();
			}
			break;

			case MESSAGE_GO:
			{
				char f1 = lb_buff->fields[FIELD1][0];

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
									saveAllEEPROM();    /* Make sure all  event values get saved */
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

			case MESSAGE_STARTFINISH:
			{
				time_t mtime = 0;

				if(lb_buff->fields[FIELD1][0] == 'S')
				{
					if(lb_buff->fields[FIELD2][0])
					{
						mtime = atol(lb_buff->fields[FIELD2]);
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
					if(lb_buff->fields[FIELD1][0] == 'F')
					{
						if(lb_buff->fields[FIELD2][0])
						{
							mtime = atol(lb_buff->fields[FIELD2]);
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

			case MESSAGE_CLOCK:
			{
				g_wifi_active = TRUE;

				if(lb_buff->type == LINKBUS_MSG_COMMAND)    /* ignore replies since, as the time source, we should never be sending queries anyway */
				{
					if(lb_buff->fields[FIELD1][0])
					{
						strncpy(g_tempStr, lb_buff->fields[FIELD1], 20);
						ds3231_set_date_time(g_tempStr, RTC_CLOCK);
						set_system_time(ds3231_get_epoch(NULL));    /* update system clock */
					}
					else
					{
						sprintf(g_tempStr, "%lu", time(NULL));
						lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_CLOCK_LABEL, g_tempStr);
					}
				}
				else
				{
					if(lb_buff->type == LINKBUS_MSG_QUERY)
					{
						if(lb_buff->fields[FIELD1][0] == 'X')
						{
							int8_t age = 0;

							if(lb_buff->fields[FIELD2][0])
							{
								age = (int8_t)atoi(lb_buff->fields[FIELD2]);
								ds3231_set_aging(&age);
							}
							else
							{
								age = ds3231_get_aging();
								sprintf(g_tempStr, "X,%d", age);
								lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_CLOCK_LABEL, g_tempStr);
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
								lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_CLOCK_LABEL, g_tempStr);
								lastTime = temp_time;
							}
						}
					}
				}
			}
			break;

			case MESSAGE_SET_STATION_ID:
			{
				event_parameter_count++;    /* Any ID or no ID is acceptable */

				if(lb_buff->fields[FIELD1][0])
				{
					strncpy(g_messages_text[STATION_ID], lb_buff->fields[FIELD1], MAX_PATTERN_TEXT_LENGTH);

					if(g_messages_text[STATION_ID][0])
					{
						g_time_needed_for_ID = (500 + timeRequiredToSendStrAtWPM(g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
					}
				}
			}
			break;

			case MESSAGE_CODE_SPEED:
			{
				uint8_t speed = g_pattern_codespeed;

				if(lb_buff->fields[FIELD1][0] == 'I')
				{
					if(lb_buff->fields[FIELD2][0])
					{
						speed = atol(lb_buff->fields[FIELD2]);
						g_id_codespeed = CLAMP(5, speed, 20);
						event_parameter_count++;

						if(g_messages_text[STATION_ID][0])
						{
							g_time_needed_for_ID = (500 + timeRequiredToSendStrAtWPM(g_messages_text[STATION_ID], g_id_codespeed)) / 1000;
						}
					}
				}
				else if(lb_buff->fields[FIELD1][0] == 'P')
				{
					if(lb_buff->fields[FIELD2][0])
					{
						speed = atol(lb_buff->fields[FIELD2]);
						g_pattern_codespeed = CLAMP(5, speed, 20);
						event_parameter_count++;
						g_code_throttle = throttleValue(g_pattern_codespeed);
					}
				}
			}
			break;

			case MESSAGE_TIME_INTERVAL:
			{
				uint16_t time = 0;

				if(lb_buff->fields[FIELD1][0] == '0')
				{
					if(lb_buff->fields[FIELD2][0])
					{
						time = atol(lb_buff->fields[FIELD2]);
						g_off_air_seconds = time;
						event_parameter_count++;
					}
				}
				else if(lb_buff->fields[FIELD1][0] == '1')
				{
					if(lb_buff->fields[FIELD2][0])
					{
						time = atol(lb_buff->fields[FIELD2]);
						g_on_air_seconds = time;
						event_parameter_count++;
					}
				}
				else if(lb_buff->fields[FIELD1][0] == 'I')
				{
					if(lb_buff->fields[FIELD2][0])
					{
						time = atol(lb_buff->fields[FIELD2]);
						g_ID_period_seconds = time;
						event_parameter_count++;
					}
				}
				else if(lb_buff->fields[FIELD1][0] == 'D')
				{
					if(lb_buff->fields[FIELD2][0])
					{
						time = atol(lb_buff->fields[FIELD2]);
						g_intra_cycle_delay_time = time;
						event_parameter_count++;
					}
				}
			}
			break;

			case MESSAGE_SET_PATTERN:
			{
				if(lb_buff->fields[FIELD1][0])
				{
					strncpy(g_messages_text[PATTERN_TEXT], lb_buff->fields[FIELD1], MAX_PATTERN_TEXT_LENGTH);
					event_parameter_count++;
				}
			}
			break;

			case MESSAGE_SET_FREQ:
			{
				Frequency_Hz transmitter_freq = 0;

				if(lb_buff->fields[FIELD1][0])
				{
					static Frequency_Hz f;
					f = atol(lb_buff->fields[FIELD1]);

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
					lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_SET_FREQ_LABEL, g_tempStr);
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

			case MESSAGE_BAT:
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

			case MESSAGE_TEMP:
			{
				int16_t v;
				if(!ds3231_get_temp(&v))
				{
					lb_broadcast_num(v, "!TEM");
				}
			}
			break;

			case MESSAGE_VER:
			{
				lb_send_msg(LINKBUS_MSG_REPLY, MESSAGE_VER_LABEL, SW_REVISION);
			}
			break;


			default:
			{
				linkbus_reset_rx(); /* flush buffer */
				g_last_error_code = ERROR_CODE_ILLEGAL_COMMAND_RCVD;
			}
			break;
		}

		lb_buff->id = MESSAGE_EMPTY;
		if(send_ack)
		{
			linkbus_send_text(MESSAGE_ACK);
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

void initializeEEPROMVars()
{
	
}

void saveAllEEPROM()
{
	
}

void wdt_init(WDReset resetType)
{
	
}

uint16_t throttleValue(uint8_t speed)
{
	return 0;
}

EC activateEventUsingCurrentSettings(SC* statusCode)
{
	return ERROR_CODE_NO_ERROR;
}

EC launchEvent(SC* statusCode)
{
	return ERROR_CODE_NO_ERROR;
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
