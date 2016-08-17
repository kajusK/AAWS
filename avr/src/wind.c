/*
 * Wind measurements, using timer1
 *
 * Speed - up to anemometer possibilities, I doubt it could survive more than
 *		about 110km/h anyway - equals to violent storm)
 * Direction - +-6° (single track gray code - 5 sensors)
 *
 * https://en.wikipedia.org/wiki/Rotary_encoder
 * https://en.wikipedia.org/wiki/Gray_code#Single-track_Gray_code
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "config.h"

#define TIMER1_PRESALER_1 1

//compatibility issues...
#ifndef TICIE1
	#define TICIE1 ICIE1
#endif

static const uint16_t wind_table[] PROGMEM = {288, 216, 276, 144, 156, 204, 168, 72, 300, 84, 264, 132, 120, 96, 108, 0, 348, 228, 240, 12, 336, 192, 180, 60, 312, 48, 252, 24, 324, 36};

//time units between two signals from anemometer
//0 means no wind
static volatile uint16_t wind_pulse_length;

ISR(TIMER1_CAPT_vect)
{
	//set timer to measure from beggining of the pulse
	TCNT1 -= ICR1;

	//invert edge polarity
	TCCR1B ^= _BV(ICES1);

	// waiting for rising edge, do nothing
	if (TCCR1B & _BV(ICES1))
		return;

	wind_pulse_length = ICR1;
}

ISR(TIMER1_OVF_vect)
{
	wind_pulse_length = 0;
	//waiting for failing edge
	if (!(TCCR1B & _BV(ICES1)))
		return;

	//wait for next failing edge and ignore current pulse
	TCCR1B ^= _BV(ICES1);
}

void wind_init(void)
{
	//configure port for wind direction
	DDR(WIND_DIR_PORT) &= ~(0x1f << WIND_DIR_START);

	//enable noise canceler, wait for failing edge, prescaler 64
	TCCR1A = 0;
	TCCR1B = _BV(ICNC1) | _BV(CS11) | _BV(CS10);

	//enable interrupt on capture and overflow
	TIMSK |= _BV(TOIE1) | _BV(TICIE1);
}

/*
 * Designed for single track gray code like:
 * https://upload.wikimedia.org/wikipedia/commons/thumb/5/5e/Enkelspoors-Graycode.svg/220px-Enkelspoors-Graycode.svg.png
 *
 * returns direction in degrees from 0 to 348°
 */
uint16_t wind_dir(void)
{
	uint8_t state;

	state = PIN(WIND_DIR_PORT) >> WIND_DIR_START;
	state &= 0x1f;
	--state;		//00001 not defined
	if (state > 29)
		return 666;	//not defined

	return pgm_read_dword(&wind_table[state]);
}

/*
 * Returns latest wind speed in tenths of m/s
 */
uint16_t wind_speed(void)
{
	uint16_t local_length;

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		local_length = wind_pulse_length;
	}

	if (local_length == 0)
		return 0;

	//TODO do some real life measurements
	return F_CPU/local_length;//*WIND_CORRECTION;
}
