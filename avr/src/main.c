/*
 * Weather station with serial output
 *
 * temperature -40 to 85 (+-0,1) °C
 * air pressure 300 to 1100 (+-6) hPa
 * humidity 0-100 (+-2) %RH
 *
 * Data format:
 * ------------
 * #T:23.5;H:32;P:1013.25;R:2.55;W:11.5;D:E;@
 *
 * Explanation:
 * # - start of the message
 * T - temperatur in °C
 * H - humidity in %RH
 * P - absolute pressure in hPa
 * R - rain counter since last reset in mm
 * W - wind speed in m/s
 * D - wind direction in degrees
 * @ - end of message
 * E - error, sensor dead
 *
 * Ack reply:
 * ----------
 * #A;@
 *
 * Commands:
 * ---------
 * #R;@ - reset rain counter (e.g. every hour...), responds with ACK
 * #T;@ - test if station is alive, responds with ACK
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>

#include "config.h"
#include "bmp180.h"
#include "dht22.h"
#include "rain.h"
#include "wind.h"
#include "utils/uart.h"

ISR(BADISR_vect)
{

}

void delay_s(uint8_t sec)
{
	uint8_t i;

	for (i = 0; i < sec; i++) {
		_delay_ms(1000);
	}
}

static void print_decimal(char *buffer, uint8_t decimals)
{
	uint8_t i = 0;

	if (strlen(buffer) <= decimals) {
		putchar('0');
	} else {
		for (; i < strlen(buffer) - decimals; i++)
			putchar(buffer[i]);
	}

	if (!decimals)
		return;

	putchar('.');
	for ( ; i < strlen(buffer); i++ )
		putchar(buffer[i]);
}

/*
 * Print integer to stdour, insert dot to separated decimals
 */
static void print_num(int num, uint8_t decimals)
{
	char buffer[9];
	itoa(num, buffer, 10);
	print_decimal(buffer, decimals);
}

static void print_32num(uint32_t num, uint8_t decimals)
{
	char buffer[9];
	ultoa(num, buffer, 10);
	print_decimal(buffer, decimals);
}

int main(int argc, char *argv[])
{
	int16_t temp;
	uint32_t pres;
	uint16_t hum;

	//TODO detect type of reset (watchdog....)
	uart_init();
	sei();

	fputs("Weather station, FW version " VERSION"\n\r", stdout);
	fputs("Kajus 2016\n\r", stdout);

	bmp180_init();
	rain_init();
	wind_init();

	puts("Init done");

	//set_sleep_mode()

	while (1) {
		//TODO print with errors
		putchar('#');

		bmp180_read(&temp, &pres);
		fputs("P:", stdout);
		print_32num(pres, 2);

		dht22_read(&temp, &hum);
		fputs(";T:", stdout);
		print_num(temp, 1);
		fputs(";H:", stdout);
		print_num(hum, 1);

		fputs(";W:", stdout);
		print_num(wind_speed(), 1);
		fputs(";D:", stdout);
		print_num(wind_dir(), 0);

		fputs(";R:", stdout);
		print_num(rain_get(), 1);

		fputs(";@\n", stdout);

		delay_s(5);
	}

	return 0;
}
