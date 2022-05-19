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

#include "defs.h"
#include "eeprommanager.h"
#include "serialbus.h"
#include "i2c.h"
#include "transmitter.h"
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

extern volatile Fox_t g_fox;
extern volatile int8_t g_utc_offset;
extern uint8_t g_unlockCode[];
extern uint32_t g_rtty_offset;

extern char g_messages_text[][MAX_PATTERN_TEXT_LENGTH + 1];
extern uint32_t g_80m_frequency;
extern volatile time_t g_event_start_epoch;
extern volatile time_t g_event_finish_epoch;
extern uint16_t g_80m_power_level_mW;
extern volatile uint8_t g_id_codespeed;
extern volatile uint8_t g_pattern_codespeed;
extern volatile int16_t g_off_air_seconds;
extern volatile int16_t g_on_air_seconds;
extern volatile int16_t g_ID_period_seconds;
extern volatile int16_t g_intra_cycle_delay_time;

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

void EepromManager::updateEEPROMVar(EE_var_t v, void* val)
{
	if(!val)
	{
		return;
	}

	switch(v)
	{
		case Frequency:
		{
			avr_eeprom_write_dword(Frequency, *(uint32_t*)val);
		}
		break;
		
		case RTTY_offset:
		{
			avr_eeprom_write_dword(RTTY_offset, *(uint32_t*)val);
		}
		break;
		
		case RF_Power:
		{
			avr_eeprom_write_word(RF_Power, *(uint32_t*)val);			
		}
		break;
		
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

		case Pattern_text:
		{
			char* char_addr = (char*)val;
			char c = *char_addr++;
			
			eeprom_addr_t j = (eeprom_addr_t)Pattern_text;

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
		
		case Pattern_Code_Speed:
		{
			avr_eeprom_write_byte(Pattern_Code_Speed, *(uint8_t*)val);
		}
		break;

		case Off_Air_Seconds:
		{
			avr_eeprom_write_word(Off_Air_Seconds, *(uint16_t*)val);
		}
		break;
		
		case On_Air_Seconds:
		{
			avr_eeprom_write_word(On_Air_Seconds, *(uint16_t*)val);
		}
		break;
		
		case ID_Period_Seconds:
		{
			avr_eeprom_write_word(ID_Period_Seconds, *(uint16_t*)val);
		}
		break;
		
		case Intra_Cycle_Delay_Seconds:
		{
			avr_eeprom_write_word(Intra_Cycle_Delay_Seconds, *(uint16_t*)val);
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

/** 
 * Store any changed EEPROM variables
 */
void EepromManager::saveAllEEPROM(void)
{
	uint16_t i;
	
	if(g_id_codespeed != eeprom_read_byte(&(EepromManager::ee_vars.id_codespeed)))
	{
		updateEEPROMVar(Id_codespeed, (void*)&g_id_codespeed);
	}
	
	if(g_fox != eeprom_read_byte(&(EepromManager::ee_vars.fox_setting)))
	{
		updateEEPROMVar(Fox_setting, (void*)&g_fox);
	}
	
	if(g_event_start_epoch != eeprom_read_dword(&(EepromManager::ee_vars.event_start_epoch)))
	{
		updateEEPROMVar(Event_start_epoch, (void*)&g_event_start_epoch);
	}
	
	if(g_event_finish_epoch != eeprom_read_dword(&(EepromManager::ee_vars.event_finish_epoch)))
	{
		updateEEPROMVar(Event_finish_epoch, (void*)&g_event_finish_epoch);
	}
	
	if(g_utc_offset != eeprom_read_byte(&(EepromManager::ee_vars.utc_offset)))
	{
		updateEEPROMVar(Utc_offset, (void*)&g_utc_offset);
	}
	
	for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
	{
		if(g_messages_text[STATION_ID][i] != (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.stationID_text[i]))))
		{
			updateEEPROMVar(StationID_text, (void*)g_messages_text[STATION_ID]);
			break;
		}
	}

	for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
	{
		if(g_messages_text[PATTERN_TEXT][i] != (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.pattern_text[i]))))
		{
			updateEEPROMVar(Pattern_text, (void*)g_messages_text[PATTERN_TEXT]);
			break;
		}
	}

	for(i = 0; i < MAX_UNLOCK_CODE_LENGTH; i++)
	{
		if(g_unlockCode[i] != (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.unlockCode[i]))))
		{
			updateEEPROMVar(UnlockCode, (void*)g_unlockCode);
			break;
		}
	}
	
	if(g_80m_frequency != eeprom_read_dword(&(EepromManager::ee_vars.frequency)))
	{
		updateEEPROMVar(Frequency, (void*)&g_80m_frequency);
	}
	
	if(g_rtty_offset != eeprom_read_dword(&(EepromManager::ee_vars.rtty_offset)))
	{
		updateEEPROMVar(RTTY_offset, (void*)&g_rtty_offset);
	}
	
	if(g_80m_power_level_mW != eeprom_read_word(&(EepromManager::ee_vars.rf_power)))
	{
		updateEEPROMVar(RF_Power, (void*)&g_80m_power_level_mW);
	}
	
	if(g_pattern_codespeed != eeprom_read_byte(&(EepromManager::ee_vars.pattern_codespeed)))
	{
		updateEEPROMVar(Pattern_Code_Speed, (void*)&g_pattern_codespeed);
	}

	if(g_off_air_seconds != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.off_air_seconds)))
	{
		updateEEPROMVar(Off_Air_Seconds, (void*)&g_off_air_seconds);
	}
	
	if(g_on_air_seconds != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.on_air_seconds)))
	{
		updateEEPROMVar(On_Air_Seconds, (void*)&g_on_air_seconds);
	}
	
	if(g_ID_period_seconds != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.ID_period_seconds)))
	{
		updateEEPROMVar(ID_Period_Seconds, (void*)&g_ID_period_seconds);
	}
	
	if(g_intra_cycle_delay_time != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.intra_cycle_delay_time)))
	{
		updateEEPROMVar(Intra_Cycle_Delay_Seconds, (void*)&g_intra_cycle_delay_time);
	}	
}


