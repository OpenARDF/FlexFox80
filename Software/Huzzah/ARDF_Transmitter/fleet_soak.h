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
 * Dependency-free Fleet Soak naming and assignment boundaries.
 *
 * The names in this file are a permanent reserved namespace. Cleanup code must
 * iterate this allowlist; it must never accept a browser-supplied filename.
 */

#pragma once

#include <stddef.h>

#define FLEET_SOAK_PROTOCOL_VERSION 1
#define FLEET_SOAK_EVENT_COUNT 12
#define FLEET_SOAK_PAGE_PATH "/fleet-soak.html"
#define FLEET_SOAK_READY_PATH "/fleet-soak.ready"
#define FLEET_SOAK_READY_VALUE "fleet-soak-v1-ready"
#define FLEET_SOAK_EVENT_VERSION "fleet-soak-v1"

enum FleetSoakMode
{
  FLEET_SOAK_MODE_OFF = 0,
  FLEET_SOAK_MODE_PROVISION,
  FLEET_SOAK_MODE_CLEANUP
};

static const char *const FLEET_SOAK_EVENT_PATHS[FLEET_SOAK_EVENT_COUNT] = {
  "/FS01-Sprint.event",
  "/FS02-Classic.event",
  "/FS03-Sprint.event",
  "/FS04-Classic.event",
  "/FS05-Sprint.event",
  "/FS06-Classic.event",
  "/FS07-Sprint.event",
  "/FS08-Classic.event",
  "/FS09-Sprint.event",
  "/FS10-Classic.event",
  "/FS11-Sprint.event",
  "/FS12-Classic.event"
};

static const char *const FLEET_SOAK_ME_PATHS[FLEET_SOAK_EVENT_COUNT] = {
  "/FS01-Sprint.me",
  "/FS02-Classic.me",
  "/FS03-Sprint.me",
  "/FS04-Classic.me",
  "/FS05-Sprint.me",
  "/FS06-Classic.me",
  "/FS07-Sprint.me",
  "/FS08-Classic.me",
  "/FS09-Sprint.me",
  "/FS10-Classic.me",
  "/FS11-Sprint.me",
  "/FS12-Classic.me"
};

static const char *const FLEET_SOAK_STAGING_PATHS[FLEET_SOAK_EVENT_COUNT] = {
  "/.fleet-soak-01.tmp",
  "/.fleet-soak-02.tmp",
  "/.fleet-soak-03.tmp",
  "/.fleet-soak-04.tmp",
  "/.fleet-soak-05.tmp",
  "/.fleet-soak-06.tmp",
  "/.fleet-soak-07.tmp",
  "/.fleet-soak-08.tmp",
  "/.fleet-soak-09.tmp",
  "/.fleet-soak-10.tmp",
  "/.fleet-soak-11.tmp",
  "/.fleet-soak-12.tmp"
};

static inline const char *fleetSoakCanonicalPath(const char *path)
{
  if (!path)
  {
    return NULL;
  }
  while (*path == '/')
  {
    path++;
  }
  return path;
}

static inline bool fleetSoakPathsEqual(const char *left, const char *right)
{
  left = fleetSoakCanonicalPath(left);
  right = fleetSoakCanonicalPath(right);
  if (!left || !right)
  {
    return false;
  }
  while (*left && *right)
  {
    if (*left != *right)
    {
      return false;
    }
    left++;
    right++;
  }
  return *left == *right;
}

static inline int fleetSoakEventIndex(const char *path)
{
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (fleetSoakPathsEqual(path, FLEET_SOAK_EVENT_PATHS[index]))
    {
      return (int)index;
    }
  }
  return -1;
}

static inline bool fleetSoakIsReservedEventPath(const char *path)
{
  return fleetSoakEventIndex(path) >= 0;
}

static inline bool fleetSoakAssignmentIsValid(const char *assignment)
{
  if (!assignment || assignment[0] < '0' || assignment[0] > '3' ||
      assignment[1] != ':' || assignment[2] < '0' || assignment[2] > '4' ||
      assignment[3] != '\0')
  {
    return false;
  }

  const int role = assignment[0] - '0';
  const int slot = assignment[2] - '0';
  return (role <= 1 && slot <= 4) || (role >= 2 && slot == 0);
}
