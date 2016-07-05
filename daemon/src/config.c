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
static void str_trim(char *str)
{
	char *end;

	while (isspace(*str))
		str++;

	end = str + strlen(str) - 1;
	while (isspace(*end))
		end--;

	*(end+1) = '\0';
}

int config_parse(struct s_config *config, char *filename)
{
	char buf[256];
	char *saveptr, *c, *name, *value;
	FILE *f;

	f = fopen(filename, "r");
	if (f == NULL)
		return -1;

	while ((c = fgets(buf, sizeof(buf), f)) != NULL) {
		if (c[0] == '\n' || c[0] == '#')
			continue;

		name = strtok_r(buf, "=", &saveptr);
		if (name == NULL)
			continue;
		value = strtok_r(buf, "=", &saveptr);
		if (value == NULL) {
			fprintf(stderr, "Empty config option '%s':", name);
			continue;
		}

		str_trim(name);
		str_trim(value);
	}

	fclose(f);
	return 0;
}

