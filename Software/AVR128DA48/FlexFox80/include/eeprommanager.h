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

#ifndef __EEPROMMANAGER_H__
#define __EEPROMMANAGER_H__

#include "defs.h"

#ifdef ATMEL_STUDIO_7
#include <avr/eeprom.h>
#endif  /* ATMEL_STUDIO_7 */

#include <time.h>
#include <avr/eeprom.h>

#define GUARDSIZE sizeof(uint32_t)

struct EE_prom
{
	uint16_t eeprom_initialization_flag;
	uint32_t guard4_1;
	time_t event_start_epoch; 
	uint32_t guard4_2;
	time_t event_finish_epoch; 
	uint32_t guard4_3;
 	char stationID_text[MAX_PATTERN_TEXT_LENGTH + 1]; 
	uint32_t guard4_4;
 	char pattern_text[MAX_PATTERN_TEXT_LENGTH + 1]; 
	uint32_t guard4_5;
	uint8_t unlockCode[MAX_UNLOCK_CODE_LENGTH + 1];
	uint32_t guard4_6;
	uint8_t id_codespeed; 
	uint32_t guard4_7;
	Fox_t fox_setting; 
	uint32_t guard4_8;
	uint8_t utc_offset; 
	uint32_t guard4_9;
	uint32_t frequency;
	uint32_t guard4_10;
	Frequency_Hz rtty_offset; 
	uint32_t guard4_11;
	uint16_t rf_power;
	uint32_t guard4_12;
	uint8_t pattern_codespeed;
	uint32_t guard4_13;
	int16_t off_air_seconds; 
	uint32_t guard4_14;
	int16_t on_air_seconds; 
	uint32_t guard4_15;
	int16_t ID_period_seconds; 
	uint32_t guard4_16;
	int16_t intra_cycle_delay_time; 
	uint32_t guard4_17;
	Event_t event_setting; 
	uint32_t guard4_18;
	uint32_t foxoring_frequencyA; 
	uint32_t guard4_19;
	uint32_t foxoring_frequencyB; 
	uint32_t guard4_20;
	uint32_t foxoring_frequencyC; 
	uint32_t guard4_21;
	Fox_t foxoring_fox_setting; 
	uint32_t guard4_22;
	bool master_setting;
	uint32_t guard4_23;
	char foxA_pattern_text[MAX_PATTERN_TEXT_LENGTH + 1]; 
	uint32_t guard4_24;
	char foxB_pattern_text[MAX_PATTERN_TEXT_LENGTH + 1]; 
	uint32_t guard4_25;
	char foxC_pattern_text[MAX_PATTERN_TEXT_LENGTH + 1]; 
	uint32_t guard4_26;
	float voltage_threshold;
	uint32_t guard4_27;
	uint16_t clock_calibration;
	uint32_t guard4_28;
};

