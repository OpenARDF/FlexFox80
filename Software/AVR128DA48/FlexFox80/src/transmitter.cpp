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
#include "dac0.h"
#include "binio.h"

extern volatile AntConnType g_antenna_connect_state;

static volatile bool g_tx_initialized = false;
volatile Frequency_Hz g_80m_frequency = EEPROM_TX_80M_FREQUENCY_DEFAULT;
volatile uint16_t g_80m_power_level_mW = EEPROM_TX_80M_POWER_MW_DEFAULT;
volatile Frequency_Hz g_rtty_offset = EEPROM_RTTY_OFFSET_FREQUENCY_DEFAULT;

static volatile bool g_transmitter_keyed = false;
volatile bool g_tx_power_is_zero = true;

uint8_t g_80m_power_table[16] = DEFAULT_80M_POWER_TABLE;

/*
 *       This function sets the VFO frequency (CLK0 of the Si5351) based on the intended frequency passed in by the parameter (freq),
 *       and the VFO configuration in effect. The VFO  frequency might be above or below the intended  frequency, depending on the VFO
 *       configuration setting in effect for the radio band of the frequency.
 */
	bool txSetFrequency(Frequency_Hz *freq, bool leaveClockOff)
	{
		bool err = true;

		if(!freq) return(err);
		
		if((*freq < TX_MAXIMUM_80M_FREQUENCY) && (*freq > TX_MINIMUM_80M_FREQUENCY))    /* 80m */
		{
			if(!si5351_set_freq(*freq, TX_CLOCK_HF_0, leaveClockOff))
			{
				g_80m_frequency = *freq;
				err = false;
			}
		}

		return(err);
	}

	Frequency_Hz txGetFrequency(void)
	{
		return( g_80m_frequency);
	}

	EC powerToTransmitter(bool on)
	{
		fet_driver(on);

		return(ERROR_CODE_NO_ERROR);
	}

	void keyTransmitter(bool on)
	{
		if(g_tx_initialized)
		{			
			if(on)
			{
				if(!g_transmitter_keyed)
				{
					if(si5351_clock_enable(TX_CLOCK_HF_0, SI5351_CLK_ENABLED) == ERROR_CODE_NO_ERROR)
					{
						g_transmitter_keyed = true;
					}
				}
			}
			else
			{
				if(si5351_clock_enable(TX_CLOCK_HF_0, SI5351_CLK_DISABLED) == ERROR_CODE_NO_ERROR)
				{
					g_transmitter_keyed = false;
				}
			}
		}
	}

	uint16_t txGetPowerMw(void)
	{
		return( g_80m_power_level_mW);
	}

	EC __attribute__((optimize("O0"))) txSetParameters(uint16_t* power_mW, bool* enableDriverPwr)
/*	EC txSetParameters(uint16_t* power_mW, bool* enableDriverPwr) */
	{
		bool err = false;
		EC code = ERROR_CODE_NO_ERROR;
		uint16_t power = 0;

		if(power_mW != null)
		{
			power = *power_mW;

			if(power <= MAX_TX_POWER_80M_MW)
			{
				uint8_t drainVoltageDAC, modLevelHigh, modLevelLow;
				code = txMilliwattsToSettings(&power, &drainVoltageDAC, &modLevelHigh, &modLevelLow);
				err = (code == ERROR_CODE_SW_LOGIC_ERROR);

				g_tx_power_is_zero = (power == 0);

				if(!err)
				{
					g_80m_power_level_mW = power;

					if(g_antenna_connect_state == ANT_CONNECTED)
					{
						DAC0_setVal(drainVoltageDAC);
					}
					else
					{
						g_tx_power_is_zero = true;
						err = true;
						code = ERROR_CODE_NO_ANTENNA_PREVENTS_POWER_SETTING;
					}

					if(g_tx_power_is_zero || (drainVoltageDAC == 0))
					{
						powerToTransmitter(OFF); /* Turn off FET driver */
					}
					else
					{
						powerToTransmitter(ON); /* Turn on FET driver */
					}
				}

				*power_mW = power;
			}
			else
			{
				err = true;
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
		bool err;
		
		DAC0_init();

		if((err = si5351_init(SI5351_CRYSTAL_LOAD_6PF, 0)))
		{
			return(ERROR_CODE_RF_OSCILLATOR_ERROR);
		}

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

// 		if((code = si5351_drive_strength(TX_CLOCK_VHF, SI5351_DRIVE_8MA)))
// 		{
// 			return( code);
// 		}
// 		if((code = si5351_clock_enable(TX_CLOCK_VHF, SI5351_CLK_DISABLED)))
// 		{
// 			return( code);
// 		}
// 
// 		if((code = si5351_drive_strength(TX_CLOCK_VHF_FM, SI5351_DRIVE_8MA)))
// 		{
// 			return( code);
// 		}
// 		if((code = si5351_clock_enable(TX_CLOCK_VHF_FM, SI5351_CLK_DISABLED)))
// 		{
// 			return( code);
// 		}

		uint16_t pwr_mW = g_80m_power_level_mW;
		
		err = txSetFrequency((Frequency_Hz*)&g_80m_frequency, true);
		if(!err)
		{
			code = txSetParameters(&pwr_mW, NULL);
			if((code == ERROR_CODE_NO_ERROR) || (code == ERROR_CODE_NO_ANTENNA_PREVENTS_POWER_SETTING))
			{
				g_tx_initialized = true;
			}
		}

		return( code);
	}


EC txMilliwattsToSettings(uint16_t* powerMW, uint8_t* driveLevel, uint8_t* modLevelHigh, uint8_t* modLevelLow)
{
	EC ec = ERROR_CODE_NO_ERROR;
	int16_t maxPwr;
	uint8_t index;

	if(powerMW == NULL)
	{
		return(ERROR_CODE_SW_LOGIC_ERROR);
	}

	maxPwr = MAX_TX_POWER_80M_MW;

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

	*driveLevel = g_80m_power_table[index];
	*modLevelHigh = 0;
	*modLevelLow = 0;
	*driveLevel = MIN(*driveLevel, MAX_80M_PWR_SETTING);

	return(ec);
}