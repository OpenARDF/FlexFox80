#include "clone_reset_marker.h"
#include "clone_sync_safety.h"

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
	expect(cloneClockReadbackAccepted(1000, 1000), "clock_accepts_exact_second");
	expect(cloneClockReadbackAccepted(1000, 1001), "clock_accepts_next_second");
	expect(!cloneClockReadbackAccepted(1000, 999), "clock_rejects_backward_second");
	expect(!cloneClockReadbackAccepted(1000, 1002), "clock_rejects_two_seconds_forward");
	expect(cloneClockReadbackDelta(1000, 999) == -1, "clock_reports_signed_negative_delta");
	expect(cloneClockReadbackDelta(1000, 1001) == 1, "clock_reports_signed_positive_delta");

	expect(!masterCloneProgressTimedOut(true, 100, 15100), "watchdog_allows_exact_deadline");
	expect(masterCloneProgressTimedOut(true, 100, 15101), "watchdog_expires_after_deadline");
	expect(!masterCloneProgressTimedOut(false, 100, 20000), "watchdog_ignores_inactive_session");
	expect(!masterCloneProgressTimedOut(true, 0, 20000), "watchdog_requires_progress_anchor");
	expect(
		masterCloneProgressTimedOut(
			true,
			std::numeric_limits<uint32_t>::max() - 99U,
			MASTER_CLONE_PROGRESS_TIMEOUT_MILLIS),
		"watchdog_survives_millis_wrap");

	CloneResetMarker marker = cloneResetMarkerActive(8, 4, 3);
	expect(cloneResetMarkerWasInterrupted(&marker), "reset_marker_retains_active_clone");
	expect(marker.phase == 8 && marker.fileIndex == 4 && marker.fileCount == 3,
	       "reset_marker_retains_phase_and_file_progress");
	marker.fileIndex++;
	expect(!cloneResetMarkerWasInterrupted(&marker), "reset_marker_rejects_corruption");
	marker = cloneResetMarkerClear();
	expect(!cloneResetMarkerWasInterrupted(&marker), "cleared_marker_is_not_interrupted");
	expect(CLONE_RESET_RTC_OFFSET_WORDS + sizeof(CloneResetMarker) / sizeof(uint32_t) <= 120U,
	       "reset_marker_does_not_overlap_filesystem_marker");

	std::cout << "All ESP clone sync safety tests passed\n";
	return 0;
}
