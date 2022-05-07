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

#include <bod.h>
#include <ccp.h>

/**
 * \brief Initialize Brownout Detector
 *
 * \return Initialization status.
 */
int8_t BOD_init()
{

	// ccp_write_io((void*)&(BOD.CTRLA),BOD_SLEEP_DIS_gc /* Disabled */);

	BOD.INTCTRL = 0 << BOD_VLMIE_bp        /* voltage level monitor interrrupt enable: disabled */
	              | BOD_VLMCFG_FALLING_gc; /* Interrupt when supply goes below VLM level */

	// BOD.VLMCTRLA = BOD_VLMLVL_5ABOVE_gc; /* VLM threshold 5% above BOD level */

	return 0;
}
