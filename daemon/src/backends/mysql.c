/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdlib.h>

#include "utils/weather.h"
#include "config.h"

void mysql_close()
{
	//TODO
}

int mysql_init(struct s_db *config)
{
	//TODO
	atexit(mysql_close);

	return 0;
}

int backend_mysql(struct s_weather *weather, struct s_station *station,
		 void *additional)
{
	//TODO
	return 0;
}

int mysql_add_rain(float rain)
{
	return 0;
}
