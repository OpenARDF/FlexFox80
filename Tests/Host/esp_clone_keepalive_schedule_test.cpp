#include "clone_keepalive_schedule.h"

#include <cstdlib>
#include <iostream>
#include <limits>

namespace
{
void expect(bool condition, const char *name)
{
	if (!condition)
	{
		std::cerr << "FAIL " << name << '\n';
		std::exit(1);
	}
	std::cout << "PASS " << name << '\n';
}
} // namespace

int main()
{
	CloneKeepAliveSchedule schedule = {0, false, false};
	expect(!cloneKeepAliveIsDue(&schedule, 0), "inactive_schedule_is_not_due");

	cloneKeepAliveBegin(&schedule);
	expect(cloneKeepAliveIsDue(&schedule, 1000), "clone_start_is_due_immediately");
	expect(cloneKeepAliveIsDue(&schedule, 2000), "full_buffer_does_not_consume_due_heartbeat");

	cloneKeepAliveWasQueued(&schedule, 2000);
	expect(!cloneKeepAliveIsDue(&schedule, 21999), "heartbeat_waits_for_full_interval");
	expect(cloneKeepAliveIsDue(&schedule, 22000), "heartbeat_is_due_at_twenty_seconds");

	cloneKeepAliveWasQueued(&schedule, std::numeric_limits<uint32_t>::max() - 9999U);
	expect(
		!cloneKeepAliveIsDue(&schedule, 9999),
		"millis_wrap_preserves_pre_interval_wait");
	expect(
		cloneKeepAliveIsDue(&schedule, 10000),
		"millis_wrap_preserves_twenty_second_interval");

	cloneKeepAliveEnd(&schedule);
	expect(!cloneKeepAliveIsDue(&schedule, 50000), "clone_end_stops_heartbeat");

	std::cout << "All ESP clone keep-alive schedule tests passed\n";
	return 0;
}
