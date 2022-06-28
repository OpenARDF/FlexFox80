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


#include "dac0.h"
#include "tcb.h"

static void VREF_init(void);


/* DAC Value */
#define DAC_DEFAULT_VALUE        (0x000)
/* VREF start-up time */
#define VREF_STARTUP_TIME_MS        (10)
/* Mask needed to get the 2 LSb for DAC Data Register */
#define LSB_MASK                 (0x03)



static void VREF_init(void)
{
//	VREF.DAC0REF = VREF_REFSEL_VDD_gc; /* Select the 2.048V Internal Voltage Reference for DAC */
//	VREF.DAC0REF = VREF_REFSEL_2V048_gc; /* Select the 2.048V Internal Voltage Reference for DAC */
	VREF.DAC0REF = VREF_REFSEL_2V500_gc; /* Select the 2.500V Internal Voltage Reference for DAC */
//	| VREF_ALWAYSON_bm;    /* Set the Voltage Reference in Always On mode */
//	VREF.DAC0REF = VREF_REFSEL_2V500_gc; /* Select the 2.048V Internal Voltage Reference for DAC */
	/* Wait VREF start-up time */
	while(util_delay_ms(VREF_STARTUP_TIME_MS));
}

void DAC0_init(void)
{
	VREF_init();
	
    /* Disable digital input buffer */
    PORTD.PIN6CTRL &= ~PORT_ISC_gm;
    PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    /* Disable pull-up resistor */
    PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;   
    DAC0.CTRLA = DAC_ENABLE_bm          /* Enable DAC */
               | DAC_OUTEN_bm           /* Enable output buffer */
               | DAC_RUNSTDBY_bm;       /* Enable Run in Standby mode */

// 	/* Disable digital input buffer */
// 	PORTD.PIN6CTRL &= ~PORT_ISC_gm;
// 	PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
// 	/* Disable pull-up resistor */
// 	PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;
// 	DAC0.CTRLA = DAC_ENABLE_bm          /* Enable DAC */
// 	| DAC_OUTEN_bm;           /* Enable output buffer */
// 	
// 	DAC0_setVal(DAC_DEFAULT_VALUE);
}

void DAC0_setVal(uint16_t value)
{
	/* Store the two LSbs in DAC0.DATAL */
	DAC0.DATAL = (value & LSB_MASK) << 6;
	/* Store the eight MSbs in DAC0.DATAH */
	DAC0.DATAH = value >> 2;
}
