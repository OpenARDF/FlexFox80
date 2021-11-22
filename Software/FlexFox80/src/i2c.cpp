/**********************************************************************************************
 * Copyright © 2017 Digital Confections LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 **********************************************************************************************
 *
 * i2c.c
 */

#include <util/twi.h>
#include <avr/power.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
//#include <util/delay.h>
#include <util/atomic.h>

#include "si5351.h"
#include "i2c.h"

#ifdef TWI_BAUD
#undef TWI_BAUD
#endif

#define TWI_BAUD(F_SCL, T_RISE)        (uint8_t)((((((float)F_CPU / (float)(F_SCL)) - 10 - (((float)(F_CPU) * (T_RISE))/1000000.0))) / 2))

#define I2C_SCL_FREQ                    100000  /* Frequency [Hz] */

enum {
	I2C_INIT = 0,
	I2C_ACKED,
	I2C_NACKED,
	I2C_READY,
	I2C_ERROR
};

void I2C_0_Init(void)
{
	/* Select I2C pins PC2/PC3 */
	PORTMUX.TWIROUTEA = 0x02;
	
	/* Host Baud Rate Control */
	TWI0.MBAUD = TWI_BAUD((I2C_SCL_FREQ), 0.3);
	
	/* Enable TWI */
	TWI0.MCTRLA = TWI_ENABLE_bm;
	
	/* Initialize the address register */
	TWI0.MADDR = 0x00;
	
	/* Initialize the data register */
	TWI0.MDATA = 0x00;
	
	/* Set bus state idle */
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

static uint8_t i2c_0_WaitW(void)
{
	uint8_t state = I2C_INIT;
	do
	{
		if(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))
		{
			if(!(TWI0.MSTATUS & TWI_RXACK_bm))
			{
				/* client responded with ack - TWI goes to M1 state */
				state = I2C_ACKED;
			}
			else
			{
				/* address sent but no ack received - TWI goes to M3 state */
				state = I2C_NACKED;
			}
		}
		else if(TWI0.MSTATUS & (TWI_BUSERR_bm | TWI_ARBLOST_bm))
		{
			/* get here only in case of bus error or arbitration lost - M4 state */
			state = I2C_ERROR;
		}
	} while(!state);
	
	return state;
}

static uint8_t i2c_0_WaitR(void)
{
	uint8_t state = I2C_INIT;
	
	do
	{
		if(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))
		{
			state = I2C_READY;
		}
		else if(TWI0.MSTATUS & (TWI_BUSERR_bm | TWI_ARBLOST_bm))
		{
			/* get here only in case of bus error or arbitration lost - M4 state */
			state = I2C_ERROR;
		}
	} while(!state);
	
	return state;
}

/* Returns how many bytes have been sent, -1 means NACK at address, 0 means client ACKed to client address */
uint8_t I2C_0_SendData(uint8_t address, uint8_t *pData, uint8_t len)
{
	uint8_t retVal = (uint8_t) - 1;
	
	/* start transmitting the client address */
	TWI0.MADDR = address & ~0x01;
	if(i2c_0_WaitW() != I2C_ACKED)
	return retVal;

	retVal = 0;
	if((len != 0) && (pData != NULL))
	{
		while(len--)
		{
			TWI0.MDATA = *pData;
			if(i2c_0_WaitW() == I2C_ACKED)
			{
				retVal++;
				pData++;
				continue;
			}
			else // did not get ACK after client address
			{
				break;
			}
		}
	}
	
	return retVal;
}

/* Returns how many bytes have been received, -1 means NACK at address */
uint8_t I2C_0_GetData(uint8_t address, uint8_t *pData, uint8_t len)
{
	uint8_t retVal = (uint8_t) -1;
	
	/* start transmitting the client address */
	TWI0.MADDR = address | 0x01;
	if(i2c_0_WaitW() != I2C_ACKED)
		return retVal;
	
	/* if pData[0] contains a register address, send it first */
	if(pData[0])
	{
		TWI0.MDATA = pData[0];
		if(i2c_0_WaitW() != I2C_ACKED)
		{
			return retVal;
		}
	}

	retVal = 0;
	if((len != 0) && (pData !=NULL ))
	{
		while(len--)
		{
			if(i2c_0_WaitR() == I2C_READY)
			{
				*pData = TWI0.MDATA;
				TWI0.MCTRLB = (len == 0)? TWI_ACKACT_bm | TWI_MCMD_STOP_gc : TWI_MCMD_RECVTRANS_gc;
				retVal++;
				pData++;
				continue;
			}
			else
			break;
		}
	}
	
	return retVal;
}