bool EepromManager::readNonVols(void)
{
	bool failure = true;
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

		for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
		{
			g_messages_text[PATTERN_TEXT][i] = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.pattern_text[i])));
			if(!g_messages_text[PATTERN_TEXT][i])
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
		
		g_80m_frequency = CLAMP(TX_MINIMUM_80M_FREQUENCY, eeprom_read_dword(&(EepromManager::ee_vars.frequency)), TX_MAXIMUM_80M_FREQUENCY);
		g_rtty_offset =eeprom_read_dword(&(EepromManager::ee_vars.rtty_offset));
		g_80m_power_level_mW = CLAMP(MIN_RF_POWER_MW, eeprom_read_word(&(EepromManager::ee_vars.rf_power)), MAX_TX_POWER_80M_MW);

		/* Perform sanity checks */
		if(g_event_start_epoch && (g_event_finish_epoch <= g_event_start_epoch))
		{
			g_event_finish_epoch = g_event_start_epoch + SECONDS_24H;
		}
		
		g_pattern_codespeed = CLAMP(5, eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.pattern_codespeed))), 20);
		g_off_air_seconds = CLAMP(0, (int16_t)eeprom_read_word((const uint16_t*)&(EepromManager::ee_vars.off_air_seconds)), 3600);
		g_on_air_seconds = CLAMP(0, (int16_t)eeprom_read_word((const uint16_t*)&(EepromManager::ee_vars.on_air_seconds)), 3600);
		g_ID_period_seconds = CLAMP(0, (int16_t)eeprom_read_word((const uint16_t*)&(EepromManager::ee_vars.ID_period_seconds)), 3600);
		g_intra_cycle_delay_time = CLAMP(0, (int16_t)eeprom_read_word((const uint16_t*)&(EepromManager::ee_vars.intra_cycle_delay_time)), 3600);

		failure = false;
	}

	return( failure);
}

