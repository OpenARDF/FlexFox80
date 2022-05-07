/* 
* leds.h
*
* Created: 5/5/2022 10:27:49 AM
* Author: charl
*/


#ifndef __LEDS_H__
#define __LEDS_H__

typedef enum {
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
	}Blink_t;

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
	
	void blink(Blink_t blinkMode);
	bool active(void);
	Blink_t setting(void);
protected:
private:
//	leds( const leds &c );
	leds& operator=( const leds &c );

}; //leds

#endif //__LEDS_H__