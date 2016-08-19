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
#include <util/atomic.h>

#include "config.h"

static volatile uint16_t rain_count;

#if RAIN_INT == 1
ISR(INT1_vect)
#else
ISR(INT0_vect)
#endif
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
#if RAIN_INT == 1
	MCUCR |= ISC10 | ISC11;
	GIMSK |= _BV(INT1);
#else
	MCUCR |= ISC00 | ISC01;
	GIMSK |= _BV(INT0);
#endif
}

/*
 * Return current value of rain counter in tenths of mm
 */
uint16_t rain_get(void)
{
	uint16_t rain_local;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		rain_local = rain_count;
	}
	return rain_local*RAIN_MM_PER_PULSE;
}

/*
 * Reset rain counter
 */
void rain_reset(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		rain_count = 0;
	}
}
