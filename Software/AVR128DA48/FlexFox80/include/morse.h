/*
 *  MIT License
 *
 *  Copyright (c) 2020 DigitalConfections
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

#ifndef MORSE_H_
#define MORSE_H_

#include "defs.h"

#define PROCESSSOR_CLOCK_HZ			(8000000L)
#define WPM_TO_MS_PER_DOT(w)		(1200/(w))
#define THROTTLE_VAL_FROM_WPM(w)	(PROCESSSOR_CLOCK_HZ / 8000000L) * ((7042 / (w)) / 10)

/**
Load a string to send by passing in a pointer to the string in the argument.
Call this function with a NULL argument at intervals of 1 element of time to generate Morse code.
Once loaded with a string each call to this function returns a bool indicating whether a CW carrier should be sent
 */
bool makeMorse(char* s, bool* repeating, bool* finished);

/**
Returns the number of milliseconds required to send the string pointed to by the first argument at the WPM code speed
passed in the second argument.
*/
uint16_t timeRequiredToSendStrAtWPM(char* str, uint16_t spd);

#endif /* MORSE_H_ */