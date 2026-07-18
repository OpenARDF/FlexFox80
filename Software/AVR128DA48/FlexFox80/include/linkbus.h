/*
 *  MIT License
 *
 *  Copyright (c) 2022-2026 DigitalConfections
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
 * FlexFox Linkbus framing, parsing, command dispatch, and reporting.
 */

#ifndef LINKBUS_H_
#define LINKBUS_H_

#include "defs.h"
#include "usart_basic.h"

#define LINKBUS_USART USART_1

#define LINKBUS_MAX_MSG_LENGTH 50
#define LINKBUS_MIN_MSG_LENGTH 3    /* shortest message: $TTY; */
#define LINKBUS_MAX_MSG_FIELD_LENGTH 21
#define LINKBUS_MAX_MSG_NUMBER_OF_FIELDS 3
#define LINKBUS_MAX_MSG_ID_LENGTH 3
#define LINKBUS_NUMBER_OF_RX_MSG_BUFFERS 3
#define LINKBUS_NUMBER_OF_TX_MSG_BUFFERS 4

#define LINKBUS_POWERUP_DELAY_SECONDS 6

#define LINKBUS_MIN_TX_INTERVAL_MS 100

#define FOSC 8000000    /* Clock Speed */
#define LB_BAUD 9600
 

#define FINISH_TIME_RECEIVED_B			(1 << 0)
#define MESSAGE_PATTERN_RECEIVED_B		(1 << 1)
#define OFF_TIME_RECEIVED_B				(1 << 2)
#define ON_TIME_RECEIVED_B				(1 << 3)
#define OFFSET_TIME_RECEIVED_B			(1 << 4)
#define ID_INTERVAL_RECEIVED_B			(1 << 5)
#define TX_POWER_RECEIVED_B				(1 << 6)
#define FREQUENCY_RECEIVED_B			(1 << 7)
#define STATION_ID_RECEIVED_B			(1 << 8)
#define ID_CODE_SPEED_RECEIVED_B		(1 << 9)
#define PATTERN_CODE_SPEED_RECEIVED_B	(1 << 10)
#define START_TIME_RECEIVED_B			(1 << 11)
#define FULLY_CONFIGURED_EVENT			(FINISH_TIME_RECEIVED_B | MESSAGE_PATTERN_RECEIVED_B | OFF_TIME_RECEIVED_B | ON_TIME_RECEIVED_B | OFFSET_TIME_RECEIVED_B | ID_INTERVAL_RECEIVED_B | TX_POWER_RECEIVED_B | FREQUENCY_RECEIVED_B | STATION_ID_RECEIVED_B | PATTERN_CODE_SPEED_RECEIVED_B | ID_CODE_SPEED_RECEIVED_B | START_TIME_RECEIVED_B)


//#define MYUBRR(b) (FOSC / 16 / (b) - 1)

// typedef enum
// {
// 	LB_EMPTY_BUFF,
// 	LB_FULL_BUFF
// } BufferState;

/*  Linkbus Messages
 *       Message formats:
 *               $id,f1,f2... fn;
 *               !id,f1,f2,... fn;
 *               $id,f1,f2,... fn?
 *
 *               where
 *                       $ = command - ! indicates a response or broadcast to subscribers
 *                       id = linkbus MessageID
 *                       fn = variable length fields
 *                       ; = end of message flag - ? = end of query
 *       Null fields in settings commands indicates no change should be applied
 *       All null fields indicates a polling request for current settings
 *       ? terminator indicates subscription request to value changes
 *       Sending a query with fields containing data, is the equivalent of sending
 *         a command followed by a query (i.e., a response is requested).
 *
 *       TEST EQUIPMENT MESSAGE FAMILY (DEVICE MESSAGING)
 *       $TST - Test message
 *       !ACK - Simple acknowledgment to a command (sent when required)
 *       $CK0 - Set Si5351 CLK0: field1 = freq (Hz); field2 = enable (bool)
 *       $CK1 - Set Si5351 CLK1: field1 = freq (Hz); field2 = enable (bool)
 *       $CK2 - Set Si5351 CLK2: field1 = freq (Hz); field2 = enable (bool)
 *       $VOL - Set audio volume: field1 = inc/decr (bool); field2 = % (int)
 *       $BAT? - Subscribe to battery voltage reports
 *
 */

#define LINKBUS_ID1(a) ((uint32_t)(uint8_t)(a))
#define LINKBUS_ID2(a, b) ((LINKBUS_ID1(a) << 8) | LINKBUS_ID1(b))
#define LINKBUS_ID3(a, b, c) ((LINKBUS_ID2(a, b) << 8) | LINKBUS_ID1(c))

enum LBMessageID : uint32_t
{
	LB_MESSAGE_EMPTY = 0,

	/* LEGACY MESSAGES */
	LB_MESSAGE_BAND = LINKBUS_ID3('B', 'N', 'D'),      /* $BND,; / $BND? / !BND,; // Set band; field1 = RadioBand */
	LB_MESSAGE_TX_MOD = LINKBUS_ID3('M', 'O', 'D'),    /* Sets 2m modulation format to AM or CW */
	
	/* INFORMATIONAL MESSAGES */
	LB_MESSAGE_VER = LINKBUS_ID3('V', 'E', 'R'),		/* Request current software version number */
	LB_MESSAGE_BAT = LINKBUS_ID3('B', 'A', 'T'),       /* Battery charge data */
	LB_MESSAGE_TEMP = LINKBUS_ID3('T', 'E', 'M'),      /* Temperature  data */

