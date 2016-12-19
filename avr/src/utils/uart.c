/*
 * Serial communications using stdin/out, stderr is redirected to stdout
 * Global interrupt must be enabled
 *
 * Using one stop bit and no parity
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "config.h"
#include <util/setbaud.h>

#include "utils/ring_buf.h"

#if defined USART_RXC_vect
	#define USART_RX_vect USART_RXC_vect
#endif

#define TX_BUF_LEN 8
#define RX_BUF_LEN 8

static ring_buf ring_rx, ring_tx;
static char rxbuf[RX_BUF_LEN], txbuf[TX_BUF_LEN];

ISR(USART_RX_vect)
{
	ring_push(&ring_rx, UDR);
}

ISR(USART_UDRE_vect)
{
	if (ring_empty(&ring_tx))
		UCSRB &= ~_BV(UDRIE);
	else
		UDR = ring_pop(&ring_tx);
}

int uart_putc(char c, FILE __attribute__((__unused__)) *stream)
{
	if (c == '\n')
		uart_putc('\r', stream);

	while (ring_full(&ring_tx))
		;
	ring_push(&ring_tx, c);

	//enable interrupt for empty TX buffer
	UCSRB |= _BV(UDRIE);

	return 0;
}

int uart_stderr_putc(char c, FILE *stream)
{
	return uart_putc(c, stream);
}

int uart_getc(FILE __attribute__((__unused__)) *stream)
{
	while (ring_empty(&ring_rx))
		;
	return ring_pop(&ring_rx);
}

/*
 * check if there are data in input queue
 *
 * returns 0 if empty, 1 otherwise
 */
uint8_t uart_check_rx(void)
{
	return !ring_empty(&ring_rx);
}

/*
 * Open stdin/err/out descriptors, setup avr hardware...
 */
void uart_init(void)
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

	// redirect stdout/in to uart
	static FILE s_out = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);
	static FILE s_err = FDEV_SETUP_STREAM(uart_stderr_putc, NULL, _FDEV_SETUP_WRITE);
	static FILE s_in = FDEV_SETUP_STREAM(NULL, uart_getc, _FDEV_SETUP_READ);
	stdin = &s_in;
	stderr = &s_err;
	stdout = &s_out;

	ring_init(&ring_rx, rxbuf, RX_BUF_LEN);
	ring_init(&ring_tx, txbuf, TX_BUF_LEN);

	// enable interrupts
	UCSRB |= _BV(RXCIE);
}
