#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "event_schedule_state.h"

namespace {

constexpr std::uint32_t minimumValidEpoch = 1609459200UL;
constexpr std::uint32_t startEpoch = minimumValidEpoch + 100UL;
constexpr std::uint32_t finishEpoch = startEpoch + 10UL;

void expect(bool condition, const char *name)
{
	if(!condition)
	{
		std::cerr << "FAIL " << name << '\n';
		std::exit(EXIT_FAILURE);
	}

	std::cout << "PASS " << name << '\n';
}

void invalid_windows_are_rejected()
{
	expect(
		eventSchedulePosition(minimumValidEpoch, startEpoch, finishEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_INVALID,
		"invalid_current_time_is_rejected");
	expect(
		eventSchedulePosition(startEpoch, minimumValidEpoch, finishEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_INVALID,
		"invalid_start_is_rejected");
	expect(
		eventSchedulePosition(startEpoch, startEpoch, startEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_INVALID,
		"empty_window_is_rejected");
	expect(
		eventSchedulePosition(startEpoch, finishEpoch, startEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_INVALID,
		"reversed_window_is_rejected");
}

void exact_boundaries_match_the_rtc_isr_contract()
{
	expect(
		eventSchedulePosition(startEpoch - 1, startEpoch, finishEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_FUTURE,
		"start_minus_one_is_future");
	expect(
		eventSchedulePosition(startEpoch, startEpoch, finishEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_ACTIVE,
		"start_is_active");
	expect(
		eventSchedulePosition(startEpoch + 1, startEpoch, finishEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_ACTIVE,
		"start_plus_one_is_active");
	expect(
		eventSchedulePosition(finishEpoch - 1, startEpoch, finishEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_ACTIVE,
		"finish_minus_one_is_active");
	expect(
		eventSchedulePosition(finishEpoch, startEpoch, finishEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_FINISHED,
		"finish_is_finished");
	expect(
		eventSchedulePosition(finishEpoch + 1, startEpoch, finishEpoch, minimumValidEpoch) ==
			EVENT_SCHEDULE_FINISHED,
		"finish_plus_one_is_finished");
}

void schedule_predicates_share_the_same_position()
{
	expect(
		eventScheduledForTheFutureAt(startEpoch - 1, startEpoch, finishEpoch, minimumValidEpoch),
		"future_predicate_accepts_start_minus_one");
	expect(
		!eventScheduledForTheFutureAt(startEpoch, startEpoch, finishEpoch, minimumValidEpoch),
		"future_predicate_rejects_start");
	expect(
		eventScheduledForNowAt(startEpoch, startEpoch, finishEpoch, minimumValidEpoch),
		"active_predicate_accepts_start");
	expect(
		!eventScheduledForNowAt(finishEpoch, startEpoch, finishEpoch, minimumValidEpoch),
		"active_predicate_rejects_finish");
	expect(
		eventScheduledAt(startEpoch - 1, startEpoch, finishEpoch, minimumValidEpoch),
		"scheduled_predicate_accepts_future");
	expect(
		eventScheduledAt(startEpoch, startEpoch, finishEpoch, minimumValidEpoch),
		"scheduled_predicate_accepts_start");
	expect(
		!eventScheduledAt(finishEpoch, startEpoch, finishEpoch, minimumValidEpoch),
		"scheduled_predicate_rejects_finish");
}

} // namespace

int main()
{
	invalid_windows_are_rejected();
	exact_boundaries_match_the_rtc_isr_contract();
	schedule_predicates_share_the_same_position();

	std::cout << "All AVR event schedule state tests passed\n";
	return EXIT_SUCCESS;
}
