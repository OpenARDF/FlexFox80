/**********************************************************************************************
 * Copyright © 2017 Digital Confections LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 **********************************************************************************************
 *
 * receiver.c
 *
 */

#include <string.h>
#include <stdlib.h>
#include "transmitter.h"
#include "i2c.h"    /* DAC on 80m VGA of Rev X1 Receiver board */

#ifdef INCLUDE_TRANSMITTER_SUPPORT

	extern volatile AntConnType g_antenna_connect_state;
	extern uint8_t g_mod_up;
	extern uint8_t g_mod_down;
	extern volatile BatteryType g_battery_type;

#ifdef SUPPORT_STATE_MACHINE
		EC (*g_txTask)(BiasStateMachineCommand* smCommand) = NULL;  /* allows the transmitter to specify functions to run in the foreground */
#endif /* SUPPORT_STATE_MACHINE */

	static volatile BOOL g_tx_initialized = FALSE;
	static volatile Frequency_Hz g_2m_frequency = DEFAULT_TX_2M_FREQUENCY;
	static volatile Frequency_Hz g_80m_frequency = DEFAULT_TX_80M_FREQUENCY;
	static volatile uint16_t g_2m_power_level_mW = DEFAULT_TX_2M_POWER_MW;
	static volatile uint16_t g_80m_power_level_mW = DEFAULT_TX_80M_POWER_MW;
	static volatile Frequency_Hz g_rtty_offset = DEFAULT_RTTY_OFFSET_FREQUENCY;
	static volatile RadioBand g_activeBand = DEFAULT_TX_ACTIVE_BAND;
	static volatile Modulation g_2m_modulationFormat = DEFAULT_TX_2M_MODULATION;
	static volatile uint8_t g_am_drive_level_high = DEFAULT_AM_DRIVE_LEVEL_HIGH;
	static volatile uint8_t g_am_drive_level_low = DEFAULT_AM_DRIVE_LEVEL_LOW;
/*	static volatile uint8_t g_cw_drive_level = DEFAULT_CW_DRIVE_LEVEL; */

	static volatile BOOL g_transmitter_keyed = FALSE;
	volatile BOOL g_tx_power_is_zero = TRUE;

/* EEPROM Defines */
#define EEPROM_BAND_DEFAULT BAND_80M

	static BOOL EEMEM ee_eeprom_initialization_flag = EEPROM_INITIALIZED_FLAG;
	static int32_t EEMEM ee_si5351_ref_correction = EEPROM_SI5351_CALIBRATION_DEFAULT;

	static uint8_t EEMEM ee_active_band = EEPROM_BAND_DEFAULT;
	static uint32_t EEMEM ee_active_2m_frequency = DEFAULT_TX_2M_FREQUENCY;
	static uint16_t EEMEM ee_2m_power_level_mW = DEFAULT_TX_2M_POWER_MW;
	static uint32_t EEMEM ee_active_80m_frequency = DEFAULT_TX_80M_FREQUENCY;
	static uint16_t EEMEM ee_80m_power_level_mW = DEFAULT_TX_80M_POWER_MW;
	static uint32_t EEMEM ee_cw_offset_frequency = DEFAULT_RTTY_OFFSET_FREQUENCY;
	static uint8_t EEMEM ee_am_drive_level_high = DEFAULT_AM_DRIVE_LEVEL_HIGH;
	static uint8_t EEMEM ee_am_drive_level_low = DEFAULT_AM_DRIVE_LEVEL_LOW;
/*	static uint8_t EEMEM ee_cw_drive_level = DEFAULT_CW_DRIVE_LEVEL; */
	static uint8_t EEMEM ee_active_2m_modulation = DEFAULT_TX_2M_MODULATION;
	static uint8_t EEMEM ee_80m_power_table[16] = DEFAULT_80M_POWER_TABLE;
	static uint8_t EEMEM ee_2m_am_power_table[16] = DEFAULT_2M_AM_POWER_TABLE;
	static uint8_t EEMEM ee_2m_am_drive_low_table[16] = DEFAULT_2M_AM_DRIVE_LOW_TABLE;
	static uint8_t EEMEM ee_2m_am_drive_high_table[16] = DEFAULT_2M_AM_DRIVE_HIGH_TABLE;
	static uint8_t EEMEM ee_2m_cw_power_table[16] = DEFAULT_2M_CW_POWER_TABLE;
	static uint8_t EEMEM ee_2m_cw_drive_table[16] = DEFAULT_2M_CW_DRIVE_TABLE;

