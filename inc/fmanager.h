#ifndef FMANAGER_H
#define FMANAGER_H

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../inc/commands.h"
#include "../inc/keys.h"


#define WIDTH getmaxx(stdscr)
#define HEIGHT getmaxy(stdscr)

#define WIDTH getmaxx(stdscr)
#define HEIGHT getmaxy(stdscr)

#define MARGIN_TOP    6
#define MARGIN_BOTTOM 2

#define CONTROL_HEIGHT 1
#define CONTROL_WIDTH  WIDTH
#define CONTROL_MARGIN 1
#define ADDITION       WIDTH / 25
#define MAIN_HEIGHT    HEIGHT - MARGIN_BOTTOM - CONTROL_HEIGHT
#define MAIN_WIDTH     WIDTH / 3 + ADDITION 
#define LEFT_HEIGHT    HEIGHT - MARGIN_BOTTOM - CONTROL_HEIGHT
#define LEFT_WIDTH     WIDTH / 3 - 2 * ADDITION
#define RIGHT_HEIGHT   HEIGHT - MARGIN_BOTTOM - CONTROL_HEIGHT
#define RIGHT_WIDTH    WIDTH / 3 + ADDITION

/*
 * max amount of dirs in cwd
 * max len of folder path
 * max len of folder/file title
 */
#define DIRS_MAX  320000
#define PATH_MAX  16000
#define TITLE_MAX 1000

#define COMMAND_MAX 128

#define d_COLOR_CYAN   1
#define d_COLOR_GREEN  2
#define d_COLOR_BLUE   3
#define d_COLOR_RED    4
#define d_COLOR_YELLOW 5

#define BINARY_COLOR      d_COLOR_GREEN
#define FOLDER_COLOR      d_COLOR_BLUE
#define INFO_FOLDER_COLOR d_COLOR_RED
#define INFO_FILE_COLOR   d_COLOR_YELLOW

#define START_X 0
#define START_Y 1

void init();
void init_colors();
void init_wins();
void check_win_err();
void process_kb();
void process_kup();
void process_kdown();
void process_control();
void get_cwd();
void change_cwd(const char *new_dir); 
void open_wd(const char *wd, char **dirs_arr, size_t *ndirs_arr);
void upd_prev_cwd(char *wd);
void assign_ndir(char *new_dir, int choice);
bool is_file(char *src, char *fname);
void create_path(char *fullpath, char *src, char *fname);
void print_main();
void print_linfo();
void print_rinfo();
void colored_print(WINDOW *win, int y, int x, char *text, int color);
void print_file(char *fname);
void print_folder(char *fname);
void end();

#endif // FMANAGER_H

