/*
 *  MIT License
 *
 *  Copyright (c) 2026 DigitalConfections
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
 * Dependency-free AVR WiFi power-lease policy shared with host tests.
 */

#ifndef WIFI_POWER_LEASE_H_INCLUDED
#define WIFI_POWER_LEASE_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#define WIFI_NORMAL_SHUTDOWN_SECONDS 120U
#define WIFI_UPDATE_SHUTDOWN_SECONDS 300U

static inline uint16_t wifiUpdateLeaseRenew(void)
{
	return WIFI_UPDATE_SHUTDOWN_SECONDS;
}

static inline bool wifiUpdateLeaseTick(volatile uint16_t *seconds)
{
	if(!seconds || !*seconds)
	{
		return false;
	}

	(*seconds)--;
	return !*seconds;
}

static inline void wifiUpdateLeaseRelease(volatile uint16_t *seconds)
{
	if(seconds)
	{
		*seconds = 0;
	}
}

#endif /* WIFI_POWER_LEASE_H_INCLUDED */
