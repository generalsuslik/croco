/*
 * Functions for file processing 
 * and getting it's info
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

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
 * fulong longpath = src + fname
 */
void create_path(char *fullpath, const char *src, const char *fname)
{
	strcpy(fullpath, src);
	strcat(fullpath, fname);
}

/*
 * builds file info 
 * <permissions>|<size>|last_modified
 */
char *build_info(const char *path)
{
	size_t info_size = 64;
	char *info = (char *)malloc(info_size * sizeof(char));
	memset(info, '\0', info_size);
	
	// ---------PERMISSIONS-----------------//
	char permissions[10] = { '\0' };
	get_fpermissions(path, permissions);

	strcat(info, permissions);
	strcat(info, "|");

	// -----------LAST MODIFIED TIME--------//
	time_t last_modified = get_last_modified(path);
	char time_str[20];
	struct tm *time = localtime(&last_modified);
	strftime(time_str, sizeof(time_str), "%H:%M:%S %d-%m-%Y", time);

	strcat(info, time_str);
	strcat(info, "|");
	
	// -------------SIZE-----------------//
	const double KB = 1024.0;
	const double MB = KB * 1024.0;
	const double GB = MB * 1024.0;

	long long size = get_size(path);
	size_t size_len = 9;
	char size_str[size_len];
	memset(size_str, ' ', size_len);
	size_str[size_len] = '\0';

	size_t added_len = 0;
	if (size < KB) {
		sprintf(size_str, "%lld", size);
		size_str[size_len - 1] = 'B';
	} else if (size < MB) {
		double kilobytes = size / KB;
		sprintf(size_str, "%.2f", kilobytes);
		size_str[size_len - 1] = 'B';
		size_str[size_len - 2] = 'k';
	} else if (size < GB) {
		double megabytes = size / MB;
		sprintf(size_str, "%.2f", megabytes);
		size_str[size_len - 1] = 'B';
		size_str[size_len - 2] = 'M';
	} else {
		double gigabytes = size / GB;
		sprintf(size_str, "%.2f", gigabytes);
		size_str[size_len - 1] = 'B';
		size_str[size_len - 2] = 'G';
	}
	added_len = strlen(size_str);
	size_str[added_len] = ' ';
	strcat(info, size_str);

	return info;
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
	struct stat file_info = get_stat(path);
	mode_t mode		      = file_info.st_mode;

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

long long get_size(const char *path)
{
	struct stat file_info = get_stat(path);
	return (long long)file_info.st_size;
}

time_t get_last_modified(const char *path)
{
	struct stat file_info = get_stat(path);
	return file_info.st_mtime;
}
