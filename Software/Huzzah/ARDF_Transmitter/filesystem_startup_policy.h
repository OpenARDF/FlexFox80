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
 * Dependency-free ESP filesystem startup marker policy shared with host tests.
 */

#ifndef FILESYSTEM_STARTUP_POLICY_H_INCLUDED
#define FILESYSTEM_STARTUP_POLICY_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#ifndef FLEXFOX_FILESYSTEM_RECOVERY_QUALIFICATION
#define FLEXFOX_FILESYSTEM_RECOVERY_QUALIFICATION 0
#endif

/* Eboot reserves the first 128 bytes of RTC user memory for update commands.
 * This marker occupies the final eight bytes of the separate 512-byte area. */
#define FILESYSTEM_STARTUP_RTC_OFFSET_WORDS 120U
#define FILESYSTEM_STARTUP_RTC_MAGIC 0x46534653UL
#define FILESYSTEM_STARTUP_RTC_IN_PROGRESS 0x53544152UL

typedef struct filesystemStartupMarker
{
	uint32_t magic;
	uint32_t state;
} FilesystemStartupMarker;

static inline FilesystemStartupMarker filesystemStartupMarkerInProgress(void)
{
	FilesystemStartupMarker marker = {
		FILESYSTEM_STARTUP_RTC_MAGIC,
		FILESYSTEM_STARTUP_RTC_IN_PROGRESS,
	};
	return marker;
}

static inline FilesystemStartupMarker filesystemStartupMarkerClear(void)
{
	FilesystemStartupMarker marker = {0, 0};
	return marker;
}

static inline bool filesystemStartupMarkerWasInterrupted(
	const FilesystemStartupMarker *marker)
{
	return marker &&
		marker->magic == FILESYSTEM_STARTUP_RTC_MAGIC &&
		marker->state == FILESYSTEM_STARTUP_RTC_IN_PROGRESS;
}

#endif /* FILESYSTEM_STARTUP_POLICY_H_INCLUDED */