typedef enum
{
	Eeprom_initialization_flag = 0, /* 2 bytes */
	Guard4_1 = Eeprom_initialization_flag + sizeof(uint16_t),		/**** Guard = 4 bytes ****/
	Event_start_epoch = Guard4_1 + GUARDSIZE, /* 4 bytes */
	Guard4_2 = Event_start_epoch + sizeof(time_t),					/**** Guard = 4 bytes ****/
	Event_finish_epoch =  Guard4_2 + GUARDSIZE, /* 4 bytes */
	Guard4_3 = Event_finish_epoch + sizeof(time_t),					/**** Guard = 4 bytes ****/
	StationID_text =  Guard4_3 + GUARDSIZE, /* MAX_PATTERN_TEXT_LENGTH + 1 bytes */
	Guard4_4 = StationID_text + MAX_PATTERN_TEXT_LENGTH + 1,		/**** Guard = 4 bytes ****/
	Pattern_text =  Guard4_4 + GUARDSIZE, /* MAX_PATTERN_TEXT_LENGTH + 1  bytes */
	Guard4_5 = Pattern_text + MAX_PATTERN_TEXT_LENGTH + 1,			/**** Guard = 4 bytes ****/
	UnlockCode =  Guard4_5 + GUARDSIZE, /* UNLOCK_CODE_SIZE + 1 bytes */
	Guard4_6 = UnlockCode + UNLOCK_CODE_SIZE + 1,					/**** Guard = 4 bytes ****/
	Id_codespeed =  Guard4_6 + GUARDSIZE, /* 1 byte */
	Guard4_7 = Id_codespeed + sizeof(uint8_t),						/**** Guard = 4 bytes ****/
	Fox_setting =  Guard4_7 + GUARDSIZE, /* 1 bytes */ 
	Guard4_8 = Fox_setting + sizeof(Fox_t),							/**** Guard = 4 bytes ****/
	Utc_offset =  Guard4_8 + GUARDSIZE, /* 1 byte */
	Guard4_9 = Utc_offset + sizeof(uint8_t),						/**** Guard = 4 bytes ****/
	Frequency =  Guard4_9 + GUARDSIZE, /* 4 bytes */
	Guard4_10 = Frequency + sizeof(Frequency_Hz),					/**** Guard = 4 bytes ****/
	RTTY_offset =  Guard4_10 + GUARDSIZE, /* 4 bytes */
	Guard4_11 = RTTY_offset + sizeof(Frequency_Hz),					/**** Guard = 4 bytes ****/
	RF_Power =  Guard4_11 + GUARDSIZE, /* 2 bytes */
	Guard4_12 = RF_Power + sizeof(int16_t),							/**** Guard = 4 bytes ****/
	Pattern_Code_Speed =  Guard4_12 + GUARDSIZE, /* 1 byte */
	Guard4_13 = Pattern_Code_Speed + sizeof(uint8_t),				/**** Guard = 4 bytes ****/
	Off_Air_Seconds =  Guard4_13 + GUARDSIZE, /* 2 bytes */
	Guard4_14 = Off_Air_Seconds + sizeof(int16_t),					/**** Guard = 4 bytes ****/
	On_Air_Seconds =  Guard4_14 + GUARDSIZE, /* 2 bytes */
	Guard4_15 = On_Air_Seconds + sizeof(int16_t),					/**** Guard = 4 bytes ****/
	ID_Period_Seconds =  Guard4_15 + GUARDSIZE, /* 2 bytes */
	Guard4_16 = ID_Period_Seconds + sizeof(int16_t),				/**** Guard = 4 bytes ****/
	Intra_Cycle_Delay_Seconds =  Guard4_16 + GUARDSIZE, /* 2 bytes */
	Guard4_17 = Intra_Cycle_Delay_Seconds + sizeof(int16_t),		/**** Guard = 4 bytes ****/
	Event_setting =  Guard4_17 + GUARDSIZE, /* 1 byte */ 
	Guard4_18 = Event_setting + sizeof(Event_t),					/**** Guard = 4 bytes ****/
	Foxoring_FrequencyA =  Guard4_18 + GUARDSIZE,  /* 4 bytes */
	Guard4_19 = Foxoring_FrequencyA + sizeof(Frequency_Hz),			/**** Guard = 4 bytes ****/
	Foxoring_FrequencyB =  Guard4_19 + GUARDSIZE,  /* 4 bytes */
	Guard4_20 = Foxoring_FrequencyB + sizeof(Frequency_Hz),			/**** Guard = 4 bytes ****/
	Foxoring_FrequencyC =  Guard4_20 + GUARDSIZE,  /* 4 bytes */
	Guard4_21 = Foxoring_FrequencyC + sizeof(Frequency_Hz),			/**** Guard = 4 bytes ****/
	Foxoring_fox_setting =  Guard4_21 + GUARDSIZE, /* 1 byte */ 
	Guard4_22 = Foxoring_fox_setting + sizeof(Fox_t),				/**** Guard = 4 bytes ****/
	Master_setting =  Guard4_22 + GUARDSIZE, /* bool: 1 byte */ 
	Guard4_23 = Master_setting + sizeof(bool),						/**** Guard = 4 bytes ****/
	FoxA_pattern_text =  Guard4_23 + GUARDSIZE,  /* MAX_PATTERN_TEXT_LENGTH + 1 bytes */
	Guard4_24 = FoxA_pattern_text + MAX_PATTERN_TEXT_LENGTH + 1,	/**** Guard = 4 bytes ****/
	FoxB_pattern_text =  Guard4_24 + GUARDSIZE,  /* MAX_PATTERN_TEXT_LENGTH + 1 bytes */
	Guard4_25 = FoxB_pattern_text + MAX_PATTERN_TEXT_LENGTH + 1,	/**** Guard = 4 bytes ****/
	FoxC_pattern_text =  Guard4_25 + GUARDSIZE,  /* MAX_PATTERN_TEXT_LENGTH + 1 bytes */
	Guard4_26 = FoxC_pattern_text + MAX_PATTERN_TEXT_LENGTH + 1,	/**** Guard = 4 bytes ****/
	Voltage_threshold =  Guard4_26 + GUARDSIZE,   /* 4 bytes */
	Guard4_27 = Voltage_threshold + sizeof(float),					/**** Guard = 4 bytes ****/
	Clock_calibration =  Guard4_27 + GUARDSIZE,   /* 2 bytes */
	Guard4_28 = Clock_calibration + sizeof(uint16_t),				/**** Guard = 4 bytes ****/
} EE_var_t;


class EepromManager
{
	/*variables */
	public:
	protected:
	private:

	/*functions */
	public:
	EepromManager();
	~EepromManager();

	static const struct EE_prom ee_vars;

	bool initializeEEPROMVars(void);
	bool readNonVols(void);
	void updateEEPROMVar(EE_var_t v, void* val);
	void saveAllEEPROM();

	protected:
	private:
	EepromManager( const EepromManager &c );
	EepromManager& operator=( const EepromManager &c );
};  /*EepromManager */

#endif  /*__EEPROMMANAGER_H__ */
