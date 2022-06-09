/* 
* leds.cpp
*
* Created: 5/5/2022 10:27:49 AM
* Author: charl
*/

#include "defs.h"
#include "atmel_start_pins.h"
#include "leds.h"

#define FAST_ON 25
#define FAST_OFF 25
#define SLOW_ON 500
#define SLOW_OFF 500
#define BRIEF_ON 25
#define BRIEF_OFF 150
#define LED_TIMEOUT 60000

static volatile Blink_t lastBlinkSetting = LEDS_OFF;
static volatile uint32_t led_timeout_count = 0;
static volatile int16_t red_blink_on_period = 0;
static volatile int16_t red_blink_off_period = 0;
static volatile int16_t green_blink_on_period = 0;
static volatile int16_t green_blink_off_period = 0;
static volatile int16_t red_blink_count = 0;
static volatile int16_t green_blink_count = 0;
static volatile bool red_led_configured = false;
static volatile bool green_led_configured = false;
static volatile bool g_override = false;

// default constructor
leds::leds()
{
} //leds

// default destructor
leds::~leds()
{
} //~leds

/* LED enunciation timer interrupt */
ISR(TCB3_INT_vect)
{
	if(led_timeout_count || g_override)
	{
		led_timeout_count--;
		
		if(TCB3.INTFLAGS & TCB_CAPT_bm)
		{
			if(red_blink_count)
			{
				if(red_blink_count > 1)
				{
					LED_set_RED_level(ON);
					red_blink_count--;
				}
				else if(red_blink_count < -1)
				{
					LED_set_RED_level(OFF);
					red_blink_count++;
				}
			
				if(red_blink_count == 1)
				{
					if(red_blink_off_period)
					{
						red_blink_count = -red_blink_off_period;
					}
					else /* constantly on */
					{
						red_blink_count = red_blink_on_period;
					}
				}
				else if(red_blink_count == -1)
				{
					red_blink_count = red_blink_on_period;
				}
			
			}
			else if(red_led_configured)
			{
				LED_set_RED_level(OFF);
			}
		
			if(green_blink_count)
			{
				if(green_blink_count > 1)
				{
					LED_set_GREEN_level(ON);
					green_blink_count--;
				}
				else if(green_blink_count < -1)
				{
					LED_set_GREEN_level(OFF);
					green_blink_count++;
				}
			
				if(green_blink_count == 1)
				{
					if(green_blink_off_period)
					{
						green_blink_count = -green_blink_off_period;
					}
					else /* constantly on */
					{
						green_blink_count = green_blink_on_period;
					}
				}
				else if(green_blink_count == -1)
				{
					green_blink_count = green_blink_on_period;
				}	
			}
			else if(green_led_configured)
			{
				LED_set_GREEN_level(OFF);
			}
		}
	}
	else
	{
		TCB3.INTCTRL &= ~TCB_CAPT_bm;   /* Capture or Timeout: disabled */
		LED_set_RED_level(OFF);
		LED_set_GREEN_level(OFF);
// 		red_led_configured = false;
// 		green_led_configured = false;
	}
		
	TCB3.INTFLAGS |= TCB_CAPT_bm; /* clear interrupt flag */
}

void leds::reset(void)
{
	g_override = false;
	blink(LEDS_OFF);
}


bool leds::active(void)
{
	return(led_timeout_count && (TCB3.INTCTRL & (1 << TCB_CAPT_bp)));
}

void leds::setRed(bool on)
{
	if(g_override) return;

	if(on)
	{
		if(led_timeout_count)
		{
			LED_set_RED_level(ON);
		}
	}
	else
	{
		LED_set_RED_level(OFF);
	}
}

void leds::setGreen(bool on)
{
	if(g_override) return;

	if(on)
	{
		if(led_timeout_count)
		{
			LED_set_GREEN_level(ON);
		}
	}
	else
	{
		LED_set_GREEN_level(OFF);
	}
}

void leds::resume(void)
{
	if(!led_timeout_count)
	{
		led_timeout_count = LED_TIMEOUT;
		TCB3.INTCTRL |= TCB_CAPT_bm;   /* Capture or Timeout: enabled */
	}
}

