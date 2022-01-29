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
/*
 * serialbus.h - a simple serial inter-processor communication protocol.
 */

#ifndef SERIALBUS_H_
#define SERIALBUS_H_

#include "defs.h"
#include "usart_basic.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define SERIALBUS_USART USART_1

#define SERIALBUS_MAX_MSG_LENGTH 50
#define SERIALBUS_MIN_MSG_LENGTH 2    /* shortest message: GO */
#define SERIALBUS_MAX_MSG_FIELD_LENGTH 20
#define SERIALBUS_MAX_MSG_NUMBER_OF_FIELDS 3
#define SERIALBUS_NUMBER_OF_RX_MSG_BUFFERS 2
#define SERIALBUS_MAX_TX_MSG_LENGTH 41
#define SERIALBUS_NUMBER_OF_TX_MSG_BUFFERS 3

#define SERIALBUS_MAX_COMMANDLINE_LENGTH ((1 + SERIALBUS_MAX_MSG_FIELD_LENGTH) * SERIALBUS_MAX_MSG_NUMBER_OF_FIELDS)

#define SERIALBUS_POWERUP_DELAY_SECONDS 6

#define SERIALBUS_MIN_TX_INTERVAL_MS 100

#define SB_BAUD 9600
//#define MYUBRR(b) ((F_CPU + b * 8L) / (b * 16L) - 1)

// typedef enum
// {
// 	SB_EMPTY_BUFF,
// 	SB_FULL_BUFF
// } BufferState;

/*  Serialbus Messages
 *       Message formats:
 *               CMD [a1 [a2]]
 *
 *               where
 *                       CMD = command
 *                       a1, a2 = optional arguments or data fields
 *
 */

typedef enum
{
	SB_MESSAGE_EMPTY = 0,

	/*	ARDUCON MESSAGE FAMILY (SERIAL MESSAGING) */
	SB_MESSAGE_SET_FOX = 'F' * 100 + 'O' * 10 + 'X',           /* Set the fox role to be used to define timing and signals */
	SB_MESSAGE_UTIL = 'U' * 100 + 'T' * 10 + 'I',              /* Temperature  and Voltage data */
	SB_MESSAGE_SET_STATION_ID = 'I' * 10 + 'D',                /* Sets amateur radio callsign text */
	SB_MESSAGE_SYNC = 'S' * 100 + 'Y' * 10 + 'N',              /* Synchronizes transmissions */
	SB_MESSAGE_CODE_SETTINGS = 'S' * 100 + 'E' * 10 + 'T',     /* Set Morse code speeds */
	SB_MESSAGE_CLOCK = 'C' * 100 + 'L' * 10 + 'K',             /* Set or read the RTC */
	SB_MESSAGE_PASSWORD = 'P' * 100 + 'W' * 10 + 'D',			/* Password command */
	SB_MESSAGE_TX_POWER = 'P' * 100 + 'W' * 10 + 'R',			/* Transmit power */
	SB_MESSAGE_TX_FREQ = 'F' * 100 + 'R' * 10 + 'E',				/* Transmit frequency */

	SB_INVALID_MESSAGE = MAX_UINT16                            /* This value must never overlap a valid message ID */
} SBMessageID;

typedef enum
{
	SERIALBUS_MSG_UNKNOWN = 0,
	SERIALBUS_MSG_COMMAND,
	SERIALBUS_MSG_QUERY,
	SERIALBUS_MSG_REPLY,
	SERIALBUS_MSG_INVALID
} SBMessageType;

typedef enum
{
	SB_FIELD1 = 0,
	SB_FIELD2 = 1
} SBMessageField;

typedef enum
{
	SB_BATTERY_BROADCAST = 0x0001,
	SB_RSSI_BROADCAST = 0x0002,
	SB_RF_BROADCAST = 0x0004,
	SB_UPC_TEMP_BROADCAST = 0x0008,
	SB_ALL_BROADCASTS = 0x000FF
} SBbroadcastType;

// typedef enum
// {
// 	SB_NO_ID = 0,
// 	SB_CONTROL_HEAD_ID = 1,
// 	SB_RECEIVER_ID = 2,
// 	SB_TRANSMITTER_ID = 3
// } DeviceID;

typedef char SerialbusTxBuffer[SERIALBUS_MAX_TX_MSG_LENGTH];

typedef struct
{
	SBMessageType type;
	SBMessageID id;
	char fields[SERIALBUS_MAX_MSG_NUMBER_OF_FIELDS][SERIALBUS_MAX_MSG_FIELD_LENGTH];
} SerialbusRxBuffer;

#define WAITING_FOR_UPDATE -1
#define HELP_TEXT_TXT (char*)"\nCommands:\n  CLK [T|S|F [\"YYMMDDhhmmss\"]] - Read/set time/start/finish\n  FOX [fox]- Set fox role\n  FRE [frequency] - Set tx frequency\n  PWR [0-5000] - Set transmit power mW\n  ID [callsign] -  Set callsign\n  SYN 0-3 - Synchronize\n  PWD [pwd] - Set DTMF password\n  UTI - Read volts & temp\n  SET S [setting] - Set ID code speed\n\0"


/**
 */
void serialbus_init(uint32_t baud, USART_Number_t usart);

/**
 * Immediately turns off receiver and flushes receive buffer
 */
void serialbus_disable(void);

/**
 */
void serialbus_end_tx(void);

/**
 */
// void serialbus_reset_rx(void);

/**
 */
SerialbusTxBuffer* nextEmptySBTxBuffer(void);

/**
 */
SerialbusTxBuffer* nextFullSBTxBuffer(void);

/**
 */
bool serialbusTxInProgress(void);

/**
 */
SerialbusRxBuffer* nextEmptySBRxBuffer(void);

/**
 */
SerialbusRxBuffer* nextFullSBRxBuffer(void);

/**
 */
void sb_send_NewPrompt(void);

/**
 */
void sb_send_NewLine(void);

/**
 */
void sb_echo_char(uint8_t c);

/**
 */
bool sb_send_string(char* str);

/**
 */
void sb_send_value(uint16_t value, char* label);

/**
 */
bool sb_enabled(void);

#ifdef __cplusplus
	}
#endif

#endif  /* SERIALBUS_H_ */