void I2C_0_EndSession(void)
{
	TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/


void I2C_1_Init(void)
{
	/* Select I2C pins PB2/PB3 */
	PORTMUX.TWIROUTEA = 0x08;
	
	/* Host Baud Rate Control */
	TWI1.MBAUD = TWI_BAUD((I2C_SCL_FREQ), 0.3);
	
	/* Enable TWI */
	TWI1.MCTRLA = TWI_ENABLE_bm;
	
	/* Initialize the address register */
	TWI1.MADDR = 0x00;
	
	/* Initialize the data register */
	TWI1.MDATA = 0x00;
	
	/* Set bus state idle */
	TWI1.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

static uint8_t I2C_1_WaitW(void)
{
	uint8_t state = I2C_INIT;
	do
	{
		if(TWI1.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))
		{
			if(!(TWI1.MSTATUS & TWI_RXACK_bm))
			{
				/* client responded with ack - TWI goes to M1 state */
				state = I2C_ACKED;
			}
			else
			{
				/* address sent but no ack received - TWI goes to M3 state */
				state = I2C_NACKED;
			}
		}
		else if(TWI1.MSTATUS & (TWI_BUSERR_bm | TWI_ARBLOST_bm))
		{
			/* get here only in case of bus error or arbitration lost - M4 state */
			state = I2C_ERROR;
		}
	} while(!state);
	
	return state;
}

static uint8_t I2C_1_WaitR(void)
{
	uint8_t state = I2C_INIT;
	
	do
	{
		if(TWI1.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))
		{
			state = I2C_READY;
		}
		else if(TWI1.MSTATUS & (TWI_BUSERR_bm | TWI_ARBLOST_bm))
		{
			/* get here only in case of bus error or arbitration lost - M4 state */
			state = I2C_ERROR;
		}
	} while(!state);
	
	return state;
}

/* Returns how many bytes have been sent, -1 means NACK at address, 0 means client ACKed to client address */
uint8_t I2C_1_SendData(uint8_t address, uint8_t *pData, uint8_t len)
{
	uint8_t retVal = (uint8_t) - 1;
	
	/* start transmitting the client address */
	TWI1.MADDR = address & ~0x01;
	if(I2C_1_WaitW() != I2C_ACKED)
	return retVal;

	retVal = 0;
	if((len != 0) && (pData != NULL))
	{
		while(len--)
		{
			TWI1.MDATA = *pData;
			if(I2C_1_WaitW() == I2C_ACKED)
			{
				retVal++;
				pData++;
				continue;
			}
			else // did not get ACK after client address
			{
				break;
			}
		}
	}
	
	return retVal;
}

/* Returns how many bytes have been received, -1 means NACK at address */
uint8_t I2C_1_GetData(uint8_t address, uint8_t *pData, uint8_t len)
{
	uint8_t retVal = (uint8_t) -1;
	
	/* start transmitting the client address */
	TWI1.MADDR = address | 0x01;
	if(I2C_1_WaitW() != I2C_ACKED)
	return retVal;
	
	/* if pData[0] contains a register address, send it first */
	if(pData[0])
	{
		TWI1.MDATA = pData[0];
		if(I2C_1_WaitW() != I2C_ACKED)
		{
			return retVal;
		}
	}

	retVal = 0;
	if((len != 0) && (pData !=NULL ))
	{
		while(len--)
		{
			if(I2C_1_WaitR() == I2C_READY)
			{
				*pData = TWI1.MDATA;
				TWI1.MCTRLB = (len == 0)? TWI_ACKACT_bm | TWI_MCMD_STOP_gc : TWI_MCMD_RECVTRANS_gc;
				retVal++;
				pData++;
				continue;
			}
			else
			break;
		}
	}
	
	return retVal;
}

void I2C_1_EndSession(void)
{
	TWI1.MCTRLB = TWI_MCMD_STOP_gc;
}