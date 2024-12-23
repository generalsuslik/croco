#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <strings.h>

#include "../inc/util.h"

void sort_dirs(char *dirs[], const size_t len)
{
	qsort(dirs, len, sizeof(char *), compare);
}

int compare(const void *arg1, const void *arg2)
{
	const char *str1 = *(const char **)arg1;
	const char *str2 = *(const char **)arg2;
	
	bool is_dot1 = str1[0] == '.';
	bool is_dot2 = str2[0] == '.';

	if (is_dot1 != is_dot2) {
		return is_dot1 - is_dot2;
	}

	return strcasecmp(str1, str2);
}

