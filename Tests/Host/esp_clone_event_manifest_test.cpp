#include "clone_event_manifest.h"

#include <cstdlib>
#include <iostream>
#include <string>

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
	expect(CLONE_EVENT_MANIFEST_CAPACITY == 25, "manifest_supports_25_event_files");
	CloneEventManifest manifest;
	cloneEventManifestReset(&manifest);
	expect(manifest.count == 0 && !manifest.invalid, "manifest_starts_empty_and_valid");
	expect(cloneEventPathIsEventFile("Sprint80m-3.event"), "event_suffix_is_accepted");
	expect(cloneEventPathIsEventFile("/Sprint80m-3.event"), "leading_slash_is_normalized");
	expect(!cloneEventPathIsEventFile("Sprint80m-3.me"), "me_assignment_file_is_never_an_event");
	expect(!cloneEventPathIsEventFile("Sprint80m-3.event.bak"), "event_suffix_must_be_terminal");

	expect(cloneEventManifestRecord(&manifest, "/Sprint80m-3.event"), "valid_event_is_recorded");
	expect(manifest.count == 1, "recorded_event_increments_count");
	expect(cloneEventManifestContains(&manifest, "Sprint80m-3.event"), "canonical_event_is_found");
	expect(cloneEventManifestRecord(&manifest, "Sprint80m-3.event"), "duplicate_event_is_accepted");
	expect(manifest.count == 1, "duplicate_event_is_not_counted_twice");
	expect(
		cloneEventPathIsEventFile("Classic80m-Set1-1.event") &&
		!cloneEventManifestContains(&manifest, "Classic80m-Set1-1.event"),
		"target_only_event_is_selected_for_cleanup");
	expect(
		!cloneEventPathIsEventFile("Classic80m-1.me"),
		"target_assignment_file_is_excluded_from_cleanup");

	cloneEventManifestReset(&manifest);
	bool filledManifest = true;
	for (size_t index = 0; index < CLONE_EVENT_MANIFEST_CAPACITY; index++)
	{
		const std::string path = "Event-" + std::to_string(index) + ".event";
		filledManifest = cloneEventManifestRecord(&manifest, path.c_str()) && filledManifest;
	}
	expect(filledManifest, "manifest_accepts_every_supported_event");
	expect(
		!cloneEventManifestRecord(&manifest, "OneTooMany.event") && manifest.invalid,
		"manifest_overflow_fails_closed");
	expect(
		!cloneEventManifestContains(&manifest, "Event-0.event"),
		"invalid_manifest_cannot_authorize_deletion");

	cloneEventManifestReset(&manifest);
	expect(!cloneEventManifestRecord(&manifest, "Classic80m-1.me"), "me_file_cannot_enter_manifest");
	expect(manifest.invalid, "non_event_manifest_entry_fails_closed");

	cloneEventManifestReset(&manifest);
	const std::string tooLong(CLONE_EVENT_PATH_CAPACITY, 'x');
	expect(
		!cloneEventManifestRecord(&manifest, (tooLong + ".event").c_str()),
		"overlong_event_path_is_rejected");
	expect(manifest.invalid, "overlong_event_path_invalidates_manifest");

	std::cout << "All ESP clone event manifest tests passed\n";
	return 0;
}
