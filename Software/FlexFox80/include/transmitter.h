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
 *
 *
 * transmitter.h
 *
 */


#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#include "defs.h"
#include "include/si5351.h"

typedef int16_t Attenuation;

#define MAX_TX_POWER_80M_MW (uint16_t)5000

/*
 * Define clock pins
 */
#define TX_CLOCK_VHF_FM SI5351_CLK2
#define TX_CLOCK_HF_0 SI5351_CLK1
#define TX_CLOCK_VHF SI5351_CLK0

#define BIAS_DELAY_PERIOD_TICKS 100

#define EEPROM_TX_80M_FREQUENCY_DEFAULT 3540000
#define EEPROM_RTTY_OFFSET_FREQUENCY_DEFAULT 170
#define EEPROM_TX_80M_POWER_MW_DEFAULT 500

#define MAX_80M_PWR_SETTING 255 /* maximum safe DAC setting */

#define BUCK_9V 175
#define BUCK_8V 150
#define BUCK_7V 125
#define BUCK_6V 100
#define BUCK_5V 75
#define BUCK_0V 0
                                               /*  0,10,100,200,300,400,500,600,800,1000,1500,2000,2500,3000,4000,5000 */
#define DEFAULT_80M_POWER_TABLE {0, 2, 20, 40, 54, 62, 70, 78, 91, 100, 130, 155, 180, 200, 245, 254}

#define TX_MINIMUM_80M_FREQUENCY (uint32_t)3500000
#define TX_MAXIMUM_80M_FREQUENCY (uint32_t)4000000

/**
 */
	EC init_transmitter(void);

/**
 */
	EC txSetParameters(uint16_t* power_mW, bool* enableDriverPwr);

/**
 */
	bool txSetFrequency(Frequency_Hz *freq, bool leaveClockOff);

/**
 */
	Frequency_Hz txGetFrequency(void);
	
/**
 */
	uint16_t txGetPowerMw(void);


/**
 */
void keyTransmitter(bool on);

/**
 */
EC powerToTransmitter(bool on);

/**
 */
EC txMilliwattsToSettings(uint16_t* powerMW, uint8_t* powerLevel, uint8_t* modLevelHigh, uint8_t* modLevelLow);

/**
Returns true if an antenna for the active band is connected to the transmitter
 */
bool txIsAntennaForBand(void);


#endif  /* TRANSMITTER_H_ */