	/*	DUAL-BAND TX MESSAGE FAMILY (FUNCTIONAL MESSAGING) */
	LB_MESSAGE_SET_FREQ = LINKBUS_ID3('F', 'R', 'E'),  /* $FRE,Fhz; / $FRE,FHz? / !FRE,; // Set/request current frequency */
	LB_MESSAGE_CLOCK = LINKBUS_ID3('T', 'I', 'M'),		/* Sets/reads the real-time clock */
	LB_MESSAGE_STARTFINISH = LINKBUS_ID2('S', 'F'),		/* Sets the start and finish times */
	LB_MESSAGE_PERM = LINKBUS_ID3('P', 'R', 'M'),		/* Saves most settings to EEPROM "perm" */
	LB_MESSAGE_TX_POWER = LINKBUS_ID3('P', 'O', 'W'),	/* Sets transmit power level */
#ifdef DONOTUSE
	LB_MESSAGE_DRIVE_LEVEL = LINKBUS_ID3('D', 'R', 'I'), /*  Adjust 2m drive level */
#endif // DONOTUSE
	LB_MESSAGE_SET_STATION_ID = LINKBUS_ID2('I', 'D'), /* Sets amateur radio callsign text */
	LB_MESSAGE_SET_PATTERN = LINKBUS_ID2('P', 'A'),    /* Sets unique transmit pattern */
	LB_MESSAGE_CODE_SPEED = LINKBUS_ID3('S', 'P', 'D'), /* Sets id and pattern code speeds */
	LB_MESSAGE_TIME_INTERVAL = LINKBUS_ID1('T'),		/* Sets on-air, off-air, delay, and ID time intervals */
	LB_MESSAGE_ESP_COMM = LINKBUS_ID3('E', 'S', 'P'),  /* Communications with ESP8266 controller */
	LB_MESSAGE_GO = LINKBUS_ID2('G', 'O'),				/* Start transmitting now without delay */

	/* UTILITY MESSAGES */
	LB_MESSAGE_KEY = LINKBUS_ID3('K', 'E', 'Y'),		/* Key down/up */
	LB_MESSAGE_RESET = LINKBUS_ID3('R', 'S', 'T'),		/* Processor reset */
	LB_MESSAGE_WIFI = LINKBUS_ID2('W', 'I'),			/* Enable/disable WiFi */
	LB_MESSAGE_UPDATE = LINKBUS_ID3('U', 'P', 'D'),	/* Enter resident bootloader after ESP authorization and safety checks */
	LB_INVALID_MESSAGE = 0xFFFFFFFFUL					/* This value must never overlap a valid message ID */
};

#define LB_MESSAGE_CLOCK_LABEL "TIM"
#define LB_MESSAGE_ESP_LABEL "ESP"
#define LB_MESSAGE_ERRORCODE_LABEL "EC"
#define LB_MESSAGE_STATUSCODE_LABEL "SC"
#define LB_MESSAGE_BAND_LABEL "BND"
#define LB_MESSAGE_VER_LABEL "VER"
#define LB_MESSAGE_UPDATE_LABEL "UPD"
#define LB_MESSAGE_SET_FREQ_LABEL "FRE"
#define LB_MESSAGE_TX_POWER_LABEL "POW"
#define LB_MESSAGE_ACK "!ACK;"
#define LB_MESSAGE_NACK "!NAK;"

typedef enum
{
	LINKBUS_MSG_UNKNOWN = 0,
	LINKBUS_MSG_COMMAND,
	LINKBUS_MSG_QUERY,
	LINKBUS_MSG_REPLY,
	LINKBUS_MSG_INVALID
} LBMessageType;

typedef enum
{
	LB_MSG_FIELD1 = 0,
	LB_MSG_FIELD2 = 1,
	LB_MSG_FIELD3 = 2
} LBMessageField;

// typedef enum
// {
// 	LB_BATTERY_BROADCAST = 0x0001,
// 	LB_RSSI_BROADCAST = 0x0002,
// 	LB_RF_BROADCAST = 0x0004,
// 	LB_UPC_TEMP_BROADCAST = 0x0008,
// 	LB_ALL_BROADCASTS = 0x000FF
// } LBbroadcastType;

// typedef enum
// {
// 	NO_ID = 0,
// 	CONTROL_HEAD_ID = 1,
// 	RECEIVER_ID = 2,
// 	TRANSMITTER_ID = 3
// } DeviceID;

typedef char LinkbusTxBuffer[LINKBUS_MAX_MSG_LENGTH];

typedef struct
{
	LBMessageType type;
	LBMessageID id;
	char fields[LINKBUS_MAX_MSG_NUMBER_OF_FIELDS][LINKBUS_MAX_MSG_FIELD_LENGTH];
} LinkbusRxBuffer;

#define WAITING_FOR_UPDATE -1

/**
 */
void linkbus_init(uint32_t baud, USART_Number_t usart);

/**
 * Immediately turns off receiver and flushes receive buffer
 */
void linkbus_disable(void);

/**
 * Undoes linkbus_disable()
 */
void linkbus_enable(void);

/**
 */
void linkbus_end_tx(void);

/**
 */
void linkbus_reset_rx(void);

/**
 */
LinkbusTxBuffer* nextEmptyLBTxBuffer(void);

/**
 */
LinkbusTxBuffer* nextFullLBTxBuffer(void);

/**
 */
bool linkbusTxInProgress(void);

/**
 */
LinkbusRxBuffer* nextEmptyLBRxBuffer(void);

/**
 */
LinkbusRxBuffer* nextFullLBRxBuffer(void);

/**
 */
void lb_send_sync(void);

/**
 */
bool lb_send_text(char* text);

/**
*/
void lb_send_ESP(LBMessageType msgType, char* msg);

/**
 */
void lb_send_msg(LBMessageType msgType, const char* msgLabel, char* msgStr);

/**
 */
void lb_broadcast_str(const char* data, const char* str);

#endif  /* LINKBUS_H_ */
