/*
 *  MIT License
 *
 *  Copyright (c) 2021 DigitalConfections
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


#include "eeprommanager.h"
#include "serialbus.h"
#include "i2c.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>

#if INCLUDE_DS3231_SUPPORT
#include "ds3231.h"
#endif

#ifdef ATMEL_STUDIO_7
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#endif  /* ATMEL_STUDIO_7 */

/***********************************************************************
 * Global Variables & String Constants
 *
 * Identify each global with a "g_" prefix
 * Whenever possible limit globals' scope to this file using "static"
 * Use "volatile" for globals shared between ISRs and foreground loop
 ************************************************************************/

const struct EE_prom EEMEM EepromManager::ee_vars =
{
	/* .eeprom_initialization_flag = */ 0,
	/* .event_start_epoch = */ 0,
	/* .event_finish_epoch = */ 0,
	/* .stationID_text = */ "\0",

	/* .unlockCode = */ EEPROM_DTMF_UNLOCK_CODE_DEFAULT,

	/* .id_codespeed = */ 0,
	/* .fox_setting = */ 0,
	/* .utc_offset = */ 0
};

extern char g_messages_text[][MAX_PATTERN_TEXT_LENGTH + 1];
extern volatile uint8_t g_id_codespeed;
extern volatile uint8_t g_pattern_codespeed;
extern volatile Fox_t g_fox;
extern volatile time_t g_event_start_epoch;
extern volatile time_t g_event_finish_epoch;
extern volatile int8_t g_utc_offset;
extern uint8_t g_unlockCode[];

extern char g_tempStr[];

/* default constructor */
EepromManager::EepromManager()
{
}   /*EepromManager */

/* default destructor */
EepromManager::~EepromManager()
{
}   /*~EepromManager */

#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/io.h>

typedef uint16_t eeprom_addr_t;

// to write
void avr_eeprom_write_byte(eeprom_addr_t index, uint8_t in) {
	while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
	*(uint8_t*)(eeprom_addr_t)(MAPPED_EEPROM_START+index) = in;
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
}

void avr_eeprom_write_word(eeprom_addr_t index, uint16_t in) {
	while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
	*(uint16_t*)(eeprom_addr_t)(MAPPED_EEPROM_START+index) = in;
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
}

void avr_eeprom_write_dword(eeprom_addr_t index, uint32_t in) {
	while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
	*(uint32_t*)(eeprom_addr_t)(MAPPED_EEPROM_START+index) = in;
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
}

// to read
uint8_t avr_eeprom_read_byte(eeprom_addr_t index) {
	uint8_t r;
	r = eeprom_read_byte((uint8_t *)index);
	return r;
}

uint8_t avr_eeprom_read_word(eeprom_addr_t index) {
	uint16_t r;
	r = eeprom_read_word((uint16_t *)index);
	return r;
}

void EepromManager::updateEEPROMVar(EE_var_t v, void* val)
{
	if(!val)
	{
		return;
	}

	switch(v)
	{
		case StationID_text:
		{
			char* char_addr = (char*)val;
			char c = *char_addr++;
			
			eeprom_addr_t j = (eeprom_addr_t)StationID_text;

			while(c)
			{
				avr_eeprom_write_byte(j++, c);
				c = *char_addr++;
			}

			avr_eeprom_write_byte(j, 0);
		}
		break;

		case UnlockCode:
		{
			uint8_t* uint8_addr = (uint8_t*)val;
			uint8_t c = *uint8_addr++;
			int i = 0;
			uint8_t j = (uint8_t)UnlockCode;
			
			while(c && (i < MAX_UNLOCK_CODE_LENGTH))
			{
				avr_eeprom_write_byte(j++, c);
				c = *uint8_addr++;
			}

			avr_eeprom_write_byte(j, 0);
		}
		break;

		case Id_codespeed:
		{
			avr_eeprom_write_byte(Id_codespeed, *(uint8_t*)val);
		}
		break;

		case Fox_setting:
		{
			avr_eeprom_write_byte(Fox_setting, *(uint8_t*)val);
		}
		break;

		case Event_start_epoch:
		{
			avr_eeprom_write_dword(Event_start_epoch, *(uint32_t*)val);
		}
		break;

		case Event_finish_epoch:
		{
			avr_eeprom_write_dword(Event_finish_epoch, *(uint32_t*)val);
		}
		break;

		case Utc_offset:
		{
			avr_eeprom_write_byte(Utc_offset, *(uint8_t*)val);
		}
		break;

		case Eeprom_initialization_flag:
		{
			avr_eeprom_write_word(Eeprom_initialization_flag, *(uint16_t*)val);
		}
		break;

		default:
		{

		}
		break;
	}
}


