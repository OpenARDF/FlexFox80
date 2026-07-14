/*
 * Dependency-free event-window decisions shared by foreground schedule helpers.
 * Keep this header host-testable so exact start and finish semantics stay aligned
 * with the RTC ISR's >= boundary checks.
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
