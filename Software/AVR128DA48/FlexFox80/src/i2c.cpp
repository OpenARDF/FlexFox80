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
 * i2c.c
 */

#include "defs.h"
#include <util/twi.h>
#include <avr/power.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/atomic.h>

#include "si5351.h"
#include "i2c.h"
#include "port.h"

#ifdef TWI_BAUD
#undef TWI_BAUD
#endif

// #define TWI_BAUD(F_SCL, T_RISE)        (uint8_t)((((((float)F_CPU / (float)(F_SCL)) - 10 - (((float)(F_CPU) * (T_RISE))/1000000.0))) / 2))
// #define I2C_SCL_FREQ                    100000  /* Frequency [Hz] */

#define CLK_PER                                         24000000     // 24MHz default clock
#define TWI0_BAUD(F_SCL, T_RISE)                 ((((((float)CLK_PER / (float)F_SCL)) - 10 - ((float)CLK_PER * T_RISE))) / 2)
#define TWI1_BAUD(F_SCL, T_RISE)                 ((((((float)CLK_PER / (float)F_SCL)) - 10 - ((float)CLK_PER * T_RISE))) / 2)

#define I2C_SCL_FREQ                                    100000


enum {
	I2C_INIT = 0,
	I2C_ACKED,
	I2C_NACKED,
	I2C_READY,
	I2C_ERROR
};

volatile uint16_t g_i2c0_timeout_ticks = 200; 
volatile uint16_t g_i2c1_timeout_ticks = 200;

/************************************************************************/
/* I2C_0                                                               */
/************************************************************************/

void I2C_0_Shutdown(void)
{
	/* Set bus state idle */
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
	TWI0.MCTRLA = 0;
}

void I2C_0_Init(void)
{
	PORTMUX.TWIROUTEA &= 0x0A;
	PORTMUX.TWIROUTEA |= 0x02;
	
	/* Host Baud Rate Control */
//	TWI0.MBAUD = TWI0_BAUD((I2C_SCL_FREQ), 0.3);
	TWI0.MBAUD = (uint8_t)TWI0_BAUD(I2C_SCL_FREQ, 0);
	
	/* Enable TWI */
	TWI0.MCTRLA = TWI_ENABLE_bm;
	
	/* Initialize the address register */
	TWI0.MADDR = 0x00;
	
	/* Initialize the data register */
	TWI0.MDATA = 0x00;
	
	/* Set bus state idle */
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
	/* Select I2C pins PC2/PC3 */
}

static uint8_t i2c_0_WaitW(void)
{
	uint8_t state = I2C_INIT;
	
	g_i2c0_timeout_ticks = 50;
	
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
	} while(!state && g_i2c0_timeout_ticks);
	
	if(!g_i2c0_timeout_ticks) 
	{
		state = I2C_ERROR;
	}
	
	return state;
}

static uint8_t i2c_0_WaitR(void)
{
	uint8_t state = I2C_INIT;
	
	g_i2c0_timeout_ticks = 50;
	
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
	} while(!state && g_i2c0_timeout_ticks);
	
	return state;
}

