/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "serial.h"
#include "station.h"
#include "db.h"

#define DEFAULT_DB_FILE "weather.db"
#define DEFAULT_SERIAL "/dev/ttyAMA0"
#define DEFAULT_PID_FILE "/var/run/weatherd.pid"
#define DEFAULT_LOG "/var/log/weatherd.log"
#define DEFAULT_BAUDR 9600
#define SAMPLE_PERIOD 5

/* save data to db every 5 minutes */
#define SAVE_PERIOD 5*60

static int serial_fd = -1;

static void sig_handler(int signum)
{
	fprintf(stderr, "SIGINT received, exiting...\n");
	serial_close(serial_fd);
	db_close();

	exit(0);
}

static int daemonize(char *pid_file, char *log)
{
	FILE *f;
	pid_t pid, sid;

	printf("Will become a daemon...\n");

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Unable to fork, running anyway...\n");
		return -1;
	}

	if (pid > 0) {
		f = fopen(pid_file, "w");
		if (f == NULL) {
			fprintf(stderr, "Unable to create pid file\n");
			exit(1);
		}
		fprintf(f, "%d", pid);
		fclose(f);
		exit(0);
	}

	/* child code */
	umask(0);
	sid = setsid();
	if (sid < 0)
		fprintf(stderr, "Warning: unable to create new session\n");

	if (chdir("/var") < 0)
		fprintf(stderr, "Warning: unable to change directory\n");

	if (log != 0) {
		if (freopen(log, "a", stdout) == NULL)
			fprintf(stderr, "Warning: unable to redirect stdout\n");
		if (freopen(log, "a", stderr) == NULL)
			fprintf(stderr, "Warning: unable to redirect stderr\n");
	}

	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	close(STDIN_FILENO);

	return 0;
}

static void usage(const char *name)
{
	fprintf(stderr,
			"Usage: %s [-s serial] [options]\n\n"
			"-h\t\t\tShow this help and exit\n"
			"-s serial\t\tSerial interface to read data from\n"
			"-o database\t\tDatabase file to store data to\n"
			"-b baudrate\t\tBaudrate for serial\n"
			"-d\t\t\tRun as a daemon\n"
			"\t-p filename\tPid filename\n"
			"\t-l filename\tFile to store logs to\n", name);
}

static void loop(int fd)
{
	int cycles = 0;
	int hour_counter = 0;
	struct s_message data;
	struct s_message avg;
	float wind_max = 0;
	float rain_prev = 0;

	memset(&avg, 0, sizeof(avg));

	while (1) {
		data = station_read(fd);

		wind_max = data.wind_speed > wind_max ? data.wind_speed : wind_max;
		//average data
		avg.wind_speed += data.wind_speed;
		avg.wind_dir += data.wind_dir;
		avg.humidity += data.humidity;
		avg.temp += data.temp;
		avg.pressure += data.pressure;

		sleep(SAMPLE_PERIOD);
		cycles++;
		hour_counter++;

		//save period elapsed
		if (cycles >= SAVE_PERIOD/SAMPLE_PERIOD) {
			cycles = 0;

			//get average
			avg.wind_speed = avg.wind_speed / cycles;
			avg.wind_dir = avg.wind_dir / cycles;
			avg.humidity = avg.humidity / cycles;
			avg.temp = avg.temp / cycles;
			avg.pressure = avg.pressure / cycles;
			//get rain intensity in last SAVE_PERIOD
			avg.rain = (data.rain - rain_prev)*3600.0/SAVE_PERIOD;

			db_add_weather(&avg, wind_max);

			//clear stuff
			wind_max = 0;
			memset(&avg, 0, sizeof(avg));
			rain_prev = data.rain;
		}

		//one hour elapsed
		if (hour_counter >= 3600/SAMPLE_PERIOD){
			hour_counter = 0;
			db_add_rain(data.rain);

			station_rain_reset(fd);
			rain_prev = 0;
		}
	}
}

int main(int argc, char **argv)
{
	char *serial = DEFAULT_SERIAL;
	char *db_file = DEFAULT_DB_FILE;
	char *log = DEFAULT_LOG;
	char *pid_file = DEFAULT_PID_FILE;
	int baudr = DEFAULT_BAUDR;
	int daemon = 0;

	int opt;

	while((opt = getopt(argc, argv, "hds:o:p:l:b:")) != -1) {
		switch (opt) {
		case 'h':
			usage(argv[0]);
			exit(0);
			break;
		case 'd':
			daemon = 1;
			break;
		case 'b':
			baudr = atoi(optarg);
			break;
		case 's':
			serial = optarg;
			break;
		case 'o':
			db_file = optarg;
			break;
		case 'p':
			pid_file = optarg;
			break;
		case 'l':
			log = optarg;
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}

	serial_fd = serial_open(serial, baudr);
	db_init(db_file);

	signal(SIGINT, sig_handler);

	if (daemon)
		daemonize(pid_file, log);

	loop(serial_fd);

	return 0;
}
