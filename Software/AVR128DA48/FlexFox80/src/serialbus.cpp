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
volatile uint16_t g_serial_timeout_ticks = 200;
USART_Number_t g_serialbus_usart_number = USART_NOT_SET;
static volatile bool g_serialbus_disabled = true;
static const char crlf[] = "\n";
static char lineTerm[8] = "\n";
static const char textPrompt[] = "> ";

static char g_tempMsgBuff[SERIALBUS_MAX_MSG_LENGTH];

/* Local function prototypes */
bool serialbus_start_tx(void);
bool serialbus_send_text(char* text);
static void USART1_initialization(uint32_t baud);
static void USART4_initialization(uint32_t baud);

/* Module global variables */
static volatile bool serialbus_tx_active = false; /* volatile is required to ensure optimizer handles this properly */
static volatile SerialbusTxBuffer tx_buffer[SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS];
static volatile SerialbusRxBuffer rx_buffer[SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS];

SerialbusTxBuffer* nextFullSBTxBuffer(void)
{
	bool found = true;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(tx_buffer[bufferIndex][0] == '\0')
	{
		if(++count >= SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			found = false;
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
		return((SerialbusTxBuffer*)&tx_buffer[bufferIndex]);
	}

	return(null);
}

SerialbusTxBuffer* nextEmptySBTxBuffer(void)
{
	bool found = true;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(tx_buffer[bufferIndex][0] != '\0')
	{
		if(++count >= SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS)
		{
			found = false;
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
		return((SerialbusTxBuffer*)&tx_buffer[bufferIndex]);
	}

	return(null);
}

SerialbusRxBuffer* nextEmptySBRxBuffer(void)
{
	bool found = true;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(rx_buffer[bufferIndex].id != SB_MESSAGE_EMPTY)
	{
		if(++count >= SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			found = false;
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
		for(int i=0; i<SERIALBUS_MAX_MSG_NUMBER_OF_FIELDS; i++)
		{
			rx_buffer[bufferIndex].fields[i][0] = '\0';
		}
		
		return((SerialbusRxBuffer*)&rx_buffer[bufferIndex]);
	}

	return(null);
}

SerialbusRxBuffer* nextFullSBRxBuffer(void)
{
	bool found = true;
	static uint8_t bufferIndex = 0;
	uint8_t count = 0;

	while(rx_buffer[bufferIndex].id == SB_MESSAGE_EMPTY)
	{
		if(++count >= SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS)
		{
			found = false;
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
		return((SerialbusRxBuffer*)&rx_buffer[bufferIndex]);
	}

	return(null);
}


/***********************************************************************
 * serialbusTxInProgress(void)
 ************************************************************************/
bool serialbusTxInProgress(void)
{
	return(serialbus_tx_active);
}

bool serialbus_start_tx(void)
{
	bool success = !serialbus_tx_active;

	if(success) /* message will be lost if transmit is busy */
	{
		serialbus_tx_active = true;
		
		if(g_serialbus_usart_number == USART_0)
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

void serialbus_end_tx(void)
{
	if(serialbus_tx_active)
	{
		if(g_serialbus_usart_number == USART_4)
		{
			USART4.CTRLA &= ~(1 << USART_DREIE_bp); /* Transmit Data Register Empty Interrupt Enable: disable */
		}
		else
		{
			USART1.CTRLA &= ~(1 << USART_DREIE_bp); /* Transmit Data Register Empty Interrupt Enable: disable */
		}
		
		serialbus_tx_active = false;
	}
}

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


void serialbus_init(uint32_t baud, USART_Number_t usart)
{
	memset((SerialbusRxBuffer*)rx_buffer, 0, sizeof(*(SerialbusRxBuffer*)rx_buffer));
	serialbus_end_tx();

	for(int bufferIndex=0; bufferIndex<SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}

	if((usart != USART_DO_NOT_CHANGE) || (g_serialbus_usart_number == USART_NOT_SET))
	{
		if(usart == USART_4)
		{
			USART4_initialization(baud);
		}
		else
		{
			USART1_initialization(baud);
		}
	
		g_serialbus_usart_number = usart;
	}

	g_serialbus_disabled = false;
}

void serialbus_disable(void)
{
	uint8_t bufferIndex;

	g_serialbus_disabled = true;

	if(g_serialbus_usart_number == USART_4)
	{	
		USART4_disable();
	}
	else
	{
		USART1_disable();
	}
	
	serialbus_end_tx();
	memset((SerialbusRxBuffer*)rx_buffer, 0, sizeof(*(SerialbusRxBuffer*)rx_buffer));

	for(bufferIndex=0; bufferIndex<SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS; bufferIndex++)
	{
		tx_buffer[bufferIndex][0] = '\0';
	}
}


bool serialbus_send_text(char* text)
{
	bool err = true;

	if(g_serialbus_disabled)
	{
		return( err);
	}

	uint16_t tries;
	if(text)
	{
		SerialbusTxBuffer* buff = nextEmptySBTxBuffer();
		tries = 200;
		
		while(!buff && tries--)
		{
			uint16_t spin = 500;
			while(serialbusTxInProgress() && spin--); /* Wait for previous transmission to complete */
			buff = nextEmptySBTxBuffer();
		}

		if(buff)
		{
			sprintf(*buff, text);
			serialbus_start_tx();
			err = false;
		}
	}

	return(err);
}


/***********************************************************************************
 *  Support for creating and sending various Terminal Mode Serialbus messages is provided below.
 ************************************************************************************/

void sb_send_NewPrompt(void)
{
	if(g_serialbus_disabled)
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
	if(g_serialbus_disabled)
	{
		return;
	}
	serialbus_send_text((char*)crlf);
}

void sb_echo_char(uint8_t c)
{
	if(g_serialbus_disabled)
	{
		return;
	}
	g_tempMsgBuff[0] = c;
	g_tempMsgBuff[1] = '\0';
	serialbus_send_text(g_tempMsgBuff);
}

bool sb_send_string(char* str)
{
	char buf[SERIALBUS_MAX_TX_MSG_LENGTH+1];
	bool err = false;
	uint16_t length, lengthToSend, lengthSent=0;
	bool done = false;

	if(g_serialbus_disabled)
	{
		return( true);
	}

	if(str == NULL)
	{
		return( true);
	}

	if(!*str)
	{
		return(true);
	}

	length = strlen(str);

	do
	{
		lengthToSend = MIN(length-lengthSent, (uint16_t)(SERIALBUS_MAX_TX_MSG_LENGTH-1));
		strncpy(buf, &str[lengthSent], lengthToSend);

		buf[lengthToSend] = '\0';
		err = serialbus_send_text(buf);
		
		g_serial_timeout_ticks = 200;
		if(!err)
		{
			while(serialbusTxInProgress() && g_serial_timeout_ticks)
			{
				;
			}
			
			if(!g_serial_timeout_ticks) err = true;
		}

		lengthSent += lengthToSend;
		done = err || (lengthSent >= length);
	}while(!done);

	return( err);
}

void sb_send_value(uint16_t value, char* label)
{
	bool err;

	if(g_serialbus_disabled)
	{
		return;
	}

	sprintf(g_tempMsgBuff, "> %s=%d%s", label, value, lineTerm);
	while((err = serialbus_send_text(g_tempMsgBuff)))
	{
		;
	}
	
	g_serial_timeout_ticks = 200;
	while(!err && serialbusTxInProgress() && g_serial_timeout_ticks)
	{
		;
	}
}


bool sb_enabled(void)
{
	return( !g_serialbus_disabled);
}
