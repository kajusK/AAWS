/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#ifndef __DB_H_
#define __DB_H_

extern int db_init(char *filename);

extern void db_close();

extern int db_add_weather(int wind_dir, float wind_speed, float wind_gusts,
		float press, float temp, float humidity, float rain);

extern int db_add_rain(float rain);

#endif
