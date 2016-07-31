/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Transmission protocol: http://openweathermap.org/stations#trans
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <stdlib.h>

#include "utils/http.h"
#include "utils/weather.h"
#include "backends/backends.h"

#define W_URL "http://openweathermap.org/data/post"

int backend_openweathermap(struct s_weather *weather, struct s_station *station,
		   void *additional)
{
	char *auth_header;
	char buf[256];
	int req_res;
	struct s_creds *creds = (struct s_creds *) additional;

	auth_header = http_gen_auth_header(creds->username, creds->password);
	if (auth_header == NULL)
		return -1;

	sprintf(buf, "wind_dir=%d&"
		"wind_speed=%.1f&"
		"wind_gusts=%.1f&"
		"temp=%.1f&"
		"humidity=%.1f&"
		"pressure=%.2f&"
		"rain_1h=%.2f&"
		"dewpoint=%.1f&"
		"lat=%s&"
		"long=%s&"
		"alt=%d&"
		"name=%s",
		weather->wind_dir,
		weather->wind_speed,
		weather->wind_gusts,
		weather->temp,
		weather->humidity,
		weather->pressure,
		weather->rain_1h,
		weather->dew_point,
		station->latitude,
		station->longitude,
		station->elevation,
		station->name);

	req_res = http_request(W_URL, POST, buf, auth_header, NULL, 0);
	free(auth_header);

	switch (req_res) {
	case 401:
		fprintf(stderr, "Credentials for openweathermap are incorrect\n");
		return -1;
		break;
	case -1:
		return -1;
		break;
	case 200:
		break;
	default:
		fprintf(stderr, "OpenWeatherMap returned %d\n", req_res);
		return -1;
	}

	return 0;
}
