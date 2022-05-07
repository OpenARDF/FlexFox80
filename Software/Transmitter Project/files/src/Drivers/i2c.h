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
 * i2c.h
 */

#include "defs.h"

#ifndef I2C_H_
#define I2C_H_

#ifndef TRANQUILIZE_WATCHDOG
	#define I2C_TIMEOUT_SUPPORT /* limit number of tries for i2c success */
#endif // TRANQUILIZE_WATCHDOG
/* #define SUPPORT_I2C_CLEARBUS_FUNCTION */

#ifndef BOOL
	typedef uint8_t BOOL;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

/**
 */
	void i2c_init(void);

/**
 */
	BOOL i2c_device_read(uint8_t slaveAddr, uint8_t addr, uint8_t data[], uint8_t bytes2read);

/**
 */
	BOOL i2c_device_write(uint8_t slaveAddr, uint8_t addr, uint8_t data[], uint8_t bytes2write);

#ifdef SUPPORT_I2C_CLEARBUS_FUNCTION
/**
 */
	BOOL i2c_clearBus(void);
#endif  /* SUPPORT_I2C_CLEARBUS_FUNCTION */

#ifdef INCLUDE_DAC081C085_SUPPORT

	#define DAC081C_I2C_SLAVE_ADDR_A0 0x18 /* 24dec: ADR0 = FLT, ADR1 = FLT */
//	#define DAC081C_I2C_SLAVE_ADDR_A1 0x1A /* 26dec: ADR0 = GND, ADR1 = FLT */
//	#define DAC081C_I2C_SLAVE_ADDR_A2 0x10 /* 16dec: ADR0 = FLT, ADR1 = GND */
	#define MCP4552_I2C_SLAVE_ADDR_A0 0x58

	/**
	   Set the DAC to the value passed in setting.
	*/
	BOOL dac081c_set_dac(uint8_t setting, uint8_t addr);

#ifdef READ_DAC_SUPPORT
	/**
	   Get the current DAC setting.
	*/
	BOOL dac081c_read_dac(uint8_t *val, uint8_t addr);
#endif // READ_DAC_SUPPORT

#endif // INCLUDE_DAC081C085_SUPPORT

	/**
	   Set the POT to the value passed in setting.
	*/
	BOOL mcp4552_set_pot(uint16_t setting, uint8_t addr);


#endif  /* I2C_H_ */
