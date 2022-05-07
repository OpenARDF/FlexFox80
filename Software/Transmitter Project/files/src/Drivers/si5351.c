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
 * si5353.c
 *
 */

#include "defs.h"

#ifdef INCLUDE_SI5351_SUPPORT

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <util/twi.h>

#include "i2c.h"
#include "si5351.h"

#define SI5351_I2C_SLAVE_ADDR                            0xC0    /* I2C slave address */

/*******************************
 * Global Variables
 ********************************/
	static int32_t g_si5351_ref_correction = EEPROM_SI5351_CALIBRATION_DEFAULT;
	static Frequency_Hz freqVCOB = 0;

	static Frequency_Hz xtal_freq = SI5351_XTAL_FREQ;
	static uint8_t enabledClocksMask = 0x00;
	static Frequency_Hz clock_out[3] = { 0, 0, 0 };

#ifdef SUPPORT_STATUS_READS
		Si5351Status dev_status;
		Si5351IntStatus dev_int_status;
#endif

#ifdef DEBUGGING_ONLY
/* Paste ClockBuilder-generated register values below. These will */
/* be compared with what this library generates. */
		Reg_Data const Reg_Store[NUM_REGS_MAX] = {
			{ 15,  0x00 },
			{ 16,  0x4C },
			{ 17,  0x2C },
			{ 18,  0x6C },
			{ 19,  0x80 },
			{ 20,  0x80 },
			{ 21,  0x80 },
			{ 22,  0x80 },
			{ 23,  0xC0 },
			{ 24,  0x00 },
			{ 25,  0x00 },
			{ 26,  0x00 },
			{ 27,  0x7D },
			{ 28,  0x00 },
			{ 29,  0x0D },
			{ 30,  0xFE },
			{ 31,  0x00 },
			{ 32,  0x00 },
			{ 33,  0x7A },
			{ 34,  0x00 },
			{ 35,  0x01 },
			{ 36,  0x00 },
			{ 37,  0x0C },
			{ 38,  0x00 },
			{ 39,  0x00 },
			{ 40,  0x00 },
			{ 41,  0x00 },
			{ 42,  0x00 },
			{ 43,  0x01 },
			{ 44,  0x00 },
			{ 45,  0x01 },
			{ 46,  0x00 },
			{ 47,  0x00 },
			{ 48,  0x00 },
			{ 49,  0x00 },
			{ 50,  0x00 },
			{ 51,  0x6B },
			{ 52,  0x00 },
			{ 53,  0x1E },
			{ 54,  0xB5 },
			{ 55,  0x00 },
			{ 56,  0x00 },
			{ 57,  0x59 },
			{ 58,  0x00 },
			{ 59,  0x01 },
			{ 60,  0x00 },
			{ 61,  0x03 },
			{ 62,  0x00 },
			{ 63,  0x00 },
			{ 64,  0x00 },
			{ 65,  0x00 },
			{ 66,  0x00 },
			{ 67,  0x00 },
			{ 68,  0x00 },
			{ 69,  0x00 },
			{ 70,  0x00 },
			{ 71,  0x00 },
			{ 72,  0x00 },
			{ 73,  0x00 },
			{ 74,  0x00 },
			{ 75,  0x00 },
			{ 76,  0x00 },
			{ 77,  0x00 },
			{ 78,  0x00 },
			{ 79,  0x00 },
			{ 80,  0x00 },
			{ 81,  0x00 },
			{ 82,  0x00 },
			{ 83,  0x00 },
			{ 84,  0x00 },
			{ 85,  0x00 },
			{ 86,  0x00 },
			{ 87,  0x00 },
			{ 88,  0x00 },
			{ 89,  0x00 },
			{ 90,  0x00 },
			{ 91,  0x00 },
			{ 92,  0x00 },
			{ 149, 0x00 },
			{ 150, 0x00 },
			{ 151, 0x00 },
			{ 152, 0x00 },
			{ 153, 0x00 },
			{ 154, 0x00 },
			{ 155, 0x00 },
			{ 156, 0x00 },
			{ 157, 0x00 },
			{ 158, 0x00 },
			{ 159, 0x00 },
			{ 160, 0x00 },
			{ 161, 0x00 },
			{ 162, 0x00 },
			{ 163, 0x00 },
			{ 164, 0x00 },
			{ 165, 0x00 },
			{ 166, 0x00 },
			{ 167, 0x00 },
			{ 168, 0x00 },
			{ 169, 0x00 },
			{ 170, 0x00 },
		};
#endif  /* #ifdef DEBUGGING_ONLY */

/*******************************
 * Private function prototypes
 ********************************/

	void pll_reset(Si5351_pll);

#ifdef DEBUGGING_ONLY
		uint32_t pll_calc(Frequency_Hz, Union_si5351_regs *, int32_t);
#else
	#ifdef APPLY_XTAL_CALIBRATION_VALUE
		BOOL pll_calc(Frequency_Hz, Union_si5351_regs *, int32_t);
	#else
		BOOL pll_calc(Frequency_Hz, Union_si5351_regs *);
	#endif
#endif

#ifdef SUPPORT_FOUT_BELOW_1024KHZ
		uint8_t select_r_div(Frequency_Hz *);
#endif

#ifdef DEBUGGING_ONLY
		uint32_t multisynth_calc(Frequency_Hz, Union_si5351_regs *, BOOL *, BOOL *, uint32_t *);
#else
		uint32_t multisynth_calc(Frequency_Hz, Union_si5351_regs *, BOOL *, BOOL *);
#endif

#ifdef DEBUGGING_ONLY
		uint32_t set_pll(Frequency_Hz, Si5351_pll);
#else
		void set_pll(Frequency_Hz, Si5351_pll);
