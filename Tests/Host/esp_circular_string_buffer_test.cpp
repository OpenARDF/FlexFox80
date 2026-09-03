#include <Arduino.h>
#include "CircularStringBuff.h"

#include <cstdlib>
#include <iostream>

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
	CircularStringBuff buffer(3);
	expect(buffer.empty() && buffer.capacity() == 3, "buffer_starts_empty");
	buffer.put(String("alpha"));
	buffer.put(String("bravo"));
	expect(buffer.size() == 2, "buffer_tracks_items");
	String first = buffer.get();
	expect(first == "alpha", "buffer_preserves_fifo_order");
	String second = buffer.get();
	expect(second == "bravo" && buffer.empty(), "consumed_items_leave_buffer_empty");
	buffer.put(String("charlie"));
	buffer.reset();
	expect(buffer.empty(), "reset_discards_owned_strings");

	std::cout << "All ESP circular string buffer tests passed\n";
	return 0;
}
