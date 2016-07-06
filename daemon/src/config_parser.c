/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "config_parser.h"

/* trim whitespaces on beginnign and end of a string */
static char *str_trim(char *str)
{
	char *end;

	while (isspace(*str))
		str++;

	end = str + strlen(str) - 1;
	while (isspace(*end))
		end--;

	*(end+1) = '\0';
	return str;
}

static int is_int(char *str)
{
	int i = 0;
	if (str[0] == '+' || str[0] == '-')
		i = 1;

	for (; i < strlen(str); i++) {
		if (!isdigit(str[i]))
			return 0;
	}

	return 1;
}

static int is_float(char *str)
{
	int dots = 0;
	int i = 0;
	if (str[0] == '+' || str[0] == '-')
		i = 1;

	for (; i < strlen(str); i++) {
		if (str[i] == '.' || str[i] == ',') {
			dots++;
			continue;
		}

		if (!isdigit(str[i]))
			return 0;
	}

	if (dots > 1)
		return 0;

	return 1;
}

static int config_assign(struct s_config_parse *conf, char *value, char *name,
			 int line)
{
	switch (conf->type) {
	case C_INT:
		if (!is_int(value)) {
			fprintf(stderr, "Option '%s' must be integer, line %d\n",
				name, line);
			return -1;
		}
		*((int *) conf->value) = strtod(value, NULL);
		break;
	case C_FLOAT:
		if (!is_float(value)) {
			fprintf(stderr, "Option '%s' must be real number, line %d\n",
				name, line);
			return -1;
		}
		*((float *) conf->value) = strtof(value, NULL);
		break;
	case C_STRING:
		strcpy((char *) conf->value, value);
		break;
	}

	return 0;
}

/*
 * Parse config file into structure
 *
 * Example:
 *	struct s_config_parse conf[3] = {{"int", (void *) &number, C_INT},
 *			{"float", (void *) &f, C_FLOAT},
 *			{"text", (void *) string, C_STRING}};
 *	config_parse("config.txt", &conf, 3);
 */
int config_parse(const char *filename, struct s_config_parse conf[], int count)
{
	char buf[256];
	char *saveptr, *name, *value;
	int line = 0, assigned, ret = 0;
	FILE *f;

	f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr, "Unable to open config file %s", filename);
		return -2;
	}

	while (fgets(buf, sizeof(buf), f) != NULL) {
		line++;
		if (buf[0] == '\n' || buf[0] == '#')
			continue;

		name = strtok_r(buf, "=", &saveptr);
		if (name == NULL)
			continue;
		value = strtok_r(NULL, "=", &saveptr);
		if (value == NULL) {
			fprintf(stderr, "Error parsing config '%s', line %d\n",
				str_trim(name), line);
			ret = -1;
			continue;
		}

		name = str_trim(name);
		value = str_trim(value);
		if (strlen(value) == 0) {
			fprintf(stderr, "Empty config option '%s', line %d\n",
				name, line);
			ret = -1;
			continue;
		}

		assigned = 0;
		for (int i = 0; i < count; i++) {
			if (strcmp(name, conf[i].name) != 0)
				continue;

			if (config_assign(&conf[i], value, name, line) != 0)
				ret = -1;
			assigned = 1;
		}

		if (!assigned) {
			fprintf(stderr, "Unknown config option '%s', line %d\n",
				name, line);
			ret = -1;
		}
	}

	fclose(f);
	return ret;
}

