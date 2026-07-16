#include "event_data_read_guard.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{
constexpr int roleCapacity = 2;
constexpr int txCapacity = 2;

struct Tx
{
	std::string pattern;
	std::string onTime;
	std::string offTime;
	std::string delayTime;
};

struct Role
{
	std::string rolename;
	int numberOfTxs;
	long frequency;
	int powerLevel_mW;
	int code_speed;
	int id_interval;
	Tx *tx[txCapacity];
};

struct EventData
{
	std::string tx_assignment;
	std::string tx_role_name;
	std::string tx_role_pwr;
	std::string tx_role_freq;
	bool tx_assignment_is_default;
	std::string event_name;
	std::string event_file_version;
	std::string event_band;
	std::string event_antenna_port;
	std::string event_callsign;
	std::string event_callsign_speed;
	std::string event_start_date_time;
	std::string event_finish_date_time;
	std::string event_modulation;
	int event_number_of_tx_types;
	Role *role[roleCapacity];
};

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
	Tx tx00 = {"MO", "60", "240", "0"};
	Tx tx01 = {"MOE", "60", "240", "60"};
	Role role0 = {"Finish", 2, 3510000, 1000, 15, 60, {&tx00, &tx01}};
	EventData event = {
		"3:0",
		"Finish - MO",
		"1000",
		"3510000",
		false,
		"Classic 80m",
		"Set 1-1",
		"80m",
		"80_0",
		"MO",
		"15",
		"2026-07-16T12:00:00Z",
		"2026-07-16T16:00:00Z",
		"CW",
		1,
		{&role0, nullptr},
	};

	resetEventDataForRead(&event, roleCapacity, txCapacity);
	expect(event.event_name.empty(), "prior_event_name_is_cleared");
	expect(event.event_file_version.empty(), "prior_internal_set_label_is_cleared");
	expect(event.tx_assignment.empty(), "prior_assignment_is_cleared_before_me_file_read");
	expect(event.event_number_of_tx_types == -1, "missing_role_count_cannot_reuse_prior_count");
	expect(role0.rolename.empty() && role0.numberOfTxs == 0, "prior_role_metadata_is_cleared");
	expect(tx00.pattern.empty() && tx01.pattern.empty(), "prior_transmit_patterns_are_cleared");
	expect(eventReadCountWithinBounds(1, roleCapacity), "positive_in_range_count_is_accepted");
	expect(!eventReadCountWithinBounds(0, roleCapacity), "zero_count_is_rejected");
	expect(!eventReadCountWithinBounds(roleCapacity + 1, roleCapacity), "oversized_count_is_rejected");

	resetEventDataForRead<EventData>(nullptr, roleCapacity, txCapacity);
	expect(!eventReadCountWithinBounds(1, 0), "zero_capacity_is_rejected");

	std::cout << "All ESP event-data read guard tests passed\n";
	return 0;
}
