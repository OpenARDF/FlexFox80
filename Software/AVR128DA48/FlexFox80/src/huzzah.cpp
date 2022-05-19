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
#include "binio.h"
#include "port.h"

#define WIFI_RESET_gc (1 << 6)  /* WiFi reset port pin on PORTA*/
#define WIFI_POWER_gc (1 << 5)  /* WiFi enable power port pin on PORTA */
#define WIFI_MODULE_DETECT_gc (1 << 7) /* WiFi module detect pin on PORTD */

void wifi_power(bool state)
{
	if(state == ON)
	{
		PORTA_set_pin_level(WIFI_ENABLE, HIGH);
	}
	else
	{
		PORTA_set_pin_level(WIFI_ENABLE, LOW);
	}
}

void wifi_reset(bool state)
{
	if(state == ON)
	{
		PORTA_set_pin_level(WIFI_RESET, LOW);
	}
	else
	{
		PORTA_set_pin_level(WIFI_RESET, HIGH);
	}
}

bool wifiPresent(void)
{
	return(VPORTD.IN & WIFI_MODULE_DETECT_gc);
}

bool wifiEnabled(void)
{
	if(!(PORTA.OUT & (1 << WIFI_ENABLE))) /* read output setting on power-enable line */
	{
		return false;
	}
	
	if(!(PORTA.OUT & (1 << WIFI_RESET))) /* read output setting on reset line */
	{
		return false;
	}

	return true;
}


