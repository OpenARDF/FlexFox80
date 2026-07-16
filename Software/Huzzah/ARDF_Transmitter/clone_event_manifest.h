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
 * Dependency-free manifest tracking for opt-in clone event reconciliation.
 */

#pragma once

#include <stddef.h>

#define CLONE_EVENT_MANIFEST_CAPACITY 20
#define CLONE_EVENT_PATH_CAPACITY 64

struct CloneEventManifest
{
  char paths[CLONE_EVENT_MANIFEST_CAPACITY][CLONE_EVENT_PATH_CAPACITY];
  size_t count;
  bool invalid;
};

static inline void cloneEventManifestReset(CloneEventManifest *manifest)
{
  if (!manifest)
  {
    return;
  }

  manifest->count = 0;
  manifest->invalid = false;
  for (size_t index = 0; index < CLONE_EVENT_MANIFEST_CAPACITY; index++)
  {
    manifest->paths[index][0] = '\0';
  }
}

static inline const char *cloneEventCanonicalPath(const char *path)
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

static inline bool cloneEventPathIsEventFile(const char *path)
{
  static const char suffix[] = ".event";
  const char *canonical = cloneEventCanonicalPath(path);
  if (!canonical)
  {
    return false;
  }

  size_t length = 0;
  while (canonical[length] != '\0')
  {
    length++;
  }
  if (length <= sizeof(suffix) - 1)
  {
    return false;
  }

  const size_t suffixBegin = length - (sizeof(suffix) - 1);
  for (size_t index = 0; index < sizeof(suffix) - 1; index++)
  {
    if (canonical[suffixBegin + index] != suffix[index])
    {
      return false;
    }
  }
  return true;
}

static inline bool cloneEventPathsEqual(const char *left, const char *right)
{
  left = cloneEventCanonicalPath(left);
  right = cloneEventCanonicalPath(right);
  if (!left || !right)
  {
    return false;
  }

  while (*left != '\0' && *right != '\0')
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

static inline bool cloneEventManifestContains(
  const CloneEventManifest *manifest,
  const char *path)
{
  if (!manifest || manifest->invalid || !cloneEventPathIsEventFile(path))
  {
    return false;
  }

  for (size_t index = 0; index < manifest->count; index++)
  {
    if (cloneEventPathsEqual(manifest->paths[index], path))
    {
      return true;
    }
  }
  return false;
}

static inline bool cloneEventManifestRecord(CloneEventManifest *manifest, const char *path)
{
  if (!manifest || manifest->invalid || !cloneEventPathIsEventFile(path))
  {
    if (manifest)
    {
      manifest->invalid = true;
    }
    return false;
  }
  if (cloneEventManifestContains(manifest, path))
  {
    return true;
  }
  if (manifest->count >= CLONE_EVENT_MANIFEST_CAPACITY)
  {
    manifest->invalid = true;
    return false;
  }

  const char *canonical = cloneEventCanonicalPath(path);
  size_t length = 0;
  while (canonical[length] != '\0')
  {
    if (length + 1 >= CLONE_EVENT_PATH_CAPACITY)
    {
      manifest->invalid = true;
      return false;
    }
    length++;
  }

  for (size_t index = 0; index <= length; index++)
  {
    manifest->paths[manifest->count][index] = canonical[index];
  }
  manifest->count++;
  return true;
}
