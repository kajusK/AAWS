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
#include <time.h>

#include "utils/weather.h"
#include "backends/backends.h"

int backend_live(struct s_weather *weather, struct s_station *station,
		void *additional)
{
	FILE *f;
	char *path = (char *) additional;
	time_t timestamp = time(NULL);
	struct tm *info;
	char date[32];
	char buf[256];
	char *buf_start = buf;
	struct s_print_item data[] = {
			{"temperature: %.1f °C\n", weather->data.temp,
				weather->valid & V_TEMP},
			{"dew_point: %.1f °C\n", weather->data.dew_point,
				weather->valid & V_DEW_POINT},
			{"humidity: %.1f %%RH\n", weather->data.humidity,
				weather->valid & V_HUMIDITY},
			{"pressure: %.2f hPa\n", weather->data.pressure,
				weather->valid & V_PRESSURE},
			{"uv_index: %.1f\n", weather->data.uv,
				weather->valid & V_UV},
			{"wind_speed: %.1f m/s\n", weather->data.wind_speed,
				weather->valid & V_WIND_SPEED},
			{"wind_direction: %.0f °\n", (float) weather->data.wind_dir,
				weather->valid & V_WIND_DIR},
			{"wind_gusts_speed: %.1f m/s\n", weather->data.wind_gusts,
				weather->valid & V_WIND_GUSTS},
			{"wind_gusts_direction: %.0f °\n", (float) weather->data.wind_gusts_dir,
				weather->valid & V_WIND_GUSTS_DIR},
			{"rain: %.2f mm/h\n", weather->data.rain,
				weather->valid & V_RAIN},
			{"rain_last_hour: %.2f mm\n", weather->data.rain_1h,
				weather->valid & V_RAIN_1H}};

	f = fopen(path, "w");
	if (f == NULL) {
		fprintf(stderr, "Unable to open %s\n", path);
		return -1;
	}

	info = localtime(&timestamp);
	strftime(date, 32, "%d.%m.%Y %H:%M", info),

	buf_start += sprintf(buf, "station: %s\n"
		   "date: %s\n"
		   "elevation: %dm AMSL\n",
		   station->name,
		   date,
		   station->elevation);
	sprintf_valid(buf_start, data, sizeof(data)/sizeof(struct s_print_item));

	fputs(buf, f);

	fclose(f);
	return 0;
}
