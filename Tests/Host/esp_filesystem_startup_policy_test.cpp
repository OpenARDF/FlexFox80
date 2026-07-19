#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "filesystem_startup_policy.h"

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

} // namespace

int main()
{
	FilesystemStartupMarker marker = filesystemStartupMarkerClear();
	expect(!filesystemStartupMarkerWasInterrupted(&marker), "clear_marker_allows_normal_mount");

	marker.magic = FILESYSTEM_STARTUP_RTC_MAGIC;
	expect(!filesystemStartupMarkerWasInterrupted(&marker), "magic_alone_does_not_force_recovery");

	marker = filesystemStartupMarkerClear();
	marker.state = FILESYSTEM_STARTUP_RTC_IN_PROGRESS;
	expect(!filesystemStartupMarkerWasInterrupted(&marker), "state_alone_does_not_force_recovery");

	marker = filesystemStartupMarkerInProgress();
	expect(filesystemStartupMarkerWasInterrupted(&marker), "exact_in_progress_marker_forces_recovery");
	expect(!filesystemStartupMarkerWasInterrupted(nullptr), "null_marker_is_not_interrupted_startup");
	expect(
		FILESYSTEM_STARTUP_RTC_OFFSET_WORDS * 4U >= 128U &&
			FILESYSTEM_STARTUP_RTC_OFFSET_WORDS * 4U + sizeof(marker) <= 512U,
		"marker_avoids_eboot_command_area_and_fits_rtc_memory");

	std::cout << "All ESP filesystem startup policy tests passed\n";
	return EXIT_SUCCESS;
}
