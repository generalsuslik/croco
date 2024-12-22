#include "../inc/commands.h"

/*
 * commands:
 *    a - adds childnode:
 *		:a file.c
 *		:a folder/
 *	  
 *	  r - remove childnode:
 *		:r file.jpg
 *		:r folder/
 *
 */
void process_command(const char *wd, const char *buffer)
{
	char copy_wd[1024];
	strcpy(copy_wd, wd);

	size_t command_len    = 0; // length of **command array
	char **command        = parse_command(buffer, &command_len);
	operation_t operation = get_operation(buffer);  	
	char *node_name;
	char *path;
	size_t n;
	switch (operation) {
		case ADD: // add file/folder
			node_name = command[1];
			path = strcat(copy_wd, node_name);
			n = strlen(path);		
			if (path[n - 1] == '/') {
				// create folder
				create_dir(path);
			} else {
				// create file
				create_file(path);
			}
			break;

		case REMOVE: // remove file/folder
			node_name = command[1];
			path = strcat(copy_wd, node_name);
			n = strlen(path);
			if (path[n - 1] == '/') {
				remove_dir(path);
			} else {
				int err = remove_file(path);
				if (err) {
					exit(EXIT_FAILURE);
				}
			}
			break;

		case FIND:
			break;

		case UNRECOGNIZED:
			break;

		default:
			break;
	}
	
	for (size_t i = 0; i < command_len; ++i) {
		free(command[i]);
	}
	free(command);
}

void create_dir(const char *path)
{
	struct stat st = {0};

	if (stat(path, &st) == -1) {
		mkdir(path, 0777);
	}	
}

void remove_dir(const char *path)
{
	size_t path_len;
	char  *full_path;

	DIR *dir;
	struct stat stat_path, stat_entry;
	struct dirent *entry;

	stat(path, &stat_path);

	if (S_ISDIR(stat_path.st_mode) == 0) {
		fprintf(stderr, "No such directory: %s\n", path);
		exit(EXIT_FAILURE);
	}	

	if ((dir = opendir(path)) == NULL) {
		fprintf(stderr, "Could not open a directory: %s\n", path);
	}	

	path_len = strlen(path);

	while ((entry = readdir(dir)) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
			continue;
		}

		full_path = calloc(path_len + 1 + strlen(entry->d_name) + 1, sizeof(char));
		strcpy(full_path, path);
		if (full_path[strlen(full_path) - 1] != '/') {
			strcat(full_path, "/");
		}
		strcat(full_path, entry->d_name);

		stat(full_path, &stat_entry);

		if (S_ISDIR(stat_entry.st_mode) != 0) {
			remove_dir(full_path);
			free(full_path);
			continue;
		}

		if (unlink(full_path) == 0) {
			printf("Removed file: \t%s\n", full_path);
		} else {
			printf("Could not remove file: \t%s\n", full_path);
		}
		free(full_path);
	}

	if (rmdir(path) == 0) {
		printf("Removed: %s\n", path);
	} else {
		printf("Could not remove: %s\n", path);
	}

	closedir(dir);
}

void create_file(const char *path)
{
	FILE *fptr = fopen(path, "w");
	if (fptr == NULL) {
		printf("Could not create file %s\n", path);
		exit(EXIT_FAILURE);
	}
	fclose(fptr);
}

int remove_file(const char *path) 
{
	if (remove(path) == 0) {
		return 0;
	}
	return 1;
}

size_t search(size_t *res, char **arr, const size_t arr_len, const char *pattern)
{
	const char *str;
	size_t res_len = 0;
	for (size_t i = 0; i < arr_len; ++i) {
		str = arr[i];
		if (strncmp(str, pattern, min_len(str, pattern)) == 0) {
			res[res_len++] = i;
		}
	}
	return res_len;
}

/*
 * @return min(len(str1), len(str2))
 */ 
size_t min_len(const char *str1, const char *str2)
{
	size_t n = strlen(str1);
	size_t m = strlen(str2);
	return n > m ? m : n;
}

/*
 * @param buffer[]   = not parsed command string. like: "a folder/"
 *
 * @return operation = operations type enum 
 */
operation_t get_operation(const char *buffer)
{
	size_t command_len = 0;
	char **command     = parse_command(buffer, &command_len);
	char *op_str       = command[0]; // operation string
	if (strlen(op_str) != 1) {
		return UNRECOGNIZED;
	}

	char op_char = op_str[0]; // ["a", "file.c"] -> "a"[0] -> 'a'
	
	for (size_t i = 0; i < command_len; ++i) {
		free(command[i]);
	}
	free(command);

	switch (op_char) {
		case 'a':
			return ADD;
		
		case 'r':
			return REMOVE;

		case 'f':
			return FIND;

		default:
			break;
	}
	
	return UNRECOGNIZED;
}

/*
 * @param buffer[] = command string to split
 * @param count = len(command) (will be returned as a parameter)
 * 
 * @return splitted command (command.split())
 */ 
char **parse_command(const char *buffer, size_t *count)
{
	char buffer_copy[1024];
	strcpy(buffer_copy, buffer);

	size_t capacity = 0;
	size_t size     = 0;
	char **res      = malloc(capacity * sizeof(char *));
	if (res == NULL) {
		return NULL;
	}
	
	char *token = strtok(buffer_copy, " ");
	while (token != NULL) {
		res[size] = (char *)malloc(strlen(token) * sizeof(char));
		strcpy(res[size], token);
		if (res[size] == NULL) {
			for (size_t i = 0; i < size; ++i) {
				free(res[i]);
			}
			free(res);
			return NULL;
		}
		++size;
		token = strtok(NULL, " ");
	}

	*count = size;
	res = realloc(res, size * sizeof(char *));
	return res;
}



