#include "event_file_integrity.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

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

std::uint32_t transferredChecksum(const std::vector<std::string> &lines)
{
	std::uint32_t checksum = 0;
	bool started = false;
	for (const std::string &line : lines)
	{
		if (!started)
		{
			started = line == "EVENT_START";
			if (!started)
			{
				continue;
			}
		}
		checksum += static_cast<std::uint32_t>(line.size());
		if (line == "EVENT_END")
		{
			break;
		}
	}
	return checksum;
}

EventFileIntegrityState observe(const std::vector<std::string> &lines)
{
	EventFileIntegrityState state = eventFileIntegrityInitialState();
	for (const std::string &line : lines)
	{
		eventFileIntegrityObserveLine(&state, line.c_str(), line.size());
	}
	return state;
}
} // namespace

int main()
{
	const std::vector<std::string> legacy = {
		"EVENT_START",
		"EVENT_NAME,Classic 80m",
		"EVENT_END",
	};

	EventFileIntegrityState state = observe(legacy);
	expect(eventFileIntegrityValid(&state, false), "legacy_file_without_checksum_remains_valid");
	expect(!eventFileIntegrityValid(&state, true), "clone_file_requires_checksum");

	std::vector<std::string> transferred = {
		"FILENAME,/Classic80m.event",
		"EVENT_START",
		"EVENT_NAME,Classic 80m",
		"EVENT_END",
	};
	transferred.push_back("CHECK," + std::to_string(transferredChecksum(transferred)));
	state = observe(transferred);
	expect(eventFileIntegrityValid(&state, true), "matching_transfer_checksum_is_valid");
	expect(state.checksumSeen, "checksum_after_event_end_is_observed");

	std::vector<std::string> changedLength = transferred;
	changedLength[2] += "X";
	state = observe(changedLength);
	expect(!eventFileIntegrityValid(&state, true), "changed_line_length_fails_checksum");

	std::vector<std::string> malformed = transferred;
	malformed.back() = "CHECK,12x";
	state = observe(malformed);
	expect(!eventFileIntegrityValid(&state, true), "malformed_checksum_is_rejected");

	std::vector<std::string> missingEnd = {
		"EVENT_START",
		"EVENT_NAME,Classic 80m",
		"CHECK,42",
	};
	state = observe(missingEnd);
	expect(!eventFileIntegrityValid(&state, false), "missing_event_end_is_rejected");

	std::vector<std::string> checksumBeforeEnd = {
		"EVENT_START",
		"CHECK,0",
		"EVENT_END",
	};
	state = observe(checksumBeforeEnd);
	expect(!eventFileIntegrityValid(&state, true), "checksum_before_event_end_does_not_qualify_clone");

	std::vector<std::string> duplicate = transferred;
	duplicate.push_back(transferred.back());
	state = observe(duplicate);
	expect(!eventFileIntegrityValid(&state, true), "duplicate_checksum_is_rejected");

	state = eventFileIntegrityInitialState();
	eventFileIntegrityObserveLine(nullptr, "EVENT_START", 11);
	eventFileIntegrityObserveLine(&state, nullptr, 0);
	expect(!eventFileIntegrityValid(nullptr, false), "null_state_is_rejected");
	expect(!eventFileIntegrityValid(&state, false), "null_line_does_not_create_valid_state");

	std::cout << "All ESP event-file integrity tests passed\n";
	return 0;
}
