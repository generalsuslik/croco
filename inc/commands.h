#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum operation {
	ADD,
	REMOVE,
	FIND,
	QUIT,
	UNRECOGNIZED,
} operation_t;

bool process_command(const char *wd, const char *buffer);

void create_dir(const char *path);
void remove_dir(const char *path);

void create_file(const char *path);
int  remove_file(const char *path);

size_t search(size_t *res, char **arr, const size_t arr_len, const char *pattern);
size_t min_len(const char *str1, const char *str2);

operation_t get_operation(const char *buffer);

char **parse_command(const char *buffer, size_t *count);

#endif // COMMANDS_H

