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
 * Dependency-free outcome tracking for one queued ESP-to-AVR Linkbus command.
 */

#pragma once

#include <stdint.h>

#define LINKBUS_COMMAND_LOCAL_DEADLINE_MILLIS 12000UL

enum LinkbusCommandResult
{
  LINKBUS_COMMAND_INVALID,
  LINKBUS_COMMAND_WAITING,
  LINKBUS_COMMAND_ACKNOWLEDGED,
  LINKBUS_COMMAND_NACKED,
  LINKBUS_COMMAND_ACK_TIMEOUT,
  LINKBUS_COMMAND_LOCAL_TIMEOUT
};

struct LinkbusCommandTransaction
{
  uint32_t startedMillis;
};

static inline LinkbusCommandTransaction linkbusCommandTransactionBegin(
  uint32_t nowMillis)
{
  LinkbusCommandTransaction transaction = {nowMillis};
  return transaction;
}

static inline LinkbusCommandResult linkbusCommandTransactionObserve(
  const LinkbusCommandTransaction *transaction,
  uint32_t nowMillis,
  bool queueEmpty,
  bool ackPending,
  bool ackTimeoutOccurred,
  int nacksReceived)
{
  if (!transaction)
  {
    return LINKBUS_COMMAND_INVALID;
  }

  if (nacksReceived > 0)
  {
    return LINKBUS_COMMAND_NACKED;
  }

  /* A retry flag is conclusive only after the pending command has finished. */
  if (!ackPending && ackTimeoutOccurred)
  {
    return LINKBUS_COMMAND_ACK_TIMEOUT;
  }

  if (queueEmpty && !ackPending)
  {
    return LINKBUS_COMMAND_ACKNOWLEDGED;
  }

  if ((uint32_t)(nowMillis - transaction->startedMillis) >=
      LINKBUS_COMMAND_LOCAL_DEADLINE_MILLIS)
  {
    return LINKBUS_COMMAND_LOCAL_TIMEOUT;
  }

  return LINKBUS_COMMAND_WAITING;
}