/* Returns how many bytes have been sent, -1 means NACK at address, 0 means client ACKed to client address */
uint8_t I2C_0_SendData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
	uint8_t retVal = (uint8_t) - 1;
	
	/* Send slave address */
	TWI0.MADDR = slaveAddr;
	if(i2c_0_WaitW() != I2C_ACKED)
	{
		return retVal;
	}
	
	/* Send the register address */
	TWI0.MDATA = regAddr;
	if(i2c_0_WaitW() != I2C_ACKED)
	{
		return retVal;
	}

	retVal = 0;
	if((len != 0) && (pData != null))
	{
		while(len--)
		{
			TWI0.MDATA = *pData;
			if(i2c_0_WaitW() == I2C_ACKED)
			{
				retVal++;
				pData++;
				if(!len) I2C_0_EndSession();
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
uint8_t I2C_0_GetData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
	uint8_t retVal = (uint8_t) -1;
	
	/* Send the client address for write */
	TWI0.MADDR = slaveAddr;
	if(i2c_0_WaitW() != I2C_ACKED)
	{
		return retVal;
	}
	
	/* Send the register address */
	TWI0.MDATA = regAddr;
	if(i2c_0_WaitW() != I2C_ACKED)
	{
		return retVal;
	}
	
	/* Send the client address for read */
	TWI0.MADDR = slaveAddr | 0x01;
	if(i2c_0_WaitW() != I2C_ACKED)
	{
		return retVal;
	}
	
	retVal = 0;
	if((len != 0) && (pData !=null ))
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
/* I2C_1                                                               */
/************************************************************************/

void I2C_1_Shutdown(void)
{
	/* Set bus state idle */
	TWI1.MSTATUS = TWI_BUSSTATE_IDLE_gc;
	TWI1.MCTRLA = 0;
}

void I2C_1_Init(void)
{
	/* Select I2C pins PB2/PB3 */
	PORTMUX.TWIROUTEA &= 0x03;
	PORTMUX.TWIROUTEA |= 0x08;
	
	/* Host Baud Rate Control */
//	TWI1.MBAUD = TWI1_BAUD((I2C_SCL_FREQ), 0.3);
	TWI1.MBAUD = (uint8_t)TWI0_BAUD(I2C_SCL_FREQ, 0);
		
	/* Enable TWI */
	TWI1.MCTRLA = TWI_ENABLE_bm;
	
	/* Initialize the address register */
	TWI1.MADDR = 0x00;
	
	/* Initialize the data register */
	TWI1.MDATA = 0x00;
	
	/* Set bus state idle */
	TWI1.MSTATUS = TWI_BUSSTATE_IDLE_gc;
	
	PORTB_set_pin_pull_mode(2, PORT_PULL_UP);
	PORTB_set_pin_pull_mode(3, PORT_PULL_UP);
}

static uint8_t i2c_1_WaitW(void)
{
	uint8_t state = I2C_INIT;
	
	g_i2c1_timeout_ticks = 50;
	
	do
	{
		uint8_t stat = TWI1.MSTATUS;
		if(stat & (TWI_WIF_bm | TWI_RIF_bm))
		{
			if(!(stat & TWI_RXACK_bm))
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
		else if(stat & (TWI_BUSERR_bm | TWI_ARBLOST_bm))
		{
			/* get here only in case of bus error or arbitration lost - M4 state */
			state = I2C_ERROR;
		}
	} while(!state && g_i2c1_timeout_ticks);
	
	return state;
}

static uint8_t I2C_1_WaitR(void)
{
	uint8_t state = I2C_INIT;
	
	g_i2c0_timeout_ticks = 50;
	
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
	} while(!state && g_i2c0_timeout_ticks);
	
	return state;
}

/* Returns how many bytes have been sent, -1 means NACK at address, 0 means client ACKed to client address */
uint8_t I2C_1_SendData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
	uint8_t retVal = (uint8_t) - 1;
	
	/* Send the slave address */
	TWI1.MADDR = slaveAddr & ~0x01;
	if(i2c_1_WaitW() != I2C_ACKED)
	{
		return retVal;
	}
	
	/* Send the register address */
	TWI1.MDATA = regAddr;	
	if(i2c_1_WaitW() != I2C_ACKED)
	{
		return retVal;
	}	

	retVal = 0;
	if((len != 0) && (pData != null))
	{
		while(len--)
		{
			TWI1.MDATA = *pData;
			if(i2c_1_WaitW() == I2C_ACKED)
			{
				retVal++;
				pData++;
				if(!len) I2C_1_EndSession();
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
uint8_t I2C_1_GetData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
	uint8_t retVal = (uint8_t) -1;
	
	/* Send the client address for write */
	TWI1.MADDR = slaveAddr;
	if(i2c_1_WaitW() != I2C_ACKED)
	{
		return retVal;
	}
	
	/* Send the register address */
	TWI1.MDATA = regAddr;	
	if(i2c_1_WaitW() != I2C_ACKED)
	{
		return retVal;
	}
	
	/* Send the client address for read */
	TWI1.MADDR = slaveAddr | 0x01;
	if(i2c_1_WaitW() != I2C_ACKED)
	{
		return retVal;	
	}
	
	retVal = 0;
	if((len != 0) && (pData !=null ))
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

