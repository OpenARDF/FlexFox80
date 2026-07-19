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

#include "temperature_contract.h"

#include <cstdlib>
#include <initializer_list>
#include <iostream>

static void expect(bool condition, const char* name)
{
	if(!condition)
	{
		std::cerr << "FAIL " << name << '\n';
		std::exit(1);
	}
	std::cout << "PASS " << name << '\n';
}

int main()
{
	expect(temperatureCelsiusIsValid(-20), "minimum_temperature_is_valid");
	expect(temperatureCelsiusIsValid(120), "maximum_temperature_is_valid");
	expect(!temperatureCelsiusIsValid(-21), "below_minimum_is_invalid");
	expect(!temperatureCelsiusIsValid(121), "above_maximum_is_invalid");

	double parsed = 999.0;
	expect(temperatureCelsiusFromText(" 25.0 ", &parsed) && parsed == 25.0,
		"numeric_payload_is_accepted");
	expect(temperatureCelsiusFromText("-20", &parsed), "minimum_payload_is_accepted");
	expect(temperatureCelsiusFromText("120.0", &parsed), "maximum_payload_is_accepted");
	for(const char* invalid : {"", "NA", "25.0C", "-20.1", "120.1", "nan", "inf", "+25", "0x19", "2.5e1"})
	{
		parsed = 999.0;
		expect(!temperatureCelsiusFromText(invalid, &parsed) && parsed == 999.0,
			"invalid_payload_cannot_publish_a_number");
	}

	/* With slope 4096, this fabricated calibration makes C = offset - ADC - 273. */
	int16_t celsius = 777;
	expect(temperatureCelsiusFromAdc(702, 1000, 4096, &celsius) && celsius == 25,
		"completed_temperature_conversion_is_calibrated");
	expect(temperatureCelsiusFromAdc(747, 1000, 4096, &celsius) && celsius == -20,
		"minimum_adc_temperature_is_accepted");
	expect(temperatureCelsiusFromAdc(607, 1000, 4096, &celsius) && celsius == 120,
		"maximum_adc_temperature_is_accepted");

	for(uint16_t invalid : {uint16_t(606), uint16_t(748), uint16_t(4096)})
	{
		celsius = 777;
		expect(!temperatureCelsiusFromAdc(invalid, 1000, 4096, &celsius) && celsius == 777,
			"invalid_adc_result_cannot_publish_a_number");
	}
	celsius = 777;
	expect(!temperatureCelsiusFromAdc(0, 65535, 65535, &celsius) && celsius == 777,
		"corrupt_calibration_cannot_wrap_into_range");

	celsius = 777;
	expect(!temperatureCelsiusFromAdc(702, 1000, 4096, 0) && celsius == 777,
		"absent_output_cannot_publish_a_number");
	return 0;
}
