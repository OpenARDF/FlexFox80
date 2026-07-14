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
 * Dependency-free event-window decisions shared by AVR schedule paths.
 */

#ifndef EVENT_SCHEDULE_STATE_H_INCLUDED
#define EVENT_SCHEDULE_STATE_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	EVENT_SCHEDULE_INVALID,
	EVENT_SCHEDULE_FUTURE,
	EVENT_SCHEDULE_ACTIVE,
	EVENT_SCHEDULE_FINISHED
} EventSchedulePosition;

static inline EventSchedulePosition eventSchedulePosition(
	uint32_t now,
	uint32_t start,
	uint32_t finish,
	uint32_t minimumValidEpoch)
{
	if((now <= minimumValidEpoch) ||
		(start <= minimumValidEpoch) ||
		(finish <= minimumValidEpoch) ||
		(finish <= start))
	{
		return EVENT_SCHEDULE_INVALID;
	}

	if(now >= finish)
	{
		return EVENT_SCHEDULE_FINISHED;
	}

	return (now >= start) ? EVENT_SCHEDULE_ACTIVE : EVENT_SCHEDULE_FUTURE;
}

static inline bool eventScheduledForNowAt(
	uint32_t now,
	uint32_t start,
	uint32_t finish,
	uint32_t minimumValidEpoch)
{
	return eventSchedulePosition(now, start, finish, minimumValidEpoch) == EVENT_SCHEDULE_ACTIVE;
}

static inline bool eventScheduledForTheFutureAt(
	uint32_t now,
	uint32_t start,
	uint32_t finish,
	uint32_t minimumValidEpoch)
{
	return eventSchedulePosition(now, start, finish, minimumValidEpoch) == EVENT_SCHEDULE_FUTURE;
}

static inline bool eventScheduledAt(
	uint32_t now,
	uint32_t start,
	uint32_t finish,
	uint32_t minimumValidEpoch)
{
	EventSchedulePosition position = eventSchedulePosition(now, start, finish, minimumValidEpoch);
	return (position == EVENT_SCHEDULE_FUTURE) || (position == EVENT_SCHEDULE_ACTIVE);
}

#endif /* EVENT_SCHEDULE_STATE_H_INCLUDED */
