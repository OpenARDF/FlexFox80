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


#include "morse.h"
#include <stddef.h>

/*
   Structure to describe a Morse code character
*/
typedef struct {
	uint8_t		pattern;
	uint8_t		lengthInSymbols;
	uint8_t		lengthInElements;
} MorseCharacter;

MorseCharacter getMorseChar(char c);

#define SOLID_KEYDOWN 0xFF
#define INTER_CHAR_SPACE 0xFE
#define INTER_WORD_SPACE 0xFD

/*
 *  Load a string to send by passing in a pointer via the first argument.
 *  Call this function with a NULL argument at intervals of 1 element of time to generate Morse code.
 *  Once loaded with a string each call to this function returns a bool indicating whether a CW carrier should be sent
 *  Pass in a pointer to a bool in the second and third arguments:
 */
bool makeMorse(char* s, bool* repeating, bool* finished)
{
	static char* str = NULL;
	static char c = ' ';
	static bool repeat = true;
	static MorseCharacter morseInProgress;
	static uint8_t charIndex;       /* letters, numbers, punctuation */
	static uint8_t symbolIndex;     /* dits and dahs */
	static uint8_t elementIndex;    /* units of time: dit = 1, dah = 3, intersymbol = 1, intercharacter = 3, etc. */
	static uint8_t addedSpace;      /* adds additional time to make an inter-character space */
	static bool completedString = true;
	static bool carrierOn = false;
	static bool holdKeyDown = false;

	if(s)   /* load a new NULL-terminated string to send */
	{
		holdKeyDown = false;

		if(repeating)
		{
			repeat = *repeating;
		}

		if(*s)
		{
			str = s;
			c = str[0];
			morseInProgress = getMorseChar(*str);
			charIndex = 0;
			symbolIndex = 0;
			elementIndex = 0;
			addedSpace = 0;
			completedString = false;
		}
		else    /* a zero-length string shuts down makeMorse */
		{
			str = NULL;
			completedString = true;
			if(finished)
			{
				*finished = true;
			}
		}

		carrierOn = OFF;
		return( OFF);
	}
	else if(str)
	{
		if(repeating)
		{
			*repeating = repeat;
		}

		if(completedString)
		{
			if(finished)
			{
				*finished = true;
				holdKeyDown = false;
			}
			return( OFF);
		}

		if(elementIndex)
		{
			elementIndex--;
		}
		else if(carrierOn && !holdKeyDown)  /* carrier is on, so turn it off and wait appropriate amount of space */
		{
			carrierOn = false;
			/* wait one element = inter-symbol space */
			if(addedSpace)
			{
				elementIndex = addedSpace;
				addedSpace = 0;
			}
			/* wait inter-character space */
		}
		else    /* carrier is off, so turn it on and get next symbol */
		{
			if(symbolIndex >= morseInProgress.lengthInSymbols)
			{
				c = (*(str + ++charIndex));

				if(!c)  /* wrap to beginning of text */
				{
					if(repeat)
					{
						c = *str;
						charIndex = 0;
					}
					else
					{
						str = NULL;
						carrierOn = OFF;
						completedString = true;
						if(finished)
						{
							*finished = true;
						}
						holdKeyDown = false;
						return( OFF );
					}
				}

				morseInProgress = getMorseChar(c);
				symbolIndex = 0;
			}

			if(morseInProgress.pattern < INTER_WORD_SPACE)
			{
				bool isDah = morseInProgress.pattern & (1 << symbolIndex++);

				if(isDah)
				{
					elementIndex = 2;
				}
				else
				{
					elementIndex = 0;
				}

				carrierOn = true;

				if(symbolIndex >= morseInProgress.lengthInSymbols)
				{
					addedSpace = 2;
				}
			}
			else
			{
				uint8_t sym = morseInProgress.lengthInSymbols;
				symbolIndex = 255;  /* ensure the next character gets read */
				carrierOn = false;
				if(sym >= 4 )
				{
					elementIndex = morseInProgress.lengthInSymbols - 4;
				}
				else
				{
					elementIndex = 0;
				}
			}
		}

		/* Overrides for key on and key off special characters */
		if(c == '<')    /* constant tone */
		{
			holdKeyDown = true;
		}
		else
		{
			holdKeyDown = false;
		}
	}

	if(finished)
	{
		*finished = completedString;
	}

	if(holdKeyDown)
	{
		return(true);
	}
	else
	{
		return( carrierOn);
	}
}

/**
 *  Returns the number of milliseconds required to send the string pointed to by the first argument at the WPM code speed
 *  passed in the second argument.
 */
