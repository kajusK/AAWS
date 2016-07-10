/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "http.h"

#define LINE_MAX_LEN 100

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

static int socket_write(int fd, const char *data)
{
	int bytes;
	int len, sent;
	len = strlen(data);
	sent = 0;

	do {
		bytes = send(fd, data+sent, len - sent, 0);
		if (bytes == -1) {
			perror("Unable to send data to socket");
			return -1;
		}
		if (bytes == 0)
			break;
		sent += bytes;
	} while (sent != len);

	return 0;
}

static void separate_host(char *url, char *host)
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

/*
 * Get content-length from buffer or -1;
 */
static int header_get_length(char *buffer)
{
	char *line, *end = buffer;
	int cont_len = -1;

	while (end != NULL) {
		line = end;
		end = strchr(line, '\n');
		if (end != NULL) {
			*end = '\0';
			end += 1;
		}

		if (sscanf(line, "Content-Length: %d", &cont_len) <= 0)
			sscanf(line, "content-length: %d", &cont_len);
	}

	return cont_len;
}

/*
 * If read_response is 0, only status code is parsed, rest is ignored
 * response must be free() after used
 *
 * Return status code (200, 404,...) or -1;
 */
static int http_read_reply(int fd, char **response, int read_response)
{
	char buffer[LINE_MAX_LEN + 1], *end;
	int bytes, len, cont_len, status_code = -1;
	int received = 0, header_end = 0;

	if ((bytes = recv(fd, buffer, LINE_MAX_LEN, 0)) == -1) {
		perror("Unable to read response from socket");
		return -1;
	}
	buffer[bytes] = '\0';
	sscanf(buffer, "HTTP/%*f %d", &status_code);
	if (!read_response) {
		response = NULL;
		return status_code;
	}

	//read line by line until end of header
	while (bytes != 0) {
		end = strstr(buffer, "\r\n\r\n");
		if (end == NULL)
			end = strrchr(buffer, '\n');
		else
			header_end = 1;

		if (*end != '\0')
			*end++ = '\0';

		cont_len = header_get_length(buffer);

		len = strlen(end);
		memcpy(buffer, end, len);
		if (header_end)
			break;

		bytes = recv(fd, buffer+len, LINE_MAX_LEN-len, 0);
		if (bytes == -1) {
			perror("Unable to read response from socket");
			return -1;
		}
		buffer[bytes+len] = '\0';
	}

	if (bytes == 0 || cont_len <= 0) {
		response = NULL;
		return 0;
	}

	//read message content
	*response = (char *) malloc(cont_len+1);
	if (*response == NULL) {
		fprintf(stderr, "Unable to allocate memory\n");
		return -1;
	}

	if (len > 3) {
		buffer[len] = '\0';
		strcpy(*response, buffer+3);
		received = len - 3;
	}

	do {
		bytes = recv(fd, *response+received, cont_len - received, 0);
		if (bytes == -1) {
			perror("Unable to read data from socket");
			free(*response);
			return -1;
		}
		if (bytes == 0)
			break;

		received += bytes;
	} while (received != cont_len);
	(*response)[received] = '\0';

	return status_code;
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
 * Get Auth header for basic html auth (result must be freed after used)
 */
char *http_gen_auth_header(const char *user, const char *pass)
{
	char *base64, *str, *header, *p;

	int data_len =  strlen(user) + strlen(pass) + 2;
	int b64_len = (data_len/3+1)*4 + 1;
	int header_len = b64_len + strlen("Authorization: Basic \r\n") + 1;

	p = (char *) malloc(data_len + b64_len + header_len);
	if (p == NULL) {
		fprintf(stderr, "Unable to allocate memory\n");
		return NULL;
	}
	header = p;
	base64 = p + header_len;
	str = base64 + b64_len;

	sprintf(str, "%s:%s", user, pass);
	base64_encode(str, strlen(str), base64);

	sprintf(header, "Authorization: Basic %s\r\n", base64);
	return header;
}

/*
 * Send http request and store reply in response (free() after use)
 * if read_response == 0, response is ignored
 *
 * HTML response code is returned
 */
int http_request(char *url, enum req_type type, char *payload, char *headers,
	         char **response, int read_response)
{
	int fd, len;
	char *req, host[100];

	separate_host(url, host);
	len = strlen(url) + strlen(host) + strlen(headers) + 1;
	len += type == GET ? 17+10+25 : 18+10+51+20+25+strlen(payload);

	req = (char *) malloc(len);
	if (req == NULL) {
		fprintf(stderr, "Unable to allocate memory\n");
		return -1;
	}

	if (type == GET) {
		sprintf(req, "GET %s HTTP/1.1\r\n"
			     "Host: %s\r\n%s"
			     "Connection: close\r\n\r\n", url, host, headers);
	} else {
		sprintf(req, "POST %s HTTP/1.1\r\n"
			     "Host: %s\r\n%s"
			     "Content-Type: application/x-www-form-urlencoded\r\n"
			     "Content-Length: %zu\r\n"
			     "Connection: close\r\n\r\n"
			     "%s", url, host, headers, strlen(payload), payload);
	}

	fd = socket_open(host, "http");
	if (fd == -1) {
		free(req);
		return -1;
	}

	if (socket_write(fd, req) == -1) {
		free(req);
		return -1;
	}
	free(req);

	return http_read_reply(fd, response, read_response);
}
