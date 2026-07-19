#include "fleet_soak.h"

#include <cstdlib>
#include <iostream>
#include <set>
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
	std::set<std::string> eventPaths;
	std::set<std::string> mePaths;
	std::set<std::string> stagingPaths;
	for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
	{
		expect(fleetSoakEventIndex(FLEET_SOAK_EVENT_PATHS[index]) == static_cast<int>(index),
		       "reserved_event_has_stable_index");
		expect(fleetSoakIsReservedEventPath(FLEET_SOAK_EVENT_PATHS[index]),
		       "reserved_event_is_recognized");
		eventPaths.insert(FLEET_SOAK_EVENT_PATHS[index]);
		mePaths.insert(FLEET_SOAK_ME_PATHS[index]);
		stagingPaths.insert(FLEET_SOAK_STAGING_PATHS[index]);
	}
	expect(eventPaths.size() == FLEET_SOAK_EVENT_COUNT, "reserved_event_paths_are_unique");
	expect(mePaths.size() == FLEET_SOAK_EVENT_COUNT, "reserved_assignment_paths_are_unique");
	expect(stagingPaths.size() == FLEET_SOAK_EVENT_COUNT, "reserved_staging_paths_are_unique");
	expect(fleetSoakEventIndex("FS01-Sprint.event") == 0,
	       "leading_slash_is_not_required");
	expect(!fleetSoakIsReservedEventPath("FS01-Sprint.me"),
	       "assignment_file_is_not_deletable_event");
	expect(!fleetSoakIsReservedEventPath("FS01-Sprint.event.bak"),
	       "backup_file_is_not_deletable_event");
	expect(!fleetSoakIsReservedEventPath("Ordinary.event"),
	       "ordinary_event_is_outside_cleanup_scope");
	expect(fleetSoakAbortMaySuspend(false, "Ordinary.event"),
	       "idle_target_may_enter_explicit_abort");
	expect(fleetSoakAbortMaySuspend(true, "FS01-Sprint.event"),
	       "active_reserved_event_may_be_suspended");
	expect(!fleetSoakAbortMaySuspend(true, "Ordinary.event"),
	       "active_ordinary_event_cannot_be_suspended");
	expect(!fleetSoakAbortMaySuspend(true, nullptr),
	       "unidentified_active_event_cannot_be_suspended");

	for (int role = 0; role <= 3; role++)
	{
		for (int slot = 0; slot <= 4; slot++)
		{
			const std::string assignment = std::to_string(role) + ":" + std::to_string(slot);
			const bool expected = role <= 1 || slot == 0;
			expect(fleetSoakAssignmentIsValid(assignment.c_str()) == expected,
			       "assignment_range_is_enforced");
		}
	}
	expect(!fleetSoakAssignmentIsValid(""), "empty_assignment_is_rejected");
	expect(!fleetSoakAssignmentIsValid("0:5"), "out_of_range_slot_is_rejected");
	expect(!fleetSoakAssignmentIsValid("4:0"), "out_of_range_role_is_rejected");
	expect(!fleetSoakAssignmentIsValid("0:0,extra"), "assignment_suffix_is_rejected");

	std::cout << "All ESP Fleet Soak boundary tests passed\n";
	return 0;
}
