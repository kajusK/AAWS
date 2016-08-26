/*
 * ADC module
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#ifndef __ADC_H_
#define __ADC_H_

typedef enum {
	AREF = 0, /*if voltage on AREF is present, no other modes can be used */
	AVCC = 1, /* AVCC with capacitor on AREF */
	INTREF = 3, /* internal 2.56V, capacitor on AREF */
} ad_ref;

/*
 * Get 10 bit ADC value
 *
 * port_mux 0-7 for single channel, higher are with gain or differential input
 */
extern uint16_t adc_read(uint8_t port_mux, ad_ref ref);

/*
 * Get 8 bit ADC value
 *
 * port_mux 0-7 for single channel, higher are with gain or differential input
 */
extern uint8_t adc_read8(uint8_t port_mux, ad_ref ref);

#endif
