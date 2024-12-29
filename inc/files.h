#ifndef FILES_H
#define FILES_H

#define long long ll

struct stat get_stat(const char *path);
bool is_file(const char *path);
void create_path(char *fullpath, const char *src, const char *fname);
void get_fpermissions(const char *path, char *str);

#endif // FILES_H

