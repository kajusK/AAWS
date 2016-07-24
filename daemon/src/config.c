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
	strcpy(conf->db_file, DEFAULT_DB_FILE);
	strcpy(conf->pid_file, DEFAULT_PID_FILE);
	strcpy(conf->log_file, DEFAULT_LOG_FILE);

	strcpy(conf->serial, DEFAULT_SERIAL);
	conf->baudr = DEFAULT_BAUDR;

	conf->save_period = DEFAULT_SAVE_PERIOD;
	conf->elevation = DEFAULT_ELEVATION;
}

int conf_read(char *filename)
{
	int ret = 0;

	conf_default(&conf);

	struct s_config_parse parse[] = {
		{"database", (void *) conf.db_file, C_STRING},
		{"serial", (void *) conf.serial, C_STRING},
		{"baudrate", (void *) &conf.baudr, C_INT},
		{"pid_file", (void *) conf.pid_file, C_STRING},
		{"log_file", (void *) conf.log_file, C_STRING},
		{"save_period", (void *) conf.log_file, C_INT},
		{"elevation", (void *) &conf.elevation, C_INT},
	};

	ret = config_parse(filename, (struct s_config_parse *)&parse,
		     sizeof(parse)/sizeof(struct s_config_parse));
	if (ret != 0)
		return ret;

	if (conf.elevation == DEFAULT_ELEVATION) {
		fprintf(stderr, "Elevation must be defined in config file '%s' "
				"to calculate absolute pressure\n", filename);
		return -1;
	}
	return 0;
}

struct s_config *conf_get(void)
{
	return &conf;
}
