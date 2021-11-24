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

#include "Goertzel.h"
#include <string.h>

#if !INIT_EEPROM_ONLY

float _SAMPLING_FREQUENCY;
float _TARGET_FREQUENCY;
volatile int _N;
float g_coeff;
float Q1;
float Q2;
volatile int _index = 0;
volatile bool _samplesReady = false;
int highValueCount = 0;

int* testData;

Goertzel::Goertzel(float N, float sampling_frequency)
{
	_SAMPLING_FREQUENCY = sampling_frequency;   /*on 16mhz, ~8928.57142857143, on 8mhz ~44444 */
	_N = CLAMP(100, N, MAXN);
	testData = (int*)malloc(_N * sizeof(int));
}

Goertzel::~Goertzel()
{
	free(testData);
}

void Goertzel::Flush(void)
{
	memset(testData, 0x00, _N * sizeof(int));
	ResetGoertzel();
}

void Goertzel::SetTargetFrequency(float target_frequency)
{
	float floatN = (float)_N;

	_TARGET_FREQUENCY = target_frequency;   /* Ideally, this should be an integer multiple of SAMPLING_RATE/N */
	int k = (int)(0.5 + ((floatN * _TARGET_FREQUENCY) / _SAMPLING_FREQUENCY));
	float w = (2.0 * PI * k) / floatN;
	g_coeff = 2.0 * cos(w);
	ResetGoertzel();
}

/* Call this routine before every "block" (size=N) of samples. */
void Goertzel::ResetGoertzel(void)
{
	Q2 = 0;
	Q1 = 0;
	_samplesReady = false;
}

/* Call this routine for every sample. */
void Goertzel::ProcessSample(int sample)
{
	float Q0 = g_coeff * Q1 - Q2 + (float)(sample - ADCCENTER);

	if(sample > 230)
	{
		highValueCount++;
	}

	Q2 = Q1;
	Q1 = Q0;
}

bool Goertzel::DataPoint(int data)
{
	if(_samplesReady)
	{
		return(_samplesReady);
	}

	testData[_index] = data;
	_index++;
	if(_index >= _N)
	{
		_index = 0;
		_samplesReady = true;
	}

	return(_samplesReady);
}

bool Goertzel::SamplesReady(void)
{
	return(_samplesReady);
}

float Goertzel::Magnitude2(int *highCount)
{
	float magnitude2;

	highValueCount = 0;

	/* Process the samples. */
	for(int index = 0; index < _N; index++)
	{
		ProcessSample(testData[index]);
	}

	if(highCount) *highCount = highValueCount;

	magnitude2 = Q1 * Q1 + Q2 * Q2 - g_coeff * Q1 * Q2;

	ResetGoertzel();
	return( magnitude2);
}
#endif // !INIT_EEPROM_ONLY
