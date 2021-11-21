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
 * mcp23017.h
 *
 * http://ww1.microchip.com/downloads/en/DeviceDoc/20001952C.pdf
 * The MCP23017 device provides 16-bit, general
 *  purpose parallel I/O expansion for I2C bus applications.
 *
 */


#ifndef MCP23017_H_
#define MCP23017_H_

typedef enum {
	MCP23017_PORTA,
	MCP23017_PORTB
	}ExpanderPort;

	/* PortB
	*	PB0 - FTDI Present Input (low)
	*	PB1 - Clone Present Input (low)
	*	PB2 - WiFi Reset Output (low)
	*	PB3 - WiFi Module Power Enable Output (high)
	*	PB4 - DI Master/Clone Output (high = Master; low = Clone)
	*	PB5 - Audio Amp Enable Output (high)
	*	PB6 - Headphone Power Latch (high)
	*	PB7 - Client UART Enable (low)
	*/

typedef enum {
	FTDI_PRESENT,
	CLONE_PRESENT,
	WIFI_RESET,
	WIFI_POWER_ENABLE,
	MASTER_DEVICE,
	AUDIO_AMP_ENABLE,
	POWER_LATCH_ENABLE,
	CLIENT_UART_ENABLE
} DIbit;	

/**
 */
void mcp23017_init(void);

/**
 */
void mcp23017_writePort(uint8_t data, uint8_t port);

/**
 */
BOOL mcp23017_readPort(uint8_t *databyte, ExpanderPort port);

/**
 */
void mcp23017_set( ExpanderPort port, DIbit bit, BOOL value );

#endif  /* MCP23017_H_ */