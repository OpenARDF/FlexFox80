/*
 * Preserve RTC edges that occur while the normal-priority PORTA interrupt is blocked.
 * All fields are one byte so the Level-1 observer and normal-priority consumer can
 * exchange state atomically on the AVR.
 */

#ifndef RTC_EDGE_TRACKER_H_INCLUDED
#define RTC_EDGE_TRACKER_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	uint8_t observed;
	uint8_t serviced;
	bool previousLevel;
} RtcEdgeTracker;

static inline void rtcEdgeTrackerReset(volatile RtcEdgeTracker* tracker, bool currentLevel)
{
	tracker->observed = tracker->serviced;
	tracker->previousLevel = currentLevel;
}

static inline void rtcEdgeTrackerObserve(volatile RtcEdgeTracker* tracker, bool currentLevel)
{
	if(currentLevel && !tracker->previousLevel)
	{
		tracker->observed++;
	}

	tracker->previousLevel = currentLevel;
}

static inline uint8_t rtcEdgeTrackerTake(volatile RtcEdgeTracker* tracker)
{
	uint8_t elapsed = (uint8_t)(tracker->observed - tracker->serviced);

	/* PORTA can run before the Level-1 sampler observes the same physical edge. */
	if(!elapsed)
	{
		elapsed = 1;
	}

	tracker->serviced += elapsed;
	return elapsed;
}

static inline uint8_t rtcEdgeTrackerGeneration(volatile RtcEdgeTracker* tracker)
{
	return tracker->observed;
}

#endif /* RTC_EDGE_TRACKER_H_INCLUDED */
