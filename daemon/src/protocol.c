/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <stdlib.h>

#include "protocol.h"

/*
 * Decode message (#X:123.456;Y:567.89;@) to structure
 */
struct s_message decode_message(char *message)
{
	struct s_message msg;
	char *c = message;

	if (*c == '#')
		c++;

	while (*c != '@') {
		switch (*c) {
		case ';':
		case ':':
			break;
		case '\0':
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
		default:
			fprintf(stderr, "Unrecognized message item %c", *c);
			break;
		}

		c++;
	}

	return msg;
}
