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
 *mcp23017.c
 *
 */

#include "defs.h"

#ifdef INCLUDE_MCP23017_SUPPORT

#include "mcp23017.h"
#include <util/twi.h>
#include "i2c.h"

#define MCP23017_I2C_SLAVE_ADDR_A100 0x48

#define ADDR_IODIRA 0x00
#define ADDR_IODIRB 0x01
#define ADDR_IPOLA 0x02
#define ADDR_IPOLB 0x03
#define ADDR_GPINTENA 0x04
#define ADDR_GPINTENB 0x05
#define ADDR_DEFVALA 0x06
#define ADDR_DEFVALB 0x07
#define ADDR_INTCONA 0x08
#define ADDR_INTCONB 0x09
#define ADDR_IOCON 0x0b
#define ADDR_GPPUA 0x0c
#define ADDR_GPPUB 0x0d
#define ADDR_INTFA 0x0e
#define ADDR_INTFB 0x0f
#define ADDR_INTCAPA 0x10
#define ADDR_INTCAPB 0x11
#define ADDR_GPIOA 0x12
#define ADDR_GPIOB 0x13
#define ADDR_OLATA 0x14
#define ADDR_OLATB 0x15

static uint8_t g_data[2] = {0, 0};

#ifdef SELECTIVELY_DISABLE_OPTIMIZATION
	void __attribute__((optimize("O0"))) mcp23017_init(void)
#else
	void mcp23017_init(void)
#endif
{
	g_data[0] = 0b00100000; // disable SEQOP
	if(i2c_device_write(MCP23017_I2C_SLAVE_ADDR_A100, ADDR_IOCON, g_data, 1))
	{
		g_data[0] = 0;
	}
	
	/*
	* PortA
	*	PA0 - 80m Antenna Detect 0 (input)
	*	PA1 - 80m Antenna Detect 1 (input)
	*	PA2 - 2m Antenna Detect (input)
	*	PA3 - VHF enable (high)
	*	PA4 - HF enable (high)
	*	PA5 - Test Point
	*	PA6 - T enable (transmit switch - high)
	*	PA7 - R enable (receive switch - high)
	*/
	g_data[0] = 0b00000111;
	if(i2c_device_write(MCP23017_I2C_SLAVE_ADDR_A100, ADDR_IODIRA, g_data, 1))
	{
		g_data[0] = 0;
	}

	g_data[0] = 0b00000000;
	if(i2c_device_write(MCP23017_I2C_SLAVE_ADDR_A100, ADDR_OLATA, g_data, 1))
	{
		g_data[0] = 0;
	}

	/*
	* PortB
	*	PB0 - FTDI Present Input (low)
	*	PB1 - Clone Present Input (low)
	*	PB2 - WiFi Reset Output (low)
	*	PB3 - WiFi Module Power Enable Output (high)
	*	PB4 - DI Master/Clone Output (high = Master; low = Clone)
	*	PB5 - Audio Amp Enable Output (high)
	*	PB6 - Headphone Power Latch (high)
	*	PB7 - Client UART Enable (low)
	*/
	g_data[0] = 0b00000011;
	if(i2c_device_write(MCP23017_I2C_SLAVE_ADDR_A100, ADDR_IODIRB, g_data, 1))
	{
		g_data[0] = 0;
	}

	g_data[0] = 0b10000000; // 0b10001100;
	if(i2c_device_write(MCP23017_I2C_SLAVE_ADDR_A100, ADDR_OLATB, g_data, 1))
	{
		g_data[0] = 0;
	}
}

void mcp23017_set( ExpanderPort port, DIbit bit, BOOL value )
{
	uint8_t mask = (1 << bit);	

	mcp23017_readPort(g_data, port);
	
	if(value)
	{
		g_data[0] |= mask;
	}
	else
	{
		g_data[0] &= ~mask;
	}
	
	mcp23017_writePort(g_data[0], port);
}

void mcp23017_writePort( uint8_t databyte, ExpanderPort port)
{
	i2c_device_write(MCP23017_I2C_SLAVE_ADDR_A100, (port == MCP23017_PORTA) ? ADDR_GPIOA:ADDR_GPIOB, &databyte, 1);
}

BOOL mcp23017_readPort(uint8_t *databyte, ExpanderPort port)
{
	return(i2c_device_read(MCP23017_I2C_SLAVE_ADDR_A100, (port == MCP23017_PORTA) ? ADDR_GPIOA:ADDR_GPIOB, databyte, 1));
}

#endif  /* #ifdef INCLUDE_MCP23017_SUPPORT */
