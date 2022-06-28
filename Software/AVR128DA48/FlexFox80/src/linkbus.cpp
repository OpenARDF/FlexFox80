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
 * linkbus.c
 *
 */

#include "linkbus.h"
#include "defs.h"
#include "usart_basic.h"
#include <string.h>
#include <stdio.h>
#include <avr/wdt.h>

/* Global Variables */
static volatile bool g_bus_disabled = true;
USART_Number_t g_linkbus_usart_number = USART_NOT_SET;

static char g_tempMsgBuff[LINKBUS_MAX_MSG_LENGTH];

/* Local function prototypes */
static bool linkbus_start_tx(void);
// static void USART0_initialization(uint32_t baud);
static void USART1_initialization(uint32_t baud);
static void USART4_initialization(uint32_t baud);

/* Module global variables */
static volatile bool linkbus_tx_active = false; // volatile is required to ensure optimizer handles this properly
static LinkbusTxBuffer tx_buffer[LINKBUS_NUMBER_OF_TX_MSG_BUFFERS];
static LinkbusRxBuffer rx_buffer[LINKBUS_NUMBER_OF_RX_MSG_BUFFERS];

LinkbusTxBuffer* nextFullLBTxBuffer(void)
{
	bool found = true;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(tx_buffer[bufferIndex][0] == '\0')
	{
		if(++count >= LINKBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			found = false;
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

LinkbusTxBuffer* nextEmptyLBTxBuffer(void)
{
	bool found = true;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(tx_buffer[bufferIndex][0] != '\0')
	{
		if(++count >= LINKBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			found = false;
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

LinkbusRxBuffer* nextEmptyLBRxBuffer(void)
{
	bool found = true;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(rx_buffer[bufferIndex].id != LB_MESSAGE_EMPTY)
	{
		if(++count >= LINKBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			found = false;
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

LinkbusRxBuffer* nextFullLBRxBuffer(void)
{
	bool found = true;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(rx_buffer[bufferIndex].id == LB_MESSAGE_EMPTY)
	{
		if(++count >= LINKBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			found = false;
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
bool linkbusTxInProgress(void)
{
	return(linkbus_tx_active);
}

bool linkbus_start_tx(void)
{
	bool success = !linkbus_tx_active;

	if(success) /* message will be lost if transmit is busy */
	{
		linkbus_tx_active = true;
		
		if(g_linkbus_usart_number == USART_4)
		{	
			USART4_enable_tx();
		}
		else
		{
			USART1_enable_tx();
		}
	}

	return(success);
}

void linkbus_end_tx(void)
{
	if(linkbus_tx_active)
	{
		if(g_linkbus_usart_number == USART_0)
		{	
			USART4.CTRLA &= ~(1 << USART_DREIE_bp); /* Transmit Data Register Empty Interrupt Enable: disable */
		}
		else
		{
			USART1.CTRLA &= ~(1 << USART_DREIE_bp); /* Transmit Data Register Empty Interrupt Enable: disable */
		}
		
		linkbus_tx_active = false;
	}
}

void linkbus_reset_rx(void)
{
		if(g_linkbus_usart_number == USART_0)
		{	
			if(USART4.CTRLB & (1 << USART_RXEN_bp))   /* perform only if rx is currently enabled */
			{
				USART4.CTRLB &= ~(1 << USART_RXEN_bp);
				memset(rx_buffer, 0, sizeof(rx_buffer));
				USART4.CTRLB |= (1 << USART_RXEN_bp);
			}
		}
		else
		{
			if(USART1.CTRLB & (1 << USART_RXEN_bp))   /* perform only if rx is currently enabled */
			{
				USART1.CTRLB &= ~(1 << USART_RXEN_bp);
				memset(rx_buffer, 0, sizeof(rx_buffer));
				USART1.CTRLB |= (1 << USART_RXEN_bp);
			}
		}
}

/* configure the pins and initialize the registers */
// void USART0_initialization(uint32_t baud)
// {
// 
// 	// Set Rx pin direction to input
// 	PA1_set_dir(PORT_DIR_IN);
// 	PA1_set_pull_mode(PORT_PULL_OFF);
// 
// 	// Set Tx pin direction to output
// 	PA0_set_dir(PORT_DIR_OUT);
// 	PA0_set_level(HIGH);
// 
// 	USART0_init(baud);
// }

/* configure the pins and initialize the registers */
void USART1_initialization(uint32_t baud)
{

	// Set Rx pin direction to input
	PC1_set_dir(PORT_DIR_IN);
	PC1_set_pull_mode(PORT_PULL_OFF);

	// Set Tx pin direction to output
	PC0_set_dir(PORT_DIR_OUT);
	PC0_set_level(HIGH);

	USART1_init(baud, false);
}

/* configure the pins and initialize the registers */
void USART4_initialization(uint32_t baud)
{

	// Set Rx pin direction to input
	PE1_set_dir(PORT_DIR_IN);
	PE1_set_pull_mode(PORT_PULL_OFF);

	// Set Tx pin direction to output
	PE0_set_dir(PORT_DIR_OUT);
	PE0_set_level(HIGH);

	USART4_init(baud, false);
}

void linkbus_init(uint32_t baud, USART_Number_t usart)
{
	memset(rx_buffer, 0, sizeof(rx_buffer));

	for(int bufferIndex=0; bufferIndex<LINKBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}

	if((usart != USART_DO_NOT_CHANGE) || (g_linkbus_usart_number == USART_NOT_SET))
	{
		if(usart == USART_4)
		{
			USART4_initialization(baud);
		}
		else
		{
			USART1_initialization(baud);
		}
		
		g_linkbus_usart_number = usart;
	}

	g_bus_disabled = false;
}

void linkbus_disable(void)
{
	uint8_t bufferIndex;

	g_bus_disabled = true;

	if(g_linkbus_usart_number == USART_4)
	{	
		USART4_disable();
	}
	else
	{
		USART1_disable();
	}
	
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

	g_bus_disabled = false;

	if(g_linkbus_usart_number == USART_4)
	{	
		USART4_enable();
	}
	else
	{
		USART1_enable();
	}

	memset(rx_buffer, 0, sizeof(rx_buffer));

	for(bufferIndex=0; bufferIndex<LINKBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}
}


bool lb_send_text(char* text)
{
	bool err = true;
	uint16_t tries = 200;

	if(g_bus_disabled) return err;

	if(text)
	{
		LinkbusTxBuffer* buff = nextEmptyLBTxBuffer();

		while(!buff && tries)
		{
			while(linkbusTxInProgress() && tries)
			{
				if(tries) tries--;   /* wait until transmit finishes */
			}
			buff = nextEmptyLBTxBuffer();
		}

		if(buff)
		{
			sprintf(*buff, text);

			linkbus_start_tx();
			err = false;
		}
	}

	return(err);
}


/***********************************************************************************
 *  Support for creating and sending various Linkbus messages is provided below.
 ************************************************************************************/

void lb_send_msg(LBMessageType msgType, const char* msgLabel, char* msgStr)
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

	lb_send_text(g_tempMsgBuff);
}


void lb_send_sync(void)
{
	sprintf(g_tempMsgBuff, ".....");
	lb_send_text(g_tempMsgBuff);
}


void lb_broadcast_str(const char* data, const char* str)
{
	if(str)
	{
		sprintf(g_tempMsgBuff, "%s,%s;", str, data);
	}

	if(g_tempMsgBuff[0]) lb_send_text(g_tempMsgBuff);
}
