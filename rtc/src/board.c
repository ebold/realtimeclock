/*
 * ----------------------------------------------------------------------
 * Title		: Hardware dependent functions
 * ----------------------------------------------------------------------
 * Processor	: ATmega8
 * Clock		: 3.6864 MHz
 * Date			: 10.05.2011
 * Version		: 1.0
 * Author		: Enkhbold Ochirsuren
 * ----------------------------------------------------------------------
 */

#include "board.h"
#include "time.h"
#include "switch.h"
#include "display.h"

extern const uint8 column[MAX_LED_COL];
/* macros for enabling/disabling SPI interrupt */
#define EN_SPI_INT		SPCR |= (1 << SPIE)
#define DIS_SPI_INT		SPCR &= ~(1 << SPIE)

/* ASCII constants */
#define ASCII_ESC	0x1B

/* counter used to generate event every 1/900Hz */
volatile uint8 tim_1ms = 0;

/* clock used to generate 1/10 second */
volatile uint8 tim_90ms = CNT_90MS;

/* counter for transmitted bytes over SPI */
volatile uint8 spiTxCnt;

/* variables and buffers used in USART */
volatile uint8 uart_ch_cnt;
volatile char uart_buffer[MAX_USART_LEN + 1] = "";

/* SPI interface initialization */
void initSPIPort(void)
{
	/* turn off LEDs */
	LED_OFF;

	/* Set MOSI, SCK, SS, OE, LD output, all others input */
	DDR_SPI = DD_MOSI | DD_SCK | DD_SS | DD_S_OE | DD_S_LD;

	/* turn off LEDs */
	LED_OFF;

	/* Enable SPI, Master, set clock rate fclk/2 (1.84MHz)*/
	/* MSB first, SPI mode 0 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPI2X);
}

/* Function to send a single byte over SPI */
void sendSPIData(uint8 data)
{
	/* Start transmission */
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1 << SPIF)));
}

/* Port initialization for LED and switch */
/* Switch 1 and 2 are connected to PortD.2 and PortD.3, red LED to PortD.3 */
void initLEDPort(void)
{
	/* Set LED output, all others input */
    DDR_LED = DD_LED_RD;
    /* Enable pull-up at switch input */
    PORT_SWITCH = (DD_SW_1 | DD_SW_2);
}

/* USART setup */
void initUSART(uint16 baudrate)
{
	/* enable receiver and transmitter, enable receive interrupt */
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);

	/* set frame format: 8 data bits, 1 stop bits, no parity */
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

	/* set baud rate */
	UBRRH = (uint8)(baudrate >> 8);
	UBRRL = (uint8)baudrate;

	/* reset variables */
	uart_ch_cnt = 0;
}

/* send a byte over USART */
void sendUSART(uint8 data)
{
	/* wait for empty transmit buffer */
	while (!(UCSRA & (1 << UDRE)));

	/* put data into buffer, sends the data */
	UDR = data;
}

/* Turn on/off LED */
void turnOnLED(void)
{
  	// if any switch is pressed, then turn on LED
   	// otherwise turn it off
    if (!(PIN_SWITCH & DD_SW_1) || !(PIN_SWITCH & DD_SW_2))
    	PORT_LED |= DD_LED_RD;		// turn on
    else
	    PORT_LED &= ~DD_LED_RD;		// turn off
}

/* Enable timer0 operation */
void enableTimer0(void)
{
	// clock source for the counter is set to clk/8 (3.6864MHz/8=460.8KHz)
	TCCR0 = PSC_8;
	// value of TCNT0 is incremented at each timer clock (460.8KHz)
	// generating overflow frequency of 1800Hz=460.8KHz/256

	tim_1ms = 0;
}

/* Disable timer0 operation */
void disableTimer0(void)
{
	TCCR0 = 0x00;
}

/* Enable timer0 overflow interrupt */
void enableTimer0IRQ(void)
{
	TIMSK |= (1 << TOIE0);
}

/* Disable timer0 overflow interrupt */
void disableTimer0IRQ(void)
{
	TIMSK &= ~(1 << TOIE0);
}

