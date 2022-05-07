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
#include <util/delay.h>
#include <util/atomic.h>

#include "si5351.h"
#include "i2c.h"

/**
Prototypes for local functions
*/

#ifdef I2C_TIMEOUT_SUPPORT
/**
 */
	BOOL i2c_start(void);
#else
/**
 */
	void i2c_start(void);
#endif // I2C_TIMEOUT_SUPPORT

/**
 */
void i2c_stop(void);

/**
 */
BOOL i2c_write_success(uint8_t, uint8_t);

/**
 */
uint8_t i2c_read_ack(void);

/**
 */
uint8_t i2c_read_nack(void);

/**
 */
BOOL i2c_status(uint8_t);



volatile BOOL g_i2c_not_timed_out = TRUE;
static uint8_t g_i2c_access_semaphore = 1;

/**
 * This routine turns off the I2C bus and clears it
 * on return SCA and SCL pins are tri-state inputs.
 * You need to call Wire.begin() after this to re-enable I2C
 * This routine does NOT use the Wire library at all.
 *
 * returns 0 if bus cleared
 *         1 if SCL held low.
 *         2 if SDA held low by slave clock stretch for > 2sec
 *         3 if SDA held low after 20 clocks.
 */

#ifndef SDA_PIN
#define         SDA_PIN (1 << PINC4)
#endif

#ifndef SCL_PIN
#define         SCL_PIN (1 << PINC5)
#endif

#ifndef SDA
#define         SDA SDA_PIN
#endif

#ifndef SCL
#define         SCL SCL_PIN
#endif

#ifndef I2C
#define         I2C (SCL_PIN | SDA_PIN)
#endif

#ifdef SUPPORT_I2C_CLEARBUS_FUNCTION
	BOOL i2c_clearBus(void)
	{
#if defined (TWCR) && defined (TWEN)
			TWCR &= ~(_BV(TWEN));   /*Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly */
#endif

		/* Make SDA (data) and SCL (clock) pins Inputs with pullup. */
		uint8_t temp = DDRC;
		DDRC = temp & ~I2C;
		temp = PORTC;
		PORTC = temp | I2C;
		temp = PORTC;

		_delay_ms(1000);                    /* Wait for power to settle */
		BOOL scl_low = !(PINC & SCL_PIN);   /* Check if SCL is Low. */

		if(scl_low)
		{                                   /*If it is held low processor cannot become the I2C master. */
			return(1);                     /*I2C bus error. Could not clear SCL clock line held low */
		}

		BOOL sda_low = !(PINC & SDA_PIN);   /* vi. Check SDA input. */
		int clockCount = 20;                /* > 2x9 clock */

		while(sda_low && clockCount--)
		{
			/* Note: I2C bus is open collector so do NOT drive SCL or SDA high. */
			PORTC &= ~SCL;  /* disable pull-up on SCL */
			DDRC |= SCL;    /* drive SCL Low by making it an output */
			_delay_ms(1);
			DDRC &= ~SCL;   /* release SCL by making it an input */
			PORTC |= SCL;   /* pull SCL high again */
			_delay_ms(1);

			scl_low = !(PINC & SCL_PIN);
			int counter = 20;

			while(scl_low && counter--)
			{   /*  loop waiting for SCL to become High only wait 2sec. */
				_delay_ms(100);
				scl_low = !(PINC & SCL_PIN);
			}

			if(scl_low)
			{
				return(2);
			}

			sda_low = !(PINC & SDA_PIN);    /* Check SDA input. */
		}

		if(sda_low)
		{                                   /* still low */
			return(3);                     /* I2C bus error. Could not clear. SDA data line held low */
		}

		/* Pull SDA line low for Start or Repeated Start */
		PORTC &= ~SDA;                      /* remove SDA pull-up */
		DDRC |= SDA;                        /* drive SDA low */
		_delay_ms(1);
		DDRC &= ~SDA;                       /* make SDA input */
		PORTC |= SDA;                       /* pull SDA high */
		_delay_ms(1);
		DDRC &= ~I2C;                       /* tri-state SCL and SDA with pull-ups enabled */

		g_i2c_not_timed_out = TRUE;
		g_i2c_access_semaphore = 1;

		return(0);                          /* all ok */
	}
#endif /* SUPPORT_I2C_CLEARBUS_FUNCTION */

