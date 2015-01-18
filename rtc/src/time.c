/*
 * ----------------------------------------------------------------------
 * Title		: Time functions
 * ----------------------------------------------------------------------
 * Processor	: ATmega8
 * Clock		: 3.6864 MHz
 * Date			: 12.10.2011
 * Version		: 1.0
 * Author		: Enkhbold Ochirsuren
 * ----------------------------------------------------------------------
 */

#include "time.h"

uint8 msecond = 0;							// 1/10 second
uint8 second = 0;							// second (0..59)
uint8 minute = 0;							// minute (0..59)
uint8 hour = 0;								// hour (0..23)
uint8 dimmer = DIMMER_OFF;					// disable dimmer

/* update dimmer */
void updateDimmer(void)
{
	if ((TIME_DIMMER_START <= hour) || (hour <= TIME_DIMMER_END))
	{
		dimmer = DIMMER_ON;
	}
	else
	{
		dimmer = DIMMER_OFF;
	}
}
/* count datetime */
void countTime(void)
{
	msecond++;
	if (msecond == TIME_SECOND)
	{
		msecond = 0;
		second++;
		if (second == TIME_MINUTE)
		{
			low_event |= EVNT_TIME_UPDATE;
			second = 0;
			minute++;
			if (minute == TIME_HOUR)
			{
				low_event |= EVNT_TIME_UPDATE;
				minute = 0;
				hour++;
				if (hour == TIME_DAY)
				{
					hour = 0;
				}
				updateDimmer();			// apply dimming
			}
		}
		low_event |= EVNT_BLINK_ON;
	}
	else if (msecond == TIME_HALF_SECOND)
	{
		low_event |= EVNT_BLINK_OFF;
	}
}

/* adjust minute manually */
void adjustMinute(void)
{
	second = 0;
	minute++;
	if (minute == TIME_HOUR)
	{
		minute = 0;
	}
	low_event |= EVNT_TIME_UPDATE;
}

/* adjust hour manually */
void adjustHour(void)
{
	hour++;
	if (hour == TIME_DAY)
	{
		hour = 0;
	}
	updateDimmer();					// apply dimming
	low_event |= EVNT_TIME_UPDATE;
}
