#include "../inc/commands.h"

/*
 * commands:
 *    a - adds childnode:
 *		:a file.c
 *		:a folder/
 *	  
 *	  r - remove childnode
 */
void process_command(const char *wd, const char *buffer)
{
	char copy_wd[1024];// = strdup(wd);
	strcpy(copy_wd, wd);

	size_t command_len = 0; // length of **command array
	char **command     = parse_command(buffer, &command_len);
	char *operation    = command[0];

	if (strcmp(operation, "a") == 0) {
		char *node_name = command[1];
		char *path = strcat(copy_wd, node_name);
		size_t n = strlen(path);		
		if (path[n - 1] == '/') {
			// create folder
			create_dir(path);
		} else {
			// create file
			create_file(path);
		}
	} else if (strcmp(operation, "r") == 0) {
		char *node_name = command[1];
		char *path = strcat(copy_wd, node_name);
		size_t n = strlen(path);
		if (path[n - 1] == '/') {
			remove_dir(path);
		} else {
			int err = remove_file(path);
			if (err) {
				exit(EXIT_FAILURE);
			}
		}
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
			printf("Removed a file: \t%s\n", full_path);
		} else {
			printf("Can't remove a file: \t%s\n", full_path);
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

char **parse_command(const char *buffer, size_t *count)
{
	char buffer_copy[1024];// = strdup(buffer);
	strcpy(buffer_copy, buffer);

	size_t capacity = 0;
	size_t size     = 0;
	char **res      = malloc(capacity * sizeof(char *));
	if (res == NULL) {
		return NULL;
	}
	
	char *token = strtok(buffer_copy, " ");
	while (token != NULL) {
		//res[size] = strdup(token);
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



