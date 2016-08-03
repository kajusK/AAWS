/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <string.h>

#include "utils/config_parser.h"
#include "config.h"

static struct s_config conf;

static void conf_default(struct s_config *conf)
{
	strcpy(conf->pid_file, DEFAULT_PID_FILE);
	strcpy(conf->log_file, DEFAULT_LOG_FILE);
	conf->save_period = DEFAULT_SAVE_PERIOD;

	strcpy(conf->serial, DEFAULT_SERIAL);
	conf->baudr = DEFAULT_BAUDR;

	conf->station.elevation = DEFAULT_ELEVATION;
	strcpy(conf->station.latitude, "default");
	strcpy(conf->station.longitude, "default");
	strcpy(conf->station.name, "default");

	conf->backends = 0;
	strcpy(conf->sqlite_path, DEFAULT_SQLITE_FILE);
	strcpy(conf->live_path, DEFAULT_LIVE_FILE);

	strcpy(conf->mysql.username, "default");
	strcpy(conf->mysql.password, "default");
	strcpy(conf->mysql.database, "default");
	strcpy(conf->mysql.host, "default");

	strcpy(conf->wunderground.username, "default");
	strcpy(conf->wunderground.password, "default");

	strcpy(conf->oweathermap.username, "default");
	strcpy(conf->oweathermap.password, "default");
}

static int conf_validate(struct s_config *conf)
{
	int ret = 0;

	if (conf->station.elevation == DEFAULT_ELEVATION) {
		fprintf(stderr, "Elevation must be defined to calculate "
			        "relative pressure correctly\n");
		ret = 1;
	}

	if (conf->backends == 0) {
		fprintf(stderr, "At least one backend must be enabled\n");
		ret = 1;

	}

	//all enabled options must be set
	return ret;
}

int conf_read(char *filename)
{
	int ret = 0;
	int b_live, b_sqlite, b_mysql, b_wunder, b_oweather;
	b_live = b_sqlite = b_mysql = b_wunder = b_oweather = 0;

	conf_default(&conf);

	struct s_config_parse general[] = {
		{"serial", (void *) conf.serial, C_STRING},
		{"baudrate", (void *) &conf.baudr, C_INT},
		{"pid_file", (void *) conf.pid_file, C_STRING},
		{"log_file", (void *) conf.log_file, C_STRING},
		{"save_period", (void *) &conf.save_period, C_INT},
	};

	struct s_config_parse station[] = {
		{"elevation", (void *) &conf.station.elevation, C_INT},
		{"latitude", (void *) conf.station.latitude, C_STRING},
		{"longitude", (void *) conf.station.longitude, C_STRING},
		{"name", (void *) conf.station.name, C_STRING},
	};

	struct s_config_parse sqlite[] = {
		{"enabled", (void *) &b_sqlite, C_BOOL},
		{"filename", (void *) conf.sqlite_path, C_STRING},
	};

	struct s_config_parse mysql[] = {
		{"enabled", (void *) &b_mysql, C_BOOL},
		{"username", (void *) conf.mysql.username, C_STRING},
		{"password", (void *) conf.mysql.password, C_STRING},
		{"host", (void *) conf.mysql.host, C_STRING},
		{"database", (void *) conf.mysql.database, C_STRING},
	};

	struct s_config_parse live[] = {
		{"enabled", (void *) &b_live, C_BOOL},
		{"filename", (void *) conf.live_path, C_STRING},
	};

	struct s_config_parse wunderground[] = {
		{"enabled", (void *) &b_wunder, C_BOOL},
		{"id", (void *) conf.wunderground.username, C_STRING},
		{"password", (void *) conf.wunderground.password, C_STRING},
	};

	struct s_config_parse oweathermap[] = {
		{"enabled", (void *) &b_oweather, C_BOOL},
		{"username", (void *) conf.oweathermap.username, C_STRING},
		{"password", (void *) conf.oweathermap.password, C_STRING},
	};

	struct s_config_cat cats[] = {
		{"general", sizeof(general)/sizeof(struct s_config_parse),
			(struct s_config_parse *) &general},
		{"station", sizeof(station)/sizeof(struct s_config_parse),
			(struct s_config_parse *) &station},
		{"sqlite", sizeof(sqlite)/sizeof(struct s_config_parse),
			(struct s_config_parse *) &sqlite},
		{"mysql", sizeof(mysql)/sizeof(struct s_config_parse),
			(struct s_config_parse *) &mysql},
		{"live", sizeof(live)/sizeof(struct s_config_parse),
			(struct s_config_parse *) &live},
		{"openweathermap", sizeof(oweathermap)/sizeof(struct s_config_parse),
			(struct s_config_parse *) &oweathermap},
		{"wunderground", sizeof(wunderground)/sizeof(struct s_config_parse),
			(struct s_config_parse *) &wunderground},
	};

	ret = config_parse_cat(filename, (struct s_config_cat *) &cats,
		     sizeof(cats)/sizeof(struct s_config_cat));
	if (ret != 0)
		return ret;

	/* set enabled backends */
	if (b_live)
		conf.backends |= B_LIVE;
	if (b_sqlite)
		conf.backends |= B_SQLITE;
	if (b_mysql)
		conf.backends |= B_MYSQL;
	if (b_wunder)
		conf.backends |= B_WUNDERGROUND;
	if (b_oweather)
		conf.backends |= B_OWEATHERMAP;

	return conf_validate(&conf);
}

struct s_config *conf_get(void)
{
	return &conf;
}
