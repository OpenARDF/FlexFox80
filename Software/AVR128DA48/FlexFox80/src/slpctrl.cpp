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

#include <slpctrl.h>

/**
 * \brief Initialize Sleep Controller
 *
 * \return Initialization status.
 */
int8_t SLPCTRL_init()
{

	SLPCTRL.CTRLA = 0 << SLPCTRL_SEN_bp; /* Sleep enable: disabled */
//			 | SLPCTRL_SMODE_IDLE_gc;  /* Idle mode */
//			 | SLPCTRL_SMODE_STDBY_gc; /* Standby Mode */
//			 | SLPCTRL_SMODE_PDOWN_gc; /* Power-down Mode */

	return 0;
}

void SLPCTRL_set_sleep_mode(SLPCTRL_SMODE_t setmode)
{
	SLPCTRL.CTRLA = (SLPCTRL.CTRLA & ~SLPCTRL_SMODE_gm) | (setmode & SLPCTRL_SMODE_gm);
}
