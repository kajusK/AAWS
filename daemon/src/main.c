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

#include "utils/weather.h"
#include "utils/serial.h"
#include "station.h"
#include "config.h"
#include "backends/backends.h"

#define AAWS_VERSION "0.1"
#define CONF_DEFAULT "/etc/aaws/config"
#define SAMPLE_PERIOD 5
#define RETRY_INTERVAL 10

static int serial_fd = -1;

static void sig_handler(int signum)
{
	fprintf(stderr, "SIGINT received, exiting...\n");
	serial_close(serial_fd);

	exit(0);
}

static int daemonize(char *pid_file, char *log)
{
	FILE *f;
	pid_t pid, sid;

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Unable to fork, running in foreground...\n");
		return -1;
	}

	if (pid > 0) {
		f = fopen(pid_file, "w");
		if (f == NULL) {
			serial_close(serial_fd);
			fprintf(stderr, "Unable to create pid file, ending\n");
			exit(1);
		}
		fprintf(f, "%d", pid);
		fclose(f);

		printf("%d\n", pid);
		exit(0);
	}

	/* child code */
	umask(0);
	sid = setsid();
	if (sid < 0)
		fprintf(stderr, "Warning: unable to create new session\n");

	if (chdir("/var") < 0)
		fprintf(stderr, "Warning: unable to change directory\n");

	if (log != NULL) {
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
		"AAWS - weather station recording daemon\n"
		"Usage: %s [-c filename]\n\n"
		"Options:\n"
		"  -h\t\t\tShow this help and exit\n"
		"  -v\t\t\tPrint version info and exit\n"
		"  -t\t\t\tTest configuraion and exit\n"
		"  -c filename\t\tSet config file (default "CONF_DEFAULT")\n"
		"  -f\t\t\tRun in foreground\n", name);
}

static void loop(int fd, struct s_config *conf)
{
	int cycles = 0;
	int hour_counter = 0;
	struct s_message data;
	struct s_weather weather;
	float rain_prev = 0;
	float rain_1h = 0;

	memset(&weather, 0, sizeof(weather));

	while (1) {
		data = station_read(fd);

		if (data.wind_speed > weather.wind_gusts) {
			weather.wind_gusts_dir = data.wind_dir;
			weather.wind_gusts = data.wind_speed;
		}
		//average data
		weather.wind_speed += data.wind_speed;
		weather.wind_dir += data.wind_dir;
		weather.humidity += data.humidity;
		weather.temp += data.temp;
		weather.pressure += data.pressure;

		sleep(SAMPLE_PERIOD);
		cycles++;
		hour_counter++;

		//save period elapsed
		if (cycles >= conf->save_period/SAMPLE_PERIOD) {
			cycles = 0;

			//get average
			weather.wind_speed = weather.wind_speed / cycles;
			weather.wind_dir = weather.wind_dir / cycles;
			weather.humidity = weather.humidity / cycles;
			weather.temp = weather.temp / cycles;

			weather.pressure = weather.pressure / cycles;
			weather.pressure = press_relative(weather.pressure, conf->station.elevation);
			weather.dew_point = dew_point(weather.humidity, weather.temp);
			weather.rain_1h = rain_1h;

			//get rain intensity in last SAVE_PERIOD
			weather.rain = (data.rain - rain_prev)*3600.0/conf->save_period;

			backends_send(&weather, conf);

			//clear stuff
			memset(&weather, 0, sizeof(weather));
			rain_prev = data.rain;
		}

		//one hour elapsed
		if (hour_counter >= 3600/SAMPLE_PERIOD){
			hour_counter = 0;
			rain_1h = data.rain;
			backends_add_rain(data.rain);

			station_rain_reset(fd);
			rain_prev = 0;
		}
	}
}

int main(int argc, char **argv)
{
	int opt;
	char *conf_filename = CONF_DEFAULT;
	int test_config = 0;
	int daemon = 1;
	struct s_config *conf;

	while((opt = getopt(argc, argv, "hc:tfv")) != -1) {
		switch (opt) {
		case 'h':
			usage(argv[0]);
			exit(0);
			break;
		case 'v':
			printf("AAWS version: "AAWS_VERSION"\n");
			exit(0);
			break;
		case 't':
			test_config = 1;
			break;
		case 'f':
			daemon = 0;
			break;
		case 'c':
			conf_filename = optarg;
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}

	if (conf_read(conf_filename) != 0) {
		fprintf(stderr, "Incorrect config file, ending...\n");
		exit(-1);
	}

	if (test_config) {
		printf("Config file is OK\n");
		exit(0);
	}

	conf = conf_get();

	if (backends_init(conf) != 0)
		exit(-1);
	if ((serial_fd = serial_open(conf->serial, conf->baudr)) < 0)
		exit(-1);

	signal(SIGINT, sig_handler);

	while (!station_alive(serial_fd)) {
		fprintf(stderr, "Station is not responding, "
			"trying again in %d seconds...\n", RETRY_INTERVAL);
		sleep(RETRY_INTERVAL);
	}

	if (daemon)
		daemonize(conf->pid_file, conf->log_file);

	loop(serial_fd, conf);

	return 0;
}
