/*
 *  MIT License
 *
 *  Copyright (c) 2022 DigitalConfections
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


#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "transmitter.h"
#include "i2c.h"    /* DAC on 80m VGA of Rev X1 Receiver board */

extern volatile AntConnType g_antenna_connect_state;

#ifdef INCLUDE_TRANSMITTER_SUPPORT

	extern uint8_t g_mod_up;
	extern uint8_t g_mod_down;
	extern volatile BatteryType g_battery_type;

	static volatile BOOL g_tx_initialized = FALSE;
	static volatile Frequency_Hz g_80m_frequency = DEFAULT_TX_80M_FREQUENCY;
	static volatile uint16_t g_80m_power_level_mW = DEFAULT_TX_80M_POWER_MW;
	static volatile Frequency_Hz g_rtty_offset = DEFAULT_RTTY_OFFSET_FREQUENCY;

	static volatile BOOL g_transmitter_keyed = FALSE;
	volatile BOOL g_tx_power_is_zero = TRUE;

/* EEPROM Defines */
#define EEPROM_BAND_DEFAULT 0

	static BOOL EEMEM ee_eeprom_initialization_flag = EEPROM_INITIALIZED_FLAG;
	static int32_t EEMEM ee_si5351_ref_correction = EEPROM_SI5351_CALIBRATION_DEFAULT;

	static uint32_t EEMEM ee_active_80m_frequency = DEFAULT_TX_80M_FREQUENCY;
	static uint16_t EEMEM ee_80m_power_level_mW = DEFAULT_TX_80M_POWER_MW;
	static uint32_t EEMEM ee_cw_offset_frequency = DEFAULT_RTTY_OFFSET_FREQUENCY;
	static uint8_t EEMEM ee_80m_power_table[16] = DEFAULT_80M_POWER_TABLE;

/*
 *       Local Function Prototypes
 *
 */
	/**
	 */
	void saveAllTransmitterEEPROM(void);

/*
 *       This function sets the VFO frequency (CLK0 of the Si5351) based on the intended frequency passed in by the parameter (freq),
 *       and the VFO configuration in effect. The VFO  frequency might be above or below the intended  frequency, depending on the VFO
 *       configuration setting in effect for the radio band of the frequency.
 */
	BOOL txSetFrequency(Frequency_Hz *freq, BOOL leaveClockOff)
	{
		BOOL activeBandSet = FALSE;

		if((*freq < TX_MAXIMUM_80M_FREQUENCY) && (*freq > TX_MINIMUM_80M_FREQUENCY))    /* 80m */
		{
			si5351_set_freq(*freq, TX_CLOCK_HF_0, leaveClockOff);
			g_80m_frequency = *freq;
			activeBandSet = TRUE;
		}

		return( activeBandSet);
	}

	Frequency_Hz txGetFrequency(void)
	{
		if(g_tx_initialized)
		{
			return( g_80m_frequency);
		}

		return( FREQUENCY_NOT_SPECIFIED);
	}

	EC powerToTransmitter(BOOL on)
	{
		EC result = ERROR_CODE_NO_ERROR;

		if(on)
		{
// 			PORTB |= (1 << PORTB1);     /* Turn HF on */
		}
		else
		{
// 			PORTB &= ~(1 << PORTB1);  /* Turn off 80m band */
		}

		return(result);
	}

	void keyTransmitter(BOOL on)
	{
		if(g_tx_initialized)
		{			
			if(on)
			{
				if(!g_transmitter_keyed)
				{
					if(si5351_clock_enable(TX_CLOCK_HF_0, SI5351_CLK_ENABLED) == ERROR_CODE_NO_ERROR)
					{
						g_transmitter_keyed = TRUE;
					}
				}
			}
			else
			{
				if(si5351_clock_enable(TX_CLOCK_HF_0, SI5351_CLK_DISABLED) == ERROR_CODE_NO_ERROR)
				{
					g_transmitter_keyed = FALSE;
				}
			}
		}
	}

	EC __attribute__((optimize("O0"))) txSetParameters(uint16_t* power_mW, BOOL* enableDriverPwr)
