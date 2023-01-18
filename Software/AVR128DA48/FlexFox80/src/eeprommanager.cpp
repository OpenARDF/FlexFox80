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

const struct EE_prom EEMEM EepromManager::ee_vars
=
{
	0x00, // 	uint16_t eeprom_initialization_flag; 
	0x00000000, // 	Guard
	0x00000000, // 	time_t event_start_epoch;
	0x00000000, // 	Guard
	0x00000000, // time_t event_finish_epoch; 
	0x00000000, // 	Guard
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",  // char stationID_text[MAX_PATTERN_TEXT_LENGTH + 1];
	0x00000000, // 	Guard
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", // 	char pattern_text[MAX_PATTERN_TEXT_LENGTH + 1]; 
	0x00000000, // 	Guard
	"\0\0\0\0\0\0\0\0", // 	uint8_t unlockCode[MAX_UNLOCK_CODE_LENGTH + 1]; 
	0x00000000, // 	Guard
	0x00, // 	uint8_t id_codespeed;  
	0x00000000, // 	Guard
	(Fox_t)0x00, // Fox_t fox_setting;  
	0x00000000, // 	Guard
	0x00, // 	uint8_t utc_offset; 
	0x00000000, // 	Guard
	0x00000000, // 	uint32_t frequency;
	0x00000000, // 	Guard
	0x00000000, // 	uint32_t rtty_offset; 
	0x00000000, // 	Guard
	0x0000, // 	uint16_t rf_power; 
	0x00000000, // 	Guard
	0x00, // 	uint8_t pattern_codespeed; 
	0x00000000, // 	Guard
	0x0000, // 	int16_t off_air_seconds; 
	0x00000000, // 	Guard
	0x0000, // 	int16_t on_air_seconds; 
	0x00000000, // 	Guard
	0x0000, // 	int16_t ID_period_seconds; 
	0x00000000, // 	Guard
	0x0000, // 	int16_t intra_cycle_delay_time; 
	0x00000000, // 	Guard
	(Event_t)0x00, // Event_t event_setting;
	0x00000000, // 	Guard
	0x00000000,  // uint32_t foxoring_frequencyA;
	0x00000000, // 	Guard
	0x00000000,  //	uint32_t foxoring_frequencyB; 
	0x00000000, // 	Guard
	0x00000000,  // uint32_t foxoring_frequencyC; 
	0x00000000, // 	Guard
	(Fox_t)0x00, // uint8_t foxoring_fox_setting; 
	0x00000000, // 	Guard
	0x00, // 	uint8_t master_setting;
	0x00000000, // 	Guard
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", // 	char foxA_pattern_text[MAX_PATTERN_TEXT_LENGTH + 1]; 
	0x00000000, // 	Guard
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", // 	char foxB_pattern_text[MAX_PATTERN_TEXT_LENGTH + 1]; 
	0x00000000, // 	Guard
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", // 	char foxC_pattern_text[MAX_PATTERN_TEXT_LENGTH + 1];
	0x00000000, // 	Guard
	0x00000000, // uint32_t voltage_threshold
	0x00000000, // 	Guard
	0x0000, // uint16_t clock_calibration
	0x00000000 // 	Guard
};

bool g_isMaster;
extern volatile Fox_t g_fox;
extern Frequency_Hz g_80m_frequency;
extern volatile int8_t g_utc_offset;
extern uint8_t g_unlockCode[];
extern Frequency_Hz g_rtty_offset;

Event_t g_event = EEPROM_EVENT_SETTING_DEFAULT;
Frequency_Hz g_foxoring_frequencyA = EEPROM_FOXORING_FREQUENCYA_DEFAULT;
Frequency_Hz g_foxoring_frequencyB = EEPROM_FOXORING_FREQUENCYB_DEFAULT;
Frequency_Hz g_foxoring_frequencyC = EEPROM_FOXORING_FREQUENCYC_DEFAULT;
Fox_t g_foxoring_fox = EEPROM_FOXORING_FOX_SETTING_DEFAULT;

