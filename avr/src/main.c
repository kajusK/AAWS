/*
 * Weather station with serial output
 *
 * temperature -40 to 85 (+-0,1) °C
 * air pressure 300 to 1100 (+-4,5) hPa
 * humidity 0-100 (+-2) %RH
 *
 * Data format:
 * ------------
 * #T:23.5;H:32;P:1013.25;R:2.55;W:11.5;D:120;@
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
 *
 * Commands:
 * ---------
 * #R@ - reset rain counter (e.g. every hour...)
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include "config.h"
#include "bmp180.h"
#include "dht22.h"
#include "rain.h"
#include "wind.h"
#include "serial.h"

ISR(BADISR_vect)
{

}

void print_num(uint8_t num)
{

}

int main(int argc, char *argv[])
{
	int16_t temp;
	uint16_t pres;
	uint16_t hum;

	//TODO detect type of reset (watchdog....)
	serial_init();
	puts("Weather station, FW version " VERSION);
	puts("Kajus 2016\n\r");

	bmp180_init();
	rain_init();
	wind_init();
	//wind speed, rain counter....

//	set_sleep_mode()

	//finally, enable interrupts
	sei();

	while (1) {
		bmp180_read(&temp, &pres);
		dht22_read(&temp, &hum);
		//sleep 10 seconds
	}

	return 0;
}