#endif

	Frequency_Hz multisynth_estimate(Frequency_Hz freq_Fout, Union_si5351_regs *reg, BOOL *int_mode, BOOL *divBy4);
	void set_multisynth_registers_source(Si5351_clock, Si5351_pll);
	void set_multisynth_registers(Si5351_clock, Union_si5351_regs, uint8_t, uint8_t, BOOL);
	uint32_t calc_gcd(uint32_t, uint32_t);
	void reduce_by_gcd(uint32_t *, uint32_t *);
	BOOL si5351_write_bulk(uint8_t, uint8_t, uint8_t *);
	BOOL si5351_write(uint8_t, uint8_t);
	BOOL si5351_read(uint8_t, uint8_t *);
	void set_integer_mode(Si5351_clock, BOOL);
	void ms_div(Si5351_clock, uint8_t, BOOL);

#ifdef SUPPORT_STATUS_READS
		BOOL si5351_read_sys_status(Si5351Status *);
		BOOL si5351_read_int_status(Si5351IntStatus *);
#endif

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// */
/* Public functions */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

/*
 * init(Si5351_Xtal_load_pF xtal_load_c, Frequency_Hz ref_osc_freq)
 *
 * Setup communications to the Si5351 and set the crystal
 * load capacitance.
 *
 * xtal_load_c - Crystal load capacitance.
 * ref_osc_freq - Crystal/reference oscillator frequency (Hz).
 *
 */
	BOOL si5351_init(Si5351_Xtal_load_pF xtal_load_c, Frequency_Hz ref_osc_freq)
	{
		BOOL err = FALSE;
#ifndef DEBUG_WITHOUT_I2C
			/* Start I2C comms */
			i2c_init();
#endif

#ifdef DEBUGGING_ONLY
			uint8_t result_val = 0x00;
			BOOL result = FALSE;
#endif
		uint8_t reg_val;

		freqVCOB = 0;
		xtal_freq = SI5351_XTAL_FREQ;
		enabledClocksMask = 0x00;

		/* Disable Outputs */
		/* Set CLKx_DIS high; Reg. 3 = 0xFF */
		err = si5351_write(3, 0xff);

		/* Power down clocks */
		err |= si5351_write(16, 0xCC);
		err |= si5351_write(17, 0xCC);
		err |= si5351_write(18, 0xCC);

		/* Set crystal load capacitance */
		reg_val = 0x12; /* 0b010010 reserved value bits */
		reg_val |= xtal_load_c;

		err |= si5351_write(SI5351_CRYSTAL_LOAD, reg_val);

		if(!ref_osc_freq)
		{
			ref_osc_freq = SI5351_XTAL_FREQ;
		}

		/* Change the ref osc freq if different from default */
		if(ref_osc_freq != xtal_freq)
		{
			if(si5351_read(SI5351_PLL_INPUT_SOURCE, &reg_val))
			{
				return TRUE;
			}

			/* Clear the bits first */
			reg_val &= ~(SI5351_CLKIN_DIV_MASK);

			xtal_freq = ref_osc_freq;
			reg_val |= SI5351_CLKIN_DIV_1;

#ifdef DIVIDE_XTAL_FREQ_IF_NEEDED
				/* Divide down if greater than 30 MHz */

				if(ref_osc_freq > 30000000UL && ref_osc_freq <= 60000000UL)
				{
					xtal_freq /= 2;
					reg_val |= SI5351_CLKIN_DIV_2;
				}
				else if(ref_osc_freq > 60000000UL && ref_osc_freq <= 100000000UL)
				{
					xtal_freq /= 4;
					reg_val |= SI5351_CLKIN_DIV_4;
				}

#endif  /* #ifndef DIVIDE_XTAL_FREQ_IF_NEEDED */

			err |= si5351_write(SI5351_PLL_INPUT_SOURCE, reg_val);
		}

		return err;
	}

#ifdef DEBUGGING_ONLY
		BOOL write_register_map(void)
		{
			BOOL result = FALSE;
			uint8_t result_data;
			Reg_Data rd;
			uint16_t i = 0;

			for(i = 0; i < NUM_REGS_MAX; i++)
			{
				rd = Reg_Store[i];
				result = si5351_write(rd.Reg_Addr, rd.Reg_Val);
				if(result)
				{
					break;
				}

				result = si5351_read(rd.Reg_Addr, &result_data);
				if(result)
				{
					break;
				}
			}

			if(result)
			{
				i = i + 1;
			}

			return(result);
		}
#endif

#ifdef DEBUGGING_ONLY

		void dump_registers()
		{
			BOOL result;
			Reg_Data rd;
			uint16_t i = 0;
			uint8_t result_data;
			uint8_t addr;

			for(i = 0; i < 256; i++)
			{
/*		rd = Reg_Store[i]; */
/*		addr = rd.Reg_Addr; */
				addr = i;
				result = si5351_read(addr, &result_data);
				if(result)
				{
					break;  /* Place breakpoint here to print out register contents */
				}
			}

			if(result)
			{
				i = i + 1;  /* place breakpoint here to catch read errors */
			}

			return;
		}

		BOOL compare_with_register_map(void)
		{
			BOOL result = FALSE;
			uint8_t result_data;
			uint8_t count = 0;
			Reg_Data rd;
			uint16_t i = 0;

			for(i = 0; i < NUM_REGS_MAX; i++)
			{
				rd = Reg_Store[i];
				result = si5351_read(rd.Reg_Addr, &result_data);
				if(result)
				{
					break;
				}
				if(result_data != rd.Reg_Val)
				{
					count++;
				}
			}

			if(result)
			{
				i = i + 1;
			}

			return(result);
		}

#endif  /* #ifdef DEBUGGING_ONLY */


