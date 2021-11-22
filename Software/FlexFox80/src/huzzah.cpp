/*
 * huzzah.c
 *
 * Created: 1/23/2018 9:16:27 AM
 *  Author: charl
 */ 

#include "huzzah.h"
#include "mcp23017.h"

#define WIFI_RESET_gc (1 << 6)  /* WiFi reset port pin */
#define WIFI_POWER_gc (1 << 5)  /* WiFi enable power port pin */

BOOL wifi_enabled(void)
{
	return  VPORTA.OUT & WIFI_RESET_gc; /* read setting on reset line */
}

void wifi_reset(BOOL reset)
{
	if(reset) // assert reset low
	{
		VPORTA.OUT &= ~WIFI_RESET_gc;
	}
	else // de-assert reset high
	{
		VPORTA.OUT |= WIFI_RESET_gc;
	}
}

void wifi_power(BOOL on)
{
	if(on)
	{
		VPORTA.OUT |= WIFI_POWER_gc;
	}
	else
	{
		VPORTA.OUT &= ~WIFI_POWER_gc;
	}
}
