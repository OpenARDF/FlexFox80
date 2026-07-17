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
 * Lossless RTC-edge accounting across AVR interrupt-priority levels.
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

static inline uint8_t rtcEdgeTrackerTakePortEdge(
	volatile RtcEdgeTracker* tracker,
	bool samplerRunning,
	bool currentLevel)
{
	/*
	 * In standby the sampler is deliberately stopped, so the PORT interrupt itself
	 * is the only edge evidence. Do not advance one tracker counter without the
	 * other: a second sleeping edge would otherwise wrap their difference to 255.
	 * TIMERB_init() realigns the tracker before sampling resumes after wake.
	 */
	if(!samplerRunning)
	{
		return 1;
	}

	rtcEdgeTrackerObserve(tracker, currentLevel);
	return rtcEdgeTrackerTake(tracker);
}

static inline uint8_t rtcEdgeTrackerGeneration(volatile RtcEdgeTracker* tracker)
{
	return tracker->observed;
}

#endif /* RTC_EDGE_TRACKER_H_INCLUDED */
