/*
 * ADC module
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <avr/io.h>
#include <inttypes.h>

#include "adc.h"

/* prescaler set to 128, modify for higher sampling rate */
#define AD_PRESCAL 3

uint16_t adc_read(uint8_t port_mux, ad_ref ref)
{
	ADMUX = (ref << REFS0) | (port_mux & 0x1f);
	ADCSRA = _BV(ADEN) | _BV(ADSC) | AD_PRESCAL;
	loop_until_bit_is_clear(ADCSRA, ADSC);

	return (ADCH << 8) | ADCL;
}

uint8_t adc_read8(uint8_t port_mux, ad_ref ref)
{
	ADMUX = (ref << REFS0) | _BV(ADLAR) | (port_mux & 0x1f);
	ADCSRA = _BV(ADEN) | _BV(ADSC) | AD_PRESCAL;
	loop_until_bit_is_clear(ADCSRA, ADSC);

	return ADCH;
}
