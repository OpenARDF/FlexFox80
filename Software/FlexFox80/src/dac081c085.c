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
 * dac081c085.c
 *
 */

#include "dac081c085.h"
#include "i2c.h"
#include <util/twi.h>

void dac081c_set_dac(uint8_t setting, uint8_t addr)
{
	uint8_t byte1=0, byte2=0;
	
	byte1 |= (setting >> 4);
	byte2 |= (setting << 4);
	i2c_device_write(addr, byte1, &byte2, 1);
}

#ifdef SELECTIVELY_DISABLE_OPTIMIZATION
	uint8_t __attribute__((optimize("O0"))) dac081c_read_dac(uint8_t addr)
#else
	uint8_t dac081c_read_dac(uint8_t addr)
#endif
{
	uint8_t bytes[2];
	uint8_t bytes2read = 2;
	uint8_t index = 0;
	
	i2c_start();
	if(i2c_status(TW_START))
	{
		return(TRUE);
	}

	if(i2c_write_success((addr | TW_READ), TW_MR_SLA_ACK))
	{
		return(TRUE);
	}

	while(bytes2read--)
	{
		if(bytes2read)
		{
			bytes[index++] = i2c_read_ack();
			if(i2c_status(TW_MR_DATA_ACK))
			{
				return(TRUE);
			}
		}
		else
		{
			bytes[index] = i2c_read_nack();
			if(i2c_status(TW_MR_DATA_NACK))
			{
				return(TRUE);
			}
		}
	}

	i2c_stop();
		
	bytes[0] = (bytes[0] << 4);
	bytes[0] |= (bytes[1] >> 4);

	return(bytes[0]);
}
