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
 * Bounded RTC synchronization and stale-read rejection.
 */

#ifndef RTC_SYNC_GUARD_H_INCLUDED
#define RTC_SYNC_GUARD_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	RTC_SYNC_WAITING,
	RTC_SYNC_EDGE_READY,
	RTC_SYNC_TIMED_OUT
} RtcSyncWaitState;

static inline RtcSyncWaitState rtcSyncWaitState(bool edgeSeen, bool timerRunning)
{
	if(edgeSeen)
	{
		return RTC_SYNC_EDGE_READY;
	}

	return timerRunning ? RTC_SYNC_WAITING : RTC_SYNC_TIMED_OUT;
}

static inline bool rtcSyncReadCanCommit(
	bool rtcReadSucceeded,
	uint8_t generationBefore,
	uint8_t generationAfter)
{
	return rtcReadSucceeded && (generationBefore == generationAfter);
}

#endif /* RTC_SYNC_GUARD_H_INCLUDED */
