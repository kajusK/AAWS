/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Transmission protocol: http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils/http.h"
#include "utils/weather.h"
#include "backends/backends.h"

#define W_URL "https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php"

int backend_wunderground(struct s_weather *weather, struct s_station *station,
		   void *additional)
{
	char buf[256], *res = NULL;
	char *buf_start = buf;
	int req_res;
	struct s_creds *creds = (struct s_creds *) additional;
	struct s_print_item data[] = {
			{"winddir=%.0f&", (float) weather->data.wind_dir,
				weather->valid & V_WIND_DIR},
			{"windspeedmph=%.1f&", MStoMPH(weather->data.wind_speed),
				weather->valid & V_WIND_SPEED},
			{"windgustsmph=%.1f&", MStoMPH(weather->data.wind_gusts),
				weather->valid & V_WIND_GUSTS},
			{"windgustsdir=%.0f&", (float) weather->data.wind_gusts_dir,
				weather->valid & V_WIND_GUSTS_DIR},
			{"tempf=%.1f&", CtoF(weather->data.temp),
				weather->valid & V_TEMP},
			{"dewptf=%.1f&", CtoF(weather->data.dew_point),
				weather->valid & V_DEW_POINT},
			{"humidity=%.1f&", weather->data.humidity,
				weather->valid & V_HUMIDITY},
			{"baromin=%.2f&", HPAtoINCH(weather->data.pressure),
				weather->valid & V_PRESSURE},
			{"rainin=%.2f&", MMtoINCH(weather->data.rain_1h),
				weather->valid & V_RAIN_1H},
			{"UV=%.1f", weather->data.uv,
				weather->valid & V_UV}};

	buf_start += sprintf(buf, "action=updateraw&ID=%s&PASSWORD=%s&dateutc=now&"
		"softwaretype=AAWS&", creds->username, creds->password);
	sprintf_valid(buf_start, data, sizeof(data)/sizeof(struct s_print_item));

	req_res = http_request(W_URL, GET, buf, NULL, &res, 1);

	switch (req_res) {
	case -1:
		if (res != NULL)
			free(res);
		return -1;
		break;
	case 200:
		break;
	default:
		free(res);
		fprintf(stderr, "Wunderground returned %d\n", req_res);
		return -1;
	}

	if (strstr(res, "success") != NULL) {
		free(res);
		return 0;
	}
	if (strstr(res, "INVALID") != NULL || strstr(res, "incorrect") != NULL) {
		free(res);
		fprintf(stderr, "Wunderground credentials are incorrect\n");
		return -1;
	}

	free(res);

	return 0;
}