/*
 * BOOL si5351_set_freq(Frequency_Hz freq_Fout, Si5351_clock clk, BOOL clocksOff)
 *
 * Sets the clock frequency of the specified CLK output
 *
 * freq - Output frequency in Hz
 * clk - Clock output (use the si5351_clock enum)
 *
 * Returns TRUE on failure
 *
 */
	BOOL si5351_set_freq(Frequency_Hz freq_Fout, Si5351_clock clk, BOOL clocksOff)
	{
		Union_si5351_regs ms_reg;
		Frequency_Hz freq_VCO = 0;
		Si5351_pll target_pll;
		uint8_t clock_ctrl_addr;
		uint8_t r_div = SI5351_OUTPUT_CLK_DIV_1;
		BOOL int_mode = FALSE;
		BOOL div_by_4 = FALSE;

#ifdef DEBUGGING_ONLY
			uint32_t div = 0;
#endif

#ifdef DO_BOUNDS_CHECKING
			if(freq_Fout < SI5351_CLKOUT_MIN_FREQ)
			{
				return(TRUE);
			}

			if(freq_Fout > SI5351_CLKOUT_MAX_FREQ)
			{
				return(TRUE);
			}
#endif

#ifdef SUPPORT_FOUT_BELOW_1024KHZ
			/* Select the proper R div value used for Fout frequencies below 1.024 MHz */
			r_div = select_r_div(&freq_Fout);
#endif

#ifdef PREVENT_UNACHIEVABLE_FREQUENCIES
			/* Prevent unachievable frequencies from being entered. The Si5351 will accept these, but some may result */
			/* in no clock output. */
			if(freq_Fout > 999999)
			{
				freq_Fout /= 100;
				freq_Fout *= 100;
			}
#endif

		/* Determine which PLL to use: CLK0 gets PLLA, CLK1 and CLK2 get PLLB */
		/* The first of CLK1 or CLK2 to be configured, determines the VCO frequency used for PLLB. */
		/* The second of CLK1 or CLK2 to be configured will attempt to achieve Fout by adjusting the */
		/* Multisynth Divider values only. */
		/* Only good for Si5351A3 variant */
		switch(clk)
		{
			case SI5351_CLK0:
			{
				enabledClocksMask |= 0x01;
				clock_ctrl_addr = 16;

				/* Block 1: Disable Outputs */
				/* Set CLKx_DIS high; Reg. 3 = 0xFF */
/*			si5351_write(3, ~enabledClocksMask | 0xF9); // only disable CLK0 */

				target_pll = SI5351_PLLA;
				clock_out[SI5351_CLK0] = freq_Fout; /* store the value for reference */
			}
			break;

			case SI5351_CLK1:
			{
				/* No checking is performed to ensure that PLLB is not unavailable due to other output being < 1.024 MHz or >= 112.5 MHz */
				/* User must ensure the clock design is valid before setting clocks */

				enabledClocksMask |= 0x02;
				clock_ctrl_addr = 17;

				/* Block 1: Disable Outputs */
				/* Set CLKx_DIS high; Reg. 3 = 0xFF */
				si5351_write(3, ~enabledClocksMask | 0xFA); /* only disable CLK1 */

				target_pll = SI5351_PLLB;
				clock_out[SI5351_CLK1] = freq_Fout;         /* store the value for reference */
			}
			break;

			case SI5351_CLK2:
			{
				/* No checking is performed to ensure that PLLB is not unavailable due to other output being < 1.024 MHz or >= 112.5 MHz */
				/* User must ensure the clock design is valid before setting clocks */

				enabledClocksMask |= 0x04;
				clock_ctrl_addr = 18;

				/* Block 1: Disable Outputs */
				/* Set CLKx_DIS high; Reg. 3 = 0xFF */
				si5351_write(3, ~enabledClocksMask | 0xFC); /* only disable CLK2 */

				target_pll = SI5351_PLLB;
				clock_out[SI5351_CLK2] = freq_Fout;         /* store the value for reference */
			}
			break;

			default:
			{
				return(TRUE);
			}
			break;
		}

		/* http://www.silabs.com/Support%20Documents/TechnicalDocs/Si5351-B.pdf */
		/* Page 19 */
		/* I2C Programming Procedure */
		/* */

		/* Block 1 handled above */

		/* Block 2: */
		/* Power down all output drivers */
		/* Reg. 16, 17, 18, 19, 20, 21, 22, 23 = 0x80 */
/*	si5351_write(clock_ctrl_addr, 0xCC); // power down only clock being set, leaving that clock configured as follows: */
		/*   o Drive strength = 2 mA */
		/*   o Input source = multisynth */
		/*   o Output clock not inverted */
		/*   o PLLA is multisynth source */
		/*   o Integer mode set */
		/*   o Clock powered down */
		/* If any of the above settings is incorrect, it needs to be set correctly in the code that follows. */

		/* Block 3: */
		/* Set interrupt masks */
		/* Ignore this for Si5153A */

		/* Block 4: */
		/* Write new configuration to device using */
		/* the contents of the register map */
		/* generated in software above. This */
		/* step also needs to power up the output drivers */
		/* (Registers 15-92 and 149-170 and 183) */

#ifdef DEBUGGING_ONLY
			Frequency_Hz freq_VCO_calc;
			Frequency_Hz fout_calc;
			int32_t f_err;
#endif

		if((target_pll == SI5351_PLLA) || !freqVCOB)
		{
#ifdef DEBUGGING_ONLY
				freq_VCO = multisynth_calc(freq_Fout, &ms_reg, &int_mode, &div_by_4, &div);
#else
				freq_VCO = multisynth_calc(freq_Fout, &ms_reg, &int_mode, &div_by_4);
#endif
		}
		else
		{
#ifdef DEBUGGING_ONLY
				fout_calc = freq_Fout;  /* save the intended output frequency */
#endif
			freq_Fout = multisynth_estimate(freq_Fout, &ms_reg, &int_mode, &div_by_4);
		}

		/* Set multisynth registers (MS must be set before PLL) */
		set_multisynth_registers_source(clk, target_pll);
		set_multisynth_registers(clk, ms_reg, int_mode, r_div, div_by_4);

		/* Set PLL if necessary */
#ifdef DEBUGGING_ONLY
			if(freq_VCO)
			{
				freq_VCO_calc = set_pll(freq_VCO, target_pll);
				fout_calc = freq_VCO_calc / div;
			}

			f_err = freq_Fout - fout_calc;
#else
			if(freq_VCO)
			{
				set_pll(freq_VCO, target_pll);
			}
#endif

		/* Block 5: */
		/* Apply PLLA or PLLB soft reset */
		/* Reg. 177 = 0xAC */
/*	pll_reset(target_pll); */

		/* Block 6: */
		/* Enable desired outputs */
		/* (see Register 3) */
		if(clocksOff)
		{
			si5351_write(3, enabledClocksMask);    /* disable clock(s) in use */
		}
		else
		{
			si5351_write(3, ~enabledClocksMask);    /* only enable clock(s) in use */
		}

		/* power up the clock */
		if(target_pll == SI5351_PLLA)
		{
			si5351_write(clock_ctrl_addr, 0x4C);    /* power up only clock being set, leaving that clock configured as follows: */
			/*   o Drive strength = 2 mA */
			/*   o Input source = multisynth */
			/*   o Output clock not inverted */
			/*   o PLLA is multisynth source */
			/*   o Integer mode set */
			/*   o Clock powered up */
		}
		else
		{
			if(int_mode)
			{
				si5351_write(clock_ctrl_addr, 0x6C);    /* power up only clock being set, leaving that clock configured as follows: */
				/*   o Drive strength = 2 mA */
				/*   o Input source = multisynth */
				/*   o Output clock not inverted */
				/*   o PLLB is multisynth source */
				/*   o Integer mode set */
				/*   o Clock powered up */
			}
			else
			{
				si5351_write(clock_ctrl_addr, 0x2C);    /* power up only clock being set, leaving that clock configured as follows: */
				/*   o Drive strength = 2 mA */
				/*   o Input source = multisynth */
				/*   o Output clock not inverted */
				/*   o PLLB is multisynth source */
				/*   o Integer mode cleared */
				/*   o Clock powered up */
			}

			if(freq_VCO)
			{
				freqVCOB = freq_VCO;
			}
		}

		return(FALSE);
	}


