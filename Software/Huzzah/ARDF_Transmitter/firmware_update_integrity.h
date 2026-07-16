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
 * Dependency-light validation helpers for transactional sketch updates.
 */

#ifndef FIRMWARE_UPDATE_INTEGRITY_H_
#define FIRMWARE_UPDATE_INTEGRITY_H_

#include <stddef.h>
#include <stdint.h>

static inline bool firmwareUpdateParseSize(
  const char *text,
  size_t maximum,
  size_t *parsed)
{
  if (!text || !text[0] || !parsed)
  {
    return false;
  }

  size_t value = 0;
  for (const char *cursor = text; *cursor; ++cursor)
  {
    if (*cursor < '0' || *cursor > '9')
    {
      return false;
    }
    const size_t digit = (size_t)(*cursor - '0');
    if (value > maximum / 10 ||
        (value == maximum / 10 && digit > maximum % 10))
    {
      return false;
    }
    value = (value * 10) + digit;
  }

  if (!value || value > maximum)
  {
    return false;
  }
  *parsed = value;
  return true;
}

static inline bool firmwareUpdateParseCrc32(const char *text, uint32_t *parsed)
{
  if (!text || !parsed)
  {
    return false;
  }

  uint32_t value = 0;
  size_t length = 0;
  for (const char *cursor = text; *cursor; ++cursor)
  {
    uint8_t nibble = 0;
    if (*cursor >= '0' && *cursor <= '9')
    {
      nibble = (uint8_t)(*cursor - '0');
    }
    else if (*cursor >= 'a' && *cursor <= 'f')
    {
      nibble = (uint8_t)(10 + *cursor - 'a');
    }
    else if (*cursor >= 'A' && *cursor <= 'F')
    {
      nibble = (uint8_t)(10 + *cursor - 'A');
    }
    else
    {
      return false;
    }
    if (++length > 8)
    {
      return false;
    }
    value = (value << 4) | nibble;
  }

  if (length != 8)
  {
    return false;
  }
  *parsed = value;
  return true;
}

static inline uint32_t firmwareUpdateCrc32(
  uint32_t crc,
  const uint8_t *data,
  size_t length)
{
  for (size_t index = 0; index < length; ++index)
  {
    crc ^= data[index];
    for (uint8_t bit = 0; bit < 8; ++bit)
    {
      crc = (crc >> 1) ^ (0xedb88320UL & (uint32_t)-(int32_t)(crc & 1));
    }
  }
  return crc;
}

static inline bool firmwareUpdateHasBinExtension(const char *filename)
{
  if (!filename)
  {
    return false;
  }
  size_t length = 0;
  while (filename[length])
  {
    ++length;
  }
  if (length < 5)
  {
    return false;
  }
  const char *extension = filename + length - 4;
  return extension[0] == '.' &&
         (extension[1] == 'b' || extension[1] == 'B') &&
         (extension[2] == 'i' || extension[2] == 'I') &&
         (extension[3] == 'n' || extension[3] == 'N');
}

#endif // FIRMWARE_UPDATE_INTEGRITY_H_
