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
 *
 * util.h
 *
 * Various helper functions with no better home.
 *
 */


#ifndef UTIL_H_
#define UTIL_H_

#include "defs.h"
#include <time.h>

int32_t timeDif(time_t a, time_t b);
bool only_digits(char *s);

/***********************************************************************************************
 *  Print Formatting Utility Functions
 ************************************************************************************************/

/**
 * Converts a string of format "yyyy-mm-ddThh:mm:ss" to seconds since 1900
 */
uint32_t convertTimeStringToEpoch(char * s);
bool mystrptime(char* s, struct tm* ltm);
bool frequencyString(char* result, uint32_t freq);
bool frequencyVal(char* str, Frequency_Hz* result);
bool fox2Text(char* str, Fox_t fox);

#endif  /* UTIL_H_ */