/*
 * Frequency_Hz si5351_get_freq(Si5351_clock output)
 *
 * Returns the clock frequency setting of the specified CLK output
 *
 * output - Clock output (use the si5351_clock enum)
 *
 * Returns TRUE on failure
 *
 */
	Frequency_Hz si5351_get_frequency(Si5351_clock clock)
	{
		return(clock_out[clock]);
	}


/*
 * si5351_clock_enable(Si5351_clock clk, BOOL enable)
 *
 * Enable or disable a chosen clock
 * clk - Clock output
 * enable - 1 to enable, 0 to disable
 *
 */
	EC si5351_clock_enable(Si5351_clock clk, BOOL enable)
	{
		uint8_t reg_val;

		if(si5351_read(SI5351_OUTPUT_ENABLE_CTRL, &reg_val)) return ERROR_CODE_RTC_NONRESPONSIVE;

		if(enable)
		{
			reg_val &= ~(1 << (uint8_t)clk);
		}
		else
		{
			reg_val |= (1 << (uint8_t)clk);
		}

		if(si5351_write(SI5351_OUTPUT_ENABLE_CTRL, reg_val)) return ERROR_CODE_RTC_NONRESPONSIVE;

		return ERROR_CODE_NO_ERROR;
	}


/*
 * si5351_drive_strength(Si5351_clock clk, Si5351_drive drive)
 *
 * Sets the drive strength of the specified clock output
 *
 * clk - Clock output
 * drive - Desired drive level
 *
 */
	EC si5351_drive_strength(Si5351_clock clk, Si5351_drive drive)
	{
		uint8_t reg_val;
		const uint8_t mask = 0x03;

		if(si5351_read(SI5351_CLK0_CTRL + (uint8_t)clk, &reg_val))
		{
			return ERROR_CODE_CLKGEN_NONRESPONSIVE;
		}

		switch(drive)
		{
			case SI5351_DRIVE_2MA:
			{
				reg_val &= ~(mask);
				reg_val |= 0x00;
			}
			break;

			case SI5351_DRIVE_4MA:
			{
				reg_val &= ~(mask);
				reg_val |= 0x01;
			}
			break;

			case SI5351_DRIVE_6MA:
			{
				reg_val &= ~(mask);
				reg_val |= 0x02;
			}
			break;

			case SI5351_DRIVE_8MA:
			{
				reg_val &= ~(mask);
				reg_val |= 0x03;
			}
			break;

			default:
			{
			}
			break;
		}

		if(si5351_write(SI5351_CLK0_CTRL + (uint8_t)clk, reg_val)) return ERROR_CODE_CLKGEN_NONRESPONSIVE;

		return ERROR_CODE_NO_ERROR;
	}

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// */
/* Private functions */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

/*
 * pll_reset(Si5351_pll target_pll)
 *
 * target_pll - Which PLL to reset (use the si5351_pll enum)
 *
 * Apply a reset to the indicated PLL(s).
 *
 */
	void pll_reset(Si5351_pll target_pll)
	{
		if(target_pll & SI5351_PLLA)
		{
			si5351_write(SI5351_PLL_RESET, SI5351_PLL_RESET_A);
		}

		if(target_pll & SI5351_PLLB)
		{
			si5351_write(SI5351_PLL_RESET, SI5351_PLL_RESET_B);
		}
	}

