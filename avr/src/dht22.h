/*
 * DHT22 humidity and temperature sensor
 * 0-100 (+-2) %RH
 * -40 to 80 (+-0.5) °C
 *
 * http://akizukidenshi.com/download/ds/aosong/AM2302.pdf
 *
 * Jakub Kaderka 2016
 */

#ifndef _DHT22_H_
#define _DHT22_H_

//#define DHT22_PORT	B
//#define DHT22		0

//error return values
enum {
	NOT_RESPONDING=1,
	BUS_BLOCKED,
	CHECKSUM_ERR
};

/*
 * Read temperature (in tenths of °C) and hu[idity (tenths of %RH)
 *
 * Reading periods should be over two seconds
 */
uint8_t dht22_read(int16_t *temp, uint16_t *humidity);

#endif
