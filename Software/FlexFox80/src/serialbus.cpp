/*
 *  MIT License
 *
 *  Copyright (c) 2021 DigitalConfections
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
/* serialbus.c
 *
 */

#include "defs.h"
#include "serialbus.h"
#include "usart_basic.h"
#include <string.h>
#include <stdio.h>
#include "atmel_start_pins.h"


#ifdef ATMEL_STUDIO_7
#include <avr/eeprom.h>
#endif  /* ATMEL_STUDIO_7 */

#ifdef ATMEL_STUDIO_7
#include <string.h>
#include <stdio.h>
#endif  /* ATMEL_STUDIO_7 */

/* Global Variables */
static volatile BOOL g_bus_disabled = TRUE;
static const char crlf[] = "\n";
static char lineTerm[8] = "\n";
static const char textPrompt[] = "> ";

static char g_tempMsgBuff[SERIALBUS_MAX_MSG_LENGTH];

/* Local function prototypes */
BOOL serialbus_start_tx(void);
BOOL serialbus_send_text(char* text);

/* Module global variables */
static volatile BOOL serialbus_tx_active = FALSE; /* volatile is required to ensure optimizer handles this properly */
static SerialbusTxBuffer tx_buffer[SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS];
static SerialbusRxBuffer rx_buffer[SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS];

SerialbusTxBuffer* nextFullSBTxBuffer(void)
{
	BOOL found = TRUE;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(tx_buffer[bufferIndex][0] == '\0')
	{
		if(++count >= SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			found = FALSE;
			break;
		}

		bufferIndex++;
		if(bufferIndex >= SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			bufferIndex = 0;
		}
	}

	if(found)
	{
		return( &tx_buffer[bufferIndex]);
	}

	return(null);
}

SerialbusTxBuffer* nextEmptySBTxBuffer(void)
{
	BOOL found = TRUE;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(tx_buffer[bufferIndex][0] != '\0')
	{
		if(++count >= SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			found = FALSE;
			break;
		}

		bufferIndex++;
		if(bufferIndex >= SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			bufferIndex = 0;
		}
	}

	if(found)
	{
		return( &tx_buffer[bufferIndex]);
	}

	return(null);
}

SerialbusRxBuffer* nextEmptySBRxBuffer(void)
{
	BOOL found = TRUE;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(rx_buffer[bufferIndex].id != SB_MESSAGE_EMPTY)
	{
		if(++count >= SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			found = FALSE;
			break;
		}

		bufferIndex++;
		if(bufferIndex >= SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			bufferIndex = 0;
		}
	}

	if(found)
	{
		return( &rx_buffer[bufferIndex]);
	}

	return(null);
}

SerialbusRxBuffer* nextFullSBRxBuffer(void)
{
	BOOL found = TRUE;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(rx_buffer[bufferIndex].id == SB_MESSAGE_EMPTY)
	{
		if(++count >= SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			found = FALSE;
			break;
		}

		bufferIndex++;
		if(bufferIndex >= SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			bufferIndex = 0;
		}
	}

	if(found)
	{
		return( &rx_buffer[bufferIndex]);
	}

	return(null);
}


/***********************************************************************
 * serialbusTxInProgress(void)
 ************************************************************************/
BOOL serialbusTxInProgress(void)
{
	return(serialbus_tx_active);
}

BOOL serialbus_start_tx(void)
{
	BOOL success = !serialbus_tx_active;

	if(success) /* message will be lost if transmit is busy */
	{
		serialbus_tx_active = TRUE;
		USART0_enable_tx();
	}

	return(success);
}

void serialbus_end_tx(void)
{
	if(serialbus_tx_active)
	{
		USART0.CTRLA &= ~(1 << USART_DREIE_bp); /* Transmit Data Register Empty Interrupt Enable: disable */
		serialbus_tx_active = FALSE;
	}
}

// void serialbus_reset_rx(void)
// {
// 	if(UCSR0B & (1 << RXEN0))   /* perform only if rx is currently enabled */
// 	{
// 		UCSR0B &= ~(1 << RXEN0);
// 		memset(rx_buffer, 0, sizeof(rx_buffer));
// 		UCSR0B |= (1 << RXEN0);
// 	}
// }


