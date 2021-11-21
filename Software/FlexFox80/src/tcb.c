/**
 * \file
 *
 * \brief TCB related functionality implementation.
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \addtogroup doc_driver_tcb
 *
 * \section doc_driver_tcb_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */
#include <tcb.h>

static uint32_t g_ms_counter = 0;

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


