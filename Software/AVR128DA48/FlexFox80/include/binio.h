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


#ifndef __BINIO_H__
#define __BINIO_H__

/* PORTA *************************************************************************************/
#define V3V3_PWR_ENABLE 7
#define WIFI_RESET 6
#define WIFI_ENABLE 5
#define ANT_CONNECT_INT 4
#define FET_DRIVER_ENABLE 3
#define RTC_SQW 2
#define FAN_ENABLE 1
// #define unused 0

/* PORTB *************************************************************************************/
#define TX_FINAL_VOLTAGE_ENABLE 5
#define MAIN_PWR_ENABLE 4
#define SCL1 3
#define SDA1 2
#define PROC_TONE_OUT 1
#define PROC_PTT_OUT 0

/* PORTC *************************************************************************************/
#define SWITCH 7
#define LED_RED 6
#define LED_GREEN 5
// #define unused 0
#define RTC_SCL 3
#define RTC_SDA 2
#define FRoM_WIFI_TX 1
#define TO_WIFI_RX 0

/* PORTD *************************************************************************************/
#define WIFI_MODULE_DETECT 7
#define DAC_OUTPUT 6
#define TX_PA_VOLTAGE 5
#define TX_12_VOLTAGE 4
#define TX_BATTERY_VOLTAGE 3
#define PROC_HT_AUDIO_IN 2
#define X80M_ANTENNA_DETECT 1
#define X80M_ANTENNA_DETECT_V 0

/* PORTE *************************************************************************************/
// TBD

/* PORTF *************************************************************************************/
#define X32KHZ_SQUAREWAVE 0


void BINIO_init(void);

class binio
{
//variables
public:
protected:
private:

//functions
public:
	binio();
	~binio();
protected:
private:
	binio( const binio &c );
	binio& operator=( const binio &c );

}; //binio

#endif //__BINIO_H__
