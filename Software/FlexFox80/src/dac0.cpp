/*
 * dac0.c
 *
 * Created: 11/21/2021 8:22:00 AM
 *  Author: charl
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
//	VREF.DAC0REF = VREF_REFSEL_2V048_gc /* Select the 2.048V Internal Voltage Reference for DAC */
//	VREF.DAC0REF = VREF_REFSEL_2V500_gc /* Select the 2.048V Internal Voltage Reference for DAC */
//	| VREF_ALWAYSON_bm;    /* Set the Voltage Reference in Always On mode */
	VREF.DAC0REF = VREF_REFSEL_2V500_gc; /* Select the 2.048V Internal Voltage Reference for DAC */
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
	
	DAC0_setVal(DAC_DEFAULT_VALUE);
}

void DAC0_setVal(uint16_t value)
{
	/* Store the two LSbs in DAC0.DATAL */
	DAC0.DATAL = (value & LSB_MASK) << 6;
	/* Store the eight MSbs in DAC0.DATAH */
	DAC0.DATAH = value >> 2;
}
