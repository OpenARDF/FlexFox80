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

#include <tcb.h>

static uint32_t g_ms_counter = 0;
extern volatile uint16_t g_i2c0_timeout_ticks;
extern volatile uint16_t g_i2c1_timeout_ticks;

/**
 * \brief Initialize tcb interface
 *
 * \return Initialization status.
 */
int8_t TIMERB_init()
{
TCB0.INTCTRL = 1 << TCB_CAPT_bp   /* Capture or Timeout: enabled */
| 0 << TCB_OVF_bp; /* OverFlow Interrupt: disabled */

// Set TOP
TCB0.CCMP = 0x9C40;

TCB0.CTRLA = TCB_CLKSEL_DIV2_gc     /* CLK_PER */
| 1 << TCB_ENABLE_bp   /* Enable: enabled */
| 0 << TCB_RUNSTDBY_bp /* Run Standby: disabled */
| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */

TCB0.INTFLAGS = TCB_CAPT_bm; /* Clear flag */

/********************************************************************************/

TCB1.INTCTRL = 1 << TCB_CAPT_bp   /* Capture or Timeout: enabled */
| 0 << TCB_OVF_bp; /* OverFlow Interrupt: disabled */

// Set TOP
TCB1.CCMP = 0x5DC0;

TCB1.CTRLA = TCB_CLKSEL_DIV1_gc     /* CLK_PER */
| 1 << TCB_ENABLE_bp   /* Enable: enabled */
| 0 << TCB_RUNSTDBY_bp /* Run Standby: disabled */
| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */

TCB1.INTFLAGS = TCB_CAPT_bm; /* Clear flag */


/********************************************************************************/

CPUINT.LVL1VEC = 30; /* Set to level 1 - highest priority interrupt */
TCB2.INTCTRL = 1 << TCB_CAPT_bp   /* Capture or Timeout: enabled */
| 0 << TCB_OVF_bp; /* OverFlow Interrupt: disabled */

// Set TOP
TCB2.CCMP = 0xFFFF;

TCB2.CTRLA = TCB_CLKSEL_DIV2_gc     /* CLK_PER */
| 1 << TCB_ENABLE_bp   /* Enable: enabled */
| 0 << TCB_RUNSTDBY_bp /* Run Standby: disabled */
| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */

TCB2.INTFLAGS = TCB_CAPT_bm; /* Clear flag */
CPUINT.LVL1VEC = 30; /* Set to level 1 - highest priority interrupt */


/********************************************************************************/

TCB3.INTCTRL = 1 << TCB_CAPT_bp   /* Capture or Timeout: enabled */
| 0 << TCB_OVF_bp; /* OverFlow Interrupt: disabled */

// Set TOP
TCB3.CCMP = 0x9C40;

TCB3.CTRLA = TCB_CLKSEL_DIV2_gc     /* CLK_PER */
| 1 << TCB_ENABLE_bp   /* Enable: enabled */
| 0 << TCB_RUNSTDBY_bp /* Run Standby: disabled */
| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */

TCB3.INTFLAGS = TCB_CAPT_bm; /* Clear flag */

	return 0;
}


bool util_delay_ms(uint32_t delayValue)
{
	static uint32_t countdownValue=0;
	static bool counting = false;
	
	if(delayValue)
	{
		if(counting)
		{
			if(!g_ms_counter)
			{
				TCB1.INTCTRL = 0 << TCB_CAPT_bp   /* Capture or Timeout: disabled */
				| 0 << TCB_OVF_bp; /* OverFlow Interrupt: disabled */

				counting = false;
				countdownValue = 0;
				return(false); /* time expired */
			}
			else if(delayValue != countdownValue) /* countdown delay changed */
			{
				TCB1.CNT = 0x0000;
				countdownValue = delayValue;
				g_ms_counter = delayValue;
				return(false);
			}
		}
		else if(delayValue != countdownValue)
		{
			TCB1.CNT = 0x0000;
			countdownValue = delayValue;
			g_ms_counter = delayValue;
			counting = true;
		}
	}
	else
	{
		TCB1.INTCTRL = 0 << TCB_CAPT_bp   /* Capture or Timeout: disabled */
		| 0 << TCB_OVF_bp; /* OverFlow Interrupt: disabled */

		counting = false;
		countdownValue = 0;
		g_ms_counter = 0;
		return(false); /* timer reset */
	}
	
	TCB1.INTCTRL = 1 << TCB_CAPT_bp   /* Capture or Timeout: enabled */
	| 0 << TCB_OVF_bp; /* OverFlow Interrupt: disabled */
	
	return(true);
}

ISR(TCB2_INT_vect)
{
	if(TCB2.INTFLAGS & TCB_CAPT_bm)
	{
		if(g_i2c1_timeout_ticks) g_i2c1_timeout_ticks--;
		if(g_i2c0_timeout_ticks) g_i2c0_timeout_ticks--;
		TCB2.INTFLAGS = TCB_CAPT_bm; /* Clear flag */
	}
}



/**
One millisecond utility counter based on CPU clock.
*/
ISR(TCB1_INT_vect)
{
    if(TCB1.INTFLAGS & TCB_CAPT_bm)
    {
		if(g_ms_counter) /* check for 1-second interval */
		{
			g_ms_counter--;	
		}
		else
		{
			TCB1.INTCTRL = 0 << TCB_CAPT_bp   /* Capture or Timeout: disabled */
			| 0 << TCB_OVF_bp; /* OverFlow Interrupt: disabled */
		}

        TCB1.INTFLAGS = TCB_CAPT_bm; /* clear interrupt flag */
    }
}

int8_t TIMERB_sleep()
{
	TCB0.INTCTRL = 0;   /* Capture or Timeout: disable interrupts */
	TCB0.CTRLA = 0; /* Disable timer */

	/********************************************************************************/

	TCB1.INTCTRL = 0; /* OverFlow Interrupt: disabled */
	TCB1.CTRLA = 0; /* Disable timer */

	/********************************************************************************/

	TCB2.INTCTRL = 0; /* OverFlow Interrupt: disabled */
	TCB2.CTRLA = 0; /* Disable timer */

	/********************************************************************************/

	TCB3.INTCTRL = 0; /* OverFlow Interrupt: disabled */
	TCB3.CTRLA = 0; /* Disable timer */


	return 0;
}

