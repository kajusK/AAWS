/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __HTTP_H_
#define __HTTP_H_

enum req_type {
	GET,
	POST
};

/*
 * Get Auth header for basic html auth (result must be freed after used)
 */
extern char *http_gen_auth_header(const char *user, const char *pass);

/*
 * Send http request and store reply in response (free() after use)
 *
 * Payload is ignored when type is GET, when POST, payload is send as content
 * Headers must be terminated by \r\n or must be empty string ""
 * if read_response == 0, response is ignored
 *
 * HTML response code is returned
 */
extern int http_request(char *url, enum req_type type, char *payload,
			char *headers, char **response, int read_response);

#endif
