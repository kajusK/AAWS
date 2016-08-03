/*
 * Rain gauge sensor - based on tipping bucket
 *
 * https://en.wikipedia.org/wiki/Rain_gauge#Tipping_bucket_rain_gauge
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "config.h"

static volatile uint16_t rain_count;

ISR(INT0_vect)
{
	++rain_count;

	//TODO handle overflow better
	if (rain_count > UINT16_MAX / RAIN_MM_PER_PULSE)
		rain_count = UINT16_MAX / RAIN_MM_PER_PULSE;
}

/*
 * Enable INT0 and do some housekeeping
 */
void rain_init(void)
{
	rain_count = 0;

	// interrupt on rising edge
	MCUCR |= ISC00 | ISC01;
	GIMSK |= _BV(INT0);
}

/*
 * Return current value of rain counter in tenths of mm
 */
uint16_t rain_get(void)
{
	return rain_count*RAIN_MM_PER_PULSE;
}

/*
 * Reset rain counter
 */
void rain_reset(void)
{
	rain_count = 0;
}
