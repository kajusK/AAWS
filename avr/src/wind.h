/*
 * Wind measurements, using timer1
 *
 * Speed - up to 25,5 (+- 0,1) m/s (equals to 90 km/h, I doubt the anemometer would
 *	survive that)
 * Direction - +-12°
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
