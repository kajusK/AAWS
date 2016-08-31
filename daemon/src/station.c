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

#define check_item_err(c, err, type) { if ((c) == 'E') err |= type; }

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

	msg.err = 0;
	msg.pres = 0;

	if (*c == '#')
		c++;

	while (*c != '\0') {
		switch (*c) {
		case ';':
		case ':':
		case 'E':
			break;
		case '@':
			return msg;
			break;
		case 'T':
			check_item_err(*(c+2), msg.err, E_TEMP);
			msg.pres |= P_TEMP;
			msg.val.temp = strtof(c+2, &c);
			break;
		case 'H':
			check_item_err(*(c+2), msg.err, E_HUMIDITY);
			msg.pres |= P_HUMIDITY;
			msg.val.humidity = strtof(c+2, &c);
			break;
		case 'P':
			check_item_err(*(c+2), msg.err, E_PRESSURE);
			msg.pres |= P_PRESSURE;
			msg.val.pressure = strtof(c+2, &c);
			break;
		case 'R':
			check_item_err(*(c+2), msg.err, E_RAIN);
			msg.pres |= P_RAIN;
			msg.val.rain = strtof(c+2, &c);
			break;
		case 'W':
			check_item_err(*(c+2), msg.err, E_WIND_SPEED);
			msg.pres |= P_WIND_SPEED;
			msg.val.wind_speed = strtof(c+2, &c);
			break;
		case 'D':
			check_item_err(*(c+2), msg.err, E_WIND_DIR);
			msg.pres |= P_WIND_DIR;
			msg.val.wind_dir = strtol(c+2, &c, 10);
			break;
		case 'U':
			check_item_err(*(c+2), msg.err, E_UV);
			msg.pres |= P_UV;
			msg.val.uv = strtof(c+2, &c);
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
	while (serial_read_noblock(fd, &c, 1) == 0 && ++count < 50)
		;
	if (count == 50)
		return 0;

	while (serial_getc(fd) != '@')
		;

	return 1;
}
