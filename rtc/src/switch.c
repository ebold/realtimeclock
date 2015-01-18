/*
 * ----------------------------------------------------------------------
 * Title		: Switch interface
 * ----------------------------------------------------------------------
 * Processor	: ATmega8
 * Clock		: 3.6864 MHz
 * Date			: 12.10.2011
 * Version		: 1.0
 * Author		: Enkhbold Ochirsuren
 * ----------------------------------------------------------------------
 */

#include "switch.h"
#include "time.h"

uint8	sampleB = 0, sampleC = 0, last_switches = 0, last_debounced_switches = 0xFF;

/* read switch state */
void readSwitch(void)
{
	uint8 sample, changes;

	sample = PIN_SWITCH;	// read switches

	last_switches = ((last_switches & (sample | sampleB | sampleC)) |
				(sample & sampleB & sampleC));
	sampleC = sampleB;
	sampleB = sample;

	changes = (last_switches ^ last_debounced_switches);

	if ((!(last_switches & DD_SW_1)) && (changes & DD_SW_1))
	{
		adjustHour();
	}

	if ((!(last_switches & DD_SW_2)) && (changes & DD_SW_2))
	{
		adjustMinute();
	}

	last_debounced_switches = last_switches;

}
