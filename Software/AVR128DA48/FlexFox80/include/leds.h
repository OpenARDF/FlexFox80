/*
 *  MIT License
 *
 *  Copyright (c) 2022-2026 DigitalConfections
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

/*
 * AVR status-LED control and blink sequencing.
 *
 * Created: 2022-05-05
 */

#ifndef __LEDS_H__
#define __LEDS_H__

enum Blink_t : uint16_t{
	LEDS_OFF,
	LEDS_RED_OFF,
	LEDS_GREEN_OFF,
	LEDS_RED_BLINK_FAST,
	LEDS_RED_BLINK_SLOW,
	LEDS_GREEN_BLINK_FAST,
	LEDS_GREEN_BLINK_SLOW,
	LEDS_RED_ON_CONSTANT,
	LEDS_GREEN_ON_CONSTANT,
	LEDS_RED_AND_GREEN_BLINK_FAST,
	LEDS_RED_AND_GREEN_BLINK_SLOW,
	LEDS_RED_THEN_GREEN_BLINK_FAST,
	LEDS_RED_THEN_GREEN_BLINK_SLOW,
	LEDS_NUMBER_OF_SETTINGS
};

class leds
{
//variables
public:
protected:
private:

//functions
public:
	leds();
	~leds();
	
	void init(void);
	void init(Blink_t setBlink);
	void blink(Blink_t blinkMode);
	void blink(Blink_t blinkMode, bool resetTimeout);
	bool active(void);
	void deactivate(void);
	void setRed(bool on);
	void setGreen(bool on);
	void resume(void);
	void sendCode(char* str);
	
protected:
	void reset(void);
	void reset(Blink_t setBlink);
private:
//	leds( const leds &c );
	leds& operator=( const leds &c );

}; //leds

#endif //__LEDS_H__
