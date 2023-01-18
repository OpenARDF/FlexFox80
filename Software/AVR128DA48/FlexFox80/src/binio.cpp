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
#include "atmel_start_pins.h"

extern volatile uint16_t g_switch_closed_count;

// default constructor
binio::binio()
{
// 	portDdebounced = 0;
} //binio

// default destructor
binio::~binio()
{
} //~binio


// This function is called approximately each 1/60 to 1/30 sec.
// void binio::debounce(void)
// {
// 	// Move previously sampled raw input bits one step down the line.
// 	portDpinReadings[2] = portDpinReadings[1];
// 	portDpinReadings[1] = portDpinReadings[0];
// 	
// 	// Sample new raw input bits from PORT_A.
// 	portDpinReadings[0] = PORTD_get_port_level();
// 	
// 	// Debounce output bits using low-pass filtering.
// 	portDdebounced = portDdebounced ^ (
// 	(portDdebounced ^ portDpinReadings[0])
// 	& (portDdebounced ^ portDpinReadings[1])
// 	& (portDdebounced ^ portDpinReadings[2]));
// }
// 
// uint8_t binio::portDdebouncedVals(void)
// {
// 	return portDdebounced;
// }


/**

*/
// ISR(PORTA_PORT_vect)
// {
// 	static int count = 0;
// 	
// 	if(PORTA.INTFLAGS & (1 << RTC_SQW))
// 	{
// 		count++;
// 	}
// 	
// 	if(PORTA.INTFLAGS & (1 << ANT_CONNECT_INT))
// 	{
// 		count++;
// 	}	
// 	
// 	LED_toggle_GREEN_level();
// 	
// 	PORTA.INTFLAGS = 0xFF; /* Clear all flags */
// }