uint16_t timeRequiredToSendStrAtWPM(char* str, uint16_t spd)
{
	uint8_t elements = 0;
	MorseCharacter m;
	char c;

	for(int i = 0; i < 20; i++)
	{
		c = str[i];
		if(!c)
		{
			break;
		}
		m = getMorseChar(c);
		if(m.pattern < INTER_WORD_SPACE)
		{
			elements += 3;
		}
		elements += m.lengthInElements;
	}

	return(elements * WPM_TO_MS_PER_DOT(spd));
}

/**
 *  Morse Code characters are defined as having three attributes:
 *  pattern = a sequence of up to 8 dit and dah symbols contained in an unsigned byte, sequentially read from LSB to MSB (first symbol is bit 0)
 *  lengthInSymbols = how many symbols (dits and dahs) the character contains; this is how many pattern bits are used to represent the character
 *  lengthInElements = how long (measured in "dit lengths") is the total character including all inter-symbol spaces.
 */
MorseCharacter getMorseChar(char c)
{
	MorseCharacter morse;

	switch( c )
	{
		case 'A':
		{
			morse.pattern = 0x02;   /* 0000 0010; */
			morse.lengthInSymbols = 2;
			morse.lengthInElements = 5;
		}
		break;

		case 'B':
		{
			morse.pattern = 0x01;   /* 0000 0001; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 9;
		}
		break;

		case 'C':
		{
			morse.pattern = 0x05;   /* 0000 0101; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 11;
		}
		break;

		case 'D':
		{
			morse.pattern = 0x01;   /* 0000 0001; */
			morse.lengthInSymbols = 3;
			morse.lengthInElements = 7;
		}
		break;

		case 'E':
		{
			morse.pattern = 0x00;   /* 0000 0000; */
			morse.lengthInSymbols = 1;
			morse.lengthInElements = 1;
		}
		break;

		case 'F':
		{
			morse.pattern = 0x04;   /* 0000 0100; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 9;
		}
		break;

		case 'G':
		{
			morse.pattern = 0x03;   /* 0000 0011; */
			morse.lengthInSymbols = 3;
			morse.lengthInElements = 9;
		}
		break;

		case 'H':
		{
			morse.pattern = 0x00;   /* 0000 0000; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 7;
		}
		break;

		case 'I':
		{
			morse.pattern = 0x00;   /* 0000 0000; */
			morse.lengthInSymbols = 2;
			morse.lengthInElements = 3;
		}
		break;

		case 'J':
		{
			morse.pattern = 0x0e;   /* 0000 1110; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 13;
		}
		break;

		case 'K':
		{
			morse.pattern = 0x05;   /* 0000 0101; */
			morse.lengthInSymbols = 3;
			morse.lengthInElements = 9;
		}
		break;

		case 'L':
		{
			morse.pattern = 0x02;   /* 0000 0010; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 9;
		}
		break;

		case 'M':
		{
			morse.pattern = 0x03;   /* 0000 0011; */
			morse.lengthInSymbols = 2;
			morse.lengthInElements = 7;
		}
		break;

		case 'N':
		{
			morse.pattern = 0x01;   /* 0000 0001; */
			morse.lengthInSymbols = 2;
			morse.lengthInElements = 5;
		}
		break;

		case 'O':
		{
			morse.pattern = 0x07;   /* 0000 0111; */
			morse.lengthInSymbols = 3;
			morse.lengthInElements = 11;
		}
		break;

		case 'P':
		{
			morse.pattern = 0x06;   /* 0000 0110; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 11;
		}
		break;

		case 'Q':
		{
			morse.pattern = 0x0b;   /* 0000 1011; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 13;
		}
		break;

		case 'R':
		{
			morse.pattern = 0x02;   /* 0000 0010; */
			morse.lengthInSymbols = 3;
			morse.lengthInElements = 7;
		}
		break;

		case 'S':
		{
			morse.pattern = 0x00;   /* 0000 0000; */
			morse.lengthInSymbols = 3;
			morse.lengthInElements = 5;
		}
		break;

		case 'T':
		{
			morse.pattern = 0x01;   /* 0000 0001; */
			morse.lengthInSymbols = 1;
			morse.lengthInElements = 3;
		}
		break;

		case 'U':
		{
			morse.pattern = 0x04;   /* 0000 0100; */
			morse.lengthInSymbols = 3;
			morse.lengthInElements = 7;
		}
		break;

		case 'V':
		{
			morse.pattern = 0x08;   /* 0000 1000; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 9;
		}
		break;

		case 'W':
		{
			morse.pattern = 0x06;   /* 0000 0110; */
			morse.lengthInSymbols = 3;
			morse.lengthInElements = 9;
		}
		break;

		case 'X':
		{
			morse.pattern = 0x09;   /* 0000 1001; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 11;
		}
		break;

		case 'Y':
		{
			morse.pattern = 0x0d;   /* 0000 1101; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 13;
		}
		break;

		case 'Z':
		{
			morse.pattern = 0x03;   /* 0000 0011; */
			morse.lengthInSymbols = 4;
			morse.lengthInElements = 11;
		}
		break;

		case '0':
		{
			morse.pattern = 0x1f;   /* 0001 1111; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 19;
		}
		break;

		case '1':
		{
			morse.pattern = 0x1e;   /* 0001 1110; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 17;
		}
		break;

		case '2':
		{
			morse.pattern = 0x1c;   /* 0001 1100; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 15;
		}
		break;

		case '3':
		{
			morse.pattern = 0x18;   /* 0001 1000; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 13;
		}
		break;

		case '4':
		{
			morse.pattern = 0x10;   /* 0001 0000; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 11;
		}
		break;

		case '5':
		{
			morse.pattern = 0x00;   /* 0000 0000; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 9;
		}
		break;

		case '6':
		{
			morse.pattern = 0x01;   /* 0000 0001; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 11;
		}
		break;

		case '7':
		{
			morse.pattern = 0x03;   /* 0000 0011; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 13;
		}
		break;

		case '8':
		{
			morse.pattern = 0x07;   /* 0000 0111; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 15;
		}
		break;

		case '9':
		{
			morse.pattern = 0x0f;   /* 0000 1111; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 17;
		}
		break;

		case '.':
		{
			morse.pattern = 0x2a;   /* 0010 1010; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 17;
		}
		break;

		case ',':
		{
			morse.pattern = 0x33;   /* 0011 0011; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 19;
		}
		break;

		case '?':
		{
			morse.pattern = 0x0c;   /* 0000 1100; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 15;
		}
		break;

		case '\'':
		{
			morse.pattern = 0x1e;   /* 0001 1110; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 19;
		}
		break;

		case '!':
		{
			morse.pattern = 0x35;   /* 0011 0101; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 19;
		}
		break;

		case '/':
		{
			morse.pattern = 0x09;   /* 0000 1001; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 13;
		}
		break;

		case '(':
		{
			morse.pattern = 0x0d;   /* 0000 1101; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 15;
		}
		break;

		case ')':
		{
			morse.pattern = 0x2d;   /* 0010 1101; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 19;
		}
		break;

		case '&':
		{
			morse.pattern = 0x02;   /* 0000 0010; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 11;
		}
		break;

		case ':':
		{
			morse.pattern = 0x07;   /* 0000 0111; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 17;
		}
		break;

		case ';':
		{
			morse.pattern = 0x15;   /* 0001 0101; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 12;
		}
		break;

		case '=':
		{
			morse.pattern = 0x11;   /* 0001 0001; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 13;
		}
		break;

		case '+': /* AR */
		{
			morse.pattern = 0x0a;   /* 0000 1010; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 13;
		}
		break;

		case '~': /* SK */
		{
			morse.pattern = 0x28;   /* 0010 1000; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 15;
		}
		break;

		case '-':
		{
			morse.pattern = 0x21;   /* 0010 0001; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 15;
		}
		break;

		case '_':
		{
			morse.pattern = 0x2c;   /* 0010 1100; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 17;
		}
		break;

		case '"':
		{
			morse.pattern = 0x12;   /* 0001 0010; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 15;
		}
		break;

		case '$':
		{
			morse.pattern = 0x48;   /* 0100 1000; */
			morse.lengthInSymbols = 7;
			morse.lengthInElements = 17;
		}
		break;

		case '@':
		{
			morse.pattern = 0x16;   /* 0001 0110; */
			morse.lengthInSymbols = 6;
			morse.lengthInElements = 17;
		}
		break;

		case '|':                       /* inter-word space */
		{ morse.pattern = 0xff;         /* 1000 0000; */
		  morse.lengthInSymbols = 7;
		  morse.lengthInElements = 4;   /* adjusted by -3 to account for inter-character space */
		}
		break;

		case ' ':                       /* inter-word space */
		{ morse.pattern = 0xfe;         /* 1000 0000; */
		  morse.lengthInSymbols = 7;
		  morse.lengthInElements = 7;   /* 4 + 3 (character space) = 7 */
		}
		break;

		case '<':                       /* long keydown */
		{
			morse.pattern = 0x1f;       /* 0001 1111; */
			morse.lengthInSymbols = 5;
			morse.lengthInElements = 19;
		}
		break;

		default:
		{
			morse.pattern = 0x0000; /* 0000 0000; */
			morse.lengthInSymbols = 0;
			morse.lengthInElements = 0;
		}
		break;
	}

	return( morse);
}

