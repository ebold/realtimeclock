/*
 * ----------------------------------------------------------------------
 *  Title		:	Display example for myAVR-Board
 * ----------------------------------------------------------------------
 *  Function	:	Display text on LED dot matrix
 *  Circuit		:	Switch 1 to PortD.2, switch 2 to PortD.3
 *  			:	red LED to PortD.4
 * 				: 	SCK on PortB5, MOSI on PortB3
 * 				:	OE on PortB1, and LDO on PortB0
 * ----------------------------------------------------------------------
 *  Processor	: ATmega8
 *  Clock		: 3.6864 MHz
 *  Date		: 10.05.2011
 *  Version		: 1.3
 *  Author		: Enkhbold Ochirsuren
 *  Build command: CTRL+B
 *  Install command: CTRL+ALT+U
 * ----------------------------------------------------------------------
 */
//#define 	F_CPU 3686400	// The main clock of myAVR-Board
#include	"board.h"			// I/O function definitions
#include 	"display.h"			// display functions
#include	"time.h"			// datetime definitions
#include	"switch.h"			// switch functions

#define 	ZIFFER 	0x30

/* display buffer for text */
uint8 display_buffer[2][MAX_CHAR];

/* Text to be displayed on the LED matrix */
uint8 text[MAX_CHAR] = "Hello";
uint8 *ptext;

/* Initialize HW */
void initHW(void)
{
	initIO();
}

/* Function to display text */
int8 displayText(uint8 mode, uint8 *text, uint8 offset, uint8 length)
{
	uint8 i, len;
	if (offset + length > MAX_CHAR)
		return -1;
	if (mode > DISP_MODE_BLINK)
		return -2;

	len = 0;
	for (i = offset; len < length; i++, len++)
		display_buffer[mode][i] = *(text + i);

	updateLED(mode);
	return len;
}

int main()
{
	uint8 i;

	cli();			// disable interrupts globally
	initHW();		// initialize hardware
	initDisplay();	// initialize display variables

	displayText(DISP_MODE_STATIC, text, 0, 5);	// display hello message

	sei();			// enable interrupts globally
	startTimer();	// start the main system timer

	sendUSART('S');	// debug msg

	ptext = &text[0];

    do 	// main loop
    {
    	/* test routine to show that software is running properly */
    	turnOnLED();

    	/* check if display refresh is requested */
    	if (low_event & EVNT_DISPLAY)
    	{
    		refreshDisplay();
    		low_event &= ~EVNT_DISPLAY;
    	}

    	/* check if 1/10 second is over */
    	else if (low_event & EVNT_TIME_COUNT)
    	{
    		countTime();
    		low_event &= ~EVNT_TIME_COUNT;
    	}

    	/* blink on double points */
    	else if (low_event & EVNT_BLINK_ON)
    	{
    		*(ptext + 2) = ':';
    		low_event &= ~EVNT_BLINK_ON;
    		displayText(DISP_MODE_STATIC, ptext, 2, 1);	// display ':'
    	}

    	/* blink off double points */
    	else if (low_event & EVNT_BLINK_OFF)
    	{
    		*(ptext + 2) = '.';
    		low_event &= ~EVNT_BLINK_OFF;
    		displayText(DISP_MODE_STATIC, ptext, 2, 1);
    	}

    	/* adjust datetime */
    	else if (low_event & EVNT_TIME_ADJUST)
    	{
    		readSwitch();
    		low_event &= ~EVNT_TIME_ADJUST;
    	}

    	/* update to display datetime */
    	else if (low_event & EVNT_TIME_UPDATE)
    	{
    		i = hour / 10;
    		if (i)
    		{
    			*ptext = i | ZIFFER;
    		}
    		else
    		{
    			*ptext = ' ';
    		}
    		*(ptext + 1) = (hour % 10) | ZIFFER;
    		//*(ptext + 2) = ':';
    		i = minute / 10;
    		if (i)
    		{
    			*(ptext + 3) = i | ZIFFER;
    			*(ptext + 4) = (minute % 10) | ZIFFER;
    		}
    		else
    		{
    			*(ptext + 3) = (minute % 10) | ZIFFER;
    			*(ptext + 4) = ' ';
    		}
    		displayText(DISP_MODE_STATIC, ptext, 0, 5);	// display text[]
    		low_event &= ~EVNT_TIME_UPDATE;
    	}

    	/* check if new text is received over serial interface */
    	else if (low_event & EVNT_USART)
    	{
    		for (i = 0; i < MAX_CHAR; i++)
    			*(ptext + i) = uart_buffer[i];
    		displayText(DISP_MODE_STATIC, ptext, 0, 5);	// display received text

    		/* reply an acknowledge */
    		sendUSART('A');

    		low_event &= ~EVNT_USART;
    	}
    } while (1);

    return 0;
}

///*
// * main.c
// *
// *  Created on: 24.11.2008
// *      Author: Enkhbold Ochirsuren
// *      Build command: CTRL+B
// *      Install command: CTRL+ALT+U
// *
// */
//
//#include <avr/io.h>
///**
// * Internal clock of 3,6864MHz
// */
//int main(void)
//{
//    unsigned char i;
//    DDRC |= _BV(PC5);
//    TCCR0 = _BV(CS00) | _BV(CS02); // clk/1024 Mode
//    while(1) {
//        PORTC ^= _BV(PC5); // toggle PC5
//        for (i=0; i<4; i++) {
//        	while(bit_is_clear(TIFR, TOV0));
//        	TIFR = _BV(TOV0);
//        }
//    }
//    return 0;
//}