/*
 *       Local Function Prototypes
 *
 */
	/**
	 */
	void saveAllTransmitterEEPROM(void);

	/**
	 */
	void txSet2mModulationGlobals(uint8_t *high, uint8_t *low);

	/**
	 */
/*	EC tx2mBiasStateMachine(BiasStateMachineCommand* smCommand); */

/*
 *       This function sets the VFO frequency (CLK0 of the Si5351) based on the intended frequency passed in by the parameter (freq),
 *       and the VFO configuration in effect. The VFO  frequency might be above or below the intended  frequency, depending on the VFO
 *       configuration setting in effect for the radio band of the frequency.
 */
	BOOL txSetFrequency(Frequency_Hz *freq, BOOL leaveClockOff)
	{
		BOOL activeBandSet = FALSE;
		RadioBand bandSet = BAND_INVALID;

		if((*freq < TX_MAXIMUM_80M_FREQUENCY) && (*freq > TX_MINIMUM_80M_FREQUENCY))    /* 80m */
		{
			g_80m_frequency = *freq;
			bandSet = BAND_80M;
		}
		else if((*freq < TX_MAXIMUM_2M_FREQUENCY) && (*freq > TX_MINIMUM_2M_FREQUENCY)) /* 2m */
		{
			g_2m_frequency = *freq;
			bandSet = BAND_2M;
		}

		if(bandSet == BAND_INVALID)
		{
			*freq = FREQUENCY_NOT_SPECIFIED;
		}
		else if(g_activeBand == bandSet)
		{
			if(bandSet == BAND_2M)
			{
				si5351_set_freq(*freq, TX_CLOCK_VHF, leaveClockOff);
				si5351_set_freq(*freq + 10000, TX_CLOCK_VHF_FM, leaveClockOff);
			}
			else
			{
				si5351_set_freq(*freq, TX_CLOCK_HF_0, leaveClockOff);
			}

			activeBandSet = TRUE;
		}

		return( activeBandSet);
	}

	Frequency_Hz txGetFrequency(void)
	{
		if(g_tx_initialized)
		{
			if(g_activeBand == BAND_2M)
			{
				return( g_2m_frequency);
			}
			else if(g_activeBand == BAND_80M)
			{
				return( g_80m_frequency);
			}
		}

		return( FREQUENCY_NOT_SPECIFIED);
	}

	void txSet2mModulationGlobals(uint8_t *high, uint8_t *low)
	{
		if(g_activeBand != BAND_2M)
		{
			return;
		}

		if(high)
		{
			g_am_drive_level_high = MIN(*high, MAX_2M_AM_DRIVE_LEVEL);
/*			g_cw_drive_level = MIN(*high, MAX_2M_AM_DRIVE_LEVEL); */
			g_mod_up = g_am_drive_level_high;
		}

		if(low)
		{
			g_am_drive_level_low = MIN(*low, MAX_2M_AM_DRIVE_LEVEL);
			g_mod_down = g_am_drive_level_low;
		}
	}


	RadioBand txGetBand(void)
	{
		return(g_activeBand);
	}

	EC powerToTransmitter(BOOL on)
	{
		EC result = ERROR_CODE_NO_ERROR;

		if(on)
		{
			if(g_activeBand == BAND_80M)
			{
				PORTB &= ~(1 << PORTB0);    /* Turn VHF off */
				PORTB |= (1 << PORTB1);     /* Turn HF on */
			}
			else
			{
				PORTB &= ~(1 << PORTB1);    /* Turn HF off */
				PORTB |= (1 << PORTB0);     /* Turn VHF on */
			}
		}
		else
		{
			BOOL err;
			PORTB &= ~(1 << PORTB1);  /* Turn off 80m band */

			if(g_activeBand == BAND_2M)
			{
				err = mcp4552_set_pot(0, BIAS_POT);
				if(err)
				{
					result = ERROR_CODE_DAC2_NONRESPONSIVE;
				}
			}

			PORTB &= ~(1 << PORTB0);

			err = dac081c_set_dac(0, PA_DAC);
			if(err)
			{
				result = ERROR_CODE_DAC1_NONRESPONSIVE;
			}
		}

		return(result);
	}

	void keyTransmitter(BOOL on)
	{
		if(on)
		{
			if(!g_transmitter_keyed)
			{
				if(g_activeBand == BAND_80M)
				{
					si5351_clock_enable(TX_CLOCK_HF_0, SI5351_CLK_ENABLED);
				}
				else
				{
					si5351_clock_enable(TX_CLOCK_VHF, SI5351_CLK_ENABLED);
					si5351_clock_enable(TX_CLOCK_VHF_FM, SI5351_CLK_ENABLED);
				}

				g_transmitter_keyed = TRUE;
			}
		}
		else
		{
			if(g_activeBand == BAND_80M)
			{
				si5351_clock_enable(TX_CLOCK_HF_0, SI5351_CLK_DISABLED);
			}
			else
			{
				si5351_clock_enable(TX_CLOCK_VHF, SI5351_CLK_DISABLED);
				si5351_clock_enable(TX_CLOCK_VHF_FM, SI5351_CLK_DISABLED);
			}

			g_transmitter_keyed = FALSE;
		}
	}

	EC __attribute__((optimize("O0"))) txSetParameters(uint16_t* power_mW, RadioBand* band, Modulation* modulationType, BOOL* enableDriverPwr)
