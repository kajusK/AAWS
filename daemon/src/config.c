/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <string.h>

#include "config_parser.h"
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
}

int conf_read(char *filename)
{
	conf_default(&conf);

	struct s_config_parse parse[] = {
		{"database", (void *) conf.db_file, C_STRING},
		{"serial", (void *) conf.serial, C_STRING},
		{"baudrate", (void *) &conf.baudr, C_INT},
		{"pid_file", (void *) conf.pid_file, C_STRING},
		{"log_file", (void *) conf.log_file, C_STRING},
		{"save_period", (void *) conf.log_file, C_INT},
	};

	return config_parse(filename, (struct s_config_parse *)&parse,
		     sizeof(parse)/sizeof(struct s_config_parse));
}

struct s_config *conf_get(void)
{
	return &conf;
}
