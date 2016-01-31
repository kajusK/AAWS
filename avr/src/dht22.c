/*
 * DHT22 humidity and temperature sensor
 *
 * 0-100 (+-2) %RH
 * -40 to 80 (+-0.5) °C
 *
 * http://akizukidenshi.com/download/ds/aosong/AM2302.pdf
 *
 * Jakub Kaderka 2016
 *
 * TODO: rewrite dht22_wait_low/high to support timeout, sensor malofunction
 *	during receiving could block program execution
 */

#include <avr/io.h>
#include <util/atomic.h>
#include <inttypes.h>
#include <util/delay.h>

#include "config.h"
#include "dht22.h"

#define dht22_low() { DDR(DHT22_PORT) |= _BV(DHT22); PORT(DHT22_PORT) &= ~_BV(DHT22); }
#define dht22_high() { DDR(DHT22_PORT) &= ~_BV(DHT22); }
#define dht22_state bit_is_set(PIN(DHT22_PORT), DHT22)
#define dht22_wait_low() loop_until_bit_is_clear(PIN(DHT22_PORT), DHT22)
#define dht22_wait_high() loop_until_bit_is_set(PIN(DHT22_PORT), DHT22)

static uint8_t dht22_read_byte(void)
{
	uint8_t byte = 0;

	for (int i=7; i >= 0; i--) {
		dht22_wait_low();
		dht22_wait_high();
		_delay_us(40);

		if (dht22_state)
			byte |= (1 << i);
	}
	return byte;
}

/*
 * Read temperature (in tenths of °C) and humidity (tenths of %RH)
 *
 * Reading periods should be over two seconds
 */
uint8_t dht22_read(int16_t *temp, uint16_t *humidity)
{
	uint8_t data[5];

	if (dht22_state == 0)
		return BUS_BLOCKED;

	// send start - 1ms low
	dht22_low();
	_delay_ms(1);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		dht22_high();

		_delay_us(50);
		// if the sensor works, it should pull the line low for 80us
		if (dht22_state)
			return NOT_RESPONDING;

		dht22_wait_high();
		for (int i=0; i < 5; i++)
			data[i] = dht22_read_byte();
	}
	if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xff))
		return CHECKSUM_ERR;

	*humidity = (data[0] << 8) + data[1];
	*temp = ((data[2] & 0x7f) << 8) + data[3];
	if (data[2] & 0x80)
		*temp *= -1;

	return 0;
}
