/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdlib.h>

#include "config.h"
#include "utils/weather.h"
#include "backends/backends.h"

struct s_backend {
	void *data;
	int (* backend)(struct s_weather *weather, struct s_station *station, void *additional);
	int (* add_rain)(float rain);
};

struct s_backend backends[6];

int backends_init(struct s_config *config)
{
	int i = 0;

	if ((config->backends & B_SQLITE) && sqlite_init(config->sqlite_path) != 0)
		return -1;
	if ((config->backends & B_MYSQL) && backend_mysql_init(&config->mysql))
		return -1;

	if (config->backends & B_SQLITE) {
		backends[i].data = NULL;
		backends[i].add_rain = sqlite_add_rain;
		backends[i++].backend = backend_sqlite;
	}
	if (config->backends & B_MYSQL) {
		backends[i].data = NULL;
		backends[i].add_rain = mysql_add_rain;
		backends[i++].backend = backend_mysql;
	}
	if (config->backends & B_LIVE) {
		backends[i].data = (void *) config->live_path;
		backends[i].add_rain = NULL;
		backends[i++].backend = backend_live;
	}
	if (config->backends & B_WUNDERGROUND) {
		backends[i].data = (void *) &config->wunderground;
		backends[i].add_rain = NULL;
		backends[i++].backend = backend_wunderground;
	}
	if (config->backends & B_OWEATHERMAP) {
		backends[i].data = (void *) &config->oweathermap;
		backends[i].add_rain = NULL;
		backends[i++].backend = backend_openweathermap;
	}
	backends[i].backend = NULL;

	return 0;
}

int backends_send(struct s_weather *weather, struct s_config *config)
{
	int i = 0;
	int ret = 0;

	while (backends[i].backend != NULL) {
		ret |= backends[i].backend(weather, &config->station, backends[i].data);
		i++;
	}

	return ret;
}

int backends_add_rain(float rain)
{
	int i = 0;
	int ret = 0;

	while (backends[i].backend != NULL) {
		if (backends[i].add_rain != NULL)
			ret |= backends[i++].add_rain(rain);
	}

	return ret;
}
