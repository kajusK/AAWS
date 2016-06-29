/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>

#include "db.h"

static sqlite3 *db;

static int db_create()
{
	char *err = 0;
	int res;

	res = sqlite3_exec(db, "CREATE TABLE 'weather' ( "
			"timestamp INTEGER,"
			"temp REAL,"
			"humidity REAL,"
			"rain REAL,"
			"pressure REAL,"
			"wind_speed REAL,"
			"wind_gusts REAL,"
			"wind_direction INTEGER );"
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

int db_init(char *filename)
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

	if (create && db_create() != 0) {
		db_close();
		return -1;
	}

	return 0;
}

void db_close()
{
	sqlite3_close(db);
}

int db_add_weather(int wind_dir, float wind_speed, float wind_gusts, float press,
		   float temp, float humidity, float rain)
{
	int res = 0;
	sqlite3_stmt *stmt;

	res = sqlite3_prepare_v2(db, "INSERT INTO 'weather' (timestamp, temp,"
			"humidity, rain, pressure, wind_speed, wind_gusts,"
			"wind_direction) VALUES (strftime('%s','now'),?,?,?,?,?);",
			-1, &stmt, NULL);
	if (res != SQLITE_OK) {
		fprintf(stderr, "Unable to do query: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_bind_double(stmt, 1, temp);
	sqlite3_bind_double(stmt, 2, humidity);
	sqlite3_bind_double(stmt, 3, rain);
	sqlite3_bind_int(stmt, 4, press);
	sqlite3_bind_double(stmt, 5, wind_speed);
	sqlite3_bind_int(stmt, 6, wind_gusts);
	sqlite3_bind_int(stmt, 7, wind_dir);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		fprintf(stderr, "Unable to do query: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	sqlite3_finalize(stmt);

	return 0;
}

int db_add_rain(float rain)
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
