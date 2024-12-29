#ifndef FMANAGER_H
#define FMANAGER_H

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
#define NODE_INFO_MAX 128

#define COMMAND_MAX 128

typedef enum colors {
	d_COLOR_CYAN,
	d_COLOR_GREEN,
	d_COLOR_BLUE,
	d_COLOR_RED,
	d_COLOR_YELLOW,
} colors_t;

#define BINARY_COLOR      d_COLOR_GREEN
#define FOLDER_COLOR      d_COLOR_BLUE
#define INFO_FOLDER_COLOR d_COLOR_RED
#define INFO_FILE_COLOR   d_COLOR_YELLOW

#define PERMISSION_MARKER -1
#define PERMISSION_USER   d_COLOR_GREEN
#define PERMISSION_GROUP  d_COLOR_BLUE
#define PERMISSION_OTHER  d_COLOR_YELLOW

#define START_X 0
#define START_Y 1

void init();
void init_colors();
void init_wins();
void check_win_err();
void process_kb();
void process_kup();
void process_kdown();
void process_kleft();
void process_kright();
void process_control();
void get_cwd(char *cwd, int argc, char *argv[]);
void change_cwd(char *cwd, const char *new_dir); 
void open_wd(const char *wd, char **dirs_arr, size_t *ndirs_arr);
void upd_prev_cwd(char *wd);
void assign_ndir(char *new_dir, const char *node);
void print_main();
void update_main(size_t highlight);
void print_linfo();
void print_rinfo();
void print_file(char *fname);
void print_folder(char *fname);
void print_cursor(WINDOW* win, int y, int x);
void colored_print(WINDOW *win, int y, int x, char *text, int color);
void refresh_win(WINDOW *win);
void end();

#endif // FMANAGER_H

