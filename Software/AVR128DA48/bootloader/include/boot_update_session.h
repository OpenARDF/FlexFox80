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
 * Dependency-free BL0 protocol-2 transaction state. The resident bootloader
 * uses this state machine directly; host tests exercise the same decisions.
 */

#ifndef FLEXFOX80_BOOT_UPDATE_SESSION_H
#define FLEXFOX80_BOOT_UPDATE_SESSION_H

#include <stdbool.h>
#include <stdint.h>

#include "bootloader_config.h"

typedef struct
{
	uint32_t image_bytes;
	uint32_t image_crc32;
	uint32_t erased_address;
	uint32_t last_written_address;
	uint16_t last_written_crc16;
	uint16_t page_count;
	uint16_t next_page;
	bool active;
	bool reset_page_erased;
	bool committed;
} BootUpdateSession;

/* Only a deliberate application software-reset handoff has a known-live ESP.
 * POR/BOR recovery must run the conservative rail/reset startup sequence even
 * when the persistent update marker is set. */
static inline bool bootUpdatePreserveLiveEsp(bool application_request, bool power_start)
{
	return application_request && !power_start;
}

static inline void bootUpdateSessionReset(BootUpdateSession *session)
{
	BootUpdateSession empty = {};
	*session = empty;
	session->erased_address = UINT32_MAX;
	session->last_written_address = UINT32_MAX;
}

static inline bool bootUpdateSessionBegin(
	BootUpdateSession *session,
	uint32_t image_bytes,
	uint32_t image_crc32)
{
	if(!session || image_bytes < (2UL * FLEXFOX_FLASH_PAGE_BYTES) ||
	   image_bytes > (FLEXFOX_FLASH_BYTES - FLEXFOX_APP_START_BYTES) ||
	   (image_bytes % FLEXFOX_FLASH_PAGE_BYTES) != 0U)
	{
		return false;
	}

	bootUpdateSessionReset(session);
	session->image_bytes = image_bytes;
	session->image_crc32 = image_crc32;
	session->page_count = (uint16_t)(image_bytes / FLEXFOX_FLASH_PAGE_BYTES);
	session->next_page = 1U;
	session->active = true;
	return true;
}

static inline uint32_t bootUpdateExpectedPageAddress(const BootUpdateSession *session)
{
	return FLEXFOX_APP_START_BYTES + (uint32_t)session->next_page * FLEXFOX_FLASH_PAGE_BYTES;
}

static inline bool bootUpdateSessionAllowsErase(const BootUpdateSession *session, uint32_t address)
{
	if(!session || !session->active || session->committed) return false;
	if(address == FLEXFOX_APP_START_BYTES) return true;
	if(!session->reset_page_erased || session->next_page >= session->page_count) return false;
	return address == bootUpdateExpectedPageAddress(session);
}

static inline void bootUpdateSessionNoteErase(BootUpdateSession *session, uint32_t address)
{
	if(address == FLEXFOX_APP_START_BYTES) session->reset_page_erased = true;
	session->erased_address = address;
}

static inline bool bootUpdateSessionReadyForResetWrite(const BootUpdateSession *session)
{
	return session && session->active && session->reset_page_erased &&
	       !session->committed && session->next_page == session->page_count;
}

static inline bool bootUpdateSessionAllowsWrite(
	const BootUpdateSession *session,
	uint32_t address,
	uint16_t page_crc16)
{
	if(!session || !session->active) return false;
	if(address == session->last_written_address && page_crc16 == session->last_written_crc16)
	{
		return true; /* An OK response may have been lost; identical retry is safe. */
	}
	if(session->committed) return false;
	if(address == FLEXFOX_APP_START_BYTES) return bootUpdateSessionReadyForResetWrite(session);
	return session->reset_page_erased && session->next_page < session->page_count &&
	       address == bootUpdateExpectedPageAddress(session) &&
	       session->erased_address == address;
}

static inline void bootUpdateSessionNoteWrite(
	BootUpdateSession *session,
	uint32_t address,
	uint16_t page_crc16)
{
	const bool repeated = address == session->last_written_address &&
	                      page_crc16 == session->last_written_crc16;
	session->last_written_address = address;
	session->last_written_crc16 = page_crc16;
	if(repeated) return;
	if(address == FLEXFOX_APP_START_BYTES)
	{
		session->committed = true;
		return;
	}
	if(address == bootUpdateExpectedPageAddress(session))
	{
		session->next_page++;
		session->erased_address = UINT32_MAX;
	}
}

static inline bool bootUpdateSessionMayRunApplication(
	const BootUpdateSession *session,
	bool recovery_required,
	bool app_vector_programmed)
{
	if(!app_vector_programmed) return false;
	if(session && session->active) return session->committed;
	return !recovery_required;
}

#endif