/*
 * uint8_t select_r_div(Frequency_Hz *freq)
 *
 * The R dividers can be used to generate frequencies below about 500 kHz. Each individual output R divider can be
 * set to 1, 2, 4, 8,....128 by writing the proper setting for Rx_DIV. Set this parameter to generate frequencies down to
 * 8kHz.
 */
#ifdef SUPPORT_FOUT_BELOW_1024KHZ
		uint8_t select_r_div(Frequency_Hz *freq)
		{
			uint8_t r_div = SI5351_OUTPUT_CLK_DIV_1;

			Frequency_Hz max_freq = SI5351_CLKOUT_MIN_FREQ * 128;

			if(*freq < max_freq)
			{
				/* Choose the correct R divider for output frequencies between 8 kHz and 1.024 MHz */
				if((*freq >= SI5351_CLKOUT_MIN_FREQ) && (*freq < SI5351_CLKOUT_MIN_FREQ * 2))
				{
					r_div = SI5351_OUTPUT_CLK_DIV_128;
					*freq *= 128UL;
				}
				else if((*freq >= SI5351_CLKOUT_MIN_FREQ * 2) && (*freq < SI5351_CLKOUT_MIN_FREQ * 4))
				{
					r_div = SI5351_OUTPUT_CLK_DIV_64;
					*freq *= 64UL;
				}
				else if((*freq >= SI5351_CLKOUT_MIN_FREQ * 4) && (*freq < SI5351_CLKOUT_MIN_FREQ * 8))
				{
					r_div = SI5351_OUTPUT_CLK_DIV_32;
					*freq *= 32UL;
				}
				else if((*freq >= SI5351_CLKOUT_MIN_FREQ * 8) && (*freq < SI5351_CLKOUT_MIN_FREQ * 16))
				{
					r_div = SI5351_OUTPUT_CLK_DIV_16;
					*freq *= 16UL;
				}
				else if((*freq >= SI5351_CLKOUT_MIN_FREQ * 16) && (*freq < SI5351_CLKOUT_MIN_FREQ * 32))
				{
					r_div = SI5351_OUTPUT_CLK_DIV_8;
					*freq *= 8UL;
				}
				else if((*freq >= SI5351_CLKOUT_MIN_FREQ * 32) && (*freq < SI5351_CLKOUT_MIN_FREQ * 64))
				{
					r_div = SI5351_OUTPUT_CLK_DIV_4;
					*freq *= 4UL;
				}
				else if((*freq >= SI5351_CLKOUT_MIN_FREQ * 64) && (*freq < SI5351_CLKOUT_MIN_FREQ * 128))
				{
					r_div = SI5351_OUTPUT_CLK_DIV_2;
					*freq *= 2UL;
				}
			}

			return(r_div);
		}
#endif  /*#ifdef SUPPORT_FOUT_BELOW_1024KHZ */


/*
 * set_pll(Frequency_Hz freq_VCO, Si5351_pll target_pll)
 *
 * Set the specified PLL to a specific oscillation frequency
 *
 * freq_VCO - Desired PLL frequency
 * target_pll - Which PLL to set (use the si5351_pll enum)
 *
 */
#ifdef DEBUGGING_ONLY
		uint32_t set_pll(Frequency_Hz freq_VCO, Si5351_pll target_pll)
#else
		void set_pll(Frequency_Hz freq_VCO, Si5351_pll target_pll)
#endif
	{
		Union_si5351_regs pll_reg;
		uint8_t params[10];

		/* Output Multisynth Settings (Synthesis Stage 2) */
#ifdef DEBUGGING_ONLY
			Frequency_Hz result = pll_calc(freq_VCO, &pll_reg, g_si5351_ref_correction);
			Frequency_Hz pll_error = freq_VCO - result;
#else
	#ifdef APPLY_XTAL_CALIBRATION_VALUE
		pll_calc(freq_VCO, &pll_reg, g_si5351_ref_correction);
	#else
		pll_calc(freq_VCO, &pll_reg);
	#endif

#endif

		/* Derive the register values to write */

		/* Prepare an array for parameters to be written to */
		uint8_t i = 0;

		/* Registers 26-27 */
		params[i++] = pll_reg.reg.p3_1;
		params[i++] = pll_reg.reg.p3_0;

		/* Register 28 */
		params[i++] = pll_reg.reg.p1_2 & 0x03;

		/* Registers 29-30 */
		params[i++] = pll_reg.reg.p1_1;
		params[i++] = pll_reg.reg.p1_0;

		/* Register 31 */
		params[i] = pll_reg.reg.p3_2 << 4;
		params[i++] += pll_reg.reg.p2_2 & 0x0F;

		/* Registers 32-33 */
		params[i++] = pll_reg.reg.p2_1;
		params[i++] = pll_reg.reg.p2_0;

		/* Write the parameters */
		if(target_pll == SI5351_PLLA)
		{
			si5351_write_bulk(SI5351_PLLA_PARAMETERS, i, params);
		}
		else    /* if(target_pll == SI5351_PLLB) */
		{
			si5351_write_bulk(SI5351_PLLB_PARAMETERS, i, params);
		}

#ifdef DEBUGGING_ONLY
			return(result);
#endif
	}


#ifdef SUPPORT_STATUS_READS
/*
 * si5351_read_status(void)
 *
 * Call this to read the status structs, then access them
 * via the dev_status and dev_int_status global variables.
 *
 * See the header file for the struct definitions. These
 * correspond to the flag names for registers 0 and 1 in
 * the Si5351 datasheet.
 */
		void si5351_read_status(void)
		{
			si5351_read_sys_status(&dev_status);
			si5351_read_int_status(&dev_int_status);
		}
#endif


