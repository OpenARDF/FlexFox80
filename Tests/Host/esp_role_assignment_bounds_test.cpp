#include <cstdlib>
#include <iostream>
#include <string>

#include "role_assignment_bounds.h"

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

void valid_assignment_uses_the_complete_role_prefix()
{
	RoleAssignmentBounds bounds = {};
	std::string assignment;

	expect(roleAssignmentBounds("0:0", &bounds), "zero_role_assignment_is_valid");
	expect(bounds.roleBegin == 0 && bounds.roleEnd == 1 && bounds.slotBegin == 2,
		"zero_role_bounds_are_exact");
	assignment = "0:0";
	expect(
		assignment.substr(bounds.roleBegin, bounds.roleEnd - bounds.roleBegin) == "0",
		"zero_role_prefix_is_zero");

	expect(roleAssignmentBounds("1:0", &bounds), "one_digit_role_assignment_is_valid");
	expect(bounds.roleBegin == 0 && bounds.roleEnd == 1 && bounds.slotBegin == 2,
		"one_digit_role_keeps_its_digit");
	assignment = "1:0";
	expect(
		assignment.substr(bounds.roleBegin, bounds.roleEnd - bounds.roleBegin) == "1",
		"one_digit_role_prefix_is_one");

	expect(roleAssignmentBounds("10:2", &bounds), "two_digit_role_assignment_is_valid");
	expect(bounds.roleBegin == 0 && bounds.roleEnd == 2 && bounds.slotBegin == 3,
		"two_digit_role_keeps_both_digits");
	assignment = "10:2";
	expect(
		assignment.substr(bounds.roleBegin, bounds.roleEnd - bounds.roleBegin) == "10",
		"two_digit_role_prefix_is_ten");
}

void validation_preserves_the_existing_boundary()
{
	RoleAssignmentBounds bounds = {};

	expect(!roleAssignmentBounds(nullptr, &bounds), "null_assignment_is_rejected");
	expect(!roleAssignmentBounds("10", &bounds), "missing_colon_is_rejected");
	expect(!roleAssignmentBounds(":0", &bounds), "empty_role_is_rejected");
	expect(roleAssignmentBounds("1:", &bounds), "empty_slot_remains_accepted");
	expect(bounds.roleEnd == 1 && bounds.slotBegin == 2, "empty_slot_keeps_role_bounds");
	expect(roleAssignmentBounds("1:2:3", &bounds), "additional_colon_remains_accepted");
	expect(bounds.roleEnd == 1 && bounds.slotBegin == 2, "first_colon_defines_role_bounds");
}

} // namespace

int main()
{
	valid_assignment_uses_the_complete_role_prefix();
	validation_preserves_the_existing_boundary();

	std::cout << "All ESP role assignment bounds tests passed\n";
	return EXIT_SUCCESS;
}
