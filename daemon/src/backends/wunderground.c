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
	int req_res;
	struct s_creds *creds = (struct s_creds *) additional;

	sprintf(buf, "action=updateraw&"
		"ID=%s"
		"PASSWORD=%s"
		"dateutc=now"
		"winddir=%d&"
		"windspeedmph=%.1f&"
		"windgustsmph=%.1f&"
		"windgustsdir=%d&"
		"tempf=%.1f&"
		"dewptf=%.1f&"
		"humidity=%.2f&"
		"baromin=%.2f&"
		"rainin=%.2f&"
		"UV=%.1f&"
		"softwaretype=AAWS",
		creds->username,
		creds->password,
		weather->wind_dir,
		MStoMPH(weather->wind_speed),
		MStoMPH(weather->wind_gusts),
		weather->wind_gusts_dir,
		CtoF(weather->temp),
		CtoF(weather->dew_point),
		weather->humidity,
		HPAtoINCH(weather->pressure),
		MMtoINCH(weather->rain_1h),
		weather->uv);

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
