/*
 * Weather station with serial output
 *
 * temperature -40 to 85 (+-0,5) °C
 * air pressure 300 to 1100 (+-6) hPa
 * humidity 0-100 (+-2) %RH
 * snow depth 0-500 cm (+-3mm)
 *
 * Communication:
 * ==============
 * Data format:
 * ------------
 * #T:23.5;H:32;P:1013.25;R:2.55;W:11.5;D:E;U:5.6;S:12.5;@
 *
 * Explanation:
 * # - start of the message
 * T - temperatur in °C
 * H - humidity in %RH
 * P - absolute pressure in hPa
 * R - rain counter since last reset in mm
 * W - wind speed in m/s
 * D - wind direction in degrees
 * U - UV index (0-15)
 * S - Snow depth in cm
 * @ - end of message
 * E - error, sensor dead
 *
 * Ack reply:
 * ----------
 * #A;@ - ACK
 * #N;@ - NACK - unknown command, etc
 *
 * Firmware version
 * ----------------
 * #V:1.2;@
 *
 * Commands:
 * ---------
 * #R;@ - reset rain counter (e.g. every hour...), responds with ACK
 * #T;@ - test if station is alive, responds with ACK
 * #V;@ - firmware version
 * #C;@ - calibrate no snow distance - current distance is saved as 0cm.
 *
 * For license, see LICENSE.txt
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
#include "ml8511.h"
#include "utils/uart.h"
#include "snow.h"

#define SEND_DELAY 5

enum sensor_state {
	E_INITBMP180 = 1 << 0,
	E_BMP180 = 1 << 1,
	E_DHT22 = 1 << 2,
};

ISR(BADISR_vect)
{

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
 * Print integer to stdout, insert dot to separated decimals
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

static void check_rx(void)
{
	char command;

	if (uart_check_rx() == 0 || getchar() != '#')
		return;

	//avoid deadlock when no more data arrived
	if (uart_check_rx() == 0) {
		_delay_ms(10);
		if (uart_check_rx() == 0)
			return;
	}

	command = getchar();
	putchar('#');
	switch (command) {
	case 'R':
		rain_reset();
		putchar('A');
		break;
	case 'T':
		rain_reset();
		putchar('A');
		break;
	case 'C':
		snow_calibrate();
		putchar('A');
		break;
	case 'V':
		fputs("V:"VERSION, stdout);
		break;
	default:
		putchar('N');
	}
	fputs(";@", stdout);

	while (uart_check_rx() != 0 && getchar() != '@')
		;
}

static enum sensor_state send_data(enum sensor_state state)
{
	int16_t temp_bmp;
	int16_t temp;
	int16_t snow;
	uint32_t pres;
	uint16_t hum;
	uint8_t uv;

	state = state & E_INITBMP180;
	if (state & E_INITBMP180 || bmp180_read(&temp_bmp, &pres) != 0)
		state |= E_BMP180;
	if (dht22_read(&temp, &hum) != 0)
		state |= E_DHT22;

	putchar('#');

	fputs("P:", stdout);
	if (state & E_BMP180)
		putchar('E');
	else
		print_32num(pres, 2);

	fputs(";T:", stdout);
	if (state & E_DHT22) {
		if (state & E_BMP180)
			putchar('E');
		else
			print_num(temp_bmp, 1);

		fputs(";H:E", stdout);
	} else {
		print_num(temp, 1);
		fputs(";H:", stdout);
		print_num(hum, 1);
	}

	fputs(";W:", stdout);
	print_num(wind_speed(), 1);
	fputs(";D:", stdout);
	print_num(wind_dir(), 0);

	fputs(";R:", stdout);
	print_num(rain_get(), 1);

	fputs(";U:", stdout);
	uv = ml8511_get();
	if (uv == 0xff)
		putchar('E');
	else
		print_num(uv, 1);

	fputs(";S:", stdout);
	snow = snow_get();
	if (snow == 0xffff)
		putchar('E');
	else
		print_num(snow, 1);

	fputs(";@\n", stdout);

	// if bmp180 failed, try to init again
	if (state & E_INITBMP180 && bmp180_init() == 0)
		state &= ~E_INITBMP180;

	return state;
}

int main(int argc, char *argv[])
{
	enum sensor_state state = 0;
	int i;

	//TODO detect type of reset (watchdog....)
	uart_init();
	sei();

	fputs("Weather station, FW version " VERSION"\n\r", stdout);
	fputs("Kajus 2016\n\r", stdout);

	if (bmp180_init() != 0) {
		state |= E_INITBMP180;
		fputs("Unable to init bmp180", stderr);
	}

	rain_init();
	wind_init();

	puts("Init done");

	//set_sleep_mode()

	while (1) {
		state = send_data(state);
		for (i = 0; i < SEND_DELAY*10; i++) {
			check_rx();
			_delay_ms(100);
		}
	}

	return 0;
}