extern char g_messages_text[][MAX_PATTERN_TEXT_LENGTH + 1];
extern volatile time_t g_event_start_epoch;
extern volatile time_t g_event_finish_epoch;
extern uint16_t g_80m_power_level_mW;
extern volatile uint8_t g_id_codespeed;
extern volatile uint8_t g_pattern_codespeed;
extern volatile int16_t g_off_air_seconds;
extern volatile int16_t g_on_air_seconds;
extern volatile int16_t g_ID_period_seconds;
extern volatile int16_t g_intra_cycle_delay_time;
float g_voltage_threshold = EEPROM_BATTERY_THRESHOLD_V;
uint16_t g_clock_calibration = EEPROM_CLOCK_CALIBRATION_DEFAULT;

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

void avr_eeprom_write_float(eeprom_addr_t index, float in) {
	while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
	*(float*)(eeprom_addr_t)(MAPPED_EEPROM_START+index) = in;
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
			if(*(uint32_t*)val != eeprom_read_dword(&(EepromManager::ee_vars.frequency)))
			{
				avr_eeprom_write_dword(Frequency, *(uint32_t*)val);
			}	
		}
		break;
		
		case RTTY_offset:
		{
			if(*(uint32_t*)val != eeprom_read_dword(&(EepromManager::ee_vars.rtty_offset)))
			{
				avr_eeprom_write_dword(RTTY_offset, *(uint32_t*)val);
			}
		}
		break;
		
		case RF_Power:
		{
			if(*(uint16_t*)val != eeprom_read_word(&(EepromManager::ee_vars.rf_power)))
			{
				avr_eeprom_write_word(RF_Power, *(uint16_t*)val);
			}			
		}
		break;
		
		case StationID_text:
		{
			int cnt = 0;
			char* char_addr = (char*)val;
			char c = *char_addr++;
			char q;
			
			eeprom_addr_t j = (eeprom_addr_t)StationID_text;

			while(c && (cnt<MAX_PATTERN_TEXT_LENGTH))
			{
				q = (char)eeprom_read_byte((uint8_t*)j);
				if(c != q)
				{
					avr_eeprom_write_byte(j, c);
				}
				
				cnt++;
				c = *char_addr++;
				j++;
			}

			avr_eeprom_write_byte(j, 0);
		}
		break;

		case Pattern_text:
		{
			int cnt = 0;
			char* char_addr = (char*)val;
			char c = *char_addr++;
			char q;
			
			eeprom_addr_t j = (eeprom_addr_t)Pattern_text;

			while(c && (cnt<MAX_PATTERN_TEXT_LENGTH))
			{
				q = (char)eeprom_read_byte((uint8_t*)j);
				if(c != q)
				{
					avr_eeprom_write_byte(j, c);
				}
				
				cnt++;
				c = *char_addr++;
				j++;
			}

			avr_eeprom_write_byte(j, 0);
		}
		break;

		case UnlockCode:
		{
			int cnt = 0;
			char* char_addr = (char*)val;
			char c = *char_addr++;
			char q;
			
			eeprom_addr_t j = (eeprom_addr_t)UnlockCode;

			while(c && (cnt<MAX_UNLOCK_CODE_LENGTH))
			{
				q = (char)eeprom_read_byte((uint8_t*)j);
				if(c != q)
				{
					avr_eeprom_write_byte(j, c);
				}
				
				cnt++;
				j++;
				c = *char_addr++;
			}

			avr_eeprom_write_byte(j, 0);
		}
		break;

		case Id_codespeed:
		{
			if(*(uint8_t*)val != eeprom_read_byte(&(EepromManager::ee_vars.id_codespeed)))
			{
				avr_eeprom_write_byte(Id_codespeed, *(uint8_t*)val);
			}
		}
		break;

		case Fox_setting:
		{
			if(*(uint8_t*)val != eeprom_read_byte((uint8_t*)&(EepromManager::ee_vars.fox_setting)))
			{
				avr_eeprom_write_byte(Fox_setting, *(uint8_t*)val);
			}			
		}
		break;
		
		case Master_setting:
		{
			if(*(uint8_t*)val != eeprom_read_byte((uint8_t*)&(EepromManager::ee_vars.master_setting)))
			{
				avr_eeprom_write_byte(Master_setting, *(uint8_t*)val);
			}
		}
		break;
		
		case Event_setting:
		{
			if(*(uint8_t*)val != eeprom_read_byte((uint8_t*)&(EepromManager::ee_vars.event_setting)))
			{
				avr_eeprom_write_byte(Event_setting, *(uint8_t*)val);
			}
		}
		break;
		
		case Foxoring_FrequencyA:
		{
			if(*(uint32_t*)val != eeprom_read_dword(&(EepromManager::ee_vars.foxoring_frequencyA)))
			{
				avr_eeprom_write_dword(Foxoring_FrequencyA, *(uint32_t*)val);
			}		
		}
		break;
		
		case Foxoring_FrequencyB:
		{
			if(*(uint32_t*)val != eeprom_read_dword(&(EepromManager::ee_vars.foxoring_frequencyB)))
			{
				avr_eeprom_write_dword(Foxoring_FrequencyB, *(uint32_t*)val);
			}
		}
		break;
		
		case Foxoring_FrequencyC:
		{
			if(*(uint32_t*)val != eeprom_read_dword(&(EepromManager::ee_vars.foxoring_frequencyC)))
			{
				avr_eeprom_write_dword(Foxoring_FrequencyC, *(uint32_t*)val);
			}
		}
		break;

		case Foxoring_fox_setting:
		{
			if(*(uint8_t*)val != eeprom_read_byte((uint8_t*)&(EepromManager::ee_vars.foxoring_fox_setting)))
			{
				avr_eeprom_write_byte(Foxoring_fox_setting, *(uint8_t*)val);
			}		
		}
		break;

		case Event_start_epoch:
		{
			if(*(uint32_t*)val != eeprom_read_dword(&(EepromManager::ee_vars.event_start_epoch)))
			{
				avr_eeprom_write_dword(Event_start_epoch, *(uint32_t*)val);
			}
		}
		break;

		case Event_finish_epoch:
		{
			if(*(uint32_t*)val != eeprom_read_dword(&(EepromManager::ee_vars.event_finish_epoch)))
			{
				avr_eeprom_write_dword(Event_finish_epoch, *(uint32_t*)val);
			}		
		}
		break;

		case Utc_offset:
		{
			if(*(uint8_t*)val != eeprom_read_byte(&(EepromManager::ee_vars.utc_offset)))
			{
				avr_eeprom_write_byte(Utc_offset, *(uint8_t*)val);
			}		
		}
		break;
		
		case Pattern_Code_Speed:
		{
			if(*(uint8_t*)val != eeprom_read_byte(&(EepromManager::ee_vars.pattern_codespeed)))
			{
				avr_eeprom_write_byte(Pattern_Code_Speed, *(uint8_t*)val);
			}
		}
		break;

		case Off_Air_Seconds:
		{
			if(*(int16_t*)val != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.off_air_seconds)))
			{
				avr_eeprom_write_word(Off_Air_Seconds, *(uint16_t*)val);
			}
		}
		break;
		
		case On_Air_Seconds:
		{
			if(*(int16_t*)val != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.on_air_seconds)))
			{
				avr_eeprom_write_word(On_Air_Seconds, *(uint16_t*)val);
			}
		}
		break;
		
		case ID_Period_Seconds:
		{
			if(*(int16_t*)val != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.ID_period_seconds)))
			{
				avr_eeprom_write_word(ID_Period_Seconds, *(uint16_t*)val);
			}
		}
		break;
		
		case Intra_Cycle_Delay_Seconds:
		{
			if(*(int16_t*)val != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.intra_cycle_delay_time)))
			{
				avr_eeprom_write_word(Intra_Cycle_Delay_Seconds, *(uint16_t*)val);
			}		
		}
		break;

		case Eeprom_initialization_flag:
		{
			if(*(int16_t*)val != (int16_t)eeprom_read_word((const uint16_t *)&(EepromManager::ee_vars.eeprom_initialization_flag)))
			{
				avr_eeprom_write_word(Eeprom_initialization_flag, *(uint16_t*)val);
			}		
		}
		break;
		
		case FoxA_pattern_text:
		{
			int cnt = 0;
			char* char_addr = (char*)val;
			char c = *char_addr++;
			char q;
			
			eeprom_addr_t j = (eeprom_addr_t)FoxA_pattern_text;

			while(c && (cnt<MAX_PATTERN_TEXT_LENGTH))
			{
				q = (char)eeprom_read_byte((uint8_t*)j);
				if(c != q)
				{
					avr_eeprom_write_byte(j, c);
				}
				
				cnt++;
				c = *char_addr++;
				j++;
			}

			avr_eeprom_write_byte(j, 0);
		}
		break;
		
		case FoxB_pattern_text:
		{
			int cnt = 0;
			char* char_addr = (char*)val;
			char c = *char_addr++;
			char q;
			
			eeprom_addr_t j = (eeprom_addr_t)FoxB_pattern_text;

			while(c && (cnt<MAX_PATTERN_TEXT_LENGTH))
			{
				q = (char)eeprom_read_byte((uint8_t*)j);
				if(c != q)
				{
					avr_eeprom_write_byte(j, c);
				}
				
				cnt++;
				c = *char_addr++;
				j++;
			}

			avr_eeprom_write_byte(j, 0);
		}
		break;
		
		case FoxC_pattern_text:
		{
			int cnt = 0;
			char* char_addr = (char*)val;
			char c = *char_addr++;
			char q;
			
			eeprom_addr_t j = (eeprom_addr_t)FoxC_pattern_text;

			while(c && (cnt<MAX_PATTERN_TEXT_LENGTH))
			{
				q = (char)eeprom_read_byte((uint8_t*)j);
				if(c != q)
				{
					avr_eeprom_write_byte(j, c);
				}
				
				cnt++;
				c = *char_addr++;
				j++;
			}

			avr_eeprom_write_byte(j, 0);
		}
		break;
		
		case Voltage_threshold:
		{
			if(*(float*)val != eeprom_read_float(&(EepromManager::ee_vars.voltage_threshold)))
			{
				avr_eeprom_write_float(Voltage_threshold, *(float*)val);
			}
		}
		break;
		
		case Clock_calibration:
		{
			if(*(uint16_t*)val != eeprom_read_word(&(EepromManager::ee_vars.clock_calibration)))
			{
				avr_eeprom_write_word(Clock_calibration, *(uint16_t*)val);
			}
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
	updateEEPROMVar(Id_codespeed, (void*)&g_id_codespeed);	
	updateEEPROMVar(Fox_setting, (void*)&g_fox);
	updateEEPROMVar(Event_setting, (void*)&g_event);
	updateEEPROMVar(Foxoring_FrequencyA, (void*)&g_foxoring_frequencyA);
	updateEEPROMVar(Foxoring_FrequencyB, (void*)&g_foxoring_frequencyB);
	updateEEPROMVar(Foxoring_FrequencyC, (void*)&g_foxoring_frequencyC);
	updateEEPROMVar(Foxoring_fox_setting, (void*)&g_foxoring_fox);
	updateEEPROMVar(Event_start_epoch, (void*)&g_event_start_epoch);
	updateEEPROMVar(Event_finish_epoch, (void*)&g_event_finish_epoch);
	updateEEPROMVar(Utc_offset, (void*)&g_utc_offset);
	updateEEPROMVar(StationID_text, (void*)g_messages_text[STATION_ID]);
	updateEEPROMVar(Pattern_text, (void*)g_messages_text[PATTERN_TEXT]);
	updateEEPROMVar(UnlockCode, (void*)g_unlockCode);
	updateEEPROMVar(Frequency, (void*)&g_80m_frequency);
	updateEEPROMVar(RTTY_offset, (void*)&g_rtty_offset);
	updateEEPROMVar(RF_Power, (void*)&g_80m_power_level_mW);
	updateEEPROMVar(Pattern_Code_Speed, (void*)&g_pattern_codespeed);
	updateEEPROMVar(Off_Air_Seconds, (void*)&g_off_air_seconds);
	updateEEPROMVar(On_Air_Seconds, (void*)&g_on_air_seconds);
	updateEEPROMVar(ID_Period_Seconds, (void*)&g_ID_period_seconds);
	updateEEPROMVar(Intra_Cycle_Delay_Seconds, (void*)&g_intra_cycle_delay_time);
	updateEEPROMVar(FoxA_pattern_text, (void*)g_messages_text[FOXA_PATTERN_TEXT]);
	updateEEPROMVar(FoxB_pattern_text, (void*)g_messages_text[FOXB_PATTERN_TEXT]);
	updateEEPROMVar(FoxC_pattern_text, (void*)g_messages_text[FOXC_PATTERN_TEXT]);
	updateEEPROMVar(Voltage_threshold, (void*)&g_voltage_threshold);
	updateEEPROMVar(Clock_calibration, (void*)&g_clock_calibration);
}


