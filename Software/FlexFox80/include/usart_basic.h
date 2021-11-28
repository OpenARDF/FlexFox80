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


#ifndef USART_BASIC_H_INCLUDED
#define USART_BASIC_H_INCLUDED

#include <atmel_start.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	USART_0,
	USART_1,
	USART_2,
	USART_3,
	USART_4,
	USART_5,
	USART_DO_NOT_CHANGE,
	USART_NOT_SET
	} USART_Number_t;

/* Normal Mode, Baud register value */
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

int8_t USART0_init(uint32_t baud);

void USART0_enable();

void USART0_enable_rx();

void USART0_enable_tx();

void USART0_disable();

uint8_t USART0_get_data();

bool USART0_is_tx_ready();

bool USART0_is_rx_ready();

bool USART0_is_tx_busy();

uint8_t USART0_read(void);

void USART0_write(const uint8_t data);

/* Normal Mode, Baud register value */
#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

int8_t USART1_init(uint32_t baud);

void USART1_enable();

void USART1_enable_rx();

void USART1_enable_tx();

void USART1_disable();

uint8_t USART1_get_data();

bool USART1_is_tx_ready();

bool USART1_is_rx_ready();

bool USART1_is_tx_busy();

uint8_t USART1_read(void);

void USART1_write(const uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* USART_BASIC_H_INCLUDED */
