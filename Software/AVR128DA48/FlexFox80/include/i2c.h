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
 *
 * i2c.h
 */

#include "defs.h"

#ifndef I2C_H_INCLUDED
#define	I2C_H_INCLUDED

//#include <xc.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
	#endif /* __cplusplus */

	void    I2C_0_Init(void);
	uint8_t I2C_0_SendData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint8_t len); // returns how many bytes have been sent, -1 means NACK at address
	uint8_t I2C_0_GetData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint8_t len); // returns how many bytes have been received, -1 means NACK at address
	void    I2C_0_EndSession(void);
	void	I2C_0_Shutdown(void);

	void    I2C_1_Init(void);
	uint8_t I2C_1_SendData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint8_t len); // returns how many bytes have been sent, -1 means NACK at address
	uint8_t I2C_1_GetData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint8_t len); // returns how many bytes have been received, -1 means NACK at address
	void    I2C_1_EndSession(void);
	void	I2C_1_Shutdown(void);

	#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* I2C_H_INCLUDED */