void i2c_init(void)
{
	power_twi_enable();

	/* set SCL to ~100 kHz for 8 MHz CPU clock */
	TWSR = 0;   /* Prescale /1 */
	TWBR = 0x25;

	/* enable I2C */
	TWCR = _BV(TWEN);
	g_i2c_access_semaphore = 1;
	g_i2c_not_timed_out = TRUE;
}

#ifdef I2C_TIMEOUT_SUPPORT
	BOOL i2c_start(void)
#else
	void i2c_start(void)
#endif
{
#ifdef I2C_TIMEOUT_SUPPORT
		TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
		while(!(TWCR & _BV(TWINT)) && g_i2c_not_timed_out)
		{
			;
		}

		if(!g_i2c_not_timed_out)    /* if timed out, reset the bus */
		{
			TWCR = (1 << TWINT) | (1 << TWSTO);
			g_i2c_not_timed_out = TRUE;
			return( TRUE);
		}

		return( FALSE);
#else
		TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
		while(!(TWCR & _BV(TWINT)))
		{
			;
		}
#endif
}

void i2c_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

BOOL i2c_write_success(uint8_t data, uint8_t success)
{
#ifdef I2C_TIMEOUT_SUPPORT
		TWDR = data;
		TWCR = _BV(TWINT) | _BV(TWEN);
		while(!(TWCR & _BV(TWINT)) && g_i2c_not_timed_out)
		{
			;
		}

		uint8_t stat = TW_STATUS;

		/* ignore timeout condition to ensure i2c_stop() gets sent */
		if(stat != success)
		{
			TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
			g_i2c_not_timed_out = TRUE;
			return(TRUE);
		}

		return( FALSE);
#else
		TWDR = data;
		TWCR = _BV(TWINT) | _BV(TWEN);
		while(!(TWCR & _BV(TWINT)))
		{
			;
		}

		/* ignore timeout condition to ensure i2c_stop() gets sent */
		if(TW_STATUS != success)
		{
			TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
			g_i2c_not_timed_out = TRUE;
			return(TRUE);
		}

		return(FALSE);
#endif
}

uint8_t i2c_read_ack(void)
{
#ifdef I2C_TIMEOUT_SUPPORT
		TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
		while(!(TWCR & _BV(TWINT)) && g_i2c_not_timed_out)
		{
			;
		}

		return(TWDR);
#else
		TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
		while(!(TWCR & _BV(TWINT)))
		{
			;
		}

		return( TWDR);
#endif
}

uint8_t i2c_read_nack(void)
{
#ifdef I2C_TIMEOUT_SUPPORT
		TWCR = _BV(TWINT) | _BV(TWEN);
		while(!(TWCR & _BV(TWINT)) && g_i2c_not_timed_out)
		{
			;
		}

		return( TWDR);
#else
		TWCR = _BV(TWINT) | _BV(TWEN);
		while(!(TWCR & _BV(TWINT)))
		{
			;
		}

		return( TWDR);
#endif
}

BOOL i2c_status(uint8_t status)
{
	if(TW_STATUS != status)
	{
		TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
		return(TRUE);
	}

	return(FALSE);
}


#ifdef SELECTIVELY_DISABLE_OPTIMIZATION
	BOOL __attribute__((optimize("O0"))) i2c_device_write(uint8_t slaveAddr, uint8_t addr, uint8_t data[], uint8_t bytes2write)
#else
	BOOL i2c_device_write(uint8_t slaveAddr, uint8_t addr, uint8_t data[], uint8_t bytes2write)
#endif
{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if(!g_i2c_access_semaphore)
			{
				return TRUE;
			}

			g_i2c_access_semaphore = 0;
		}

		#ifndef DEBUG_WITHOUT_I2C
		uint8_t index = 0;

		i2c_start();
		if(i2c_status(TW_START))
		{
			g_i2c_access_semaphore = 1;
			return(TRUE);
		}

		if(i2c_write_success(slaveAddr, TW_MT_SLA_ACK))
		{
			g_i2c_access_semaphore = 1;
			return(TRUE);
		}

		if(i2c_write_success(addr, TW_MT_DATA_ACK))
		{
			g_i2c_access_semaphore = 1;
			return(TRUE);
		}

		while(bytes2write--)
		{
			if(i2c_write_success(data[index++], TW_MT_DATA_ACK))
			{
				g_i2c_access_semaphore = 1;
				return(TRUE);
			}
		}

		i2c_stop();

		#endif  /* #ifndef DEBUG_WITHOUT_I2C */

	g_i2c_access_semaphore = 1;
	return(FALSE);
}

