/*
 *  MIT License
 *
 *  Copyright (c) 2026 Digital Confections LLC
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*
 * Dependency-free schedule for the target ESP's clone-scoped AVR keep-alive.
 */

#pragma once

#include <stdint.h>

#define CLONE_KEEPALIVE_INTERVAL_MILLIS 20000UL

struct CloneKeepAliveSchedule
{
  uint32_t lastQueuedMillis;
  bool active;
  bool queueImmediately;
};

static inline void cloneKeepAliveBegin(CloneKeepAliveSchedule *schedule)
{
  if (!schedule)
  {
    return;
  }

  schedule->active = true;
  schedule->queueImmediately = true;
}

static inline bool cloneKeepAliveIsDue(
  const CloneKeepAliveSchedule *schedule,
  uint32_t nowMillis)
{
  return schedule && schedule->active &&
         (schedule->queueImmediately ||
          (uint32_t)(nowMillis - schedule->lastQueuedMillis) >=
            CLONE_KEEPALIVE_INTERVAL_MILLIS);
}

static inline void cloneKeepAliveWasQueued(
  CloneKeepAliveSchedule *schedule,
  uint32_t nowMillis)
{
  if (!schedule)
  {
    return;
  }

  schedule->lastQueuedMillis = nowMillis;
  schedule->queueImmediately = false;
}

static inline void cloneKeepAliveEnd(CloneKeepAliveSchedule *schedule)
{
  if (!schedule)
  {
    return;
  }

  schedule->active = false;
  schedule->queueImmediately = false;
}