/*	EC txSetParameters(uint16_t* power_mW, RadioBand* band, Modulation* modulationType, BOOL* enableDriverPwr) */
	{
		BOOL err = FALSE;
		EC code = ERROR_CODE_NO_ERROR;
		uint16_t power = 0;
		Modulation modulation = MODE_INVALID;

		if(power_mW)
		{
			power = *power_mW;
		}

		if(band != NULL)
		{
			/* Handle Band Setting */
			if(g_activeBand != *band)
			{
				keyTransmitter(OFF);
				powerToTransmitter(OFF);
				g_activeBand = *band;

				Frequency_Hz f = FREQUENCY_NOT_SPECIFIED;

				if(g_activeBand == BAND_2M)
				{
					f = g_2m_frequency;

					if(power_mW == NULL)
					{
						power = g_2m_power_level_mW;
					}

					if(modulationType == NULL)
					{
						modulation = g_2m_modulationFormat;
					}
				}
				else if (g_activeBand == BAND_80M)
				{
					f = g_80m_frequency;
					modulation = MODE_CW;
				}

				txSetFrequency(&f, TRUE);
			}
		}

		if(modulationType != NULL)
		{
			if(g_activeBand	== BAND_2M)
			{
				modulation = *modulationType;
				g_2m_modulationFormat = modulation;

				if(power_mW == NULL)
				{
					power = g_2m_power_level_mW;
				}
			}
			else
			{
				modulation = MODE_CW;
				if(power_mW == NULL)
				{
					power = g_80m_power_level_mW;
				}
			}
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
					if(g_activeBand == BAND_2M)
					{
						powerToTransmitter(ON);
						txSet2mModulationGlobals(&modLevelHigh, &modLevelLow);
						g_2m_power_level_mW = power;

						if(txIsAntennaForBand())
						{
							err = dac081c_set_dac(drainVoltageDAC, PA_DAC);
							if(err)
							{
								code = ERROR_CODE_DAC1_NONRESPONSIVE;
							}
							else
							{
								code = txSet2mGateBias(modLevelLow);
								err = (code != ERROR_CODE_NO_ERROR);
							}
						}
						else
						{
							g_tx_power_is_zero = TRUE;
							err = TRUE;
							code = ERROR_CODE_NO_ANTENNA_PREVENTS_POWER_SETTING;
						}

						if(g_tx_power_is_zero || err || (drainVoltageDAC == 0))
						{
							PORTB &= ~(1 << PORTB6);    /* Turn off Tx power */
						}
						else
						{
							PORTB |= (1 << PORTB6);     /* Turn on Tx power */
						}
					}
					else // BAND_80M
					{
						g_80m_power_level_mW = power;

						if(txIsAntennaForBand())
						{
							err = dac081c_set_dac(drainVoltageDAC, PA_DAC);
							if(err)
							{
								code = ERROR_CODE_DAC1_NONRESPONSIVE;
							}
						}
						else
						{
							g_tx_power_is_zero = TRUE;
							err = TRUE;
							code = ERROR_CODE_NO_ANTENNA_PREVENTS_POWER_SETTING;
						}

						if(g_tx_power_is_zero || err || (drainVoltageDAC == 0))
						{
							PORTB &= ~(1 << PORTB6);    /* Turn off Tx power */
						}
						else
						{
							PORTB |= (1 << PORTB6);     /* Turn on Tx power */
						}
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


	EC txSet2mGateBias(uint8_t bias)
	{
		EC code = ERROR_CODE_NO_ERROR;
		BOOL err = mcp4552_set_pot(bias, BIAS_POT); // set rheostat

		if(err)
		{
			code = ERROR_CODE_DAC2_NONRESPONSIVE;
		}

		return(code);
	}


	Modulation txGetModulation(void)
	{
		if(g_activeBand == BAND_2M)
		{
			return(g_2m_modulationFormat);
		}

		return(MODE_CW);
	}

	EC init_transmitter(void)
	{
		EC code;
		RadioBand bnd;

		if((code = si5351_init(SI5351_CRYSTAL_LOAD_6PF, 0)))
		{
			return( code);
		}

		initializeTransmitterEEPROMVars();

		bnd = g_activeBand;
		g_activeBand = BAND_INVALID;    /* ensure full configuration by txSetParameters() */

		if((code = txSetParameters(NULL, (RadioBand*)&bnd, NULL, NULL)))
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

		if(bnd == BAND_80M)
		{
			uint16_t pwr_mW = g_80m_power_level_mW;
			txSetFrequency((Frequency_Hz*)&g_80m_frequency, TRUE);
			txSetParameters(&pwr_mW, NULL, NULL, NULL);
		}
		else if(bnd == BAND_2M)
		{
			txSetFrequency((Frequency_Hz*)&g_2m_frequency, TRUE);
		}

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
			g_activeBand = eeprom_read_byte(&ee_active_band);
			g_2m_frequency = eeprom_read_dword(&ee_active_2m_frequency);
			g_2m_power_level_mW = eeprom_read_word(&ee_2m_power_level_mW);
			g_80m_frequency = eeprom_read_dword(&ee_active_80m_frequency);
			g_80m_power_level_mW = eeprom_read_word(&ee_80m_power_level_mW);
			g_rtty_offset = eeprom_read_dword(&ee_cw_offset_frequency);
			g_am_drive_level_high = eeprom_read_byte(&ee_am_drive_level_high);
			g_am_drive_level_low = eeprom_read_byte(&ee_am_drive_level_low);
			g_2m_modulationFormat = eeprom_read_byte(&ee_active_2m_modulation);
		}
		else
		{
			eeprom_write_byte(&ee_eeprom_initialization_flag, EEPROM_INITIALIZED_FLAG);

			g_activeBand = EEPROM_BAND_DEFAULT;
			g_2m_frequency = DEFAULT_TX_2M_FREQUENCY;
			g_2m_power_level_mW = DEFAULT_TX_2M_POWER_MW;
			g_80m_frequency = DEFAULT_TX_80M_FREQUENCY;
			g_80m_power_level_mW = DEFAULT_TX_80M_POWER_MW;
			g_rtty_offset = DEFAULT_RTTY_OFFSET_FREQUENCY;
			g_am_drive_level_high = DEFAULT_AM_DRIVE_LEVEL_HIGH;
			g_am_drive_level_low = DEFAULT_AM_DRIVE_LEVEL_LOW;
/*			g_cw_drive_level = DEFAULT_CW_DRIVE_LEVEL; */
			g_2m_modulationFormat = DEFAULT_TX_2M_MODULATION;

			saveAllTransmitterEEPROM();
		}
	}

	void saveAllTransmitterEEPROM(void)
	{
		uint8_t table[22];

		eeprom_update_byte(&ee_active_band, g_activeBand);
		eeprom_update_dword((uint32_t*)&ee_active_2m_frequency, g_2m_frequency);
		eeprom_update_word(&ee_2m_power_level_mW, g_2m_power_level_mW);
		eeprom_update_dword((uint32_t*)&ee_active_80m_frequency, g_80m_frequency);
		eeprom_update_word(&ee_80m_power_level_mW, g_80m_power_level_mW);
		eeprom_update_dword((uint32_t*)&ee_cw_offset_frequency, g_rtty_offset);
		eeprom_update_dword((uint32_t*)&ee_si5351_ref_correction, si5351_get_correction());
		eeprom_update_byte(&ee_am_drive_level_high, g_am_drive_level_high);
		eeprom_update_byte(&ee_am_drive_level_high, g_am_drive_level_low);
/*		eeprom_update_byte(&ee_cw_drive_level, g_cw_drive_level); */
		eeprom_update_byte(&ee_active_2m_modulation, g_2m_modulationFormat);
		memcpy(table, DEFAULT_80M_POWER_TABLE, sizeof(table));
		eeprom_write_block(table, ee_80m_power_table, sizeof(table));
		memcpy(table, DEFAULT_2M_AM_POWER_TABLE, sizeof(table));
		eeprom_write_block(table, ee_2m_am_power_table, sizeof(table));
		memcpy(table, DEFAULT_2M_AM_DRIVE_HIGH_TABLE, sizeof(table));
		eeprom_write_block(table, ee_2m_am_drive_high_table, sizeof(table));
		memcpy(table, DEFAULT_2M_AM_DRIVE_LOW_TABLE, sizeof(table));
		eeprom_write_block(table, ee_2m_am_drive_low_table, sizeof(table));
		memcpy(table, DEFAULT_2M_CW_POWER_TABLE, sizeof(table));
		eeprom_write_block(table, ee_2m_cw_power_table, sizeof(table));
		memcpy(table, DEFAULT_2M_CW_DRIVE_TABLE, sizeof(table));
		eeprom_write_block(table, ee_2m_cw_drive_table, sizeof(table));
	}


#endif  /*#ifdef INCLUDE_TRANSMITTER_SUPPORT */

RadioBand bandForFrequency(Frequency_Hz freq)
{
	RadioBand result = BAND_INVALID;

	if((freq >= TX_MINIMUM_2M_FREQUENCY) && (freq <= TX_MAXIMUM_2M_FREQUENCY))
	{
		result = BAND_2M;
	}
	else if((freq >= TX_MINIMUM_80M_FREQUENCY) && (freq <= TX_MAXIMUM_80M_FREQUENCY))
	{
		result = BAND_80M;
	}

	return(result);
}

EC txMilliwattsToSettings(uint16_t* powerMW, uint8_t* driveLevel, uint8_t* modLevelHigh, uint8_t* modLevelLow)
{
	EC ec = ERROR_CODE_NO_ERROR;
	RadioBand band = txGetBand();
	int16_t maxPwr;
	uint8_t index;

	if(powerMW == NULL)
	{
		return(ERROR_CODE_SW_LOGIC_ERROR);
	}

	if(band == BAND_80M)
	{
		if(g_battery_type == BATTERY_4r2V)
		{
			maxPwr = MAX_TX_POWER_80M_4r2V_MW;
		}
		else
		{
			maxPwr = MAX_TX_POWER_80M_MW;
		}
	}
	else
	{
		if(g_battery_type == BATTERY_4r2V)
		{
			maxPwr = MAX_TX_POWER_2M_4r2V_MW;
		}
		else
		{
			maxPwr = MAX_TX_POWER_2M_MW;
		}
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

	if(band == BAND_80M)
	{
		*driveLevel = eeprom_read_byte(&ee_80m_power_table[index]);
		*modLevelHigh = 0;
		*modLevelLow = 0;
		*driveLevel = MIN(*driveLevel, MAX_80M_PWR_SETTING);
	}
	else
	{
		if(g_2m_modulationFormat == MODE_AM)
		{
			*driveLevel = eeprom_read_byte(&ee_2m_am_power_table[index]);
			*modLevelHigh = eeprom_read_byte(&ee_2m_am_drive_high_table[index]);
			*modLevelLow = eeprom_read_byte(&ee_2m_am_drive_low_table[index]);
		}
		else
		{
			*driveLevel = eeprom_read_byte(&ee_2m_cw_power_table[index]);
			*modLevelHigh = eeprom_read_byte(&ee_2m_cw_drive_table[index]);
			*modLevelLow = *modLevelHigh;
		}

		*driveLevel = MIN(*driveLevel, MAX_2M_PWR_SETTING);
	}

	return(ec);
}

/**
 */
BOOL __attribute__((optimize("O0"))) txIsAntennaForBand(void)
{
	BOOL result = FALSE;
	RadioBand b = txGetBand();

	if(b == BAND_INVALID)
	{
		return( result);
	}

	switch(g_antenna_connect_state)
	{
		case ANT_2M_AND_80M_CONNECTED:
		{
			result = TRUE;
		}
		break;

		case ANT_80M_CONNECTED:
		{
			result = (b == BAND_80M);
		}
		break;

		case ANT_2M_CONNECTED:
		{
			result = (b == BAND_2M);
		}
		break;

		default:
		{

		}
		break;
	}

	return( result);
}
