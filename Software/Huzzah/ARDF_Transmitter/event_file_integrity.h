#pragma once

#include <stddef.h>
#include <stdint.h>

struct EventFileIntegrityState
{
  uint32_t checksum;
  bool startSeen;
  bool endSeen;
  bool checksumSeen;
  bool checksumMatched;
  bool invalid;
};

static inline EventFileIntegrityState eventFileIntegrityInitialState(void)
{
  EventFileIntegrityState state = {0, false, false, false, false, false};
  return state;
}

static inline bool eventFileIntegrityLineEquals(
  const char *line,
  size_t length,
  const char *expected,
  size_t expectedLength)
{
  if (!line || !expected || length != expectedLength)
  {
    return false;
  }

  for (size_t index = 0; index < length; index++)
  {
    if (line[index] != expected[index])
    {
      return false;
    }
  }
  return true;
}

static inline bool eventFileIntegrityAddLength(EventFileIntegrityState *state, size_t length)
{
  if (length > UINT32_MAX || state->checksum > UINT32_MAX - (uint32_t)length)
  {
    state->invalid = true;
    return false;
  }
  state->checksum += (uint32_t)length;
  return true;
}

static inline void eventFileIntegrityObserveLine(
  EventFileIntegrityState *state,
  const char *line,
  size_t length)
{
  static const char startLine[] = "EVENT_START";
  static const char endLine[] = "EVENT_END";
  static const char checksumPrefix[] = "CHECK,";

  if (!state)
  {
    return;
  }
  if (!line)
  {
    state->invalid = true;
    return;
  }
  if (state->invalid)
  {
    return;
  }

  if (!state->startSeen)
  {
    if (eventFileIntegrityLineEquals(line, length, startLine, sizeof(startLine) - 1))
    {
      state->startSeen = true;
      eventFileIntegrityAddLength(state, length);
    }
    return;
  }

  if (!state->endSeen)
  {
    if (!eventFileIntegrityAddLength(state, length))
    {
      return;
    }
    state->endSeen = eventFileIntegrityLineEquals(line, length, endLine, sizeof(endLine) - 1);
    return;
  }

  if (length < sizeof(checksumPrefix) - 1 ||
      !eventFileIntegrityLineEquals(
        line,
        sizeof(checksumPrefix) - 1,
        checksumPrefix,
        sizeof(checksumPrefix) - 1))
  {
    return;
  }

  if (state->checksumSeen)
  {
    state->invalid = true;
    return;
  }
  state->checksumSeen = true;

  const size_t valueBegin = sizeof(checksumPrefix) - 1;
  if (valueBegin == length)
  {
    state->invalid = true;
    return;
  }

  uint32_t received = 0;
  for (size_t index = valueBegin; index < length; index++)
  {
    const char character = line[index];
    if (character < '0' || character > '9')
    {
      state->invalid = true;
      return;
    }
    const uint32_t digit = (uint32_t)(character - '0');
    if (received > (UINT32_MAX - digit) / 10U)
    {
      state->invalid = true;
      return;
    }
    received = received * 10U + digit;
  }

  state->checksumMatched = received == state->checksum;
}

static inline bool eventFileIntegrityValid(
  const EventFileIntegrityState *state,
  bool requireChecksum)
{
  if (!state || state->invalid || !state->startSeen || !state->endSeen)
  {
    return false;
  }
  if (state->checksumSeen && !state->checksumMatched)
  {
    return false;
  }
  return !requireChecksum || state->checksumSeen;
}
