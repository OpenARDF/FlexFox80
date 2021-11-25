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


#include "binio.h"
#include "port.h"
#include "defs.h"

// default constructor
binio::binio()
{
} //binio

// default destructor
binio::~binio()
{
} //~binio


/**
Handle switch closure interrupts
*/
ISR(PORTC_PORT_vect)
{
	static int count = 0;
	
	if(PORTC.INTFLAGS & (1 << SWITCH))
	{
		count++;
	}
	
	if(PORTC.INTFLAGS & (1 << X32KHZ_SQUAREWAVE))
	{
		count++;
	}
	
	PORTC.INTFLAGS = 0xFF; /* Clear all flags */
}

/**

*/
ISR(PORTA_PORT_vect)
{
	static int count = 0;
	
	if(PORTA.INTFLAGS & (1 << RTC_SQW))
	{
		count++;
	}
	
	if(PORTA.INTFLAGS & (1 << ANT_CONNECT_INT))
	{
		count++;
	}	
	
	PORTA.INTFLAGS = 0xFF; /* Clear all flags */
}

void BINIO_init(void)
{
	/* PORTA *************************************************************************************/
	
	/* PORTA.PIN0 = TXDO USART */
	/* PORTA.PIN1 = RXD0 USART */
	
	PORTA_set_pin_dir(RTC_SQW, PORT_DIR_IN);
//	PORTA_set_pin_pull_mode(RTC_SQW, PORT_PULL_UP);
	PORTA_pin_set_isc(RTC_SQW, PORT_ISC_RISING_gc);
	
	PORTA_set_pin_dir(HF_ENABLE, PORT_DIR_OUT);
	PORTA_set_pin_level(HF_ENABLE, LOW);
	
	PORTA_set_pin_dir(ANT_CONNECT_INT, PORT_DIR_IN);
	PORTA_set_pin_pull_mode(ANT_CONNECT_INT, PORT_PULL_UP);
	PORTA_pin_set_isc(ANT_CONNECT_INT, PORT_ISC_FALLING_gc);

	PORTA_set_pin_dir(WIFI_ENABLE, PORT_DIR_OUT);
	PORTA_set_pin_level(WIFI_ENABLE, LOW);
	
	PORTA_set_pin_dir(WIFI_RESET, PORT_DIR_OUT);
	PORTA_set_pin_level(WIFI_RESET, LOW);
	
	PORTA_set_pin_dir(V3V3_PWR_ENABLE, PORT_DIR_OUT);
	PORTA_set_pin_level(V3V3_PWR_ENABLE, LOW);
	
	/* PORTB *************************************************************************************/

	PORTB_set_pin_dir(PROC_PTT_OUT, PORT_DIR_OUT);
	PORTB_set_pin_level(PROC_PTT_OUT, LOW);
	
	PORTB_set_pin_dir(PROC_TONE_OUT, PORT_DIR_OUT);
	PORTB_set_pin_level(PROC_TONE_OUT, LOW);
	
	/* PORTB.PIN2 = SDA1 I2C */
	/* PORTB.PIN3 = SCL1 I2C */
	PORTB_set_pin_dir(MAIN_PWR_ENABLE, PORT_DIR_OUT);
	PORTB_set_pin_level(MAIN_PWR_ENABLE, LOW);
	
	PORTB_set_pin_dir(TX_FINAL_VOLTAGE_ENABLE, PORT_DIR_OUT);
	PORTB_set_pin_level(TX_FINAL_VOLTAGE_ENABLE, LOW);
	

	/* PORTC *************************************************************************************/
	
	/* PORTC.PIN0 = TXD1 USART TO_WIFI_RX */
	/* PORTC.PIN1 = RXD1 USART FROM_WIFI_TX */
	/* PORTC.PIN2 = SDA0 I2C */
	/* PORTC.PIN3 = SCL0 I2C */

	PORTC_set_pin_dir(X32KHZ_SQUAREWAVE, PORT_DIR_IN);
	
	PORTC_set_pin_dir(LED_GREEN, PORT_DIR_OUT);
	PORTC_set_pin_level(LED_GREEN, LOW);
	
	PORTC_set_pin_dir(LED_RED, PORT_DIR_OUT);
	PORTC_set_pin_level(LED_RED, LOW);
	
	PORTC_set_pin_dir(SWITCH, PORT_DIR_IN);
	PORTC_set_pin_pull_mode(SWITCH, PORT_PULL_UP);
	PORTC_pin_set_isc(SWITCH, PORT_ISC_FALLING_gc);
	
	/* PORTD *************************************************************************************/
	PORTD_set_pin_dir(X80M_ANTENNA_DETECT_V, PORT_DIR_IN);
	
	PORTD_set_pin_dir(X80M_ANTENNA_DETECT, PORT_DIR_IN);
	PORTD_set_pin_pull_mode(X80M_ANTENNA_DETECT, PORT_PULL_UP);
	PORTD_pin_set_isc(X80M_ANTENNA_DETECT, PORT_ISC_FALLING_gc);
	
	/* PORTD.PIN2 = ACD2 Audio in PROC_HT_AUDIO_IN */
	/* PORTD.PIN3 = ACD3 Audio in TX_BATTERY_VOLTAGE */
	/* PORTD.PIN4 = ACD4 Audio in TX_12_VOLTAGE */
	/* PORTD.PIN5 = ACD5 Audio in TX_PA_VOLTAGE */
	/* PORTD.PIN6 = DAC0 voltage out DAC_OUTPUT */
	
	PORTD_set_pin_dir(ADC7, PORT_DIR_OFF); /* Unused */

	/* PORTE *************************************************************************************/
	PORTE_set_pin_dir(0, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(1, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(2, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(3, PORT_DIR_OFF); /* Unused */

	/* PORTF *************************************************************************************/
	PORTF_set_pin_dir(2, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(3, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(4, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(5, PORT_DIR_OFF); /* Unused */
	/* PORTF.PIN6 = Reset */
}
