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
	uint8_t unlockCode[MAX_UNLOCK_CODE_LENGTH + 1]; /* 31 */
	uint8_t id_codespeed;  /* 40 */
	uint8_t fox_setting;  /* 41 */
	uint8_t utc_offset; /* 42 */
	uint32_t frequency; /* 43 */
	uint32_t rtty_offset; /* 47 */
	uint16_t rf_power; /* 51 */
};

typedef enum
{
	Eeprom_initialization_flag = 0,
	Event_start_epoch = 2,
	Event_finish_epoch = 6,
	StationID_text = 10,
	UnlockCode = 31,
	Id_codespeed = 40,
	Fox_setting = 41,
	Utc_offset = 42,
	Frequency = 43,
	RTTY_offset = 47,
	RF_Power = 51
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

BOOL initializeEEPROMVars(void);
BOOL readNonVols(void);
void updateEEPROMVar(EE_var_t v, void* val);
void saveAllEEPROM();

protected:
private:
EepromManager( const EepromManager &c );
EepromManager& operator=( const EepromManager &c );
};  /*EepromManager */

#endif  /*__EEPROMMANAGER_H__ */
