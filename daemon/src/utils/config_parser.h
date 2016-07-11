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
	char *name;
	void *value;
	c_type type;
};

struct s_config_cat {
	char *name;
	int count;
	struct s_config_parse *items;
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


/*
 * Parse config file into structure
 *
 * Format:
 *	name = value
 *
 * Example (use lowercase names only):
 *	struct s_congig_parse cat1[] = {
 *		{"int", (void *) &i, C_INT},
 *		{"bool", (void *) &boolean, C_BOOL},
 *	};
 *	struct s_congig_parse cat2[] = {
 *		{"text", (void *) str, C_STRING}
 *	};
 *	struct s_config__cat conf[3] = {
 *		{"cat_1", 2, cat1},
 *		{"cat_2", 1, cat1}};
 *	config_parse("config.txt", &conf, 4);
 *
 * config.txt:
 *	#category must be first option in config file
 *	[cat_1]
 *	int = 55
 *	bool = yes
 *	[cat_2]
 *	text = goodbye world
 */
extern int config_parse_cat(const char *filename, struct s_config_cat cat[],
			int count);

#endif
