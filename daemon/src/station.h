/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __STATION_H_
#define __STATION_H_

struct s_values {
	float temp;
	float humidity;
	float pressure;
	float rain;
	float wind_speed;
	float uv;
	int wind_dir;
};

/* received with error */
enum message_err {
	E_TEMP = 1 << 0,
	E_HUMIDITY = 1 << 1,
	E_PRESSURE = 1 << 2,
	E_RAIN = 1 << 3,
	E_WIND_SPEED = 1 << 4,
	E_WIND_DIR = 1 << 5,
	E_UV = 1 << 6,
};

/* item was present in the message */
enum message_pres {
	P_TEMP = 1 << 0,
	P_HUMIDITY = 1 << 1,
	P_PRESSURE = 1 << 2,
	P_RAIN = 1 << 3,
	P_WIND_SPEED = 1 << 4,
	P_WIND_DIR = 1 << 5,
	P_UV = 1 << 6,
};

struct s_message {
	struct s_values val;
	enum message_err err;
	enum message_pres pres;
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