/*
 * si5351_set_correction(int32_t corr)
 *
 * Use this to set the oscillator correction factor. Caller
 * is responsible for storing an EEPROM value for this
 * if it is to be saved to non-volatile memory.
 *
 * This value is a signed 32-bit integer of the
 * parts-per-10 million value that the actual oscillation
 * frequency deviates from the specified frequency.
 *
 * Any desired test frequency within the normal range of the
 * Si5351 should be set, then the actual output frequency
 * should be measured as accurately as possible. The
 * difference between the measured and specified frequencies
 * should be calculated in Hertz, then multiplied by 10 in
 * order to get the parts-per-10 million value.
 *
 * Since the Si5351 itself has an intrinsic 0 PPM error, this
 * correction factor is good across the entire tuning range of
 * the Si5351. Once this calibration is done accurately, it
 * should not have to be done again for the same Si5351 and
 * crystal. The library will read the correction factor from
 * EEPROM during initialization for use by the tuning
 * algorithms.
 */
	void si5351_set_correction(int32_t corr)
	{
		g_si5351_ref_correction = corr;
	}


/*
 * si5351_get_correction(void)
 *
 * Returns the oscillator correction factor.
 */
	int32_t si5351_get_correction(void)
	{
		return(g_si5351_ref_correction);
	}


/*
 * BOOL pll_calc(Frequency_Hz vco_freq, Union_si5351_regs *reg, int32_t correction)
 *
 * Returns TRUE on failure
 *
 */
#ifdef DEBUGGING_ONLY
	Frequency_Hz pll_calc(Frequency_Hz vco_freq, Union_si5351_regs *reg, int32_t correction)
#else
	#ifdef APPLY_XTAL_CALIBRATION_VALUE
		BOOL pll_calc(Frequency_Hz vco_freq, Union_si5351_regs *reg, int32_t correction)
	#else
		BOOL pll_calc(Frequency_Hz vco_freq, Union_si5351_regs *reg)
	#endif
#endif
	{
#ifdef DEBUGGING_ONLY
			Frequency_Hz result = 0;
#endif
		Frequency_Hz ref_freq = xtal_freq;
		uint32_t a, b, c;

#ifdef APPLY_XTAL_CALIBRATION_VALUE
			/* Factor calibration value into nominal crystal frequency */
			/* Measured in parts-per-billion */
			if(correction)
			{
				ref_freq += (int32_t)((((((int64_t)correction) << 31) / 1000000000LL) * ref_freq) >> 31);
			}
#endif

#ifdef DO_BOUNDS_CHECKING
			if(vco_freq < SI5351_PLL_VCO_MIN)
			{
				return(TRUE);
			}
			if(vco_freq > SI5351_PLL_VCO_MAX)
			{
				return(TRUE);
			}
#endif

		/* Determine integer part of feedback equation */
		a = vco_freq / ref_freq;

#ifdef DO_BOUNDS_CHECKING
			if(a < SI5351_PLL_A_MIN)
			{
				return(TRUE);
			}
			if(a > SI5351_PLL_A_MAX)
			{
				return(TRUE);
			}
#endif

		/* Find best approximation for b/c = fVCO mod fIN */
		b = vco_freq % ref_freq;
		c = ref_freq;

		reduce_by_gcd(&b, &c);

		uint32_t bx128 = b << 7;
		uint32_t bx128overc = bx128 / c;
		reg->ms.p1 = (uint32_t)((a << 7) + bx128overc) - 512;   /* 128 * a + floor((128 * b) / c) - 512 */
		reg->ms.p2 = (uint32_t)bx128 - (c * bx128overc);        /* 128 * b - c * floor((128 * b) / c) */
		reg->ms.p3 = c;

#ifdef DEBUGGING_ONLY

			/* Recalculate frequency as fIN * (a + b/c) */
			if(a)
			{
				uint64_t temp = (uint64_t)ref_freq * (uint64_t)b;
				temp /= c;
				temp += ref_freq * a;
				result = temp;
			}

			return(result);

#else

			return(FALSE);

#endif
	}

/*
 * uint32_t calc_gcd(uint32_t m, uint32_t n)
 *
 * Simple implementation of Euclid's Algorithm for calculating GCD of two uint32's
 *
 */
	void reduce_by_gcd(uint32_t *m, uint32_t *n)
	{
		uint32_t r;
		uint32_t b = *m;
		uint32_t c = *n;

		if(!b || !c)
		{
			return;
		}

		for(r = b % c; r; b = c, c = r, r = b % c)
		{
			;
		}

		if(c > 1)
		{
			*m /= c;
			*n /= c;
		}

		return;
	}


/*
 * Frequency_Hz multisynth_calc(Frequency_Hz freq_Fout, Union_si5351_regs *reg, BOOL *int_mode, BOOL *divBy4)
 *
 * Valid Multisynth divider ratios are 4, 6, 8, and any fractional value between 8 + 1/1,048,575 and 900 + 0/1.
 * This means that if any output is greater than 112.5 MHz (900 MHz/8), then this output frequency sets one
 * of the VCO frequencies.
 *
 * Notes: This implementation only supports even integer divider ratios 4 <= div <= 900.
 *        This implementation always sets *int_mode to TRUE
 *
 * Returns zero on failure. Returns with int_mode and divBy4 set appropriately.
 *
 */
#ifdef DEBUGGING_ONLY
		Frequency_Hz multisynth_calc(Frequency_Hz freq_Fout, Union_si5351_regs *reg, BOOL *int_mode, BOOL *divBy4, uint32_t *div)
#else
		Frequency_Hz multisynth_calc(Frequency_Hz freq_Fout, Union_si5351_regs *reg, BOOL *int_mode, BOOL *divBy4)
