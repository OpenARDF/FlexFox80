/*
 * adc.c
 *
 * Created: 11/21/2021 3:52:44 PM
 *  Author: charl
 */ 

#include "defs.h"
#include <avr/io.h>
#include <stdbool.h>
#include <driver_init.h>
#include <compiler.h>
// #include "Goertzel.h"

// #define SAMPLE_RATE 24096
// #define Goertzel_N 201
// const int N = Goertzel_N;
// const float threshold = 500000. * (Goertzel_N / 100);
// const float sampling_freq = SAMPLE_RATE;
// const float x_frequencies[4] = { 1209., 1336., 1477., 1633. };
// const float y_frequencies[4] = { 697., 770., 852., 941. };
// 
// volatile int16_t g_adcVal;
// Goertzel g_goertzel(N, sampling_freq);

static void PORT_init(void);
static void VREF0_init(void);
static void ADC0_init(void);
static void ADC0_start(void);
static bool ADC0_conversionDone(void);
static int16_t ADC0_read(void);
static void ADC0_SYSTEM_init(void);

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

static void ADC0_init(void)
{
	ADC0.CTRLC = ADC_PRESC_DIV64_gc;   /* CLK_PER divided by 4 => 24096 sps */
	
	ADC0.CTRLA = ADC_ENABLE_bm        /* ADC Enable: enabled */
	| ADC_RESSEL_12BIT_gc      /* 12-bit mode */
//	| ADC_CONVMODE_bm          /* Differential Conversion */
	| ADC_FREERUN_bm;          /* Enable Free-Run mode */
	
	ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;     /* Select ADC channel AIN3 <-> PD3 */
}

int16_t ADC0_read(void)
{
	/* Clear the interrupt flag by reading the result */
	return ADC0.RES;
}

static void ADC0_start(void)
{
	ADC0.INTCTRL = 0x01; /* Enable interrupt */
	/* Start conversion */
	ADC0.COMMAND = ADC_STCONV_bm;
}

bool ADC0_conversionDone(void)
{
	/* Check if the conversion is done  */
	return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}

static void ADC0_SYSTEM_init(void)
{
	PORT_init();
	VREF0_init();
	ADC0_init();
}

void ADC0_startConversions(void)
{
	ADC0_SYSTEM_init();
	ADC0_start();
}

ISR(ADC0_RESRDY_vect)
{
// 	int16_t val = ADC0.RES;
// 	if(g_goertzel.DataPoint(val))
// 	{
// 		ADC0.INTCTRL = 0x00; /* disable ADC interrupt */
// 	}
	int16_t data = ADC0_read();
	/* Clear the interrupt flag by reading the result */
}