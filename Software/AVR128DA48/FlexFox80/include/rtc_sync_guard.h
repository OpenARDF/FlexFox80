/*
 * Small, dependency-free decisions used by bounded RTC synchronization.
 * Keep this header host-testable so timeout and stale-read behavior remain explicit.
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
