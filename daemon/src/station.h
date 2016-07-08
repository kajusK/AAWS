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


extern struct s_message station_read(int fd);

/*
 * Reset rain counter
 */
extern void station_rain_reset(int fd);

/*
 * Check if station is alive
 */
extern int station_alive(int fd);

#endif
