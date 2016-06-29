/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __SERIAL_H_
#define __SERIAL_H_

/*
 * Opens at baudrate 8n1
 * Returns file descriptor or -1 on error
 */
extern int serial_open(char *device, int baudrate);

/*
 * Returns 0 on success, -1 on error
 */
extern int serial_close(int fd);

/*
 * same as fprintf
 */
extern int serial_printf(int fd, char *format, ...);

/*
 * Returns number of bytes written or -1 on error
 */
extern ssize_t serial_write(int fd, const void *buf, size_t count);

/*
 * Returns number of bytes received
 */
extern int serial_read_noblock(int fd, void *buf, size_t count);

extern int serial_read(int fd, void *buf, size_t count);

#endif
