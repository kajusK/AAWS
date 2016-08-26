/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "utils/weather.h"
#include "config.h"

static MYSQL *conn;

static int backend_mysql_create()
{
	int res;

	printf("Initializing mysql database\n");

	res = mysql_query(conn, "CREATE TABLE `weather` ( "
			"timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP PRIMARY KEY,"
			"temp FLOAT,"
			"humidity FLOAT,"
			"rain FLOAT,"
			"pressure FLOAT,"
			"uv FLOAT,"
			"wind_speed FLOAT,"
			"wind_gusts FLOAT,"
			"wind_gusts_dir INT,"
			"wind_direction INT);");
	res |= mysql_query(conn, "CREATE TABLE `rain` ( "
			"timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP PRIMARY KEY,"
			"rain FLOAT);");

	if (res != 0) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return -1;
	}

	return 0;
}

void backend_mysql_close()
{
	mysql_close(conn);
}

int backend_mysql_init(struct s_db *config)
{
	int create = 1;
	MYSQL_RES *res;
	MYSQL_ROW row;

	conn = mysql_init(NULL);

	if (!mysql_real_connect(conn, config->host, config->username,
			config->password, config->database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return -1;
	}

	if (mysql_query(conn, "show tables") != 0) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		return -1;
	}
	res = mysql_use_result(conn);
	while ((row = mysql_fetch_row(res)) != NULL) {
		if (strcmp(row[0], "weather") == 0)
			create = 0;
	}
	mysql_free_result(res);

	atexit(backend_mysql_close);

	if (create)
		return backend_mysql_create();

	return 0;
}

int backend_mysql(struct s_weather *weather, struct s_station *station,
		 void *additional)
{
	char query[256];

	sprintf(query, "INSERT INTO `weather` (temp, humidity, rain, pressure,"
		       "wind_speed, wind_gusts, wind_direction, wind_gusts_dir,"
		       " uv) VALUES (%f,%f,%f,%f,%f,%f,%d,%d,%f)",
		       weather->temp, weather->humidity, weather->rain,
		       weather->pressure, weather->wind_speed,
		       weather->wind_gusts, weather->wind_dir,
		       weather->wind_gusts_dir, weather->uv);

	if (mysql_query(conn, query) != 0) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return -1;
	}

	return 0;
}

int mysql_add_rain(float rain)
{
	char query[64];

	sprintf(query, "INSERT INTO `rain` (rain) VALUES (%f)", rain);
	if (mysql_query(conn, query) != 0) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return -1;
	}

	return 0;
}