/* configure the pins and initialize the registers */
void USART0_initialization(uint32_t baud)
{

	// Set pin direction to input
	PA1_set_dir(PORT_DIR_IN);

	PA1_set_pull_mode(
	// <y> Pull configuration
	// <id> pad_pull_config
	// <PORT_PULL_OFF"> Off
	// <PORT_PULL_UP"> Pull-up
	PORT_PULL_OFF);

	// Set pin direction to output

	PA0_set_level(
	// <y> Initial level
	// <id> pad_initial_level
	// <false"> Low
	// <true"> High
	false);

	PA0_set_dir(PORT_DIR_OUT);

	USART0_init(baud);
}


void serialbus_init(uint32_t baud)
{
	memset(rx_buffer, 0, sizeof(rx_buffer));

	for(int bufferIndex=0; bufferIndex<SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}

	/*Set baud rate */
	USART0_initialization(SB_BAUD);
	g_bus_disabled = FALSE;
}

void serialbus_disable(void)
{
	uint8_t bufferIndex;

	g_bus_disabled = TRUE;
	USART0_disable();
	serialbus_end_tx();
	memset(rx_buffer, 0, sizeof(rx_buffer));

	for(bufferIndex=0; bufferIndex<SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}
}

// void serialbus_enable(void)
// {
// 	uint8_t bufferIndex;
//
// 	g_bus_disabled = FALSE;
// 	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
//
// 	memset(rx_buffer, 0, sizeof(rx_buffer));
//
// 	for(bufferIndex = 0; bufferIndex < SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
// 	{
// 		tx_buffer[bufferIndex][0] = '\0';
// 	}
// }


BOOL serialbus_send_text(char* text)
{
	BOOL err = TRUE;
	uint16_t tries = 200;

	if(g_bus_disabled)
	{
		return( err);
	}

	if(text)
	{
		SerialbusTxBuffer* buff = nextEmptySBTxBuffer();

		while(!buff && tries)
		{
			while(serialbusTxInProgress() && tries)
			{
				if(tries)
				{
					tries--;    /* wait until transmit finishes */
				}
			}
			buff = nextEmptySBTxBuffer();
		}

		if(buff)
		{
			sprintf(*buff, text);

			serialbus_start_tx();
			err = FALSE;
		}
	}

	return(err);
}


/***********************************************************************************
 *  Support for creating and sending various Terminal Mode Serialbus messages is provided below.
 ************************************************************************************/

void sb_send_NewPrompt(void)
{
	if(g_bus_disabled)
	{
		return;
	}

	while(serialbus_send_text((char*)textPrompt))
	{
		;
	}
}

void sb_send_NewLine(void)
{
	if(g_bus_disabled)
	{
		return;
	}
	serialbus_send_text((char*)crlf);
}

void sb_echo_char(uint8_t c)
{
	if(g_bus_disabled)
	{
		return;
	}
	g_tempMsgBuff[0] = c;
	g_tempMsgBuff[1] = '\0';
	serialbus_send_text(g_tempMsgBuff);
}

BOOL sb_send_string(char* str, BOOL wait)
{
	BOOL err = FALSE;

	if(g_bus_disabled)
	{
		return( TRUE);
	}

	if(str == NULL)
	{
		return( TRUE);
	}

	if(!*str)
	{
		return(TRUE);
	}

	if(strlen(str) > SERIALBUS_MAX_TX_MSG_LENGTH)
	{
		return( TRUE);
	}

	strncpy(g_tempMsgBuff, str, SERIALBUS_MAX_TX_MSG_LENGTH);

	if(wait)
	{
		while((err = serialbus_send_text(g_tempMsgBuff)))
		{
			;
		}
		while(!err && serialbusTxInProgress())
		{
			;
		}
	}
	else
	{
		err = serialbus_send_text(g_tempMsgBuff);
	}

	return( err);
}

void sb_send_value(uint16_t value, char* label)
{
	BOOL err;

	if(g_bus_disabled)
	{
		return;
	}

	sprintf(g_tempMsgBuff, "> %s=%d%s", label, value, lineTerm);
	while((err = serialbus_send_text(g_tempMsgBuff)))
	{
		;
	}
	while(!err && serialbusTxInProgress())
	{
		;
	}
}


BOOL sb_enabled(void)
{
	return( !g_bus_disabled);
}
