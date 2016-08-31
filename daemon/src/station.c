/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <stdlib.h>

#include "station.h"
#include "utils/serial.h"

static int station_wait_ack(int fd)
{
	char c;

	while(serial_getc(fd) != '#')
		;
	c = serial_getc(fd);

	while(serial_getc(fd) != '@')
		;

	if (c == 'A')
		return 0;
	return 1;
}

/*
 * Decode message ( format #X:123.456;Y:567.89;@) to structure
 */
static struct s_message decode_message(char *message)
{
	struct s_message msg;
	char *c = message;

	if (*c == '#')
		c++;

	while (*c != '\0') {
		switch (*c) {
		case ';':
		case ':':
			break;
		case '@':
			return msg;
			break;
		case 'T':
			msg.temp = strtof(c+2, &c);
			break;
		case 'H':
			msg.humidity = strtof(c+2, &c);
			break;
		case 'P':
			msg.pressure = strtof(c+2, &c);
			break;
		case 'R':
			msg.rain = strtof(c+2, &c);
			break;
		case 'W':
			msg.wind_speed = strtof(c+2, &c);
			break;
		case 'D':
			msg.wind_dir = strtol(c+2, &c, 10);
			break;
		case 'U':
			msg.uv = strtof(c+2, &c);
			break;
		default:
			fprintf(stderr, "Unrecognized message item %c", *c);
			break;
		}

		c++;
	}

	return msg;
}

struct s_message station_read(int fd)
{
	char buffer[100];
	char *loc = buffer;

	while (serial_getc(fd) != '#')
		;

	while ((*loc++ = serial_getc(fd)) != '@' && (buffer + 100) > loc)
		;

	*loc = '\0';

#ifdef DEBUG
	printf("Message: %s\n", buffer);
#endif

	return decode_message(buffer);
}

/*
 * Reset rain counter
 */
void station_rain_reset(int fd)
{
	serial_printf(fd, "#R@;");
	station_wait_ack(fd);
}

/*
 * Check if station is alive
 */
int station_alive(int fd)
{
	char c;
	int count = 0;

	serial_printf(fd, "#R;@");

	//wait for data to show up on serial line
	while (serial_read_noblock(fd, &c, 1) == 0 && count++ < 50)
		;
	if (count == 50)
		return 0;

	while (serial_getc(fd) != '@')
		;

	return 1;
}
