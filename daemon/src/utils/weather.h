/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __WEATHER_H_
#define __WEATHER_H_

/*
 * Calculate relative pressure
 * absolute + elevation/8.3
 */
extern float press_relative(float absolute, int elevation);

/*
 * Calculate dew point
 * https://en.wikipedia.org/wiki/Dew_point#Calculating_the_dew_point
 */
extern float dew_point(float humidity, float temp);

extern float CtoF(float value);
extern float MStoMPH(float value);
extern float MMtoINCH(float value);
extern float HPAtoINCH(float value);

#endif
