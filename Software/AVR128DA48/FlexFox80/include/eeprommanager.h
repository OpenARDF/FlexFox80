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

struct EE_prom
{
	uint16_t eeprom_initialization_flag; /* 0 */
	time_t event_start_epoch; /* 2 */
	time_t event_finish_epoch; /* 6 */
 	char stationID_text[MAX_PATTERN_TEXT_LENGTH + 1]; /* 10 */
 	char pattern_text[MAX_PATTERN_TEXT_LENGTH + 1]; /* 31 */
	uint8_t unlockCode[MAX_UNLOCK_CODE_LENGTH + 1]; /* 52 */
	uint8_t id_codespeed;  /* 61 */
	uint8_t fox_setting;  /* 62 */
	uint8_t utc_offset; /* 63 */
	uint32_t frequency; /* 64 */
	uint32_t rtty_offset; /* 68 */
	uint16_t rf_power; /* 72 */
	uint8_t pattern_codespeed; /* 74 */
	int16_t off_air_seconds; /* 75 */
	int16_t on_air_seconds; /* 77 */
	int16_t ID_period_seconds; /* 79 */
	int16_t intra_cycle_delay_time; /* 81 */
};

typedef enum
{
	Eeprom_initialization_flag = 0,
	Event_start_epoch = 2,
	Event_finish_epoch = 6,
	StationID_text = 10,
	Pattern_text = 31,
	UnlockCode = 52,
	Id_codespeed = 61,
	Fox_setting = 62,
	Utc_offset = 63,
	Frequency = 64,
	RTTY_offset = 68,
	RF_Power = 72,
	Pattern_Code_Speed = 74,
	Off_Air_Seconds = 75,
	On_Air_Seconds = 77,
	ID_Period_Seconds = 79,
	Intra_Cycle_Delay_Seconds = 81
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