/*	EC txSetParameters(uint16_t* power_mW, BOOL* enableDriverPwr) */
	{
		BOOL err = FALSE;
		EC code = ERROR_CODE_NO_ERROR;
		uint16_t power = 0;

		if(power_mW)
		{
			power = *power_mW;
		}

		if(power_mW != NULL)
		{
			if(power <= MAX_TX_POWER_80M_MW)
			{
				uint8_t drainVoltageDAC, modLevelHigh, modLevelLow;
				code = txMilliwattsToSettings(&power, &drainVoltageDAC, &modLevelHigh, &modLevelLow);
				err = (code == ERROR_CODE_SW_LOGIC_ERROR);

				g_tx_power_is_zero = (power == 0);

				if(!err)
				{
					g_80m_power_level_mW = power;

					if(txIsAntennaForBand())
					{
// 						err = dac081c_set_dac(drainVoltageDAC, PA_DAC);
// 						if(err)
// 						{
// 							code = ERROR_CODE_DAC1_NONRESPONSIVE;
// 						}
					}
					else
					{
						g_tx_power_is_zero = TRUE;
						err = TRUE;
						code = ERROR_CODE_NO_ANTENNA_PREVENTS_POWER_SETTING;
					}

					if(g_tx_power_is_zero || err || (drainVoltageDAC == 0))
					{
// 							PORTB &= ~(1 << PORTB6);    /* Turn off Tx power */
					}
					else
					{
// 							PORTB |= (1 << PORTB6);     /* Turn on Tx power */
					}
				}

				*power_mW = power;
			}
			else
			{
				err = TRUE;
				code = ERROR_CODE_POWER_LEVEL_NOT_SUPPORTED;
			}
		}

		if(!err)
		{
			if(enableDriverPwr != NULL)
			{
				powerToTransmitter(*enableDriverPwr);
			}
		}

		return(code);
	}


	EC init_transmitter(void)
	{
		EC code;

		if((code = (EC)si5351_init(SI5351_CRYSTAL_LOAD_6PF, 0)))
		{
			return( code);
		}

		initializeTransmitterEEPROMVars();

		if((code = txSetParameters(NULL, NULL)))
		{
			return( code);
		}

		if((code = si5351_drive_strength(TX_CLOCK_HF_0, SI5351_DRIVE_8MA)))
		{
			return( code);
		}
		if((code = si5351_clock_enable(TX_CLOCK_HF_0, SI5351_CLK_DISABLED)))
		{
			return( code);
		}

		if((code = si5351_drive_strength(TX_CLOCK_VHF, SI5351_DRIVE_8MA)))
		{
			return( code);
		}
		if((code = si5351_clock_enable(TX_CLOCK_VHF, SI5351_CLK_DISABLED)))
		{
			return( code);
		}

		if((code = si5351_drive_strength(TX_CLOCK_VHF_FM, SI5351_DRIVE_8MA)))
		{
			return( code);
		}
		if((code = si5351_clock_enable(TX_CLOCK_VHF_FM, SI5351_CLK_DISABLED)))
		{
			return( code);
		}

		uint16_t pwr_mW = g_80m_power_level_mW;
		txSetFrequency((Frequency_Hz*)&g_80m_frequency, TRUE);
		txSetParameters(&pwr_mW, NULL);

		g_tx_initialized = TRUE;

		return( code);
	}

	void storeTransmitterValues(void)
	{
		saveAllTransmitterEEPROM();
	}


	void initializeTransmitterEEPROMVars(void)
	{
		if(eeprom_read_byte(&ee_eeprom_initialization_flag) == EEPROM_INITIALIZED_FLAG)
		{
			g_80m_frequency = eeprom_read_dword(&ee_active_80m_frequency);
			g_80m_power_level_mW = eeprom_read_word(&ee_80m_power_level_mW);
			g_rtty_offset = eeprom_read_dword(&ee_cw_offset_frequency);
		}
		else
		{
			eeprom_write_byte(&ee_eeprom_initialization_flag, EEPROM_INITIALIZED_FLAG);

			g_80m_frequency = DEFAULT_TX_80M_FREQUENCY;
			g_80m_power_level_mW = DEFAULT_TX_80M_POWER_MW;
			g_rtty_offset = DEFAULT_RTTY_OFFSET_FREQUENCY;

			saveAllTransmitterEEPROM();
		}
	}

	void saveAllTransmitterEEPROM(void)
	{
		uint8_t table[16] = DEFAULT_80M_POWER_TABLE;

		eeprom_update_dword((uint32_t*)&ee_active_80m_frequency, g_80m_frequency);
		eeprom_update_word(&ee_80m_power_level_mW, g_80m_power_level_mW);
		eeprom_update_dword((uint32_t*)&ee_cw_offset_frequency, g_rtty_offset);
		eeprom_update_dword((uint32_t*)&ee_si5351_ref_correction, si5351_get_correction());
		eeprom_write_block(table, ee_80m_power_table, sizeof(table));
	}


