/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#ifndef __BACKENDS_H_
#define __BACKENDS_H_

#include "config.h"

extern int sqlite_init(char *filename);
extern int backend_mysql_init(struct s_db *db);
extern int sqlite_add_rain(float rain);
extern int mysql_add_rain(float rain);

/*
 * Output all measured data to simple file
 */
extern int backend_live(struct s_weather *weather, struct s_station *station,
			void *additional);

/*
 * Save data to sqlite database
 */
extern int backend_sqlite(struct s_weather *weather, struct s_station *station,
			  void *additional);

/*
 * Save data to mysql database
 */
extern int backend_mysql(struct s_weather *weather, struct s_station *station,
			 void *additional);

/*
 * Upload latest data to http://wunderground.com/
 */
extern int backend_wunderground(struct s_weather *weather,
				struct s_station *station, void *additional);

/*
 * Upload latest data to http://openweathermap.org/
 */
extern int backend_openweathermap(struct s_weather *weather,
				  struct s_station *station, void *additional);


/* Initialize enabled backends */
extern int backends_init(struct s_config *config);

/* Send data to all enabled backends */
extern int backends_send(struct s_weather *weather, struct s_config *config);

/* Save hour sum of rain to backends, if supported */
extern int backends_add_rain(float rain);

#endif
