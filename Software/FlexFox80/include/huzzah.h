/*
 * huzzah.h
 *
 * Created: 1/23/2018 9:16:50 AM
 *  Author: charl
 */ 


#ifndef HUZZAH_H_
#define HUZZAH_H_

#include "defs.h"

/*
 * TRUE = assert reset (sets line low)
 * FALSE = remove reset conditiion (sets line high)
 */ 
void wifi_reset(BOOL reset);


/*
 * ON = powers on WiFi
 * OFF = powers off WiFi
 */ 
void wifi_power(BOOL on);

/*
 * Returns state of WiFi enable (power) bit
 */ 
BOOL wifi_enabled(void);


#endif /* HUZZAH_H_ */