BOOL EepromManager::readNonVols(void)
{
	BOOL failure = TRUE;
	uint16_t i;
	uint16_t initialization_flag = eeprom_read_word(0);

	if(initialization_flag == EEPROM_INITIALIZED_FLAG)  /* EEPROM is up to date */
	{
		g_id_codespeed = CLAMP(MIN_CODE_SPEED_WPM, eeprom_read_byte(&(EepromManager::ee_vars.id_codespeed)), MAX_CODE_SPEED_WPM);
		g_fox = CLAMP(BEACON, (Fox_t)eeprom_read_byte(&(EepromManager::ee_vars.fox_setting)), SPRINT_F5);
		g_event_start_epoch = eeprom_read_dword(&(EepromManager::ee_vars.event_start_epoch));
		g_event_finish_epoch = eeprom_read_dword(&(EepromManager::ee_vars.event_finish_epoch));
		g_utc_offset = (int8_t)eeprom_read_byte(&(EepromManager::ee_vars.utc_offset));

		for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
		{
			g_messages_text[STATION_ID][i] = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.stationID_text[i])));
			if(!g_messages_text[STATION_ID][i])
			{
				break;
			}
		}

		for(i = 0; i < MAX_UNLOCK_CODE_LENGTH; i++)
		{
			g_unlockCode[i] = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.unlockCode[i])));
			if(!g_unlockCode[i])
			{
				break;
			}
		}

		/* Perform sanity checks */
		if(g_event_start_epoch && (g_event_finish_epoch <= g_event_start_epoch))
		{
			g_event_finish_epoch = g_event_start_epoch + SECONDS_24H;
		}

		failure = FALSE;
	}

	return( failure);
}

/*
 * Set volatile variables to their values stored in EEPROM
 */
	BOOL EepromManager::initializeEEPROMVars(void)
	{
		BOOL err = FALSE;
		uint16_t i, j;

#ifndef ATMEL_STUDIO_7
			/* Erase full EEPROM */
			for(i = 0; i < 0x0400; i++)
			{
				eeprom_write_byte((uint8_t*)i, 0xFF);
			}

			for(i = 0; i < 0x0400; i++)
			{
				uint8_t x = eeprom_read_byte((const uint8_t*)&i);
				if(x != 0xFF)
				{
					err = TRUE;
				}
			}
#endif  /* !ATMEL_STUDIO_7 */

		uint16_t initialization_flag = eeprom_read_word(0);

		if(initialization_flag != EEPROM_INITIALIZED_FLAG)
		{
			g_id_codespeed = EEPROM_ID_CODE_SPEED_DEFAULT;
			avr_eeprom_write_byte(Id_codespeed, g_id_codespeed);

			g_fox = EEPROM_FOX_SETTING_DEFAULT;
			avr_eeprom_write_byte(Fox_setting, g_fox);

	//		i2c_init(); /* Needs to happen before reading RTC */

			g_event_start_epoch = EEPROM_START_EPOCH_DEFAULT;
			avr_eeprom_write_dword(Event_start_epoch, g_event_start_epoch);

			g_event_finish_epoch = EEPROM_FINISH_EPOCH_DEFAULT;
			avr_eeprom_write_dword(Event_finish_epoch, g_event_finish_epoch);

			g_utc_offset = EEPROM_UTC_OFFSET_DEFAULT;
			avr_eeprom_write_byte(Utc_offset, (uint8_t)g_utc_offset);

			g_messages_text[STATION_ID][0] = '\0';
			avr_eeprom_write_byte(StationID_text, 0);

			uint8_t *v = (uint8_t*)EEPROM_DTMF_UNLOCK_CODE_DEFAULT;
			i = UnlockCode;
			for(j = 0; j < strlen(EEPROM_DTMF_UNLOCK_CODE_DEFAULT); j++)
			{
				g_unlockCode[j] = *v;
				avr_eeprom_write_byte(i++, *v++);
			}

			avr_eeprom_write_byte(i, 0);
			g_unlockCode[j] = '\0';

			/* Done */

			avr_eeprom_write_word(0, EEPROM_INITIALIZED_FLAG);
		}
		
		return(err);
	}


