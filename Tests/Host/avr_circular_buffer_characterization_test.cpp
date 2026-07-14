#include "CircularStringBuff.h"

#include <cstddef>
#include <iostream>
#include <limits>

namespace {

int failures = 0;

void expect(bool condition, const char *description, const char *file, int line)
{
  if (!condition)
  {
    std::cerr << file << ':' << line << ": expectation failed: " << description << '\n';
    ++failures;
  }
}

#define EXPECT(condition) expect((condition), #condition, __FILE__, __LINE__)

void initial_state_is_empty()
{
  CircularStringBuff buffer(4);

  EXPECT(buffer.capacity() == 4);
  EXPECT(buffer.size() == 0);
  EXPECT(buffer.empty());
  EXPECT(!buffer.full());
  EXPECT(!buffer.isBusy());
  EXPECT(buffer.get() == '\0');
}

void fifo_storage_is_uppercased()
{
  CircularStringBuff buffer(4);

  buffer.put('a');
  buffer.put('b');

  EXPECT(buffer.size() == 2);
  EXPECT(buffer.get() == 'A');
  EXPECT(buffer.get() == 'B');
  EXPECT(buffer.empty());
}

void full_buffer_overwrites_oldest_entry()
{
  CircularStringBuff buffer(3);

  buffer.put('a');
  buffer.put('b');
  buffer.put('c');

  EXPECT(buffer.full());
  EXPECT(buffer.size() == 3);

  buffer.put('d');

  EXPECT(buffer.full());
  EXPECT(buffer.size() == 3);
  EXPECT(buffer.get() == 'B');
  EXPECT(buffer.get() == 'C');
  EXPECT(buffer.get() == 'D');
  EXPECT(buffer.empty());
}

void indices_wrap_without_changing_fifo_order()
{
  CircularStringBuff buffer(3);

  buffer.put('a');
  buffer.put('b');
  EXPECT(buffer.get() == 'A');

  buffer.put('c');
  EXPECT(buffer.get() == 'B');

  buffer.put('d');
  EXPECT(buffer.get() == 'C');
  EXPECT(buffer.get() == 'D');
  EXPECT(buffer.empty());
}

void reset_clears_data_and_busy_state()
{
  CircularStringBuff buffer(3);

  buffer.put('a');
  buffer.put('b');
  buffer.setBusy(true);
  EXPECT(buffer.isBusy());

  buffer.reset();

  EXPECT(buffer.capacity() == 3);
  EXPECT(buffer.size() == 0);
  EXPECT(buffer.empty());
  EXPECT(!buffer.full());
  EXPECT(!buffer.isBusy());
  EXPECT(buffer.get() == '\0');
}

void pop_returns_entries_in_lifo_order()
{
  CircularStringBuff buffer(4);

  buffer.put('a');
  buffer.put('b');
  buffer.put('c');

  EXPECT(buffer.pop() == 'C');
  EXPECT(buffer.pop() == 'B');
  EXPECT(buffer.pop() == 'A');
  EXPECT(buffer.pop() == '\0');
  EXPECT(buffer.empty());
}

void zero_capacity_buffer_rejects_input()
{
  CircularStringBuff buffer(0);

  EXPECT(buffer.capacity() == 0);
  EXPECT(buffer.size() == 0);
  EXPECT(buffer.empty());
  EXPECT(buffer.full());

  buffer.put('a');

  EXPECT(buffer.size() == 0);
  EXPECT(buffer.get() == '\0');
  EXPECT(buffer.pop() == '\0');
}

void allocation_failure_disables_buffer()
{
  CircularStringBuff buffer(std::numeric_limits<size_t>::max());

  EXPECT(buffer.capacity() == 0);
  EXPECT(buffer.size() == 0);
  EXPECT(buffer.empty());
  EXPECT(buffer.full());

  buffer.put('a');

  EXPECT(buffer.size() == 0);
  EXPECT(buffer.get() == '\0');
  EXPECT(buffer.pop() == '\0');
}

void run(const char *name, void (*test)())
{
  const int failuresBefore = failures;
  test();
  if (failures == failuresBefore)
  {
    std::cout << "PASS " << name << '\n';
  }
  else
  {
    std::cout << "FAIL " << name << '\n';
  }
}

} // namespace

int main()
{
  run("initial_state_is_empty", initial_state_is_empty);
  run("fifo_storage_is_uppercased", fifo_storage_is_uppercased);
  run("full_buffer_overwrites_oldest_entry", full_buffer_overwrites_oldest_entry);
  run("indices_wrap_without_changing_fifo_order", indices_wrap_without_changing_fifo_order);
  run("reset_clears_data_and_busy_state", reset_clears_data_and_busy_state);
  run("pop_returns_entries_in_lifo_order", pop_returns_entries_in_lifo_order);
  run("zero_capacity_buffer_rejects_input", zero_capacity_buffer_rejects_input);
  run("allocation_failure_disables_buffer", allocation_failure_disables_buffer);

  if (failures != 0)
  {
    std::cerr << failures << " expectation(s) failed\n";
    return 1;
  }

  std::cout << "All AVR circular buffer characterization tests passed\n";
  return 0;
}
