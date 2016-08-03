/*
 * Rain gauge sensor - based on tipping bucket, INT0 used to detect pulses
 *
 * https://en.wikipedia.org/wiki/Rain_gauge#Tipping_bucket_rain_gauge
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#ifndef __RAIN_H_
#define __RAIN_H_

/*
 * Enable INT0 and do some housekeeping
 */
void rain_init(void);

/*
 * Return current value of rain counter in tenths of mm
 */
uint16_t rain_get(void);

/*
 * Reset rain counter
 */
void rain_reset(void);

#endif
