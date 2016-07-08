/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __HTTP_H_
#define __HTTP_H_

/*
 * Send http GET request and store reply in response
 */
extern int http_get(char *url, char *response, int maxlen);

#endif
