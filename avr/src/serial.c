/*
 * Serial communications using stdin/out, stderr is redirected to stdout
 *
 * Using one stop bit and no parity
 *
 * Jakub Kaderka 2016
 * TODO: add ISR RX buffer
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "config.h"
#include <util/setbaud.h>

int serial_putc(char c, FILE *stream)
{
	if (c == '\n')
		serial_putc('\r', stream);

	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
}

int serial_stderr_putc(char c, FILE *stream)
{
	return serial_putc(c, stream);
}

int serial_getc(FILE *stream)
{
	loop_until_bit_is_set(UCSRA, RXC);
	return UDR;
}

/*
 * check if there are data in input queue
 *
 * returns 0 if empty, 1 if full
 */
int serial_check_rx(void)
{
	return bit_is_set(UCSRA, RXC);
}

/*
 * Open stdin/err/out descriptors, setup avr hardware...
 */
void serial_init(void)
{
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;

#if USE_2X
	UCSRA |= _BV(U2X);
#else
	UCSRA &= ~(_BV(U2X));
#endif

	// 8 bit mode
#ifdef URSEL
	UCSRC = _BV(UCSZ1) | _BV(UCSZ0) | _BV(URSEL);
#else
	UCSRC = _BV(UCSZ1) | _BV(UCSZ0);
#endif
	UCSRB = _BV(RXEN) | _BV(TXEN);

	// redirect stdout/in to serial
	static FILE s_out = FDEV_SETUP_STREAM(serial_putc, NULL, _FDEV_SETUP_WRITE);
	static FILE s_err = FDEV_SETUP_STREAM(serial_stderr_putc, NULL, _FDEV_SETUP_WRITE);
	static FILE s_in = FDEV_SETUP_STREAM(NULL, serial_getc, _FDEV_SETUP_READ);
	stdin = &s_in;
	stderr = &s_err;
	stdout = &s_out;
}
