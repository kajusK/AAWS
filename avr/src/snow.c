/*
 * Snow ultrasonic measurement - using timer2
 *
 * Software implemented capture mode - only one present in HW already used
 * by wind sensor
 *
 * Before first measurement, the snow_calibrate must be called to set
 * no snow distance. This value is stored in eeprom and used in calculations:
 * snow_depth = calibration_distance - measured_distance
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "config.h"

#if SNOW_INT == 1
	#define INT_SOURCE	INT1
	#define ISCX0		ISC10
	#define ISCX1		ISC11
#else
	#define INT_SOURCE	INT0
	#define ISCX0		ISC00
	#define ISCX1		ISC01
#endif

static uint16_t EEMEM default_distance;
static volatile uint16_t pulse_len = 0;

ISR(TIMER2_OVF_vect)
{
	pulse_len += 256;
	if (pulse_len < 65000)
		return;

	pulse_len = 0xffff;
	//disable interruts
	TIMSK &= ~_BV(TOIE2);
	GIMSK &= ~_BV(INT_SOURCE);
}

#if SNOW_INT == 1
ISR(INT1_vect)
#else
ISR(INT0_vect)
#endif
{
	//if rising edge occured
	if (MCUCR & (_BV(ISCX0) | _BV(ISCX1))) {
		TCNT2 = 2; //start timer, /8 prescaler
		MCUCR = (MCUCR & (~(_BV(ISCX0) | _BV(ISCX1)))) | _BV(ISCX1);
		return;
	}

	//stop timer
	TCCR2 = 0x00;
	if (TIFR & _BV(TOV2))
		pulse_len += 256;
	pulse_len += TCNT2;

	//disable interruts
	TIMSK &= ~_BV(TOIE2);
	GIMSK &= ~_BV(INT_SOURCE);
	//clear edge flag
	MCUCR &= ~(_BV(ISCX0) | _BV(ISCX1));

}

/* returns distance in tenths of cm */
static uint16_t ultrasonic_distance(void)
{
	pulse_len = 0;
	TCNT2 = 0;
	GIMSK |= _BV(INT_SOURCE);
	MCUCR |= _BV(ISCX0) | _BV(ISCX1); //interrupt on rising edge

	//start measurement
	PORT(ULTRAS_TRIG_PORT) |= _BV(ULTRAS_TRIG);
	_delay_us(10);
	PORT(ULTRAS_TRIG_PORT) &= ~_BV(ULTRAS_TRIG);

	//enable interrupts
	TIMSK |= _BV(TOIE2);
	GIMSK |= _BV(INT_SOURCE);

	while (GIMSK & _BV(INT_SOURCE))
		;

	if (pulse_len == 0xffff)
		return 0xffff;

	//t = 2d/v = 58us/cm
	//t = pulse_len*prescaler/(F_CPU/1000000) us
	//d = t * 5.8 = t/10 * 58
	return pulse_len*8/(F_CPU/1000000)/10*58;
}

/* Set current distance as 0 */
void snow_calibrate(void)
{
	eeprom_write_word(&default_distance, ultrasonic_distance());
}

/* Returns amount of snow in tenths of cm */
uint16_t snow_get(void)
{
	uint16_t res = ultrasonic_distance();
	if (res == 0xffff)
		return res;

	res = eeprom_read_word(&default_distance) - ultrasonic_distance();
	if (res < SNOW_IGNORE)
		return 0;
	return res;
}