bool EepromManager::readNonVols(void)
{
	bool failure = true;
	uint16_t i;
	uint16_t initialization_flag = eeprom_read_word(0);

	if(initialization_flag == EEPROM_INITIALIZED_FLAG)  /* EEPROM is up to date */
	{
		g_isMaster = EEPROM_MASTER_SETTING_DEFAULT; // (int8_t)eeprom_read_byte(&(EepromManager::ee_vars.master_setting));
		g_id_codespeed = CLAMP(MIN_CODE_SPEED_WPM, eeprom_read_byte(&(EepromManager::ee_vars.id_codespeed)), MAX_CODE_SPEED_WPM);
		g_event = (Event_t)eeprom_read_byte((const uint8_t*)&(EepromManager::ee_vars.event_setting));
		g_foxoring_frequencyA = CLAMP(TX_MINIMUM_80M_FREQUENCY, eeprom_read_dword(&(EepromManager::ee_vars.foxoring_frequencyA)), TX_MAXIMUM_80M_FREQUENCY);
		g_foxoring_frequencyB = CLAMP(TX_MINIMUM_80M_FREQUENCY, eeprom_read_dword(&(EepromManager::ee_vars.foxoring_frequencyB)), TX_MAXIMUM_80M_FREQUENCY);
		g_foxoring_frequencyC = CLAMP(TX_MINIMUM_80M_FREQUENCY, eeprom_read_dword(&(EepromManager::ee_vars.foxoring_frequencyC)), TX_MAXIMUM_80M_FREQUENCY);
		g_foxoring_fox = (Fox_t)(CLAMP(FOXORING_EVENT_FOXA, eeprom_read_byte((uint8_t*)&(EepromManager::ee_vars.fox_setting)), FOXORING_EVENT_FOXC));
		g_fox = (Fox_t)(CLAMP(BEACON, eeprom_read_byte((uint8_t*)&(EepromManager::ee_vars.fox_setting)), SPRINT_F5));
		g_event_start_epoch = eeprom_read_dword(&(EepromManager::ee_vars.event_start_epoch));
		g_event_finish_epoch = eeprom_read_dword(&(EepromManager::ee_vars.event_finish_epoch));
		g_utc_offset = (int8_t)eeprom_read_byte(&(EepromManager::ee_vars.utc_offset));

		for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
		{
			char c = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.stationID_text[i])));
			if(c == 255) c= 0;
			g_messages_text[STATION_ID][i] = c;
			if(!c)
			{
				break;
			}
		}

		for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
		{
			char c = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.pattern_text[i])));			
			if(c==255) c=0;
			g_messages_text[PATTERN_TEXT][i] = c;
			if(!c)
			{
				break;
			}
		}

		for(i = 0; i < MAX_UNLOCK_CODE_LENGTH; i++)
		{
			char c = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.unlockCode[i])));	
			if(c==255) c=0;
			g_unlockCode[i] = c;
			if(!c)
			{
				break;
			}
		}
		
		g_80m_frequency = CLAMP(TX_MINIMUM_80M_FREQUENCY, eeprom_read_dword(&(EepromManager::ee_vars.frequency)), TX_MAXIMUM_80M_FREQUENCY);
		g_rtty_offset =eeprom_read_dword(&(EepromManager::ee_vars.rtty_offset));
		g_80m_power_level_mW = CLAMP(MIN_RF_POWER_MW, eeprom_read_word(&(EepromManager::ee_vars.rf_power)), MAX_TX_POWER_80M_MW);

		g_pattern_codespeed = CLAMP(5, eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.pattern_codespeed))), 20);
		g_off_air_seconds = CLAMP(0, (int16_t)eeprom_read_word((const uint16_t*)&(EepromManager::ee_vars.off_air_seconds)), 3600);
		g_on_air_seconds = CLAMP(0, (int16_t)eeprom_read_word((const uint16_t*)&(EepromManager::ee_vars.on_air_seconds)), 3600);
		g_ID_period_seconds = CLAMP(0, (int16_t)eeprom_read_word((const uint16_t*)&(EepromManager::ee_vars.ID_period_seconds)), 3600);
		g_intra_cycle_delay_time = CLAMP(0, (int16_t)eeprom_read_word((const uint16_t*)&(EepromManager::ee_vars.intra_cycle_delay_time)), 3600);

		for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
		{
			char c = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.foxA_pattern_text[i])));
			if(c==255) c=0;
			g_messages_text[FOXA_PATTERN_TEXT][i] = c;
			if(!c)
			{
				break;
			}
		}

		for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
		{
			char c = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.foxB_pattern_text[i])));
			if(c==255) c=0;
			g_messages_text[FOXB_PATTERN_TEXT][i] = c;
			if(!c)
			{
				break;
			}
		}

		for(i = 0; i < MAX_PATTERN_TEXT_LENGTH; i++)
		{
			char c = (char)eeprom_read_byte((uint8_t*)(&(EepromManager::ee_vars.foxC_pattern_text[i])));
			if(c==255) c=0;
			g_messages_text[FOXC_PATTERN_TEXT][i] = c;
			if(!c)
			{
				break;
			}
		}
		
		g_voltage_threshold = CLAMP(0.1, eeprom_read_float(&(EepromManager::ee_vars.voltage_threshold)), 15.0);
		
		g_clock_calibration = eeprom_read_word(&(EepromManager::ee_vars.clock_calibration));

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
			
			g_isMaster = EEPROM_MASTER_SETTING_DEFAULT;
			avr_eeprom_write_byte(Master_setting, g_isMaster);

			g_fox = EEPROM_FOX_SETTING_DEFAULT;
			avr_eeprom_write_byte(Fox_setting, (uint8_t)g_fox);
			
			g_event = EEPROM_EVENT_SETTING_DEFAULT;
			avr_eeprom_write_byte(Event_setting, (uint8_t)g_event);
			
			g_foxoring_frequencyA = EEPROM_FOXORING_FREQUENCYA_DEFAULT;
			avr_eeprom_write_dword(Foxoring_FrequencyA, g_foxoring_frequencyA);
			
			g_foxoring_frequencyB = EEPROM_FOXORING_FREQUENCYB_DEFAULT;
			avr_eeprom_write_dword(Foxoring_FrequencyB, g_foxoring_frequencyB);
			
			g_foxoring_frequencyC = EEPROM_FOXORING_FREQUENCYC_DEFAULT;
			avr_eeprom_write_dword(Foxoring_FrequencyC, g_foxoring_frequencyC);

			g_foxoring_fox = EEPROM_FOXORING_FOX_SETTING_DEFAULT;
			avr_eeprom_write_byte(Foxoring_fox_setting, (uint8_t)g_foxoring_fox);

			g_event_start_epoch = EEPROM_START_EPOCH_DEFAULT;
			avr_eeprom_write_dword(Event_start_epoch, g_event_start_epoch);

			g_event_finish_epoch = EEPROM_FINISH_EPOCH_DEFAULT;
			avr_eeprom_write_dword(Event_finish_epoch, g_event_finish_epoch);

			g_utc_offset = EEPROM_UTC_OFFSET_DEFAULT;
			avr_eeprom_write_byte(Utc_offset, (uint8_t)g_utc_offset);

			g_messages_text[STATION_ID][0] = '\0';
			avr_eeprom_write_byte(StationID_text, 0);

			uint8_t *v = (uint8_t*)EEPROM_FOX_PATTERN_DEFAULT;
			i = Pattern_text;
			for(j = 0; j < strlen(EEPROM_FOX_PATTERN_DEFAULT); j++)
			{
				g_messages_text[PATTERN_TEXT][j] = *v;
				avr_eeprom_write_byte(i++, *v++);
			}
			
			avr_eeprom_write_byte(i, '\0');

			v = (uint8_t*)EEPROM_DTMF_UNLOCK_CODE_DEFAULT;
			i = UnlockCode;
			for(j = 0; j < strlen(EEPROM_DTMF_UNLOCK_CODE_DEFAULT); j++)
			{
				g_unlockCode[j] = *v;
				avr_eeprom_write_byte(i++, *v++);
			}

			avr_eeprom_write_byte(i, '\0');
			g_unlockCode[j] = '\0';
			
			g_80m_frequency = EEPROM_TX_80M_FREQUENCY_DEFAULT;
			avr_eeprom_write_dword(Frequency, g_80m_frequency);

			g_rtty_offset = EEPROM_RTTY_OFFSET_FREQUENCY_DEFAULT;
			avr_eeprom_write_dword(Frequency, g_rtty_offset);

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

			v = (uint8_t*)EEPROM_FOXORING_FOXA_PATTERN_DEFAULT;
			i = FoxA_pattern_text;
			for(j = 0; j < strlen(EEPROM_FOXORING_FOXA_PATTERN_DEFAULT); j++)
			{
				g_messages_text[FOXA_PATTERN_TEXT][j] = *v;
				avr_eeprom_write_byte(i++, *v++);
			}

			avr_eeprom_write_byte(i, '\0');
			
			v = (uint8_t*)EEPROM_FOXORING_FOXB_PATTERN_DEFAULT;
			i = FoxB_pattern_text;
			for(j = 0; j < strlen(EEPROM_FOXORING_FOXB_PATTERN_DEFAULT); j++)
			{
				g_messages_text[FOXB_PATTERN_TEXT][j] = *v;
				avr_eeprom_write_byte(i++, *v++);
			}
			
			avr_eeprom_write_byte(i, '\0');

			v = (uint8_t*)EEPROM_FOXORING_FOXC_PATTERN_DEFAULT;
			i = FoxC_pattern_text;
			for(j = 0; j < strlen(EEPROM_FOXORING_FOXC_PATTERN_DEFAULT); j++)
			{
				g_messages_text[FOXC_PATTERN_TEXT][j] = *v;
				avr_eeprom_write_byte(i++, *v++);
			}
			
			avr_eeprom_write_byte(i, '\0');

			g_voltage_threshold = EEPROM_BATTERY_THRESHOLD_V;
			avr_eeprom_write_float(Voltage_threshold, g_voltage_threshold);
			
			g_clock_calibration = EEPROM_CLOCK_CALIBRATION_DEFAULT;
			avr_eeprom_write_word(Clock_calibration, g_clock_calibration);

			/* Done */

			avr_eeprom_write_word(0, EEPROM_INITIALIZED_FLAG);
		}
		
		return(err);
	}

