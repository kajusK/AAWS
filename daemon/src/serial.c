/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

#include "serial.h"

static int get_baudr(int baudr)
{
	switch (baudr) {
	case 50:
		return B50;
		break;
	case 75:
		return B75;
		break;
	case 110:
		return B110;
		break;
	case 134:
		return B134;
		break;
	case 150:
		return B150;
		break;
	case 200:
		return B200;
		break;
	case 300:
		return B300;
		break;
	case 600:
		return B600;
		break;
	case 1200:
		return B1200;
		break;
	case 1800:
		return B1800;
		break;
	case 2400:
		return B2400;
		break;
	case 4800:
		return B4800;
		break;
	case 9600:
		return B9600;
		break;
	case 19200:
		return B19200;
		break;
	case 38400:
		return B38400;
		break;
	case 57600:
		return B57600;
		break;
	case 115200:
		return B115200;
		break;
	}

	fprintf(stderr, "Invalid baudrate - %d\n", baudr);
	return -1;
}

/*
 * Opens at baudrate 8n1
 * Returns file descriptor or -1 on error
 */
int serial_open(char *device, int baudrate)
{
	int fd;
	struct termios config;
	int baudr;

	baudr = get_baudr(baudrate);
	if (baudr < 1)
		return -1;

	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		fprintf(stderr, "Unable to open %s\n", device);
		return -1;
	}
	if (!isatty(fd)) {
		fprintf(stderr, "%s is not a tty device\n", device);
		close(fd);
		return -1;
	}

	memset(&config, 0, sizeof(config));
	config.c_cflag = CS8 | CREAD | CLOCAL;

	config.c_iflag = 0;
	config.c_lflag = 0;
	config.c_oflag = 0;
	config.c_cc[VMIN] = 0;
	config.c_cc[VTIME] = 0;

	cfsetispeed(&config, baudr);
	cfsetospeed(&config, baudr);

	if (tcsetattr(fd, TCSANOW, &config) < 0) {
		fprintf(stderr, "Unable to config %s\n", device);
		close(fd);
		return -1;
	}
	fcntl(fd, F_SETFL, 0);

	return fd;
}

/*
 * Returns 0 on success, -1 on error
 */
int serial_close(int fd)
{
	return close(fd);
}

/*
 * same as fprintf
 */
int serial_printf(int fd, char *format, ...)
{
	int ret;
	va_list list;

	va_start(list, format);
	ret = vdprintf(fd, format, list);
	va_end(list);

	return ret;
}

/*
 * Returns number of bytes written or -1 on error
 */
ssize_t serial_write(int fd, const void *buf, size_t count)
{
	return write(fd, buf, count);
}

/*
 * Returns number of bytes received
 */
int serial_read_noblock(int fd, void *buf, size_t count)
{
	int ret;
	fcntl(fd, F_SETFL, FNDELAY);
	ret = read(fd, buf, count);
	fcntl(fd, F_SETFL, 0);

	return ret;
}

int serial_read(int fd, void *buf, size_t count)
{
	return read(fd, buf, count);
}
