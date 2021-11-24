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
 */


#include "huzzah.h"

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
