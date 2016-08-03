/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __CONFIG_H_
#define __CONFIG_H_

#define DEFAULT_SQLITE_FILE "/var/lib/aaws/weather.db"
#define DEFAULT_LIVE_FILE "/var/lib/aaws/live.txt"
#define DEFAULT_SERIAL "/dev/ttyAMA0"
#define DEFAULT_PID_FILE "/var/run/weatherd.pid"
#define DEFAULT_LOG_FILE "/var/log/weatherd.log"
#define DEFAULT_BAUDR 9600
#define DEFAULT_SAVE_PERIOD 5*60
#define DEFAULT_ELEVATION -1000

enum e_backends {
	B_SQLITE = 1 << 0,
	B_MYSQL = 1 << 1,
	B_LIVE = 1 << 2,
	B_WUNDERGROUND = 1 << 3,
	B_OWEATHERMAP = 1 << 4,
};

/* Credentials for wunderground, openweathermap... */
struct s_creds {
	char username[32];
	char password[32];
};

struct s_db {
	char username[32];
	char password[32];
	char host[64];
	char database[32];

};

struct s_station {
	char latitude[32];
	char longitude[32];
	int elevation;
	char name[64];
};

struct s_config {
	char serial[256];
	int baudr;
	char pid_file[256];
	char log_file[256];
	int save_period;

	struct s_station station;

	/* backends configuration */
	enum e_backends backends;	/* bit mask of enabled backends */
	char sqlite_path[256];		/* path to sqlite database */
	struct s_db mysql;
	char live_path[256];		/* path to live file */
	struct s_creds wunderground;
	struct s_creds oweathermap;
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
