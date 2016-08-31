/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __WEATHER_H_
#define __WEATHER_H_

enum weather_valid {
	V_TEMP = 1 << 0,
	V_DEW_POINT = 1 << 1,
	V_HUMIDITY = 1 << 2,
	V_PRESSURE = 1 << 3,
	V_WIND_SPEED = 1 << 4,
	V_WIND_GUSTS = 1 << 5,
	V_WIND_DIR = 1 << 6,
	V_WIND_GUSTS_DIR = 1 << 7,
	V_RAIN = 1 << 8,
	V_RAIN_1H = 1 << 9,
	V_UV = 1 << 10,
};

struct s_weather_data {
	float temp;		/* outdoor temp, degrees C */
	float dew_point;	/* degrees C */
	float humidity;		/* in %RH */
	float pressure;		/* hPa, relative pressure */

	float wind_speed;	/* m/s */
	float wind_gusts;	/* max value in n minutes */
	int wind_dir;		/* degrees, direction from which is blowing */
	int wind_gusts_dir;	/* direction of max value */

	float rain;		/* current rain intensity - mm/h estimation */
	float rain_1h;		/* rain in last hour */

	float uv;		/* UV index */
};

struct s_weather {
	struct s_weather_data data;
	enum weather_valid valid;
};

enum t_cloud {
	NOT_MEASURED,
	CLR,
	FEW,
	SCT,
	BKN,
	OVC
};

struct s_weather_todo {
	float temp;		/* outdoor temp, degrees C */
	float soil_temp;	/* soil, degrees C */
	float dew_point;	/* degrees C */
	float wind_chill;	/* C, feeling temp due to wind */
	float heat_index;	/* C, depends on temp and humity */

	float humidity;		/* in %RH */
	float pressure;		/* hPa, relative pressure */

	float wind_speed;	/* m/s */
	float wind_gusts;	/* max value in n minutes */
	int wind_dir;		/* degrees, direction from which is blowing */
	int wind_gusts_dir;	/* direction of max value */

	float rain;		/* current rain intensity - mm/h estimation */
	float rain_1h;		/* rain in last hour */
	float rain_24h;		/* rain in last 24 hours */
	float rain_today;	/* mm since midnight locatime */
	float snow_1h;		/* mm since midnight localtime */
	float snow_today;	/* mm since midnight localtime */

	float brightness;	/* W/m² */
	int UV;			/* UV index */
	float radiation;	/* */

	enum t_cloud clouds;	/* METAR format */
	int cloud_base;		/* hundreds of feets */
	int visibility;		/* horizontal, in meters */
};

/*
 * Calculate relative pressure
 * absolute + elevation/8.3
 */
extern float press_relative(float absolute, int elevation);

/*
 * Calculate dew point
 * https://en.wikipedia.org/wiki/Dew_point#Calculating_the_dew_point
 */
extern float dew_point(float humidity, float temp);

extern float CtoF(float value);
extern float MStoMPH(float value);
extern float MMtoINCH(float value);
extern float HPAtoINCH(float value);

#endif
