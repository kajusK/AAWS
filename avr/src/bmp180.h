/*
 * BMP180 temperature and pressure sensor
 *
 * -40 to 85 (+-2) °C
 * 300 to 1100 (+-4,5) hPa
 *
 * Jakub Kaderka 2016
 */

#ifndef _BMP180_H_
#define _BMP180_H_

//precision setting, valid values are from 0 to 3
#define BMP180_OSS 3

/*
 * Read configuration data from device, must be called before reading pressure
 */
uint8_t bmp180_init(void);

/*
 * Read the temperature (in tenths of °C) and pressure (in Pascals)
 */
void bmp180_read(int16_t *temp, uint16_t *pressure);

#endif
