/*
 *  MIT License
 *
 *  Copyright (c) 2026 DigitalConfections
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
 * Shared AVR/ESP temperature validity and conversion rules.
 */

#ifndef TEMPERATURE_CONTRACT_H_
#define TEMPERATURE_CONTRACT_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define TEMPERATURE_MINIMUM_C (-20)
#define TEMPERATURE_MAXIMUM_C 120
#define TEMPERATURE_UNAVAILABLE_TEXT "NA"

static inline bool temperatureCelsiusIsValid(int64_t temperatureC)
{
	return (temperatureC >= TEMPERATURE_MINIMUM_C) &&
		(temperatureC <= TEMPERATURE_MAXIMUM_C);
}

static inline bool temperatureCelsiusFromText(const char* text, double* temperatureC)
{
	if((text == 0) || (temperatureC == 0))
	{
		return false;
	}

	while((*text == ' ') || (*text == '\t'))
	{
		text++;
	}

	const char* numberStart = text;
	if(*text == '-')
	{
		text++;
	}

	const char* integerStart = text;
	while((*text >= '0') && (*text <= '9'))
	{
		text++;
	}
	if(text == integerStart)
	{
		return false;
	}
	if(*text == '.')
	{
		text++;
		const char* fractionStart = text;
		while((*text >= '0') && (*text <= '9'))
		{
			text++;
		}
		if(text == fractionStart)
		{
			return false;
		}
	}

	const char* numberEnd = text;
	while((*text == ' ') || (*text == '\t'))
	{
		text++;
	}
	if(*text != '\0')
	{
		return false;
	}

	char* parsedEnd = 0;
	const double parsed = strtod(numberStart, &parsedEnd);
	if((parsedEnd != numberEnd) || !((parsed >= TEMPERATURE_MINIMUM_C) &&
		(parsed <= TEMPERATURE_MAXIMUM_C)))
	{
		return false;
	}

	*temperatureC = parsed;
	return true;
}

/*
 * Apply Microchip's AVR DA factory-calibration equation.  The output is not
 * modified unless a completed 12-bit temperature-channel conversion produces
 * an in-contract value.  Callers must therefore carry availability separately;
 * no numeric sentinel can be mistaken for a real temperature.
 */
static inline bool temperatureCelsiusFromAdc(
	uint16_t adcResult,
	uint16_t signatureOffset,
	uint16_t signatureSlope,
	int16_t* temperatureC)
{
	if((temperatureC == 0) || (adcResult > 4095U))
	{
		return false;
	}

	/* Keep corrupt calibration words from wrapping into a plausible result. */
	int64_t kelvin = (int64_t)signatureOffset - (int64_t)adcResult;
	kelvin *= signatureSlope;
	kelvin += 0x0800L;
	kelvin >>= 12;

	const int64_t celsius = kelvin - 273L;
	if(!temperatureCelsiusIsValid(celsius))
	{
		return false;
	}

	*temperatureC = (int16_t)celsius;
	return true;
}

#endif /* TEMPERATURE_CONTRACT_H_ */
