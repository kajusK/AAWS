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
	char *buf_start = buf;
	int req_res;
	struct s_creds *creds = (struct s_creds *) additional;
	struct s_print_item data[] = {
			{"wind_dir=%.0f&", (float) weather->data.wind_dir,
				weather->valid & V_WIND_DIR},
			{"wind_speed=%.1f&", weather->data.wind_speed,
				weather->valid & V_WIND_SPEED},
			{"wind_gusts=%.1f&", weather->data.wind_gusts,
				weather->valid & V_WIND_GUSTS},
			{"temp=%.1f&", weather->data.temp,
				weather->valid & V_TEMP},
			{"humidity=%.1f&", weather->data.humidity,
				weather->valid & V_HUMIDITY},
			{"pressure=%.2f&", weather->data.pressure,
				weather->valid & V_PRESSURE},
			{"rain_1h=%.2f&", weather->data.rain_1h,
				weather->valid & V_RAIN_1H},
			{"dewpoint=%.1f&", weather->data.dew_point,
				weather->valid & V_DEW_POINT},
			{"uv=%.1f", weather->data.uv,
				weather->valid & V_UV}};

	auth_header = http_gen_auth_header(creds->username, creds->password);
	if (auth_header == NULL)
		return -1;

	buf_start += sprintf(buf, "lat=%s&long=%s&alt=%d&name=%s&",
		station->latitude, station->longitude, station->elevation,
		station->name);
	sprintf_valid(buf_start, data, sizeof(data)/sizeof(struct s_print_item));

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
