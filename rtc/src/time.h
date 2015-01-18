/*
 * time.h
 *
 *  Created on: 12.10.2011
 *      Author: boldoo
 */

#ifndef TIME_H_
#define TIME_H_

#include "board.h"
#include "type.h"

#define ENABLE_DIMMER

/* count datetime */
void countTime(void);

/* adjust minute */
void adjustMinute(void);

/* adjust hour */
void adjustHour(void);

/* datetime definitions */
#define TIME_HALF_SECOND 	5	// 0,5 second
#define TIME_SECOND 		10	// 1 second
#define TIME_MINUTE 		60	// 1 minute
#define TIME_HOUR 			60	// 1 hour
#define TIME_DAY 			24	// 1 day

/* time period for dimmer activation */
#define TIME_DIMMER_START	21	// dimmer is enabled starting at 21:00 hour
#define TIME_DIMMER_END		6	// dimmer is disabled starting at 6:00 hour

/* dimmer state */
#define DIMMER_ON			0x80	// dimmer is enabled
#define DIMMER_OFF			0x00	// dimmer is disabled

extern uint8 minute, hour;	// minute and time variables
extern uint8 dimmer;		// dimmer flag

#endif /* TIME_H_ */
