#ifndef COMMANDS_H
#define COMMANDS_H

#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

void process_command(const char *wd, const char *buffer);

void create_dir(const char *path);
void remove_dir(const char *path);

void create_file(const char *path);
int  remove_file(const char *path);

char **parse_command(const char *buffer, size_t *count);

#endif // COMMANDS_H

