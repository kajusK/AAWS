/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

static int socket_open(const char *host, const char *service)
{
	int sockfd, status;
	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(host, service, &hints, &servinfo);
	if (status != 0) {
		fprintf(stderr, "Unable to get addr info for '%s': %s\n",
			host, gai_strerror(status));
		return -1;
	}

	//find first working connection
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			continue;
		}

		break;
	}
	freeaddrinfo(servinfo);

	if (p == NULL) {
		fprintf(stderr, "Unable to connect to '%s'\n", host);
		return -1;
	}

	return sockfd;
}

static int socket_rw(int fd, const char *data, char *response, int maxlen)
{
	int bytes;
	int len, sent;
	len = strlen(data);
	sent = 0;

	do {
		bytes = send(fd, data+sent, len - sent, 0);
		if (bytes == -1) {
			perror("Unable to send data to socket:");
			return -1;
		}
		if (bytes == 0)
			break;
		sent += bytes;
	} while (sent != len);

	if ((bytes = recv(fd, response, maxlen - 1, 0)) == -1) {
		perror("Unable to read response from socket\n");
		return -1;
	}
	response[bytes] = '\0';

	return 0;
}

static void separate_url(char *url, char *host)
{
	char *first, *last;

	first = strstr(url, "://");
	if (first == NULL)
		first = url;
	else
		first += 3;


	last = strstr(first, ":");
	last = last == NULL ? strstr(first, "/") : last;
	if (last == NULL)
		last = url + strlen(url);
	last--;

	strncpy(host, first, last-first+1);
	host[last - first + 1] = '\0';
}

static void base64_encode(const char *src, int len, char *dest)
{
	const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char *p = dest;
	int i;

	for (i = 0; i < len - len%3; i += 3) {
		*p++ = b64[(src[i] >> 2) & 0x3F];
		*p++ = b64[((src[i] & 0x3) << 4) | ((src[i+1] & 0xF0) >> 4)];
		*p++ = b64[((src[i+1] & 0xF) << 2) | ((src[i+2] & 0xC0) >> 6)];
		*p++ = b64[src[i+2] & 0x3F];
	}

	if (i < len) {
		*p++ = b64[(src[i] >> 2) & 0x3F];
		if (i == (len - 1)) {
			*p++ = b64[(src[i] & 0x3) << 4];
			*p++ = '=';
		} else {
			*p++ = b64[((src[i] & 0x3) << 4) | ((src[i+1] & 0xF0) >> 4)];
			*p++ = b64[(src[i+1] & 0xF) << 2];
		}
		*p++ = '=';
	}

	*p++ = '\0';
}

/*
 * Send http GET request and store reply in response
 *
 * HTML response code is returned
 */
int http_get(char *url, char *response, int maxlen)
{
	int fd, response_code = 0;
	char req[512], host[100];

	separate_url(url, host);
	sprintf(req, "GET %s HTTP/1.1\r\n"
		     "Host: %s\r\n"
		     "Connection: close\r\n\r\n", url, host);

	fd = socket_open(host, "http");
	if (fd == -1)
		return -1;

	if (socket_rw(fd, req, response, maxlen) == -1)
		return -1;

	sscanf(response, "HTTP/%*f %d", &response_code);

	return response_code;
}
