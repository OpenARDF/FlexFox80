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
 * Dependency-free retained marker for detecting an ESP reset during cloning.
 */

#pragma once

#include <stdint.h>

/* Eboot owns bytes 0-127. The filesystem startup marker occupies words 120-121.
 * This marker uses words 112-118 (bytes 448-475), leaving both regions separate. */
#define CLONE_RESET_RTC_OFFSET_WORDS 112U
#define CLONE_RESET_RTC_MAGIC 0x434C4E52UL
#define CLONE_RESET_RTC_VERSION 1UL

struct CloneResetMarker
{
  uint32_t magic;
  uint32_t version;
  uint32_t phase;
  uint32_t fileIndex;
  uint32_t fileCount;
  uint32_t active;
  uint32_t checksum;
};

static inline uint32_t cloneResetMarkerChecksum(const CloneResetMarker *marker)
{
  return marker
           ? marker->magic ^ marker->version ^ marker->phase ^ marker->fileIndex ^
             marker->fileCount ^ marker->active ^ 0xA55A3CC3UL
           : 0;
}
static inline CloneResetMarker cloneResetMarkerActive(
  uint32_t phase,
  uint32_t fileIndex,
  uint32_t fileCount)
{
  CloneResetMarker marker = {
    CLONE_RESET_RTC_MAGIC,
    CLONE_RESET_RTC_VERSION,
    phase,
    fileIndex,
    fileCount,
    1U,
    0U
  };
  marker.checksum = cloneResetMarkerChecksum(&marker);
  return marker;
}

static inline CloneResetMarker cloneResetMarkerClear(void)
{
  CloneResetMarker marker = {0, 0, 0, 0, 0, 0, 0};
  return marker;
}

static inline bool cloneResetMarkerWasInterrupted(const CloneResetMarker *marker)
{
  return marker &&
         marker->magic == CLONE_RESET_RTC_MAGIC &&
         marker->version == CLONE_RESET_RTC_VERSION &&
         marker->active == 1U &&
         marker->checksum == cloneResetMarkerChecksum(marker);
}
