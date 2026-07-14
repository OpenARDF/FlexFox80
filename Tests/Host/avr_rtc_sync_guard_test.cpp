#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "rtc_sync_guard.h"

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

void wait_contract_is_bounded_and_edge_first()
{
	expect(
		rtcSyncWaitState(false, true) == RTC_SYNC_WAITING,
		"missing_edge_keeps_waiting_while_timer_runs");
	expect(
		rtcSyncWaitState(true, true) == RTC_SYNC_EDGE_READY,
		"fresh_edge_completes_wait");
	expect(
		rtcSyncWaitState(true, false) == RTC_SYNC_EDGE_READY,
		"edge_wins_at_timeout_boundary");
	expect(
		rtcSyncWaitState(false, false) == RTC_SYNC_TIMED_OUT,
		"missing_edge_times_out");
}

void read_commit_requires_success_without_an_intervening_edge()
{
	expect(rtcSyncReadCanCommit(true, 17, 17), "successful_current_generation_read_commits");
	expect(!rtcSyncReadCanCommit(false, 17, 17), "failed_i2c_read_does_not_commit");
	expect(!rtcSyncReadCanCommit(true, 17, 18), "intervening_edge_rejects_stale_read");
	expect(!rtcSyncReadCanCommit(true, 255, 0), "wrapped_generation_change_rejects_stale_read");
}

} // namespace

int main()
{
	wait_contract_is_bounded_and_edge_first();
	read_commit_requires_success_without_an_intervening_edge();

	std::cout << "All AVR RTC synchronization guard tests passed\n";
	return EXIT_SUCCESS;
}
