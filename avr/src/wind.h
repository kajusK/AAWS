/*
 * Wind measurements, using timer1
 *
 * Speed - up to anemometer possibilities, I doubt it could survive more than
 *		about 110km/h anyway - equals to violent storm)
 * Direction - +-12°
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#ifndef __WIND_H_
#define __WIND_H_

void wind_init(void);

/*
 * returns direction in degrees from 0 to 348°
 */
uint16_t wind_dir(void);

/*
 * Returns latest wind speed in tenths of m/s
 */
uint16_t wind_speed(void);

#endif
