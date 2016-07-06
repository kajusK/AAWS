/*
 * Weather station
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct s_config {
	int ook;
	char foo[50];
};

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

int config_parse(struct s_config *config, const char *filename)
{
	char buf[256];
	char *saveptr, *c, *name, *value;
	int line = 0;
	FILE *f;

	f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr, "Unable to open config file %s", filename);
		return -1;
	}

	while ((c = fgets(buf, sizeof(buf), f)) != NULL) {
		line++;
		if (c[0] == '\n' || c[0] == '#')
			continue;

		name = strtok_r(buf, "=", &saveptr);
		if (name == NULL)
			continue;
		value = strtok_r(NULL, "=", &saveptr);
		if (value == NULL) {
			fprintf(stderr, "Error parsing config '%s', line %d\n",
				str_trim(name), line);
			continue;
		}

		name = str_trim(name);
		value = str_trim(value);
		if (strlen(value) == 0) {
			fprintf(stderr, "Empty config option '%s', line %d\n",
				name, line);
			continue;
		}
	}

	fclose(f);
	return 0;
}

