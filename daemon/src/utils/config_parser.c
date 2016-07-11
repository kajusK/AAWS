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

static void strtolow(char *str)
{
	for (int i = 0; i < strlen(str); i++)
		str[i] = tolower(str[i]);
}

static int tobool(char *str)
{
	strtolow(str);

	if (strcmp("true", str) == 0 || strcmp("yes", str) == 0)
		return 1;
	if (strcmp("false", str) == 0 || strcmp("no", str) == 0)
		return 0;

	if (strlen(str) == 1) {
		if (str[0] == '0')
			return 0;
		if (isdigit(str[0]))
			return 1;
	}

	return -1;
}

static char *remove_quote(char *str)
{
	char *end = str + strlen(str) - 1;

	if (str[0] == '"' || str[0] == '\'')
		str++;

	if (*end == '"' || *end == '\'')
		*end = '\0';

	return str;
}

static int config_assign(struct s_config_parse *conf, char *value, char *name,
			 int line)
{
	int boolean;

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
		value = remove_quote(value);
		strcpy((char *) conf->value, value);
		break;
	case C_BOOL:
		boolean = tobool(value);
		if (boolean == -1) {
			fprintf(stderr, "Option '%s' must be either 'true' "
				"or 'false', line %d\n", name, line);
			return -1;
		}
		*((int *) conf->value) = boolean;

		break;
	}

	return 0;
}

static int conf_get_cat(char *name, struct s_config_cat **cur_cat,
			struct s_config_cat cat[], int count)
{
	char *end = name + strlen(name) - 1;
	name++;
	*end = '\0';

	for (int i = 0; i < count; i++) {
		if (strcmp(cat[i].name, name) == 0) {
			*cur_cat = &cat[i];
			*end = ']';
			return 0;
		}
	}

	*end = ']';

	return -1;
}

int config_parse_internal(const char *filename, struct s_config_cat cat[],
			  int count, int use_cat)
{
	char buf[256];
	char *saveptr, *name, *value;
	int line = 0, assigned, ret = 0;
	FILE *f;
	struct s_config_cat *cur_cat = NULL;

	if (count == 1 || !use_cat)
		cur_cat = &cat[0];

	f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr, "Unable to open config file %s\n", filename);
		return -2;
	}

	while (fgets(buf, sizeof(buf), f) != NULL) {
		line++;
		if (buf[0] == '\n' || buf[0] == '#')
			continue;

		/* Parse and clean up item name */
		name = strtok_r(buf, "=", &saveptr);
		if (name == NULL)
			continue;
		name = str_trim(name);
		strtolow(name);
		if (use_cat && name[0] == '[' && name[strlen(name)-1] == ']') {
			if (conf_get_cat(name, &cur_cat, cat, count) != 0) {
				fprintf(stderr, "Incorrect category '%s'"
						", line %d\n", name, line);
				return -1;
			}
			//TODO check rest of the line
			continue;
		}
		if (cur_cat == NULL) {
			fprintf(stderr, "You must specify category first"
					"! Line %d\n", line);
			return -1;
		}

		/* Parse and clean up item value */
		value = strtok_r(NULL, "=", &saveptr);
		if (value == NULL) {
			fprintf(stderr, "Error parsing config '%s', line %d\n",
				name, line);
			ret = -1;
			continue;
		}
		value = str_trim(value);
		if (strlen(value) == 0) {
			fprintf(stderr, "Empty config option '%s', line %d\n",
				name, line);
			ret = -1;
			continue;
		}

		/* try to match name with known config options */
		assigned = 0;
		for (int i = 0; i < cur_cat->count; i++) {
			if (strcmp(name, cur_cat->items[i].name) != 0)
				continue;

			if (config_assign(&cur_cat->items[i], value, name, line) != 0)
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

int config_parse(const char *filename, struct s_config_parse conf[], int count)
{
	struct s_config_cat cat[] = {{"", count, conf}};
	return config_parse_internal(filename, cat, 1, 0);
}

int config_parse_cat(const char *filename, struct s_config_cat conf[], int count)
{
	return config_parse_internal(filename, conf, count, 1);
}
