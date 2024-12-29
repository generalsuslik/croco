#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../inc/files.h"

struct stat get_stat(const char *path) 
{
	struct stat file_stat;
	if (stat(path, &file_stat) == -1) {
		exit(EXIT_FAILURE);
	}
	return file_stat;
}

bool is_file(const char *path)
{
	struct stat path_stat = get_stat(path);
	if (S_ISREG(path_stat.st_mode)) {
		return true;	
	}
	return false;
}

/*
 *  path like: 
 * fullpath = src + fname
 */
void create_path(char *fullpath, const char *src, const char *fname)
{
	strcpy(fullpath, src);
	strcat(fullpath, fname);
}

/*
 * adds to str file permissions 
 * like
 * rwxr-x--x 
 * BUT strlen(str) MUST BE AT LEAST 9    !!!!
 * (the best option is strlen(str) == 9) !!!!
 */
void get_fpermissions(const char *path, char *str)
{
	struct stat fstat = get_stat(path);
	mode_t mode		  = fstat.st_mode;

	str[0] = (mode & S_IRUSR) ? 'r' : '-';
	str[1] = (mode & S_IWUSR) ? 'w' : '-';
	str[2] = (mode & S_IXUSR) ? 'x' : '-';
	
	str[3] = (mode & S_IRGRP) ? 'r' : '-';
	str[4] = (mode & S_IWGRP) ? 'w' : '-';
	str[5] = (mode & S_IXGRP) ? 'x' : '-';

	str[6] = (mode & S_IROTH) ? 'r' : '-';
	str[7] = (mode & S_IWOTH) ? 'w' : '-';
	str[8] = (mode & S_IXOTH) ? 'x' : '-';
}

