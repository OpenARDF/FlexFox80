/*
 * adc.c
 *
 * Created: 11/21/2021 3:52:44 PM
 *  Author: charl
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
	
static bool g_adc_initialized = false;
#define FREE_RUNNING true
#define SINGLE_CONVERSION false
	
volatile ADC_Active_Channel_t g_activeADCChannel = ADCDisable;
volatile int16_t g_adcVal;
Goertzel g_goertzel(N, sampling_freq);

static void PORT_init(void);
static void VREF0_init(void);
static void ADC0_init(bool freerun);
static void ADC0_SYSTEM_init(bool freerun);
static void ADC0_SYSTEM_shutdown(void);

void ADC0_setADCChannel(ADC_Active_Channel_t chan)
{
	switch(chan)
	{
		case ADCAudioInput:
		{
			ADC0_SYSTEM_init(FREE_RUNNING); 
			ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
		}
		break;
		
		case ADCExternalBatteryVoltage:
		{
			ADC0_SYSTEM_init(SINGLE_CONVERSION);
			ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;
		}
		break;
		
		case ADC12VRegulatedVoltage:
		{
			ADC0_SYSTEM_init(SINGLE_CONVERSION);
			ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc;
		}
		break;
		
		case ADCTXAdjustableVoltage:
		{
			ADC0_SYSTEM_init(SINGLE_CONVERSION);
			ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
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
	if(g_adc_initialized)
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
	}
	else
	{
		ADC0.CTRLA = ADC_ENABLE_bm  /* ADC Enable: enabled */
		| ADC_RESSEL_12BIT_gc;      /* 12-bit mode */

		ADC0.INTCTRL = 0x00; /* Disable interrupt */
	}
}

static void ADC0_SYSTEM_init(bool freerun)
{
	PORT_init();
	VREF0_init();
	ADC0_init(freerun);
	g_adc_initialized = true;
}

static void ADC0_SYSTEM_shutdown(void)
{
	ADC0.INTCTRL = 0x00; /* Disable interrupt */
	ADC0.CTRLA = ADC_RESSEL_12BIT_gc; /* Turn off ADC leaving 12-bit resolution set */
	g_adc_initialized = false;
}

ISR(ADC0_RESRDY_vect)
{
	/* Clear the interrupt flag by reading the result */
	int val = ADC0_read();
	if(g_goertzel.DataPoint(val))
	{
		ADC0.INTCTRL = 0x00; /* disable ADC interrupt */
	}
}