/*
 * ----------------------------------------------------------------------
 * Title		: Display functions
 * ----------------------------------------------------------------------
 * Processor	: ATmega8
 * Clock		: 3.6864 MHz
 * Date			: 12.05.2011
 * Version		: 1.0
 * Author		: Enkhbold Ochirsuren
 * ----------------------------------------------------------------------
 */

#include "display.h"

// column indices of an LED block
const uint8 column[MAX_LED_COL] =
{
	0x00, // column1
	0x01, // column2
	0x02, // column3
	0x03, // column4
	0x04, // column5
	0x05, // column6
	0x06, // column7
	0x07, // column8
	0x08, // column9
	0x09  // column10
};

extern const uint8 charset6x8[96][BYTE_PAT_COL];	// 8x6 byte-pattern for ASCII characters
extern uint8 display_buffer[MAX_DISP_MODE][MAX_CHAR];	// display buffer for text

/* Initialize variables for display function */
void initDisplay(void)
{
	/* initialize display variables */
	pByte = &byte_pattern[0][0];
	led_column = 0;
	refresh_done = 1;
}

/* Refresh display */
void refreshDisplay(void)
{
	uint8 i;

	if (refresh_done)	// if previous refresh is completed, then initiate next refresh
	{
		refresh_done = 0;

		pSource = pByte + led_column;	// byte-pattern buffer + offset

		LED_OFF;	// turn off LEDs

		/* performs SPI transmission */
		for (i = 0; i < MAX_LED_BLK; ++i)
		{
			sendSPIData(~(*pSource));	// send INVERTED byte-pattern over SPI
			pSource += MAX_LED_COL;		// select byte-pattern for next LED block
		}

		sendSPIData(column[led_column]);	// send column selection data (last byte)

		PRE_LOAD;	// prepare to load bytes transmitted over SPI
		LOAD_BYTE;	// load transmitted bytes
		LED_ON;		// turn on LEDs

		++led_column;
		if (led_column == MAX_LED_COL)	// if all columns of the text display have been accessed
			led_column = 0;			// then start again from the first column

		refresh_done = 1;
	}
}

/* Convert an ASCII character to an 8x6 byte-pattern */
int8 charToPattern(uint8 mode, uint8 ascii_char)
{
	uint8 i, j;

	if ((ascii_char < ' ') || (ascii_char > 0x7F))
    {
        return -1;
    }
    ascii_char -= ' ';

    j = next_char * BYTE_PAT_COL;
    for (i = 0; i < BYTE_PAT_COL; i++)
       	byte_pattern[mode][i+j] = ~charset6x8[ascii_char][i];

    return 0;
}

/* Copy text char by char to the byte-pattern buffer */
void writeChar(uint8 mode, uint8 *source)
{
	uint8 i;

	for (i = 0; i < MAX_CHAR; i++)
	{
		charToPattern(mode, *(source + i));
		next_char++;
	}

	pByte = &byte_pattern[mode][0];
}

/* Start to copy text to the byte-pattern buffer */
void updateLED(uint8 mode)
{
	next_char = 0;
    pText = &display_buffer[mode][0];
    writeChar(mode, pText);
}
