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
 * Dependency-free state for the ESP-only clone diagnostic build.
 */

#pragma once

#include <stdint.h>

enum CloneDiagnosticPhase : uint8_t
{
  CLONE_DIAG_IDLE = 0,
  CLONE_DIAG_MASTER_ASSOCIATION,
  CLONE_DIAG_AVR_QUIET,
  CLONE_DIAG_MASTER_HANDSHAKE,
  CLONE_DIAG_CLOCK_READY,
  CLONE_DIAG_CLOCK_WRITE_READBACK,
  CLONE_DIAG_FILE_WAIT,
  CLONE_DIAG_FILE_RECEIVE,
  CLONE_DIAG_FILE_VALIDATE,
  CLONE_DIAG_FILE_VALIDATE_READ,
  CLONE_DIAG_FILE_INSTALL_REMOVE,
  CLONE_DIAG_FILE_INSTALL_RENAME,
  CLONE_DIAG_EVENT_PROGRAM,
  CLONE_DIAG_CLEANUP_RESUME,
  CLONE_DIAG_RELEASE,
  CLONE_DIAG_COMPLETE,
  CLONE_DIAG_FAILED
};

struct CloneDiagnosticState
{
  CloneDiagnosticPhase phase;
  CloneDiagnosticPhase failurePhase;
  uint32_t sessionStartedMillis;
  uint32_t phaseStartedMillis;
  uint32_t updatedMillis;
  uint16_t attempt;
  uint16_t fileIndex;
  uint16_t fileCount;
  uint32_t receivedLines;
  bool active;
  bool cleanupConfirmed;
  bool receiveBufferOverflow;
  bool temporaryFileReceptionActive;
  bool temporaryFileCleanupAttempted;
  bool temporaryFileCleanupConfirmed;
};

static inline const char *cloneDiagnosticPhaseText(CloneDiagnosticPhase phase)
{
  switch (phase)
  {
    case CLONE_DIAG_MASTER_ASSOCIATION: return "master-association";
    case CLONE_DIAG_AVR_QUIET: return "avr-quiet";
    case CLONE_DIAG_MASTER_HANDSHAKE: return "master-handshake";
    case CLONE_DIAG_CLOCK_READY: return "clock-ready";
    case CLONE_DIAG_CLOCK_WRITE_READBACK: return "clock-write-readback";
    case CLONE_DIAG_FILE_WAIT: return "file-wait";
    case CLONE_DIAG_FILE_RECEIVE: return "file-receive";
    case CLONE_DIAG_FILE_VALIDATE: return "file-validate";
    case CLONE_DIAG_FILE_VALIDATE_READ: return "file-validate-read";
    case CLONE_DIAG_FILE_INSTALL_REMOVE: return "file-install-remove";
    case CLONE_DIAG_FILE_INSTALL_RENAME: return "file-install-rename";
    case CLONE_DIAG_EVENT_PROGRAM: return "event-program";
    case CLONE_DIAG_CLEANUP_RESUME: return "cleanup-resume";
    case CLONE_DIAG_RELEASE: return "release";
    case CLONE_DIAG_COMPLETE: return "complete";
    case CLONE_DIAG_FAILED: return "failed";
    default: return "idle";
  }
}

static inline void cloneDiagnosticReset(CloneDiagnosticState *state)
{
  if (!state)
  {
    return;
  }
  *state = {
    CLONE_DIAG_IDLE,
    CLONE_DIAG_IDLE,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    false,
    false,
    false,
    false,
    false,
    false
  };
}

static inline void cloneDiagnosticBegin(CloneDiagnosticState *state, uint32_t nowMillis)
{
  if (!state)
  {
    return;
  }
  *state = {
    CLONE_DIAG_MASTER_ASSOCIATION,
    CLONE_DIAG_IDLE,
    nowMillis,
    nowMillis,
    nowMillis,
    0,
    0,
    0,
    0,
    true,
    false,
    false,
    false,
    false,
    false
  };
}

static inline void cloneDiagnosticTemporaryFileOpened(CloneDiagnosticState *state)
{
  if (!state)
  {
    return;
  }
  state->temporaryFileReceptionActive = true;
  state->temporaryFileCleanupAttempted = false;
  state->temporaryFileCleanupConfirmed = false;
}

static inline void cloneDiagnosticTemporaryFileCompleted(CloneDiagnosticState *state)
{
  if (!state)
  {
    return;
  }
  state->temporaryFileReceptionActive = false;
}

static inline bool cloneDiagnosticShouldRemoveIncompleteTemporaryFile(
  const CloneDiagnosticState *state)
{
  return state && state->temporaryFileReceptionActive;
}

static inline void cloneDiagnosticTemporaryFileCleanupFinished(
  CloneDiagnosticState *state,
  bool succeeded)
{
  if (!state)
  {
    return;
  }
  state->temporaryFileReceptionActive = false;
  state->temporaryFileCleanupAttempted = true;
  state->temporaryFileCleanupConfirmed = succeeded;
}

static inline void cloneDiagnosticSetPhase(
  CloneDiagnosticState *state,
  CloneDiagnosticPhase phase,
  uint32_t nowMillis)
{
  if (!state)
  {
    return;
  }
  state->phase = phase;
  state->phaseStartedMillis = nowMillis;
  state->updatedMillis = nowMillis;
}

static inline void cloneDiagnosticFail(CloneDiagnosticState *state, uint32_t nowMillis)
{
  if (!state)
  {
    return;
  }
  if (state->phase != CLONE_DIAG_FAILED)
  {
    state->failurePhase = state->phase;
  }
  state->phase = CLONE_DIAG_FAILED;
  state->phaseStartedMillis = nowMillis;
  state->updatedMillis = nowMillis;
  state->active = false;
}

static inline void cloneDiagnosticComplete(CloneDiagnosticState *state, uint32_t nowMillis)
{
  if (!state)
  {
    return;
  }
  cloneDiagnosticSetPhase(state, CLONE_DIAG_COMPLETE, nowMillis);
  state->active = false;
}

static inline uint32_t cloneDiagnosticElapsedMillis(
  const CloneDiagnosticState *state,
  uint32_t nowMillis)
{
  if (!state || state->phase == CLONE_DIAG_IDLE)
  {
    return 0;
  }
  const uint32_t referenceMillis = state->active ? nowMillis : state->updatedMillis;
  return (uint32_t)(referenceMillis - state->sessionStartedMillis);
}

static inline uint32_t cloneDiagnosticPhaseElapsedMillis(
  const CloneDiagnosticState *state,
  uint32_t nowMillis)
{
  if (!state || state->phase == CLONE_DIAG_IDLE)
  {
    return 0;
  }
  const uint32_t referenceMillis = state->active ? nowMillis : state->updatedMillis;
  return (uint32_t)(referenceMillis - state->phaseStartedMillis);
}
