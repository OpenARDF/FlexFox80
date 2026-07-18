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
 * Shared immutable geometry and handoff values for the FlexFox AVR boot chain.
 */
#ifndef FLEXFOX80_AVR_UPDATE_CONTRACT_H
#define FLEXFOX80_AVR_UPDATE_CONTRACT_H

#define FLEXFOX_AVR_APP_START_BYTES 0x4000UL
#define FLEXFOX_AVR_FLASH_PAGE_BYTES 512U
#define FLEXFOX_AVR_BOOT_SECTION_PAGES 32U
#define FLEXFOX_AVR_BOOTSIZE_FUSE 0x20U
#define FLEXFOX_AVR_CODESIZE_FUSE 0x00U

#define FLEXFOX_AVR_UPDATE_IMAGE_FORMAT_VERSION 2U
#define FLEXFOX_AVR_UPDATE_TRAILER_MAGIC "FF80UPD2"
#define FLEXFOX_AVR_UPDATE_TRAILER_MAGIC_BYTES 8U
#define FLEXFOX_AVR_UPDATE_TRAILER_FORMAT_OFFSET 8U
#define FLEXFOX_AVR_UPDATE_TRAILER_PAGE_SIZE_OFFSET 10U
#define FLEXFOX_AVR_UPDATE_TRAILER_APP_START_OFFSET 12U
#define FLEXFOX_AVR_UPDATE_TRAILER_PAYLOAD_BYTES_OFFSET 16U
#define FLEXFOX_AVR_UPDATE_TRAILER_PAYLOAD_CRC32_OFFSET 20U
#define FLEXFOX_AVR_UPDATE_TRAILER_VERSION_OFFSET 24U
#define FLEXFOX_AVR_UPDATE_TRAILER_VERSION_BYTES 16U

#define FLEXFOX_AVR_BOOT_APP_UPDATE_REQUEST 0xA5U
#define FLEXFOX_AVR_BOOT_HANDOFF_INFO_MAGIC 0xB0U
#define FLEXFOX_AVR_BOOT_HANDOFF_INFO_PROTOCOL_MASK 0x0FU
#define FLEXFOX_AVR_UPDATE_EEPROM_MARKER_ADDRESS 511U
#define FLEXFOX_AVR_UPDATE_EEPROM_MARKER 0xA5U

#endif