/*
 * Set volatile variables to their values stored in EEPROM
 */
	bool EepromManager::initializeEEPROMVars(void)
	{
		bool err = false;
		uint16_t i, j;

		uint16_t initialization_flag = eeprom_read_word(0);

		if(initialization_flag != EEPROM_INITIALIZED_FLAG)
		{
			g_id_codespeed = EEPROM_ID_CODE_SPEED_DEFAULT;
			avr_eeprom_write_byte(Id_codespeed, g_id_codespeed);

			g_fox = EEPROM_FOX_SETTING_DEFAULT;
			avr_eeprom_write_byte(Fox_setting, g_fox);

			g_event_start_epoch = EEPROM_START_EPOCH_DEFAULT;
			avr_eeprom_write_dword(Event_start_epoch, g_event_start_epoch);

			g_event_finish_epoch = EEPROM_FINISH_EPOCH_DEFAULT;
			avr_eeprom_write_dword(Event_finish_epoch, g_event_finish_epoch);

			g_utc_offset = EEPROM_UTC_OFFSET_DEFAULT;
			avr_eeprom_write_byte(Utc_offset, (uint8_t)g_utc_offset);

			g_messages_text[STATION_ID][0] = '\0';
			avr_eeprom_write_byte(StationID_text, 0);

			g_messages_text[PATTERN_TEXT][0] = 'M';
			g_messages_text[PATTERN_TEXT][0] = 'O';
			g_messages_text[PATTERN_TEXT][0] = '\0';
			avr_eeprom_write_byte(Pattern_text, 0);

			uint8_t *v = (uint8_t*)EEPROM_DTMF_UNLOCK_CODE_DEFAULT;
			i = UnlockCode;
			for(j = 0; j < strlen(EEPROM_DTMF_UNLOCK_CODE_DEFAULT); j++)
			{
				g_unlockCode[j] = *v;
				avr_eeprom_write_byte(i++, *v++);
			}

			avr_eeprom_write_byte(i, 0);
			g_unlockCode[j] = '\0';
			
			g_80m_frequency = EEPROM_TX_80M_FREQUENCY_DEFAULT;
			avr_eeprom_write_dword(Frequency, g_80m_frequency);

			g_rtty_offset = EEPROM_RTTY_OFFSET_FREQUENCY_DEFAULT;
			avr_eeprom_write_dword(Frequency, g_80m_frequency);

			g_80m_power_level_mW = EEPROM_TX_80M_POWER_MW_DEFAULT;
			avr_eeprom_write_dword(RF_Power, g_80m_power_level_mW);
			
			g_pattern_codespeed = EEPROM_PATTERN_CODE_SPEED_DEFAULT;
			avr_eeprom_write_byte(Pattern_Code_Speed, g_pattern_codespeed);
			
			g_off_air_seconds = EEPROM_OFF_AIR_TIME_DEFAULT;
			avr_eeprom_write_word(Off_Air_Seconds, g_off_air_seconds);
			
			g_on_air_seconds = EEPROM_ON_AIR_TIME_DEFAULT;
			avr_eeprom_write_word(On_Air_Seconds, g_on_air_seconds);
			
			g_ID_period_seconds = EEPROM_ID_TIME_INTERVAL_DEFAULT;
			avr_eeprom_write_word(ID_Period_Seconds, g_ID_period_seconds);
			
			g_intra_cycle_delay_time = EEPROM_INTRA_CYCLE_DELAY_TIME_DEFAULT;
			avr_eeprom_write_word(Intra_Cycle_Delay_Seconds, g_intra_cycle_delay_time);

			/* Done */

			avr_eeprom_write_word(0, EEPROM_INITIALIZED_FLAG);
		}
		
		return(err);
	}