#endif  /*#ifdef INCLUDE_TRANSMITTER_SUPPORT */

EC txMilliwattsToSettings(uint16_t* powerMW, uint8_t* driveLevel, uint8_t* modLevelHigh, uint8_t* modLevelLow)
{
	EC ec = ERROR_CODE_NO_ERROR;
	int16_t maxPwr;
	uint8_t index;

	if(powerMW == NULL)
	{
		return(ERROR_CODE_SW_LOGIC_ERROR);
	}

	if(g_battery_type == BATTERY_4r2V)
	{
		maxPwr = MAX_TX_POWER_80M_4r2V_MW;
	}
	else
	{
		maxPwr = MAX_TX_POWER_80M_MW;
	}

	if((int16_t)*powerMW > maxPwr)
	{
		ec = ERROR_CODE_POWER_LEVEL_NOT_SUPPORTED;
	}

	*powerMW = CLAMP(0, (int16_t)*powerMW, maxPwr);

	if(*powerMW < 5)
	{
		index = 0;
		*powerMW = 0;
	}
	else if(*powerMW < 50)
	{
		index = 1;
		*powerMW = 10;
	}
	else if(*powerMW < 150)
	{
		index = 2;
		*powerMW = 100;
	}
	else if(*powerMW < 250)
	{
		index = 3;
		*powerMW = 200;
	}
	else if(*powerMW < 350)
	{
		index = 4;
		*powerMW = 300;
	}
	else if(*powerMW < 450)
	{
		index = 5;
		*powerMW = 400;
	}
	else if(*powerMW < 550)
	{
		index = 6;
		*powerMW = 500;
	}
	else if(*powerMW < 650)
	{
		index = 7;
		*powerMW = 600;
	}
	else if(*powerMW < 900)
	{
		index = 8;
		*powerMW = 800;
	}
	else if(*powerMW < 1250)
	{
		index = 9;
		*powerMW = 1000;
	}
	else if(*powerMW < 1750)
	{
		index = 10;
		*powerMW = 1500;
	}
	else if(*powerMW < 2250)
	{
		index = 11;
		*powerMW = 2000;
	}
	else if(*powerMW < 2750)
	{
		index = 12;
		*powerMW = 2500;
	}
	else if(*powerMW < 3500)
	{
		index = 13;
		*powerMW = 3000;
	}
	else if(*powerMW < 4500)
	{
		index = 14;
		*powerMW = 4000;
	}
	else
	{
		index = 15;
		*powerMW = 5000;
	}

	*driveLevel = eeprom_read_byte(&ee_80m_power_table[index]);
	*modLevelHigh = 0;
	*modLevelLow = 0;
	*driveLevel = MIN(*driveLevel, MAX_80M_PWR_SETTING);

	return(ec);
}

/**
 */
BOOL __attribute__((optimize("O0"))) txIsAntennaForBand(void)
{
	BOOL result = FALSE;

	switch(g_antenna_connect_state)
	{
		case ANT_80M_CONNECTED:
		{
			result = TRUE;
		}
		break;

		default:
		{

		}
		break;
	}

	return( result);
}
