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
 * Dependency-free clone clock and master-progress policy shared with host tests.
 */

#pragma once

#include <stdint.h>

#define CLONE_CLOCK_MAX_FORWARD_READBACK_SECONDS 1U
#define MASTER_CLONE_PROGRESS_TIMEOUT_MILLIS 15000UL
#define CLONE_FILE_VALIDATION_TIMEOUT_MILLIS 5000UL

static inline bool cloneClockReadbackAccepted(uint32_t expected, uint32_t readback)
{
  return expected &&
         (readback == expected ||
          readback == (uint32_t)(expected + CLONE_CLOCK_MAX_FORWARD_READBACK_SECONDS));
}

static inline int32_t cloneClockReadbackDelta(uint32_t expected, uint32_t readback)
{
  return (int32_t)((int64_t)readback - (int64_t)expected);
}

static inline bool masterCloneProgressTimedOut(
  bool active,
  uint32_t lastProgressMillis,
  uint32_t nowMillis)
{
  return active && lastProgressMillis &&
         (uint32_t)(nowMillis - lastProgressMillis) > MASTER_CLONE_PROGRESS_TIMEOUT_MILLIS;
}