#if INIT_EEPROM_ONLY
	void EepromManager::sendSuccessString(void)
	{
		sendEEPROMString(TextVersion);
		sendPROGMEMString((const char*)&TEXT_EEPROM_SUCCESS_MESSAGE);
//		dumpEEPROMVars();
	}

	void EepromManager::sendPROGMEMString(const char* fl_addr)
	{
		if(!sb_enabled())
		{
			return;
		}

		if(fl_addr)
		{
			char c = pgm_read_byte(fl_addr++);

			while(c)
			{
				sb_echo_char(c);
				c = pgm_read_byte((fl_addr++));

				while(linkbusTxInProgress())
				{
					;
				}
			}
		}
	}


	void EepromManager::dumpEEPROMVars(void)
	{
		uint8_t byt;
		uint16_t wrd;
		uint32_t dwrd;

		byt = eeprom_read_byte(&(EepromManager::ee_vars.id_codespeed));
		sprintf(g_tempStr, "CS=%d\n", byt);
		sb_send_string(g_tempStr);

		byt = eeprom_read_byte(&(EepromManager::ee_vars.fox_setting));
		sprintf(g_tempStr, "FX=%d\n", byt);
		sb_send_string(g_tempStr);

		dwrd = eeprom_read_dword(&(EepromManager::ee_vars.event_start_epoch));
		sprintf(g_tempStr, "SE=%lu\n", dwrd);
		sb_send_string(g_tempStr);

		dwrd = eeprom_read_dword(&(EepromManager::ee_vars.event_finish_epoch));
		sprintf(g_tempStr, "FE=%lu\n", dwrd);
		sb_send_string(g_tempStr);

		byt = eeprom_read_byte(&(EepromManager::ee_vars.utc_offset));
		sprintf(g_tempStr, "UO=%d\n", (int8_t)byt);
		sb_send_string(g_tempStr);

		for(int i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
		{
			g_messages_text[STATION_ID][i] = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.stationID_text[i])));
			if(!g_messages_text[STATION_ID][i])
			{
				break;
			}
		}

		sprintf(g_tempStr, "ID=\"%s\"\n", g_messages_text[STATION_ID]);
		sb_send_string(g_tempStr);

		for(int i = 0; i < MAX_UNLOCK_CODE_LENGTH; i++)
		{
			g_unlockCode[i] = eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.unlockCode[i])));
			if(!g_unlockCode[i])
			{
				break;
			}
		}

		sprintf(g_tempStr, "PW=\"%s\"\n", (char*)g_unlockCode);
		sb_send_string(g_tempStr);
		sb_send_NewLine();
	}
#endif  /* INIT_EEPROM_ONLY */

void EepromManager::resetEEPROMValues(void)
{
	uint8_t i;

	uint8_t *v = (uint8_t*)EEPROM_DTMF_UNLOCK_CODE_DEFAULT;
	for(i = 0; i < strlen(EEPROM_DTMF_UNLOCK_CODE_DEFAULT); i++)
	{
		g_unlockCode[i] = *v;
		eeprom_write_byte((uint8_t*)&(g_unlockCode[i]), *v++);
	}

	eeprom_write_byte((uint8_t*)&(EepromManager::ee_vars.unlockCode[i]), 0);
	g_unlockCode[i] = '\0';

	g_messages_text[STATION_ID][0] = '\0';
	eeprom_write_byte((uint8_t*)&(EepromManager::ee_vars.stationID_text[0]), 0);

	g_event_start_epoch = EEPROM_START_EPOCH_DEFAULT;
	eeprom_write_dword((uint32_t*)&(EepromManager::ee_vars.event_start_epoch), g_event_start_epoch);

	g_event_finish_epoch = EEPROM_FINISH_EPOCH_DEFAULT;
	eeprom_write_dword((uint32_t*)&(EepromManager::ee_vars.event_finish_epoch), g_event_finish_epoch);
}
