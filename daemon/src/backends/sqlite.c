/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>

#include "utils/weather.h"
#include "backends/backends.h"

static sqlite3 *db;

static int sqlite_create()
{
	char *err = 0;
	int res;

	printf("Creating a new sqlite database\n");

	res = sqlite3_exec(db, "CREATE TABLE 'weather' ( "
			"timestamp INTEGER,"
			"temp REAL,"
			"humidity REAL,"
			"rain REAL,"
			"pressure REAL,"
			"wind_speed REAL,"
			"wind_gusts REAL,"
			"wind_gusts_dir INTEGER,"
			"wind_direction INTEGER);"
			"CREATE TABLE 'rain' ( "
			"timestamp INTEGER,"
			"rain REAL);"
			, NULL, NULL, &err);

	if (res != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s", err);
		sqlite3_free(err);
		return -1;
	}

	return 0;
}

void sqlite_close()
{
	sqlite3_close(db);
}

int sqlite_init(char *filename)
{
	int create = 0;
	if (access(filename, F_OK) == -1) {
		fprintf(stderr, "Warning, no database found, creating new\n");
		create = 1;
	}

	if (sqlite3_open(filename, &db) != SQLITE_OK) {
		fprintf(stderr, "Unable to open database %s: %s\n", filename,
				sqlite3_errmsg(db));
		return -1;
	}

	if (create && sqlite_create() != 0) {
		sqlite_close();
		return -1;
	}

	atexit(sqlite_close);
	return 0;
}


int backend_sqlite(struct s_weather *weather, struct s_station *station,
			  void *additional)
{
	int res = 0;
	sqlite3_stmt *stmt;

	res = sqlite3_prepare_v2(db, "INSERT INTO 'weather' (timestamp, temp,"
			"humidity, rain, pressure, wind_speed, wind_gusts,"
			"wind_direction, wind_gusts_dir) VALUES "
		        "(strftime('%s','now'),?,?,?,?,?,?,?,?);",
			-1, &stmt, NULL);
	if (res != SQLITE_OK) {
		fprintf(stderr, "Unable to do query: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_bind_double(stmt, 1, weather->temp);
	sqlite3_bind_double(stmt, 2, weather->humidity);
	sqlite3_bind_double(stmt, 3, weather->rain);
	sqlite3_bind_int(stmt, 4, weather->pressure);
	sqlite3_bind_double(stmt, 5, weather->wind_speed);
	sqlite3_bind_int(stmt, 6, weather->wind_gusts);
	sqlite3_bind_int(stmt, 7, weather->wind_dir);
	sqlite3_bind_int(stmt, 8, weather->wind_gusts_dir);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		fprintf(stderr, "Unable to do query: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	sqlite3_finalize(stmt);

	return 0;
}

int sqlite_add_rain(float rain)
{
	int res = 0;
	sqlite3_stmt *stmt;

	res = sqlite3_prepare_v2(db, "INSERT INTO 'rain' (timestamp, rain) "
			"VALUES (strftime('%s','now'),?);",
			-1, &stmt, NULL);
	if (res != SQLITE_OK) {
		fprintf(stderr, "Unable to do query: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_bind_double(stmt, 1, rain);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		fprintf(stderr, "Unable to do query: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	sqlite3_finalize(stmt);

	return 0;
}
