#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "board.h"
#include "type.h"

/* LED display definitions */
#define MAX_LED_BLK		3	// number of LED blocks on the LED display (1 block = 8x10 LEDs)
#define MAX_LED_COL		10	// columns of an LED block
#define MAX_CHAR		5	// number of characters on the LED display

/* Byte pattern definitions */
#define BYTE_PAT_COL	6	// number of columns per 8x6 byte-pattern

/* Display mode definitions */
/* display mode */
#define DISP_MODE_STATIC	0
#define DISP_MODE_BLINK		1
#define MAX_DISP_MODE		2

/* Initialize variables for display function */
void initDisplay(void);

/* Refresh display */
void refreshDisplay(void);

/* Start to copy text to the byte-pattern buffer */
void updateLED(uint8 mode);

uint8 led_column;							// column index of 8x10 LED block
uint8 refresh_done;							// flag that indicates display refresh completion
uint8 *pByte, *pSource, *pText;				// pointers used for display control
uint8 byte_pattern[MAX_DISP_MODE][MAX_CHAR*BYTE_PAT_COL];	// buffers for 8x6 byte-patterns of currently displayed text
uint8 next_char;							// offset within byte-pattern buffer

#endif /*DISPLAY_H_*/