/* ISR for timer0 overflow interrupt */
ISR(TIMER0_OVF_vect)
{
	// timer0 overflow interrupt is fired every 1/1800Hz
	// toggle the value to generate 900Hz (1.11msec)
    tim_1ms ^= 1;
    if (tim_1ms)
    {
    	//low_event |= EVNT_DISPLAY;
    	/* debug
    	PORT_SPI |= (1 << DDB2);
    	*/

    	// clock for counting 1/10 second
    	tim_90ms--;
    	if (!(tim_90ms))
    	{
    		tim_90ms = CNT_90MS;
    		low_event |= EVNT_TIME_COUNT;
    	}
    	else if ((tim_90ms & 0x07) == 0x07)
    	{
    		low_event |= EVNT_TIME_ADJUST;
    	}
    }
    /* debug
    else
    {
    	PORT_SPI &= ~(1 << DDB2);
    }
    */
    if (refresh_done)	// if previous refresh is completed, then initiate next refresh
    {
    	refresh_done = 0;	// reset flag

    	spiTxCnt = 0;		// reset counter for SPI transmitted byte

    	pSource = pByte + led_column;	// byte-pattern buffer + offset

   		LED_OFF;	// turn off LEDs

   		EN_SPI_INT;	// enable SPI interrupt

		sendSPIData(~(*pSource));	// send INVERTED byte-pattern over SPI

		pSource += MAX_LED_COL;		// point to next byte-pattern
   	}
}

/* Enable timer1 operation */
void enableTimer1(void)
{
	// clock source for the counter is set to clk/8 (3.6864MHz/8=460.8KHz)
	// CTC mode
	TCCR1B = PSC_8 | CTC_1B;
	// MAX is given in OCR1A
	TCCR1A = CTC_1A;

	// value of TCNT1 is incremented at each timer clock (460.8KHz)
	// generating overflow frequency of 460.8KHz/368=1.25KHz (800us)
	OCR1AH = 0x01;	// 368 = 0x0170
	OCR1AL = 0x70;
}

/* Enable timer1 overflow interrupt */
void enableTimer1IRQ(void)
{
	TIMSK |= (1 << OCIE1A);
}

/* Disable timer1 overflow interrupt */
void disableTimer1IRQ(void)
{
	TIMSK &= ~(1 << OCIE1A);
}

/* Restart timer1 counting */
void restartTimer1(void)
{
	TCNT1L = 0;
	TCNT1H = 0;
}
/* ISR for timer1 compare match A interrupt */
ISR(TIMER1_COMPA_vect)
{
	LED_ON;					// turn on LEDs
	refresh_done = 1;		// set flag
	disableTimer1IRQ();		// disable timer1 interrupt
}
/* ISR for SPI transmission */
ISR(SPI_STC_vect)
{
	spiTxCnt++;	// increment transmitted byte counter

	if (spiTxCnt < MAX_LED_BLK)	// send byte-patterns
	{
		sendSPIData(~(*pSource));	// send INVERTED byte-pattern
		pSource += MAX_LED_COL;		// point to next byte-pattern
	}
	else if (spiTxCnt == MAX_LED_BLK)	// send column byte
	{
		sendSPIData(column[led_column]);	// activate this column

		++led_column;					// select next column
		if (led_column == MAX_LED_COL)	// if all columns of the text display have been accessed
		{
			led_column = 0;			// then start again from the first column
		}
	}
	else	// stop SPI transmission
	{
		PRE_LOAD;	// prepare to load bytes transmitted over SPI
		LOAD_BYTE;	// load transmitted bytes

		DIS_SPI_INT;	// disable SPI interrupt

#ifdef ENABLE_DIMMER
		if (dimmer)	// if dimmer is activated
		{
			restartTimer1();		// then start timer1
			enableTimer1IRQ();		// and enable timer1 interrupt
			/* LEDs are turned on by timer1 ISR after a fixed time delay */
		}
		else		// otherwise LEDs are turned on immediately
#endif
		{
			LED_ON;				// turn on LEDs
			refresh_done = 1;	// set flag
		}
	}
}

/* ISR for USART receive complete interrupt */
ISR(USART_RXC_vect)
{
	uint8 next_char;

	next_char = UDR;	// read byte from UDR register
	if ((low_event & EVNT_USART) == 0)	// if reception is in progress
	{
		if (next_char == ASCII_ESC)	// if it is a frame start symbol (Esc)
		{
			uart_ch_cnt = 0;
		}
		else if (next_char != '\n' &&
			next_char != '\r')
		{
			uart_buffer[uart_ch_cnt] = next_char;
			uart_ch_cnt++;
			if (uart_ch_cnt == MAX_USART_LEN)	// if max number of bytes received
			{
				uart_buffer[uart_ch_cnt] = '\0';
				uart_ch_cnt = 0;
				low_event |= EVNT_USART;
			}
		}
	}
}

/* Initialize I/O ports */
void initIO(void)
{
	initSPIPort();	// set up the SPI interface
	initLEDPort();	// initialize the I/O port to which LED and switch are connected
	initUSART(USART_9600);	// initialize the USART with baudrate of 9600bps
}

/* Start the main timer (900Hz) */
void startTimer(void)
{
	/* enable timer0 overflow interrupt and timer0 operation*/
	enableTimer0IRQ();
	enableTimer0();
	enableTimer1();
}


