#include "linkbus_rx_bounds.h"

#include <cstdio>

namespace
{
constexpr uint8_t maximum_fields = 3;
constexpr uint8_t field_capacity = 21;
int failures = 0;

void expect(bool condition, const char* test_name)
{
	if(condition)
	{
		std::printf("PASS %s\n", test_name);
	}
	else
	{
		std::fprintf(stderr, "FAIL %s\n", test_name);
		failures++;
	}
}
}

int main()
{
	const uint32_t key_id = linkbus_rx_id_append(
		linkbus_rx_id_append(linkbus_rx_id_append(0, 'K'), 'E'), 'Y');
	const uint32_t key_alias = linkbus_rx_id_append(
		linkbus_rx_id_append(linkbus_rx_id_append(0, 'I'), 'Y'), 'Y');
	const uint32_t reset_id = linkbus_rx_id_append(
		linkbus_rx_id_append(linkbus_rx_id_append(0, 'R'), 'S'), 'T');
	const uint32_t reset_alias = linkbus_rx_id_append(
		linkbus_rx_id_append(linkbus_rx_id_append(0, 'R'), 'T'), 'J');
	const uint32_t go_id = linkbus_rx_id_append(linkbus_rx_id_append(0, 'G'), 'O');
	const uint32_t go_alias = linkbus_rx_id_append(linkbus_rx_id_append(0, 'F'), 'Y');
	const uint32_t temperature_id = linkbus_rx_id_append(
		linkbus_rx_id_append(linkbus_rx_id_append(0, 'T'), 'E'), 'M');
	const uint32_t temperature_alias = linkbus_rx_id_append(
		linkbus_rx_id_append(linkbus_rx_id_append(0, 'R'), 'X'), 'W');

	expect(
		key_id == 0x4B4559 && reset_id == 0x525354 && go_id == 0x474F,
		"message_ids_preserve_wire_characters");

	expect(
		key_id != key_alias && reset_id != reset_alias && go_id != go_alias &&
			temperature_id != temperature_alias,
		"legacy_decimal_aliases_are_distinct");

	expect(
		linkbus_rx_id_can_append(0, 3) && linkbus_rx_id_can_append(2, 3) &&
			!linkbus_rx_id_can_append(3, 3),
		"message_id_is_limited_to_three_characters");

	expect(
		linkbus_rx_field_can_append(1, 19, maximum_fields, field_capacity) &&
			linkbus_rx_field_can_append(3, 19, maximum_fields, field_capacity),
		"last_payload_byte_fits_first_and_last_fields");

	expect(
		!linkbus_rx_field_can_append(1, 20, maximum_fields, field_capacity),
		"field_reserves_space_for_null_terminator");

	expect(
		!linkbus_rx_field_can_append(0, 0, maximum_fields, field_capacity) &&
			!linkbus_rx_field_can_append(4, 0, maximum_fields, field_capacity),
		"append_rejects_message_id_and_excess_field_indices");

	expect(
		linkbus_rx_field_can_terminate(3, 20, maximum_fields, field_capacity) &&
			!linkbus_rx_field_can_terminate(3, 21, maximum_fields, field_capacity),
		"termination_stays_within_last_field");

	expect(
		linkbus_rx_can_start_next_field(2, maximum_fields) &&
			!linkbus_rx_can_start_next_field(3, maximum_fields),
		"fourth_field_is_rejected");

	expect(
		!linkbus_rx_field_can_append(1, 0, maximum_fields, 0) &&
			!linkbus_rx_field_can_terminate(1, 0, maximum_fields, 0),
		"zero_capacity_is_rejected");

	if(failures)
	{
		std::fprintf(stderr, "%d Linkbus receive bounds test(s) failed\n", failures);
		return 1;
	}

	std::puts("All AVR Linkbus receive bounds tests passed");
	return 0;
}
