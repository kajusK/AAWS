/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __CONFIG_H_
#define __CONFIG_H_

#define DEFAULT_DB_FILE "/var/lib/aaws/weather.db"
#define DEFAULT_SERIAL "/dev/ttyAMA0"
#define DEFAULT_PID_FILE "/var/run/weatherd.pid"
#define DEFAULT_LOG_FILE "/var/log/weatherd.log"
#define DEFAULT_BAUDR 9600
#define DEFAULT_SAVE_PERIOD 5*60

struct s_config {
	char db_file[256];
	char serial[256];
	int baudr;
	char pid_file[256];
	char log_file[256];
	int save_period;
};

/*
 * Read config from filename, use default values where not defined
 */
extern int conf_read(char *filename);

/*
 * Get config readed by conf_read
 */
extern struct s_config *conf_get(void);

#endif
