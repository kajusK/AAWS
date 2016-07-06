/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#ifndef __CONFIG_PARSER_H_
#define __CONFIG_PARSER_H_

typedef enum {
	C_INT,
	C_FLOAT,
	C_STRING,
} c_type;

struct s_config_parse {
	char *name ;
	void *value;
	c_type type;
};

/*
 * Parse config file into structure
 *
 * Example:
 *	struct s_config_parse conf[3] = {{"int", (void *) &number, C_INT},
 *			{"float", (void *) &f, C_FLOAT},
 *			{"text", (void *) string, C_STRING}};
 *	config_parse("config.txt", &conf, 3);
 */
extern int config_parse(const char *filename, struct s_config_parse conf[],
			int count);

#endif
