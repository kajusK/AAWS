/*
 * Serial communications using stdin/out, stderr is redirected to stdout
 * Global interrupt must be enabled
 *
 * Using one stop bit and no parity
 *
 * Jakub Kaderka 2016
 */

#ifndef _UART_H_
#define _UART_H_

//BAUD and F_CPU must be defined
//#define BAUD	9600

/*
 * check if there are data in input queue
 *
 * returns 0 if empty, 1 otherwise
 */
int uart_check_rx(void);

/*
 * Open stdin/err/out descriptors, setup avr hardware...
 */
void uart_init(void);

#endif
