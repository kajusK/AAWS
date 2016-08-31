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
	weather.valid = ~0;

	while (1) {
		data = station_read(fd);

		if (data.val.wind_speed > weather.data.wind_gusts) {
			weather.data.wind_gusts_dir = data.val.wind_dir;
			weather.data.wind_gusts = data.val.wind_speed;
		}
		//average data
		weather.data.wind_speed += data.val.wind_speed;
		weather.data.wind_dir += data.val.wind_dir;
		weather.data.humidity += data.val.humidity;
		weather.data.temp += data.val.temp;
		weather.data.pressure += data.val.pressure;
		weather.data.uv += data.val.uv;

		if (!(data.pres & P_TEMP) || (data.err & E_TEMP))
			weather.valid &= ~(V_TEMP | V_DEW_POINT);
		if (!(data.pres & P_PRESSURE) || (data.err & E_PRESSURE))
			weather.valid &= ~V_PRESSURE;
		if (!(data.pres & P_HUMIDITY) || (data.err & E_HUMIDITY))
			weather.valid &= ~(V_HUMIDITY | V_DEW_POINT);
		if (!(data.pres & P_UV) || (data.err & E_UV))
			weather.valid &= ~V_UV;
		if (!(data.pres & P_WIND_DIR) || (data.err & E_WIND_DIR))
			weather.valid &= ~(V_WIND_DIR | V_WIND_GUSTS_DIR);
		if (!(data.pres & P_WIND_SPEED) || (data.err & E_WIND_SPEED))
			weather.valid &= ~(V_WIND_SPEED | V_WIND_GUSTS | V_WIND_GUSTS_DIR);
		if (!(data.pres & P_RAIN) || (data.err & E_RAIN))
			weather.valid &= ~(V_RAIN & V_RAIN_1H);

		sleep(SAMPLE_PERIOD);
		cycles++;
		hour_counter++;

		//save period elapsed
		if (cycles >= conf->save_period/SAMPLE_PERIOD) {
			//get average
			weather.data.wind_speed = weather.data.wind_speed / cycles;
			weather.data.wind_dir = weather.data.wind_dir / cycles;
			weather.data.humidity = weather.data.humidity / cycles;
			weather.data.temp = weather.data.temp / cycles;
			weather.data.uv = weather.data.uv / cycles;

			weather.data.pressure = weather.data.pressure / cycles;
			weather.data.pressure = press_relative(weather.data.pressure, conf->station.elevation);
			weather.data.dew_point = dew_point(weather.data.humidity, weather.data.temp);
			weather.data.rain_1h = rain_1h;

			//get rain intensity in last SAVE_PERIOD
			weather.data.rain = (data.val.rain - rain_prev)*3600.0/conf->save_period;

			backends_send(&weather, conf);

			//clear stuff
			memset(&weather, 0, sizeof(weather));
			weather.valid = ~0;
			rain_prev = data.val.rain;

			cycles = 0;
		}

		//one hour elapsed
		if (hour_counter >= 3600/SAMPLE_PERIOD){
			hour_counter = 0;
			rain_1h = data.val.rain;
			backends_add_rain(data.val.rain);

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

	printf("Checking station status\n");
	while (!station_alive(serial_fd)) {
		fprintf(stderr, "Station is not responding, "
			"trying again in %d seconds...\n", RETRY_INTERVAL);
		sleep(RETRY_INTERVAL);
	}

	printf("Entering main loop\n");
	if (daemon)
		daemonize(conf->pid_file, conf->log_file);

	loop(serial_fd, conf);

	return 0;
}
