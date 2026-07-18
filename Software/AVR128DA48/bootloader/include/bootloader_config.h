/*
 *  MIT License
 *
 *  Copyright (c) 2026 DigitalConfections
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
 * Fixed memory geometry and serial protocol values for the resident bootloader.
 */
#ifndef FLEXFOX80_BOOTLOADER_CONFIG_H
#define FLEXFOX80_BOOTLOADER_CONFIG_H

#include <stdint.h>
#include "avr_update_contract.h"

#ifndef F_CPU
#define F_CPU 24000000UL
#endif

#define FLEXFOX_BOOTLOADER_VERSION "BL0.1"
#define FLEXFOX_BOOTLOADER_VERSION_MAJOR 0U
#define FLEXFOX_BOOTLOADER_VERSION_MINOR 1U
#define FLEXFOX_BOOT_PROTOCOL_VERSION 1U
#define FLEXFOX_BOOT_SECTION_PAGES FLEXFOX_AVR_BOOT_SECTION_PAGES
#define FLEXFOX_FLASH_PAGE_BYTES FLEXFOX_AVR_FLASH_PAGE_BYTES
#define FLEXFOX_FLASH_BYTES 131072UL
#define FLEXFOX_APP_START_BYTES FLEXFOX_AVR_APP_START_BYTES

#define FLEXFOX_BOOT_USART_BAUD 9600UL
#define FLEXFOX_BOOT_ENTRY_WINDOW_MS 1500U
#define FLEXFOX_BOOT_FRAME_BYTE_TIMEOUT_MS 1000U

#define FLEXFOX_UPDATE_REQUEST_CHAR 'U'
#define FLEXFOX_RUN_APP_CHAR 'R'
#define FLEXFOX_INFO_CHAR '?'
#define FLEXFOX_ERASE_PAGE_CHAR 'E'
#define FLEXFOX_WRITE_PAGE_CHAR 'W'
#define FLEXFOX_CRC_PAGE_CHAR 'C'

#define FLEXFOX_BOOT_APP_UPDATE_REQUEST FLEXFOX_AVR_BOOT_APP_UPDATE_REQUEST
#define FLEXFOX_BOOT_HANDOFF_INFO_MAGIC FLEXFOX_AVR_BOOT_HANDOFF_INFO_MAGIC
#define FLEXFOX_BOOT_HANDOFF_INFO_PROTOCOL_MASK FLEXFOX_AVR_BOOT_HANDOFF_INFO_PROTOCOL_MASK

static_assert(FLEXFOX_APP_START_BYTES == 0x4000U, "Unexpected application start");
static_assert((FLEXFOX_APP_START_BYTES % FLEXFOX_FLASH_PAGE_BYTES) == 0U, "Application must be page aligned");

#endif
