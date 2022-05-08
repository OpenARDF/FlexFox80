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

#include "defs.h"
#include "adc.h"
#include <avr/io.h>
#include <stdbool.h>
#include <driver_init.h>
#include <compiler.h>
#include "Goertzel.h"

#define SAMPLE_RATE 24096
#define Goertzel_N 201
const int N = Goertzel_N;
const float threshold = 500000. * (Goertzel_N / 100);
const float sampling_freq = SAMPLE_RATE;
const float x_frequencies[4] = { 1209., 1336., 1477., 1633. };
const float y_frequencies[4] = { 697., 770., 852., 941. };
	
#define FREE_RUNNING true
#define SINGLE_CONVERSION false
	
volatile int16_t g_adcVal;
Goertzel g_goertzel(N, sampling_freq);

static void PORT_init(void);
static void VREF0_init(void);
static void ADC0_init(bool freerun);
static void ADC0_SYSTEM_init(bool freerun);
static void ADC0_SYSTEM_shutdown(void);

typedef enum {
	ADC_NOT_INITIALIZED,
	ADC_FREE_RUN_INITIALIZED,
	ADC_SINGLE_CONVERSION_INITIALIZED
	} ADC_Init_t;
	
static ADC_Init_t g_adc_initialization = ADC_NOT_INITIALIZED;

void ADC0_setADCChannel(ADC_Active_Channel_t chan)
{
	switch(chan)
	{
		case ADCAudioInput:
		{
			if(g_adc_initialization != ADC_FREE_RUN_INITIALIZED)
			{
				ADC0_SYSTEM_init(FREE_RUNNING); 
			}
			
			ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
		}
		break;
		
		case ADCExternalBatteryVoltage:
		{
			if(g_adc_initialization != ADC_SINGLE_CONVERSION_INITIALIZED)
			{
				ADC0_SYSTEM_init(SINGLE_CONVERSION);
			}
			
			ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;
		}
		break;
		
		case ADC12VRegulatedVoltage:
		{
			if(g_adc_initialization != ADC_SINGLE_CONVERSION_INITIALIZED)
			{
				ADC0_SYSTEM_init(SINGLE_CONVERSION);
			}
			
			ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc;
		}
		break;
		
		case ADCTXAdjustableVoltage:
		{
			if(g_adc_initialization != ADC_SINGLE_CONVERSION_INITIALIZED)
			{
				ADC0_SYSTEM_init(SINGLE_CONVERSION);
			}
			
			ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
		}
		break;
		
		case ADCTemperature:
		{
			if(g_adc_initialization != ADC_SINGLE_CONVERSION_INITIALIZED)
			{
				ADC0_SYSTEM_init(SINGLE_CONVERSION);
			}
			
			ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
		}
		break;
		
		default:
		{
			ADC0_SYSTEM_shutdown();
		}
		break;	
	}
}

void ADC0_startConversion(void)
{
	if(g_adc_initialization != ADC_NOT_INITIALIZED)
	{
		ADC0.INTCTRL = 0x00; /* Disable interrupt */
		ADC0.COMMAND = ADC_STCONV_bm; /* Start conversion */
	}
}

bool ADC0_conversionDone(void)
{
	/* Check if the conversion is done  */
	return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}

int ADC0_read(void)
{
	return ADC0.RES; 	/* Reading the result also clears the interrupt flag */
}

int16_t temperatureC()
{
	uint16_t sigrow_offset = SIGROW.TEMPSENSE1; // Read unsigned value from signature row
	uint16_t sigrow_slope = SIGROW.TEMPSENSE0; // Read unsigned value from signature row
	static uint32_t wait = 100000;
	uint16_t adc_reading;
	int16_t temperature_in_C = -273.15;
	uint8_t holdMux;
	
	holdMux = ADC0.MUXPOS;
	ADC0_SYSTEM_init(SINGLE_CONVERSION);
	ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
	ADC0_startConversion();
	
	while((!ADC0_conversionDone()) && wait--);
	
	if(wait)
	{
		adc_reading = ADC0.RES;
		uint32_t temp = sigrow_offset - adc_reading;
		temp *= sigrow_slope; // Result will overflow 16-bit variable
		temp += 0x0800; // Add 4096/2 to get correct rounding on division below
		temp >>= 12; // Round off to nearest degree in Kelvin, by dividing with 2^12 (4096)
		temperature_in_C += temp;
	}
	
	ADC0.MUXPOS = holdMux; /* Restore ADC registers */
	
	return(temperature_in_C);
}


static void PORT_init(void)
{
	/* Disable interrupt and digital input buffer on PD2 */
	PORTD.PIN2CTRL &= ~PORT_ISC_gm;
	PORTD.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	/* Disable interrupt and digital input buffer on PD3 */
	PORTD.PIN3CTRL &= ~PORT_ISC_gm;
	PORTD.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	/* Disable interrupt and digital input buffer on PD4 */
	PORTD.PIN4CTRL &= ~PORT_ISC_gm;
	PORTD.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	/* Disable interrupt and digital input buffer on PD5 */
	PORTD.PIN5CTRL &= ~PORT_ISC_gm;
	PORTD.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	
	/* Disable pull-up resistor */
	PORTD.PIN2CTRL &= ~PORT_PULLUPEN_bm;
	PORTD.PIN3CTRL &= ~PORT_PULLUPEN_bm;
	PORTD.PIN4CTRL &= ~PORT_PULLUPEN_bm;
	PORTD.PIN5CTRL &= ~PORT_PULLUPEN_bm;
}

static void VREF0_init(void)
{
	VREF.ADC0REF = VREF_REFSEL_2V048_gc;  /* Internal 2.048V reference */
}

static void ADC0_init(bool freerun)
{
	ADC0.CTRLC = ADC_PRESC_DIV64_gc;   /* CLK_PER divided by 4 => 24096 sps */
	
	if(freerun)
	{
		ADC0.CTRLA = ADC_ENABLE_bm /* ADC Enable: enabled */
		| ADC_RESSEL_12BIT_gc      /* 12-bit mode */
		| ADC_FREERUN_bm;          /* Enable Free-Run mode */
		
		ADC0.INTCTRL = 0x01; /* Enable interrupt */
		
		ADC0.COMMAND = ADC_STCONV_bm; /* Start conversion */
		g_adc_initialization = ADC_FREE_RUN_INITIALIZED;
	}
	else
	{
		ADC0.CTRLA = ADC_ENABLE_bm;  /* ADC Enable: enabled; 12-bit mode is default */
		ADC0.INTCTRL = 0x00; /* Disable interrupt */
		g_adc_initialization = ADC_SINGLE_CONVERSION_INITIALIZED;
	}
}

static void ADC0_SYSTEM_init(bool freerun)
{
	PORT_init();
	VREF0_init();
	ADC0_init(freerun);
}

static void ADC0_SYSTEM_shutdown(void)
{
	ADC0.INTCTRL = 0x00; /* Disable interrupt */
	ADC0.CTRLA = ADC_RESSEL_12BIT_gc; /* Turn off ADC leaving 12-bit resolution set */
	g_adc_initialization = ADC_NOT_INITIALIZED;
}

ISR(ADC0_RESRDY_vect)
{
	/* Clear the interrupt flag by reading the result */
	int val = ADC0_read();
//	LED_toggle_level();
	if(g_goertzel.DataPoint(val))
	{
		ADC0.INTCTRL = 0x00; /* disable ADC interrupt */
	}
}