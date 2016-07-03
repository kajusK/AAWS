/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __STATION_H_
#define __STATION_H_

struct s_message {
	float temp;
	float humidity;
	float pressure;
	float rain;
	float wind_speed;
	int wind_dir;
};

extern struct s_message read_station(int fd);

#endif