void leds::blink(Blink_t blinkMode)
{
	if(g_override) return;
	
	led_timeout_count = LED_TIMEOUT;
	
	if(blinkMode != lastBlinkSetting)
	{
		TCB3.INTCTRL &= ~TCB_CAPT_bm;   /* Capture or Timeout: disabled */

		switch(blinkMode)
		{
			case LEDS_OFF:
			{
				red_blink_count = 0;
				green_blink_count = 0;
				LED_set_RED_level(OFF);
				LED_set_GREEN_level(OFF);
				red_led_configured = false;
				green_led_configured = false;
			}
			break;
			
			case LEDS_RED_OFF:
			{
				LED_set_RED_level(OFF);
				red_blink_count = 0;
				red_led_configured = false;
			}
			break;
			
			case LEDS_GREEN_OFF:
			{
				LED_set_GREEN_level(OFF);
				green_blink_count = 0;
				green_led_configured = false;
			}
			break;
			
			case LEDS_RED_BLINK_FAST:
			{
				red_blink_on_period = BRIEF_ON;
				red_blink_off_period = BRIEF_OFF;
				red_blink_count = red_blink_on_period;	
				red_led_configured = true;			
			}
			break;
			
			case LEDS_GREEN_BLINK_FAST:
			{
				green_blink_on_period = BRIEF_ON;
				green_blink_off_period = BRIEF_OFF;	
				green_blink_count = green_blink_on_period;			
				green_led_configured = true;			
			}
			break;
			
			case LEDS_RED_BLINK_SLOW:
			{
				red_blink_on_period = SLOW_ON;
				red_blink_off_period = SLOW_OFF;
				red_blink_count = red_blink_on_period;				
				red_led_configured = true;			
			}
			break;
			
			case LEDS_GREEN_BLINK_SLOW:
			{
				green_blink_on_period = SLOW_ON;
				green_blink_off_period = SLOW_OFF;	
				green_blink_count = green_blink_on_period;			
				green_led_configured = true;			
			}
			break;
			
			case LEDS_RED_THEN_GREEN_BLINK_SLOW:
			{
				green_blink_on_period = SLOW_ON;
				green_blink_off_period = SLOW_OFF;	
				green_blink_count = -green_blink_on_period;			
				red_blink_on_period = SLOW_ON;
				red_blink_off_period = SLOW_OFF;
				red_blink_count = red_blink_on_period;				
				red_led_configured = true;			
				green_led_configured = true;			
			}
			break;
			
			case LEDS_RED_THEN_GREEN_BLINK_FAST:
			{
				green_blink_on_period = FAST_ON;
				green_blink_off_period = FAST_OFF;	
				green_blink_count = -green_blink_on_period;			
				red_blink_on_period = FAST_ON;
				red_blink_off_period = FAST_OFF;
				red_blink_count = red_blink_on_period;				
				red_led_configured = true;			
				green_led_configured = true;			
			}
			break;
			
			case LEDS_RED_AND_GREEN_BLINK_SLOW:
			{
				green_blink_on_period = SLOW_ON;
				green_blink_off_period = SLOW_OFF;	
				green_blink_count = green_blink_on_period;			
				red_blink_on_period = SLOW_ON;
				red_blink_off_period = SLOW_OFF;
				red_blink_count = red_blink_on_period;				
				red_led_configured = true;			
				green_led_configured = true;			
			}
			break;
			
			case LEDS_RED_AND_GREEN_BLINK_FAST_OVERRIDE_ALL:
			{
				g_override = true;
			}
			/* Intentional fall-thru */
			case LEDS_RED_AND_GREEN_BLINK_FAST:
			{
				green_blink_on_period = FAST_ON;
				green_blink_off_period = FAST_OFF;	
				green_blink_count = green_blink_on_period;			
				red_blink_on_period = FAST_ON;
				red_blink_off_period = FAST_OFF;
				red_blink_count = red_blink_on_period;				
				red_led_configured = true;			
				green_led_configured = true;			
			}
			break;
			
			case LEDS_RED_ON_CONSTANT:
			{
				red_blink_on_period = SLOW_ON;
				red_blink_off_period = 0;
				red_blink_count = red_blink_on_period;
				red_led_configured = true;			
			}
			break;
			
			case LEDS_GREEN_ON_CONSTANT:
			{
				green_blink_on_period = SLOW_ON;
				green_blink_off_period = 0;
				green_blink_count = green_blink_on_period;
				green_led_configured = true;			
			}
			break;
			
			default:
			{
				
			}
			break;
		}
		
		if(red_led_configured || green_led_configured) 
		{
			TCB3.INTCTRL |= TCB_CAPT_bm;   /* Capture or Timeout: enabled */
		}
	}
	
	lastBlinkSetting = blinkMode;
}
