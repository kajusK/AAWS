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
	C_BOOL,
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
 * Format:
 *	name = value
 *
 * Example (use lowercase names only):
 *	struct s_config_parse conf[4] = {{"int", (void *) &number, C_INT},
 *			{"float", (void *) &f, C_FLOAT},
 *			{"bool", (void *) &boolean, C_BOOL},
 *			{"text", (void *) string, C_STRING}};
 *	config_parse("config.txt", &conf, 4);
 *
 * config.txt:
 *	#comment
 *	int = 55
 *	float = -5.25
 *	bool = true
 *	text = hello world
 *	text2 = "hello world 2"
 */
extern int config_parse(const char *filename, struct s_config_parse conf[],
			int count);

#endif
