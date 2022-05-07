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
 * linkbus.h - a simple serial inter-processor communication protocol.
 */

#ifndef LINKBUS_H_
#define LINKBUS_H_

#include "defs.h"
#include "transmitter.h"
#include "si5351.h"

#define LINKBUS_MAX_MSG_LENGTH 50
#define LINKBUS_MIN_MSG_LENGTH 3    /* shortest message: $TTY; */
#define LINKBUS_MAX_MSG_FIELD_LENGTH 21
#define LINKBUS_MAX_MSG_NUMBER_OF_FIELDS 3
#define LINKBUS_NUMBER_OF_RX_MSG_BUFFERS 2
#define LINKBUS_NUMBER_OF_TX_MSG_BUFFERS 4

#define LINKBUS_POWERUP_DELAY_SECONDS 6

#define LINKBUS_MIN_TX_INTERVAL_MS 100

#define FOSC 8000000    /* Clock Speed */
#define BAUD 9600
//#define BAUD 19200
#define MYUBRR(b) (FOSC / 16 / (b) - 1)

typedef enum
{
	EMPTY_BUFF,
	FULL_BUFF
} BufferState;

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
 *       $CK0 - Set Si5351 CLK0: field1 = freq (Hz); field2 = enable (BOOL)
 *       $CK1 - Set Si5351 CLK1: field1 = freq (Hz); field2 = enable (BOOL)
 *       $CK2 - Set Si5351 CLK2: field1 = freq (Hz); field2 = enable (BOOL)
 *       $VOL - Set audio volume: field1 = inc/decr (BOOL); field2 = % (int)
 *       $BAT? - Subscribe to battery voltage reports
 *
 *       DUAL-BAND RX MESSAGE FAMILY (FUNCTIONAL MESSAGING)
 *       $BND  - Set/Get radio band to 2m or 80m
 *       $S?   - Subscribe to signal strength reports
 *                 - Subscribe to gain setting reports
 *                 -
 */

typedef enum
{
	MESSAGE_EMPTY = 0,

	/* TEST EQUIPMENT MESSAGE FAMILY (TEST DEVICE MESSAGING) */
	MESSAGE_BAND = 'B' * 100 + 'N' * 10 + 'D',      /* $BND,; / $BND? / !BND,; // Set band; field1 = RadioBand */
	MESSAGE_OSC = 'O' * 100 + 'S' * 10 + 'C',		/* Calibrate oscillator for best baud rate */
	MESSAGE_VER = 'V' * 100 + 'E' * 10 + 'R',		/* Request current software version number */

	/*	DUAL-BAND TX MESSAGE FAMILY (FUNCTIONAL MESSAGING) */
	MESSAGE_SET_FREQ = 'F' * 100 + 'R' * 10 + 'E',  /* $FRE,Fhz; / $FRE,FHz? / !FRE,; // Set/request current frequency */
	MESSAGE_CLOCK = 'T' * 100 + 'I' * 10 + 'M',		/* Sets/reads the real-time clock */
	MESSAGE_STARTFINISH = 'S' * 10 + 'F',			/* Sets the start and finish times */
	MESSAGE_BAT = 'B' * 100 + 'A' * 10 + 'T',       /* Battery charge data */
	MESSAGE_TEMP = 'T' * 100 + 'E' * 10 + 'M',      /* Temperature  data */
	MESSAGE_PERM = 'P' * 100 + 'R' * 10 + 'M',		/* Saves most settings to EEPROM "perm" */
	MESSAGE_TX_POWER = 'P' * 100 + 'O' * 10 + 'W',	/* Sets transmit power level */
	MESSAGE_TX_MOD = 'M' * 100 + 'O' * 10 + 'D',    /* Sets 2m modulation format to AM or CW */
#ifdef DONOTUSE
	MESSAGE_DRIVE_LEVEL = 'D' * 100 + 'R' * 10 + 'I', /*  Adjust 2m drive level */
#endif // DONOTUSE
	MESSAGE_SET_STATION_ID = 'I' * 10 + 'D',        /* Sets amateur radio callsign text */
	MESSAGE_SET_PATTERN = 'P' * 10 + 'A',           /* Sets unique transmit pattern */
	MESSAGE_CODE_SPEED = 'S' * 100 + 'P' * 10 + 'D', /* Sets id and pattern code speeds */
	MESSAGE_TIME_INTERVAL = 'T',					/* Sets on-air, off-air, delay, and ID time intervals */
	MESSAGE_ESP_COMM = 'E' * 100 + 'S' * 10 + 'P',  /* Communications with ESP8266 controller */
	MESSAGE_GO = 'G' * 10 + 'O',					/* Start transmitting now without delay */

	/* UTILITY MESSAGES */
	MESSAGE_RESET = 'R' * 100 + 'S' * 10 + 'T',		/* Processor reset */
	MESSAGE_WIFI = 'W' * 10 + 'I',					/* Enable/disable WiFi */
	MESSAGE_BIAS = 'B',
	INVALID_MESSAGE = UINT16_MAX					/* This value must never overlap a valid message ID */
} LBMessageID;

#define MESSAGE_CLOCK_LABEL "TIM"
#define MESSAGE_ESP_LABEL "ESP"
#define MESSAGE_ERRORCODE_LABEL "EC"
#define MESSAGE_STATUSCODE_LABEL "SC"
#define MESSAGE_BAND_LABEL "BND"
#define MESSAGE_VER_LABEL "VER"
#define MESSAGE_SET_FREQ_LABEL "FRE"
#define MESSAGE_TX_POWER_LABEL "POW"
#define MESSAGE_ACK "!ACK;"

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
	FIELD1 = 0,
	FIELD2 = 1,
	FIELD3 = 2
} LBMessageField;

typedef enum
{
	BATTERY_BROADCAST = 0x0001,
	RSSI_BROADCAST = 0x0002,
	RF_BROADCAST = 0x0004,
	UPC_TEMP_BROADCAST = 0x0008,
	ALL_BROADCASTS = 0x000FF
} LBbroadcastType;

typedef enum
{
	NO_ID = 0,
	CONTROL_HEAD_ID = 1,
	RECEIVER_ID = 2,
	TRANSMITTER_ID = 3
} DeviceID;

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
void linkbus_init(uint32_t baud);

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
LinkbusTxBuffer* nextEmptyTxBuffer(void);

/**
 */
LinkbusTxBuffer* nextFullTxBuffer(void);

/**
 */
BOOL linkbusTxInProgress(void);

/**
 */
LinkbusRxBuffer* nextEmptyRxBuffer(void);

/**
 */
LinkbusRxBuffer* nextFullRxBuffer(void);

/**
 */
void lb_send_sync(void);

/**
 */
BOOL linkbus_send_text(char* text);

/**
*/
void lb_send_ESP(LBMessageType msgType, char* msg);

/**
 */
void lb_send_msg(LBMessageType msgType, char* msgLabel, char* msgStr);

/**
 */
void lb_broadcast_num(uint16_t data, char* str);

/**
 */
void lb_send_Help(void);


/**
 */
void calibrateOscillator(uint8_t cal);


/**
 */
uint8_t calcOSCCAL(uint8_t val);

#endif  /* LINKBUS_H_ */