void BINIO_init(void)
{
	/* PORTA *************************************************************************************/
	
	/* PORTA.PIN0 = TXDO USART */
	/* PORTA.PIN1 = RXD0 USART */
	
	PORTA_set_pin_dir(FAN_ENABLE, PORT_DIR_OUT);
	PORTA_set_pin_level(FAN_ENABLE, LOW);
	
	PORTA_set_pin_dir(RTC_SQW, PORT_DIR_IN);
	PORTA_set_pin_pull_mode(RTC_SQW, PORT_PULL_UP);
	PORTA_pin_set_isc(RTC_SQW, PORT_ISC_RISING_gc);
	
	PORTA_set_pin_dir(FET_DRIVER_ENABLE, PORT_DIR_OUT);
	PORTA_set_pin_level(FET_DRIVER_ENABLE, LOW);
	
// 	PORTA_set_pin_dir(ANT_CONNECT_INT, PORT_DIR_IN);
// 	PORTA_set_pin_pull_mode(ANT_CONNECT_INT, PORT_PULL_UP);
// 	PORTA_pin_set_isc(ANT_CONNECT_INT, PORT_ISC_FALLING_gc);

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
	PORTC_set_pin_dir(RTC_SDA, PORT_DIR_IN);
	PORTC_set_pin_pull_mode(RTC_SDA, PORT_PULL_UP);
	
	PORTC_set_pin_dir(RTC_SCL, PORT_DIR_IN);
	PORTC_set_pin_pull_mode(RTC_SCL, PORT_PULL_UP);

	PORTC_set_pin_dir(LED_GREEN, PORT_DIR_OUT);
	PORTC_set_pin_level(LED_GREEN, LOW);
	
	PORTC_set_pin_dir(LED_RED, PORT_DIR_OUT);
	PORTC_set_pin_level(LED_RED, LOW);
	
	PORTC_set_pin_dir(SWITCH, PORT_DIR_IN);
	PORTC_set_pin_pull_mode(SWITCH, PORT_PULL_UP);
	PORTC_pin_set_isc(SWITCH, PORT_ISC_RISING_gc);
	
	/* PORTD *************************************************************************************/
	PORTD_set_pin_dir(X80M_ANTENNA_DETECT_V, PORT_DIR_IN); /* use ADC for reading voltage at antenna detect pin */
	
	PORTD_set_pin_dir(X80M_ANTENNA_DETECT, PORT_DIR_IN);
	PORTD_set_pin_pull_mode(X80M_ANTENNA_DETECT, PORT_PULL_UP);
	PORTD_pin_set_isc(X80M_ANTENNA_DETECT, PORT_ISC_BOTHEDGES_gc);
	
	/* PORTD.PIN2 = ACD2 Audio in PROC_HT_AUDIO_IN */
	/* PORTD.PIN3 = ACD3 Audio in TX_BATTERY_VOLTAGE */
	/* PORTD.PIN4 = ACD4 Audio in TX_12_VOLTAGE */
	/* PORTD.PIN5 = ACD5 Audio in TX_PA_VOLTAGE */
	/* PORTD.PIN6 = DAC0 voltage out DAC_OUTPUT */
	
	PORTD_set_pin_dir(WIFI_MODULE_DETECT, PORT_DIR_IN); /* Detect presence of Huzzah module */
	PORTD_set_pin_pull_mode(WIFI_MODULE_DETECT, PORT_PULL_OFF);

	/* PORTE *************************************************************************************/
	PORTE_set_pin_dir(0, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(1, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(2, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(3, PORT_DIR_OFF); /* Unused */

	/* PORTF *************************************************************************************/
	PORTF_set_pin_dir(X32KHZ_SQUAREWAVE, PORT_DIR_OFF);	
	PORTF_set_pin_dir(1, PORT_DIR_OFF);	/* Unused */
	PORTF_set_pin_dir(2, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(3, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(4, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(5, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(6, PORT_DIR_OFF); /* Reset */
	
	/* PORT Pin Interrupts */
	PORTA.PIN2CTRL = 0x0A; /* Enable RTC SQW 1-sec interrupts */
	PORTD.PIN1CTRL = 0x09; /* Enable antenna change interrupts */
}

void BINIO_sleep()
{
	/* PORTA *************************************************************************************/
	
	/* PORTA.PIN0 = TXDO USART */
	/* PORTA.PIN1 = RXD0 USART */
	
	PORTA_set_pin_dir(FAN_ENABLE, PORT_DIR_OUT);
	PORTA_set_pin_level(FAN_ENABLE, LOW);
	
// 	PORTA_set_pin_dir(RTC_SQW, PORT_DIR_IN);
// 	PORTA_set_pin_pull_mode(RTC_SQW, PORT_PULL_UP);
// 	PORTA_pin_set_isc(RTC_SQW, PORT_ISC_RISING_gc);
	
	PORTA_set_pin_dir(FET_DRIVER_ENABLE, PORT_DIR_OUT);
	PORTA_set_pin_level(FET_DRIVER_ENABLE, LOW);
	
	// 	PORTA_set_pin_dir(ANT_CONNECT_INT, PORT_DIR_IN);
	// 	PORTA_set_pin_pull_mode(ANT_CONNECT_INT, PORT_PULL_UP);
	// 	PORTA_pin_set_isc(ANT_CONNECT_INT, PORT_ISC_FALLING_gc);

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

	PORTC_set_pin_dir(LED_GREEN, PORT_DIR_OUT);
	PORTC_set_pin_level(LED_GREEN, LOW);
	
	PORTC_set_pin_dir(LED_RED, PORT_DIR_OUT);
	PORTC_set_pin_level(LED_RED, LOW);
	
// 	PORTC_set_pin_dir(SWITCH, PORT_DIR_IN);
// 	PORTC_set_pin_pull_mode(SWITCH, PORT_PULL_UP);
// 	PORTC_pin_set_isc(SWITCH, PORT_ISC_RISING_gc);
	
	/* PORTD *************************************************************************************/
// 	PORTD_set_pin_dir(X80M_ANTENNA_DETECT_V, PORT_DIR_IN); /* use ADC for reading voltage at antenna detect pin */
	
// 	PORTD_set_pin_dir(X80M_ANTENNA_DETECT, PORT_DIR_IN);
// 	PORTD_set_pin_pull_mode(X80M_ANTENNA_DETECT, PORT_PULL_UP);
// 	PORTD_pin_set_isc(X80M_ANTENNA_DETECT, PORT_ISC_BOTHEDGES_gc);
	
	/* PORTD.PIN2 = ACD2 Audio in PROC_HT_AUDIO_IN */
	/* PORTD.PIN3 = ACD3 Audio in TX_BATTERY_VOLTAGE */
	/* PORTD.PIN4 = ACD4 Audio in TX_12_VOLTAGE */
	/* PORTD.PIN5 = ACD5 Audio in TX_PA_VOLTAGE */
	/* PORTD.PIN6 = DAC0 voltage out DAC_OUTPUT */
	
	PORTD_set_pin_dir(WIFI_MODULE_DETECT, PORT_DIR_IN); /* Detect presence of Huzzah module */
	PORTD_set_pin_pull_mode(WIFI_MODULE_DETECT, PORT_PULL_OFF);

	/* PORTE *************************************************************************************/
	PORTE_set_pin_dir(0, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(1, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(2, PORT_DIR_OFF); /* Unused */
	PORTE_set_pin_dir(3, PORT_DIR_OFF); /* Unused */

	/* PORTF *************************************************************************************/
	PORTF_set_pin_dir(X32KHZ_SQUAREWAVE, PORT_DIR_OFF);
	PORTF_set_pin_dir(1, PORT_DIR_OFF);	/* Unused */
	PORTF_set_pin_dir(2, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(3, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(4, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(5, PORT_DIR_OFF); /* Unused */
	PORTF_set_pin_dir(6, PORT_DIR_OFF); /* Reset */
	
	/* PORT Pin Interrupts */
// 	PORTA.PIN2CTRL = 0x0A; /* Enable RTC SQW 1-sec interrupts */
// 	PORTD.PIN1CTRL = 0x09; /* Enable antenna change interrupts */
}
