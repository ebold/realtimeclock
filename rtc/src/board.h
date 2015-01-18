#ifndef BOARD_H_
#define BOARD_H_

#include <avr\io.h>			// AVR register and constant definitions
#include <avr\interrupt.h> 	// AVR ISR functions
#include "type.h"

/* macros for LED matrix control signals */
#define LED_OFF		PORTB &= ~(1 << DDB1)	// DDB1
#define LED_ON		PORTB |= (1 << DDB1)	// DDB1
#define PRE_LOAD	PORTB |= (1 << DDB0)	// DDB0
#define LOAD_BYTE	PORTB &= ~(1 << DDB0)	// DDB0

/* SPI interface constants */
#define DDR_SPI		DDRB
#define DD_MOSI		0x08	// DDB3
#define DD_SCK		0x20	// DDB5
#define DD_SS		0x04	// DDB2
#define DD_S_OE		0x02	// DDB1
#define DD_S_LD		0x01	// DDB0
#define PORT_SPI	PORTB

/* Switch and LED interface constants */
#define DD_LED_RD	0x10	// red LED at DDD4
#define DD_SW_2		0x08	// switch 2 at DDD3
#define DD_SW_1		0x04	// switch 1 at DDD2
#define DDR_LED		DDRD
#define PORT_SWITCH	PORTD
#define PORT_LED	PORTD
#define PIN_SWITCH	PIND

/* Timer/counter constants */
#define PSC_8		0x02	// prescaler=clk/8

/* MAX for CTC OCR1A */
#define CTC_1A	0x00	// MAX is defined in OCR1A
#define CTC_1B	0x08	// MAX is defined in OCR1A

/* 1/10 second constant */
#define CNT_90MS	90

/* USART constants */
#define USART_9600		0x17
#define MAX_USART_LEN	5

/* buffer for serial characters */
extern volatile char uart_buffer[];

/* low-level event flag */
volatile uint8 low_event;
	#define EVNT_DISPLAY		0x01
	#define EVNT_TIME_COUNT		0x02
	#define EVNT_TIME_UPDATE	0x04
	#define EVNT_TIME_ADJUST	0x08
	#define EVNT_BLINK_ON		0x10
	#define EVNT_BLINK_OFF		0x20
	#define EVNT_USART			0x40

/* Initialize I/O ports */
void initIO(void);

/* Function to send a single byte over SPI */
void sendSPIData(uint8 data);

/* Turn on/off LED */
void turnOnLED(void);

/* Start the main timer (900Hz) */
void startTimer(void);

/* send a byte over USART */
void sendUSART(uint8 data);

#endif /*BOARD_H_*/
