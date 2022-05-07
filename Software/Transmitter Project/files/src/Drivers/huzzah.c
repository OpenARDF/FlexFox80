/*
 * huzzah.c
 *
 * Created: 1/23/2018 9:16:27 AM
 *  Author: charl
 */ 

#include "huzzah.h"
#include "mcp23017.h"

BOOL wifi_enabled(void)
{
	return PORTD & (1 << PORTD6);
}

void wifi_reset(BOOL reset)
{
	if(reset) // assert reset low
	{
		PORTD &= ~(1 << PORTD6);
	}
	else // de-assert reset high
	{
		PORTD |= (1 << PORTD6);
	}
}

void wifi_power(BOOL on)
{
	if(on)
	{
		PORTD |= (1 << PORTD7);
	}
	else
	{
		PORTD &= ~(1 << PORTD7);
	}
}
