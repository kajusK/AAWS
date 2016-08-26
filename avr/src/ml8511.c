/*
 * ML8511 UV sensor
 *
 * https://cdn.sparkfun.com/datasheets/Sensors/LightImaging/ML8511_3-8-13.pdf
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

#include "config.h"
#include "utils/adc.h"
#include "ml8511.h"

/*
 * Maximum value that can be measured, equals to UV index 15
 */
#define MAX_ML_ADC ((int)(2.8*255/VCC_VOLTAGE))

/*
 * ADC value for lowest possible output from module (UV index 0) - 1V
 */
#define MIN_ML_ADC ((int)(1*255/VCC_VOLTAGE))

/*
 * Returns UV index (0-15) in tenths (e.g 85 = 8.5) or 0xff if module doesn't work
 */
uint8_t ml8511_get(void)
{
	uint8_t adc = adc_read8(ML8511_ADC, AVCC);
	if (adc < MIN_ML_ADC/2)
		return 0xff;
	if (adc < MIN_ML_ADC)
		return 0;
	if (adc > MAX_ML_ADC)
		return 150;

	/* just using simple line equation */
	return (adc - MIN_ML_ADC)*150/(MAX_ML_ADC);
}
