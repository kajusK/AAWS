/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <math.h>

#include "weather.h"

/*
 * Calculate relative pressure
 * absolute + elevation/8.3
 */
float press_relative(float absolute, int elevation)
{
	return absolute + elevation/8.3;
}

/*
 * Calculate dew point
 * https://en.wikipedia.org/wiki/Dew_point#Calculating_the_dew_point
 */
float dew_point(float humidity, float temp)
{
	float subres = logf(humidity/100*powf(M_E, 17.67/(243.5+temp)));
	return 243.5*subres/(17.67-subres);
}

float CtoF(float value)
{
	return value*9/5 + 32;
}

float MStoMPH(float value)
{
	return value/0.44704;
}

float MMtoINCH(float value)
{
	return value*0.03937;
}

float HPAtoINCH(float value)
{
	return value*0.02952998751;
}
