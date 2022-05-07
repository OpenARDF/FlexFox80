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
 * linkbus.c
 *
 */

#include "linkbus.h"
#include "defs.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include <avr/wdt.h>

/* Global Variables */
static volatile BOOL g_bus_disabled = TRUE;

static char g_tempMsgBuff[LINKBUS_MAX_MSG_LENGTH];

/* Local function prototypes */
BOOL linkbus_start_tx(void);

/* Module global variables */
static volatile BOOL linkbus_tx_active = FALSE; // volatile is required to ensure optimizer handles this properly
static LinkbusTxBuffer tx_buffer[LINKBUS_NUMBER_OF_TX_MSG_BUFFERS];
static LinkbusRxBuffer rx_buffer[LINKBUS_NUMBER_OF_RX_MSG_BUFFERS];

LinkbusTxBuffer* nextFullTxBuffer(void)
{
	BOOL found = TRUE;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(tx_buffer[bufferIndex][0] == '\0')
	{
		if(++count >= LINKBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			found = FALSE;
			break;
		}

		bufferIndex++;
		if(bufferIndex >= LINKBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			bufferIndex = 0;
		}
	}

	if(found)
	{
		return( &tx_buffer[bufferIndex]);
	}

	return(NULL);
}

LinkbusTxBuffer* nextEmptyTxBuffer(void)
{
	BOOL found = TRUE;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(tx_buffer[bufferIndex][0] != '\0')
	{
		if(++count >= LINKBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			found = FALSE;
			break;
		}

		bufferIndex++;
		if(bufferIndex >= LINKBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			bufferIndex = 0;
		}
	}

	if(found)
	{
		return( &tx_buffer[bufferIndex]);
	}

	return(NULL);
}

LinkbusRxBuffer* nextEmptyRxBuffer(void)
{
	BOOL found = TRUE;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(rx_buffer[bufferIndex].id != MESSAGE_EMPTY)
	{
		if(++count >= LINKBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			found = FALSE;
			break;
		}

		bufferIndex++;
		if(bufferIndex >= LINKBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			bufferIndex = 0;
		}
	}

	if(found)
	{
		return( &rx_buffer[bufferIndex]);
	}

	return(NULL);
}

LinkbusRxBuffer* nextFullRxBuffer(void)
{
	BOOL found = TRUE;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(rx_buffer[bufferIndex].id == MESSAGE_EMPTY)
	{
		if(++count >= LINKBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			found = FALSE;
			break;
		}

		bufferIndex++;
		if(bufferIndex >= LINKBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			bufferIndex = 0;
		}
	}

	if(found)
	{
		return( &rx_buffer[bufferIndex]);
	}

	return(NULL);
}


/***********************************************************************
 * linkbusTxInProgress(void)
 ************************************************************************/
BOOL linkbusTxInProgress(void)
{
	return(linkbus_tx_active);
}

BOOL linkbus_start_tx(void)
{
	BOOL success = !linkbus_tx_active;

	if(success) /* message will be lost if transmit is busy */
	{
		linkbus_tx_active = TRUE;
		UCSR0B |= (1 << UDRIE0);
	}

	return(success);
}

void linkbus_end_tx(void)
{
	if(linkbus_tx_active)
	{
		UCSR0B &= ~(1 << UDRIE0);
		linkbus_tx_active = FALSE;
	}
}

void linkbus_reset_rx(void)
{
	if(UCSR0B & (1 << RXEN0))   /* perform only if rx is currently enabled */
	{
		UCSR0B &= ~(1 << RXEN0);
/*		uint16_t s = sizeof(rx_buffer); // test */
		memset(rx_buffer, 0, sizeof(rx_buffer));
/*		if(s) s = 0; // test */
		UCSR0B |= (1 << RXEN0);
	}
}

void linkbus_init(uint32_t baud)
{
	memset(rx_buffer, 0, sizeof(rx_buffer));

	for(int bufferIndex=0; bufferIndex<LINKBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}

	/*Set baud rate */
	uint16_t myubrr = MYUBRR(baud);
	UBRR0H = (uint8_t)(myubrr >> 8);
	UBRR0L = (uint8_t)myubrr;
	/* Enable receiver and transmitter and related interrupts */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
/*	UCSR0B = (1<<RXEN0) | (1<<TXEN0); */
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	g_bus_disabled = FALSE;
}

void linkbus_disable(void)
{
	uint8_t bufferIndex;

	g_bus_disabled = TRUE;
	UCSR0B = 0;
	linkbus_end_tx();
	memset(rx_buffer, 0, sizeof(rx_buffer));

	for(bufferIndex=0; bufferIndex<LINKBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}
}

void linkbus_enable(void)
{
	uint8_t bufferIndex;

	g_bus_disabled = FALSE;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	memset(rx_buffer, 0, sizeof(rx_buffer));

	for(bufferIndex=0; bufferIndex<LINKBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}
}


BOOL linkbus_send_text(char* text)
{
	BOOL err = TRUE;
	uint16_t tries = 200;

	if(g_bus_disabled) return err;

	if(text)
	{
		LinkbusTxBuffer* buff = nextEmptyTxBuffer();

		while(!buff && tries)
		{
			while(linkbusTxInProgress() && tries)
			{
				if(tries) tries--;   /* wait until transmit finishes */
			}
			buff = nextEmptyTxBuffer();
		}

		if(buff)
		{
			sprintf(*buff, text);

			linkbus_start_tx();
			err = FALSE;
		}
	}

	return(err);
}


/***********************************************************************************
 *  Support for creating and sending various Linkbus messages is provided below.
 ************************************************************************************/

void lb_send_msg(LBMessageType msgType, char* msgLabel, char* msgStr)
{
	char prefix = '$';
	char terminus = ';';

	if(msgType == LINKBUS_MSG_REPLY)
	{
		prefix = '!';
	}
	else if(msgType == LINKBUS_MSG_QUERY)
	{
		terminus = '?';
	}

	sprintf(g_tempMsgBuff, "%c%s,%s%c", prefix, msgLabel, msgStr, terminus);

	linkbus_send_text(g_tempMsgBuff);
}


void lb_send_sync(void)
{
	sprintf(g_tempMsgBuff, ".....");
	linkbus_send_text(g_tempMsgBuff);
}


void lb_broadcast_num(uint16_t data, char* str)
{
	char t[6] = "\0";

	sprintf(t, "%u", data);
	g_tempMsgBuff[0] = '\0';

	if(str)
	{
		sprintf(g_tempMsgBuff, "%s,%s;", str, t);
	}

	if(g_tempMsgBuff[0]) linkbus_send_text(g_tempMsgBuff);
}


void calibrateOscillator(uint8_t cal)
{
	sprintf(g_tempMsgBuff, "$OSC,%u;", cal);
	linkbus_send_text(g_tempMsgBuff);
	if(cal) OSCCAL = cal;
}

uint8_t calcOSCCAL(uint8_t val)
{
	static int sum = 0;
	static int count = 0;
	uint8_t result = 0;

	if((val >= 10) && (val <= 240))
	{
		sum += val;
		count++;
		result = sum / count;
	}
	else if(val == 255)
	{
		sum = 0;
		count = 0;
	}
	else if(val == 0)
	{
		if(count)
		{
			result = sum / count;
		}
	}

	return(result);
}