#endif
	{
		uint32_t a = 0;
		Frequency_Hz freq_VCO = 0;

		*int_mode = TRUE;   /* assumed */

#ifdef DO_BOUNDS_CHECKING
			/* Multisynth bounds checking */
			if(freq_Fout > SI5351_MULTISYNTH_MAX_FREQ)
			{
				return(0);
			}
			if(freq_Fout < SI5351_MULTISYNTH_MIN_FREQ)
			{
				return(0);
			}
#endif  /* DO_BOUNDS_CHECKING */

		/* All frequencies above 150 MHz must use divide by 4 */
		if(freq_Fout >= SI5351_MULTISYNTH_DIVBY4_FREQ)
		{
			a = 4;
			freq_VCO = a * freq_Fout;
		}
		else
		{
			uint32_t temp;
			uint8_t done = FALSE;
			uint8_t success = FALSE;
			uint8_t count = 0;

			/* Find a VCO frequency that is an even integer multiple of the desired Fout frequency */
			while(!done)
			{
				temp = SI5351_PLL_VCO_MAX - (count * freq_Fout);    /* SI5351_PLL_VCO_MAX assumed even */
				count += 2;

				if(temp >= SI5351_PLL_VCO_MIN)
				{
					temp /= freq_Fout;

					if(temp >= 4)   /* accepts only even integers of 4 or greater */
					{
						done = TRUE;
						success = TRUE;
						a = temp;
					}
				}
				else
				{
					done = TRUE;
				}
			}

			if(success)
			{
				freq_VCO = a * freq_Fout;
			}
		}

		*divBy4 = (a == 4);
		reg->ms.p1 = (uint32_t)(a << 7) - 512;  /* 128 * a + floor((128 * b) / c) - 512 */
		reg->ms.p2 = 0;                         /* 128 * b - c * floor((128 * b) / c) */
		reg->ms.p3 = 1;

#ifdef DEBUGGING_ONLY
			*div = a;
#endif

		return(freq_VCO);
	}


/*
 * void si5351_set_vcoB_freq(Frequency_Hz freq_VCO)
 *
 * This function provides the ability to specify a particular VCO frequency to be used for the target PLL,
 * allowing a specific clock design (calculated by ClockBuilder for instance) to be implemented.
 *
 * Currently this only works for PLLB.
 *
 */
	void si5351_set_vcoB_freq(Frequency_Hz freq_VCO)
	{
		freqVCOB = freq_VCO;
		set_pll(freq_VCO, SI5351_PLLB);
		return;
	}

/*
 * Frequency_Hz multisynth_estimate(Frequency_Hz freq_Fout, Union_si5351_regs *reg, BOOL *int_mode, BOOL *divBy4)
 *
 * Note: do not call this function with global value freqVCOB == zero
 */
	Frequency_Hz multisynth_estimate(Frequency_Hz freq_Fout, Union_si5351_regs *reg, BOOL *int_mode, BOOL *divBy4)
	{
		uint32_t a, b, c;

#ifdef DO_BOUNDS_CHECKING
			if(freqVCOB < 600000000)
			{
				return(0);
			}

			if(freqVCOB > 900000000)
			{
				return(0);
			}

			if(freq_Fout > SI5351_MULTISYNTH_MAX_FREQ)
			{
				return(0);
			}

			if(freq_Fout < SI5351_MULTISYNTH_MIN_FREQ)
			{
				return(0);
			}
#endif  /* DO_BOUNDS_CHECKING */

		/* Determine integer part of feedback equation */
		a = freqVCOB / freq_Fout;
		b = freqVCOB % freq_Fout;
		c = freq_Fout;
		reduce_by_gcd(&b, &c);  /* prevents overflow conditions and makes results agree with ClockBuilder */

		/* Calculate the approximated output frequency given by fOUT = fvco / (a + b/c) */
		freq_Fout = freqVCOB;
		freq_Fout /= (a * c + b);
		freq_Fout *= c;

		*int_mode = (b == 0) && !(a % 2);
		*divBy4 = (a == 4) && *int_mode;

		/* Calculate parameters */
		if(*divBy4)
		{
			reg->ms.p1 = 0;
			reg->ms.p2 = 0;
			reg->ms.p3 = 1;
		}
		else
		{
			uint32_t bx128 = b << 7;
			uint32_t bx128overc = bx128 / c;
			reg->ms.p1 = (uint32_t)((a << 7) + bx128overc) - 512;   /* 128 * a + floor((128 * b) / c) - 512 */
			reg->ms.p2 = (uint32_t)bx128 - (c * bx128overc);        /* 128 * b - c * floor((128 * b) / c) */
			reg->ms.p3 = c;
		}

		return(freq_Fout);
	}


	BOOL si5351_write_bulk(uint8_t addr, uint8_t bytes, uint8_t *data)
	{
		return(i2c_device_write(SI5351_I2C_SLAVE_ADDR, addr, data, bytes));
	}

	BOOL si5351_write(uint8_t addr, uint8_t data)
	{
		return(i2c_device_write(SI5351_I2C_SLAVE_ADDR, addr, &data, 1));
	}

	BOOL si5351_read(uint8_t addr, uint8_t *data)
	{
		return(i2c_device_read(SI5351_I2C_SLAVE_ADDR, addr, data, 1));
	}


#ifdef SUPPORT_STATUS_READS
		BOOL si5351_read_sys_status(Si5351Status *status)
		{
			uint8_t reg_val = 0;

			if(si5351_read(SI5351_DEVICE_STATUS, &reg_val))
			{
				return(TRUE);
			}

			/* Parse the register */
			status->SYS_INIT = (reg_val >> 7) & 0x01;
			status->LOL_B = (reg_val >> 6) & 0x01;
			status->LOL_A = (reg_val >> 5) & 0x01;
			status->LOS = (reg_val >> 4) & 0x01;
			status->REVID = reg_val & 0x03;

			return(FALSE);
		}


		BOOL si5351_read_int_status(Si5351IntStatus *status)
		{
			uint8_t reg_val = 0;

			if(si5351_read(SI5351_DEVICE_STATUS, &reg_val))
			{
				return(TRUE);
			}

			/* Parse the register */
			status->SYS_INIT_STKY = (reg_val >> 7) & 0x01;
			status->LOL_B_STKY = (reg_val >> 6) & 0x01;
			status->LOL_A_STKY = (reg_val >> 5) & 0x01;
			status->LOS_STKY = (reg_val >> 4) & 0x01;

			return(FALSE);
		}
