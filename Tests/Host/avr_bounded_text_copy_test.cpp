#include "bounded_text_copy.h"

#include <cstdio>
#include <cstring>

namespace
{
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
	{
		char destination[16] = {};
		expect(
			copy_text_to_buffer(destination, sizeof(destination), "%s%n") &&
				std::strcmp(destination, "%s%n") == 0,
			"percent_characters_are_copied_as_text");
	}

	{
		char destination[5] = {};
		expect(
			copy_text_to_buffer(destination, sizeof(destination), "1234") &&
				std::strcmp(destination, "1234") == 0,
			"maximum_length_text_is_null_terminated");
	}

	{
		char destination[5] = "safe";
		expect(
			!copy_text_to_buffer(destination, sizeof(destination), "12345") &&
				std::strcmp(destination, "safe") == 0,
			"oversized_text_is_rejected_without_partial_frame");
	}

	{
		char destination[5] = "safe";
		expect(
			!copy_text_to_buffer(nullptr, sizeof(destination), "text") &&
				!copy_text_to_buffer(destination, 0, "text") &&
				!copy_text_to_buffer(destination, sizeof(destination), nullptr),
			"invalid_copy_arguments_are_rejected");
	}

	if(failures)
	{
		std::fprintf(stderr, "%d bounded text copy test(s) failed\n", failures);
		return 1;
	}

	std::puts("All AVR bounded text copy tests passed");
	return 0;
}