#ifdef SELECTIVELY_DISABLE_OPTIMIZATION
	BOOL __attribute__((optimize("O0"))) i2c_device_read(uint8_t slaveAddr, uint8_t addr, uint8_t data[], uint8_t bytes2read)
#else
	BOOL i2c_device_read(uint8_t slaveAddr, uint8_t addr, uint8_t data[], uint8_t bytes2read)
#endif
{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if(!g_i2c_access_semaphore)
			{
				return TRUE;
			}

			g_i2c_access_semaphore = 0;
		}

		#ifndef DEBUG_WITHOUT_I2C
		uint8_t index = 0;

		i2c_start();
		if(i2c_status(TW_START))
		{
			g_i2c_access_semaphore = 1;
			return(TRUE);
		}

		if(i2c_write_success(slaveAddr, TW_MT_SLA_ACK))
		{
			g_i2c_access_semaphore = 1;
			return(TRUE);
		}

		if(i2c_write_success(addr, TW_MT_DATA_ACK))
		{
			g_i2c_access_semaphore = 1;
			return(TRUE);
		}

		i2c_start();
		if(i2c_status(TW_REP_START))
		{
			g_i2c_access_semaphore = 1;
			return(TRUE);
		}

		if(i2c_write_success((slaveAddr | TW_READ), TW_MR_SLA_ACK))
		{
			g_i2c_access_semaphore = 1;
			return(TRUE);
		}

		while(bytes2read--)
		{
			if(bytes2read)
			{
				data[index++] = i2c_read_ack();
				if(i2c_status(TW_MR_DATA_ACK))
				{
					g_i2c_access_semaphore = 1;
					return(TRUE);
				}
			}
			else
			{
				data[index] = i2c_read_nack();
				if(i2c_status(TW_MR_DATA_NACK))
				{
					g_i2c_access_semaphore = 1;
					return(TRUE);
				}
			}
		}

		i2c_stop();

		#endif  /* #ifndef DEBUG_WITHOUT_I2C */

	g_i2c_access_semaphore = 1;
	return(FALSE);
}

/************************************************************************/
/* MCP4552 Support  (8-bit rheostat)                                    */
/************************************************************************/

BOOL mcp4552_set_pot(uint16_t setting, uint8_t addr)
{
	BOOL result;
	uint8_t byte1=0, byte2=0;

	byte1 |= ((0x0100 & setting) >> 8);
	byte2 |= (uint8_t)(setting & 0x00FF);
	result = i2c_device_write(addr, byte1, &byte2, 1);

	return result;
}

/************************************************************************/
/* DAC081C085 Support                                                   */
/************************************************************************/

BOOL dac081c_set_dac(uint8_t setting, uint8_t addr)
{
	BOOL result;
	uint8_t byte1=0, byte2=0;

	byte1 |= (setting >> 4);
	byte2 |= (setting << 4);
	result = i2c_device_write(addr, byte1, &byte2, 1);

	return result;
}

#ifdef READ_DAC_SUPPORT
#ifdef SELECTIVELY_DISABLE_OPTIMIZATION
BOOL __attribute__((optimize("O0"))) dac081c_read_dac(uint8_t *val, uint8_t addr)
#else
BOOL dac081c_read_dac(uint8_t *val, addr)
#endif
{
	uint8_t bytes[2];
	uint8_t bytes2read = 2;
	uint8_t index = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(!g_i2c_access_semaphore)
		{
			return TRUE;
		}

		g_i2c_access_semaphore = 0;
	}

	i2c_start();
	if(i2c_status(TW_START))
	{
		g_i2c_access_semaphore = 1;
		return(TRUE);
	}

	if(i2c_write_success((addr | TW_READ), TW_MR_SLA_ACK))
	{
		g_i2c_access_semaphore = 1;
		return(TRUE);
	}

	while(bytes2read--)
	{
		if(bytes2read)
		{
			bytes[index++] = i2c_read_ack();
			if(i2c_status(TW_MR_DATA_ACK))
			{
				g_i2c_access_semaphore = 1;
				return(TRUE);
			}
		}
		else
		{
			bytes[index] = i2c_read_nack();
			if(i2c_status(TW_MR_DATA_NACK))
			{
				g_i2c_access_semaphore = 1;
				return(TRUE);
			}
		}
	}

	i2c_stop();

	bytes[0] = (bytes[0] << 4);
	bytes[0] |= (bytes[1] >> 4);

	g_i2c_access_semaphore = 1;
	*val = bytes[0];
	return(FALSE);
}
#endif // READ_DAC_SUPPORT
