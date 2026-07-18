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
 * Persistent ESP-side state and protocol contract for wireless AVR updates.
 */
#ifndef FLEXFOX_AVR_FIRMWARE_UPDATE_H_
#define FLEXFOX_AVR_FIRMWARE_UPDATE_H_

#include <Arduino.h>

#include "avr_update_contract.h"

#define AVR_UPDATE_IMAGE_PATH "/avr-update.bin"
#define AVR_UPDATE_STAGING_PATH "/avr-update.bin.staging"
#define AVR_UPDATE_STATE_A_PATH "/avr-update.state.a"
#define AVR_UPDATE_STATE_B_PATH "/avr-update.state.b"
#define AVR_UPDATE_STATE_TEMP_PATH "/avr-update.state.tmp"
#define AVR_UPDATE_DIAGNOSTIC_PATH "/avr-update.diag"
#define AVR_UPDATE_JOURNAL_PATH "/avr-update.log"
#define AVR_UPDATE_APP_START 0x4000UL
#define AVR_UPDATE_PAGE_SIZE 512U
#define AVR_UPDATE_FLASH_SIZE 131072UL
#define AVR_UPDATE_BAUD 38400UL
#define AVR_UPDATE_MAX_IMAGE_BYTES (AVR_UPDATE_FLASH_SIZE - AVR_UPDATE_APP_START)
#define AVR_UPDATE_FS_RESERVE_BLOCKS 4U

enum AvrUpdatePhase : uint8_t
{
  AVR_UPDATE_NONE = 0,
  AVR_UPDATE_STAGED = 1,
  AVR_UPDATE_ENTERING_BOOTLOADER = 2,
  AVR_UPDATE_PROGRAMMING = 3,
  AVR_UPDATE_VERIFYING_APPLICATION = 4,
  AVR_UPDATE_COMPLETE = 5,
  AVR_UPDATE_RECOVERY_REQUIRED = 6
};

struct AvrUpdateState
{
  uint32_t magic;
  uint16_t schema;
  uint8_t phase;
  uint8_t reserved;
  uint32_t imageBytes;
  uint32_t imageCrc32;
  uint32_t generation;
  uint16_t nextPage;
  uint16_t pageCount;
  char targetVersion[16];
  uint32_t stateCrc32;
};

uint32_t avrUpdateCrc32(uint32_t crc, const uint8_t *data, size_t length);
bool avrUpdateLoadState(AvrUpdateState *state);
bool avrUpdateCommitStagedImage(uint32_t imageBytes, uint32_t imageCrc32, const String& targetVersion, String *error);
bool avrUpdateMarkEnteringBootloader(String *error);
bool avrUpdateRestoreStaged(String *error);
bool avrUpdateHasStagedImage(void);
bool avrUpdateCanStageImage(uint32_t imageBytes, String *error);
bool avrUpdateResidentBootloaderPresent(void);
String avrUpdateStatusJson(const String& deviceSsid);
void avrUpdateResumeIfRequired(bool bootloaderAlreadyReady = false);
bool avrUpdateObserveApplicationVersion(const String& version);
bool avrUpdateArmQualificationEspRestart(uint16_t pageIndex, String *error);

#endif
