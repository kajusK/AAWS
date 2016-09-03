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

#ifndef __SNOW_H_
#define __SNOW_H_

/* Set current distance as 0 */
extern void snow_calibrate(void);

/* Returns amount of snow in tenths of cm, don't call more often than 50ms */
extern uint16_t snow_get(void);

#endif
