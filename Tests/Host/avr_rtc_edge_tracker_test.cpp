#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "rtc_edge_tracker.h"

namespace {

void expect(bool condition, const char *name)
{
	if(!condition)
	{
		std::cerr << "FAIL " << name << '\n';
		std::exit(EXIT_FAILURE);
	}

	std::cout << "PASS " << name << '\n';
}

void port_interrupt_before_sampler_counts_once()
{
	RtcEdgeTracker tracker = {};
	rtcEdgeTrackerReset(&tracker, false);

	expect(rtcEdgeTrackerTake(&tracker) == 1, "port_interrupt_before_sampler_counts_once");
	rtcEdgeTrackerObserve(&tracker, true);
	rtcEdgeTrackerObserve(&tracker, false);
	rtcEdgeTrackerObserve(&tracker, true);
	expect(rtcEdgeTrackerTake(&tracker) == 1, "sampler_catches_up_without_double_counting");
}

void sampler_before_port_interrupt_counts_once()
{
	RtcEdgeTracker tracker = {};
	rtcEdgeTrackerReset(&tracker, false);
	rtcEdgeTrackerObserve(&tracker, true);

	expect(rtcEdgeTrackerTake(&tracker) == 1, "sampler_before_port_interrupt_counts_once");
}

void blocked_port_interrupt_recovers_every_observed_edge()
{
	RtcEdgeTracker tracker = {};
	rtcEdgeTrackerReset(&tracker, false);

	for(uint8_t edge = 0; edge < 3; ++edge)
	{
		rtcEdgeTrackerObserve(&tracker, true);
		rtcEdgeTrackerObserve(&tracker, false);
	}

	expect(rtcEdgeTrackerTake(&tracker) == 3, "blocked_port_interrupt_recovers_every_observed_edge");
}

void counter_wrap_preserves_elapsed_edge_count()
{
	RtcEdgeTracker tracker = {};
	tracker.observed = 254;
	tracker.serviced = 254;
	tracker.previousLevel = false;

	for(uint8_t edge = 0; edge < 3; ++edge)
	{
		rtcEdgeTrackerObserve(&tracker, true);
		rtcEdgeTrackerObserve(&tracker, false);
	}

	expect(rtcEdgeTrackerGeneration(&tracker) == 1, "generation_counter_wraps_with_observed_edges");
	expect(rtcEdgeTrackerTake(&tracker) == 3, "counter_wrap_preserves_elapsed_edge_count");
}

void restart_discards_sleep_gap_from_disabled_sampler()
{
	RtcEdgeTracker tracker = {};
	tracker.observed = 10;
	tracker.serviced = 14;
	tracker.previousLevel = false;

	rtcEdgeTrackerReset(&tracker, true);
	expect(tracker.observed == tracker.serviced, "restart_aligns_observed_and_serviced_counts");
	expect(rtcEdgeTrackerTake(&tracker) == 1, "sleep_edge_fallback_remains_one_second");
}

} // namespace

int main()
{
	port_interrupt_before_sampler_counts_once();
	sampler_before_port_interrupt_counts_once();
	blocked_port_interrupt_recovers_every_observed_edge();
	counter_wrap_preserves_elapsed_edge_count();
	restart_discards_sleep_gap_from_disabled_sampler();

	std::cout << "All AVR RTC edge tracker tests passed\n";
	return EXIT_SUCCESS;
}
