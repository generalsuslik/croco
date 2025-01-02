#ifndef FILES_H
#define FILES_H

struct stat get_stat(const char *path);
bool is_file(const char *path);
char *build_info(const char *path);
void create_path(char *fullpath, const char *src, const char *fname);
void get_fpermissions(const char *path, char *str);
long long get_size(const char *path);
time_t get_last_modified(const char *path);

#endif // FILES_H

