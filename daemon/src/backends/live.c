/*
 * Weather station
 *
 * Saving data in text format to file
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <stdlib.h>

#include "utils/weather.h"
#include "backends/backends.h"

int backend_live(struct s_weather *weather, struct s_station *station,
		void *additional)
{
	FILE *f;
	char *path = (char *) additional;

	f = fopen(path, "w");
	if (f == NULL) {
		fprintf(stderr, "Unable to open %s\n", path);
		return -1;
	}

	fprintf(f, "station: %s\n"
		   "elevation: %d\n m AMSL"
		   "temperature: %.1f °C\n"
		   "dew_point: %.1f °C\n"
		   "humidity: %.1f %%RH\n"
		   "pressure: %.2f hPa\n"
		   "wind_speed: %.1f\n"
		   "wind_direction: %d\n"
		   "wind_gusts_speed: %.1f\n"
		   "wind_gusts_direction: %d\n"
		   "rain: %.2f mm/h\n"
		   "rain_last_hour: %.2f\n",
		   station->name,
		   station->elevation,
		   weather->temp,
		   weather->dew_point,
		   weather->humidity,
		   weather->pressure,
		   weather->wind_speed,
		   weather->wind_dir,
		   weather->wind_gusts,
		   weather->wind_gusts_dir,
		   weather->rain,
		   weather->rain_1h);

	fclose(f);
	return 0;
}