#endif  /* #ifdef SUPPORT_STATUS_READS */


/*
 * set_multisynth_registers_source(Si5351_clock clk, Si5351_pll pll)
 *
 * clk - Clock output (use the si5351_clock enum)
 * pll - Which PLL to use as the source (use the si5351_pll enum)
 *
 * Set the desired PLL source for a multisynth.
 *
 */
	void set_multisynth_registers_source(Si5351_clock clk, Si5351_pll pll)
	{
		uint8_t reg_val;
		uint8_t addr = SI5351_CLK0_CTRL + (uint8_t)clk;

		if(si5351_read(addr, &reg_val))
		{
			return;
		}

		if(pll == SI5351_PLLA)
		{
			reg_val &= ~(SI5351_CLK_PLL_SELECT);
		}
		else if(pll == SI5351_PLLB)
		{
			reg_val |= SI5351_CLK_PLL_SELECT;
		}

		si5351_write(addr, reg_val);
	}


/*
 * set_multisynth_registers(Si5351_clock clk, Union_si5351_regs ms_reg, BOOL int_mode, uint8_t r_div, BOOL div_by_4)
 *
 * Set the specified multisynth parameters.
 *
 * clk - Clock output (use the si5351_clock enum)
 * int_mode - 1 to enable, 0 to disable
 * r_div - Desired r_div ratio
 * div_by_4 - 1 Divide By 4 mode: 0 to disable
 *
 */
	void set_multisynth_registers(Si5351_clock clk, Union_si5351_regs ms_reg, BOOL int_mode, uint8_t r_div, BOOL div_by_4)
	{
		uint8_t params[10];
		uint8_t i = 0;
		uint8_t reg_val;

		/* Registers 42-43 for CLK0; 50-51 for CLK1 */
		params[i++] = ms_reg.reg.p3_1;
		params[i++] = ms_reg.reg.p3_0;

		/* Register 44 for CLK0; 52 for CLK1 */
		if(si5351_read((SI5351_CLK0_PARAMETERS + 2) + (clk * 8), &reg_val))
		{
			return;
		}

		reg_val &= 0xFC;    /*~(0x03); */
		params[i++] = reg_val | (ms_reg.reg.p1_2 & 0x03);

		/* Registers 45-46 for CLK0 */
		params[i++] = ms_reg.reg.p1_1;
		params[i++] = ms_reg.reg.p1_0;

		/* Register 47 for CLK0 */
		params[i] = (ms_reg.reg.p3_2 << 4);
		params[i++] += (ms_reg.reg.p2_2 & 0x0F);

		/* Registers 48-49 for CLK0 */
		params[i++] = ms_reg.reg.p2_1;
		params[i++] = ms_reg.reg.p2_0;

		/* Write the parameters */
		switch(clk)
		{
			case SI5351_CLK0:
			{
				si5351_write_bulk(SI5351_CLK0_PARAMETERS, i, params);
			}
			break;

			case SI5351_CLK1:
			{
				si5351_write_bulk(SI5351_CLK1_PARAMETERS, i, params);
			}
			break;

			case SI5351_CLK2:
			{
				si5351_write_bulk(SI5351_CLK2_PARAMETERS, i, params);
			}
			break;

			default:
			{
			}
			break;
		}

		set_integer_mode(clk, int_mode);
		ms_div(clk, r_div, div_by_4);
	}


/*
 * set_integer_mode(Si5351_clock clk, BOOL int_mode)
 *
 * clk - Clock output (use the si5351_clock enum)
 * enable - 1 to enable, 0 to disable
 *
 * Set the indicated multisynth into integer mode.
 */
	void set_integer_mode(Si5351_clock clk, BOOL enable)
	{
		uint8_t reg_val;

		if(si5351_read(SI5351_CLK0_CTRL + (uint8_t)clk, &reg_val))
		{
			return;
		}

		if(enable)
		{
			reg_val |= (SI5351_CLK_INTEGER_MODE);
		}
		else
		{
			reg_val &= ~(SI5351_CLK_INTEGER_MODE);
		}

		si5351_write(SI5351_CLK0_CTRL + (uint8_t)clk, reg_val);
	}


	void ms_div(Si5351_clock clk, uint8_t r_div, BOOL div_by_4)
	{
		uint8_t reg_val, reg_addr;

		switch(clk)
		{
			case SI5351_CLK0:
			{
				reg_addr = SI5351_CLK0_PARAMETERS + 2;
			}
			break;

			case SI5351_CLK1:
			{
				reg_addr = SI5351_CLK1_PARAMETERS + 2;
			}
			break;

			case SI5351_CLK2:
			{
				reg_addr = SI5351_CLK2_PARAMETERS + 2;
			}
			break;

			default:
				return;
		}

		if(si5351_read(reg_addr, &reg_val))
		{
			return;
		}

		/* Clear the appropriate bits */
		reg_val &= ~(0x7c);

		if(div_by_4)
		{
			reg_val |= (SI5351_OUTPUT_CLK_DIVBY4);
		}
		else
		{
			reg_val &= ~(SI5351_OUTPUT_CLK_DIVBY4);
		}

		reg_val |= (r_div << SI5351_OUTPUT_CLK_DIV_SHIFT);

		si5351_write(reg_addr, reg_val);
	}

#endif  /* #ifdef INCLUDE_SI5351_SUPPORT */
