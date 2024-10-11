#include <dirent.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define WIDTH getmaxx(stdscr)
#define HEIGHT getmaxy(stdscr)

/*
 * for margin for 
 * the list of files/folders
 */
#define MARGIN_TOP 6

/*
 * margin for win
 */
#define WIN_MARGIN 2

#define MAIN_HEIGHT    HEIGHT - WIN_MARGIN
#define MAIN_WIDTH     WIDTH / 2
#define INFO_HEIGHT    3 * HEIGHT / 4
#define INFO_WIDTH     WIDTH / 2
#define CONTROL_HEIGHT HEIGHT / 4 - WIN_MARGIN
#define CONTROL_WIDTH  WIDTH / 2


DIR *d;
WINDOW *main_win, *info_win, *control_win;

#define DIRS_MAX 320000
char *dirs[DIRS_MAX];
int ndirs = 0;

#define PATH_MAX 16000
char cwd[PATH_MAX]; // current working directory
int ncwd = 0;
int top_index = 0;  // cwd[top_index] will be shown on top of the main_win. it's for scrolling

#define TITLE_MAX 1000
char new_dir[TITLE_MAX];

int startx = 0;
int starty = 1;

int ch = 0;
int highlight = 1;
int choice = 0;

void init();
void init_colors();
void init_wins();
void check_win_err();
void run();
void get_cwd();
void change_cwd();
void open_cwd();
void assign_ndir(char *new_dir, int choice);
bool is_file(char *fname);
void print_main();
void colored_print(WINDOW *win, int y, int x, char *text, int color);
void print_info();
void print_control();
char *create_path(char *fullpath, char *src, char *fname);
void print_file(char *fname);
void print_folder(char *fname);
void end();

int main()
{
	init();
	init_colors();
	
	init_wins();

	get_cwd();
	open_cwd();
	
	print_main();
	print_info();
	while (ch != KEY_F(1)) {
		run();

		assign_ndir(new_dir, --choice);
		change_cwd();
		open_cwd();

		highlight = 1;
		choice = 0;
		print_main(highlight);
	
		refresh();
	}
	
	end();
	return 0;
}

void init()
{
	initscr();
	cbreak();
	start_color();	
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	
	mvprintw(0, 1, "Press any key to start. Press F1 to exit");	
	refresh();
}

void init_colors() 
{
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK); // for binaries
	init_pair(3, COLOR_BLUE, COLOR_BLACK); 	// for folders
	init_pair(4, COLOR_RED, COLOR_BLACK); 
	init_pair(5, COLOR_YELLOW, COLOR_BLACK);	
}

void init_wins() 
{
	main_win = newwin(MAIN_HEIGHT, MAIN_WIDTH, starty, startx);
	keypad(main_win, TRUE);
	box(main_win, 0, 0);
	refresh();
	wrefresh(main_win);
	
	info_win = newwin(INFO_HEIGHT, INFO_WIDTH, starty, startx + MAIN_WIDTH);
	keypad(info_win, FALSE);
	box(info_win, 0, 0);
	refresh();
	wrefresh(info_win);

	control_win = newwin(CONTROL_HEIGHT, CONTROL_WIDTH, starty + INFO_HEIGHT, startx + MAIN_WIDTH);
	keypad(control_win, FALSE);
	box(control_win, 0, 0);
	refresh();
	wrefresh(control_win);

	check_win_err();	
	
	print_control();
}

void check_win_err()
{
	if (main_win == NULL) {
		end();
		fprintf(stderr, "main err\n");
		exit(EXIT_FAILURE);
	}

	if (info_win == NULL) {
		end();
		fprintf(stderr, "info err\n");
		exit(EXIT_FAILURE);
	}

	if (control_win == NULL) {
		end();
		fprintf(stderr, "control err\n");
		exit(EXIT_FAILURE);
	}
}

void run()
{
	while (1) {
		print_main();
		print_info();

		ch = wgetch(main_win);
		switch (ch) {	
			case KEY_UP:
				if (highlight == 1) {
					highlight = ndirs;
					top_index += (HEIGHT - MARGIN_TOP) * (ndirs / (HEIGHT - MARGIN_TOP));
				} else if (highlight % (HEIGHT - MARGIN_TOP + 1) == 0) {
					top_index -= (HEIGHT - MARGIN_TOP);
					--highlight;
				} else {
					--highlight;
				}
				break;

			case KEY_DOWN:
				if (highlight == ndirs) {
					highlight = 1;
					top_index = 0;
				} else if (highlight % (HEIGHT - MARGIN_TOP) == 0) {
					++highlight;
					top_index += (HEIGHT - MARGIN_TOP);
				} else {
					++highlight;
				}
				break;

			case KEY_F(1):
				end();
				exit(EXIT_SUCCESS);
				break;

			case 10: // ENTER
				choice = highlight;
				break;

			default:
				refresh();
				break;
		}
			
		if (choice != 0) {
			break;
		}
	}
}

void get_cwd() 
{
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd error\n");
		exit(EXIT_FAILURE);
	}	

	while (cwd[ncwd] != '\0') {
		++ncwd;
	}

	cwd[ncwd++] = '/';
}

void change_cwd() 
{
	if (strcmp(new_dir, ".") == 0) {
		return;
	}

	if (strcmp(new_dir, "..") == 0) {
		cwd[--ncwd] = '\0';
		while (cwd[ncwd - 1] != '/') {
			cwd[--ncwd] = '\0';
		}
		return;
	}

	int new_index = 0;
	while (new_dir[new_index] != '\0') {
		cwd[ncwd++] = new_dir[new_index++];
	}
	cwd[ncwd++] = '/';
}

void open_cwd()
{
	int index;
	for (int i = 0; i < ndirs; ++i) {
		index = 0;
		while (dirs[i][index] != '\0') {
			dirs[i][index++] = '\0';
		}
	}
	ndirs = 0;

	d = opendir(cwd);
	struct dirent *dir;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			refresh();
			dirs[ndirs++] = dir->d_name;
		}
	} else {
		end();
		perror("Could not open a directory: ");
		fprintf(stdout, "%s\n", cwd);
		exit(EXIT_FAILURE);
	}
	refresh();
}

void assign_ndir(char *new_dir, int choice)
{
	int index = 0;
	while (new_dir[index] != '\0') {
		new_dir[index++] = '\0';
	}
	
	index = 0;
	while (dirs[choice][index] != '\0') {
		new_dir[index] = dirs[choice][index];
		++index;
	}
}

bool is_file(char *fname)
{
	char ccwd[PATH_MAX] = { '\0' };
	create_path(ccwd, cwd, fname);
	struct stat path_stat;
	if (stat(ccwd, &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
		return true;	
	}

	return false;
}

void print_main()
{	
	wclear(main_win);

	/* printing colored cwd */
	colored_print(main_win, 1, 0, cwd, 1);
	
	/* adding a line delimeter */
	mvwhline(main_win, 2, 0, 0, WIDTH / 2);

	int pos_i = 0;
	for (int i = top_index; i < ndirs; ++i) {
		pos_i = 3 + i - top_index;
		if (highlight == i + 1) {
			wattron(main_win, A_REVERSE);
			if (is_file(dirs[i])) {
				mvwprintw(main_win, pos_i, 1, dirs[i]);
			} else {
				colored_print(main_win, pos_i, 1, dirs[i], 2);
			}
			wattroff(main_win, A_REVERSE);
		} else {
			if (is_file(dirs[i])) {
				mvwprintw(main_win, pos_i, 1, dirs[i]);
			} else {
				colored_print(main_win, pos_i, 1, dirs[i], 2);
			}
		}
	}

	wrefresh(main_win);
}

void print_info()
{
	wclear(info_win);
	
	/* printing file/dir name in color in info win */	
	if (is_file(dirs[highlight - 1])) {
		colored_print(info_win, 1, 1, dirs[highlight - 1], 5);
	} else {
		colored_print(info_win, 1, 1, dirs[highlight - 1], 4);
	}
	
	/* adding a line delimeter */
	mvwhline(info_win, 2, 1, 0, WIDTH / 2 - 2);
	
	if (is_file(dirs[highlight - 1])) {
		print_file(dirs[highlight - 1]);
	} else {
		print_folder(dirs[highlight - 1]);
	}

	wrefresh(info_win);
}

void print_control()
{
	wclear(control_win);
	box(control_win, 0, 0);

	char *options[] = {
		"Ctrl + D - delete file/folder\n",
		"Ctrl + A - add node to folder\n",
		"Ctrl + R - rename file/folder\n",
	};
	size_t n = sizeof(options) / sizeof(char *);

	for (size_t i = 0; i < n; ++i) {
		mvwprintw(control_win, i + 1, 1, options[i]);
	}
	
	box(control_win, 0, 0);
	wrefresh(control_win);	
}

void colored_print(WINDOW *win, int y, int x, char *text, int color)
{
	wattron(win, COLOR_PAIR(color));
	mvwprintw(win, y, x, "%s\n", text);
	wattroff(win, COLOR_PAIR(color));
	box(win, 0, 0);
	wrefresh(main_win);
	refresh();
}

char *create_path(char *fullpath, char *src, char *fname)
{
	strcpy(fullpath, src);
	strcat(fullpath, fname);

	return fullpath;
}

void print_file(char *fname)
{
	char fpath[PATH_MAX] = { '\0' };
	create_path(fpath, cwd, fname);

	FILE *f_ptr = fopen(fpath, "r");
	if (f_ptr == NULL) {
		end();
		fprintf(stderr, "Could not open a file %s\n", fname);
	}

	char ch;
	int cur_y;
	int cur_x;

	int print_y = 3;
	int print_x = 1;
	wmove(info_win, print_y, print_x);
	while ((ch = fgetc(f_ptr)) != EOF) {
		getyx(info_win, cur_y, cur_x);
		if (cur_x == 0) {
			wmove(info_win, ++print_y, print_x);
		}
		
		if (cur_y > INFO_HEIGHT - 2) {
			break;
		}

		wprintw(info_win, "%c", ch);
	}

	fclose(f_ptr);
	box(info_win, 0, 0);
}

void print_folder(char *fname)
{
	char fpath[PATH_MAX] = { '\0' };
	create_path(fpath, cwd, fname);
	
	DIR *fd;
	struct dirent *fdir;
	fd = opendir(fpath);

	char *fd_name;
	int cur_y = 3;
	int cur_x = 2;

	int print_y = 3;
	int print_x = 1;
	wmove(info_win, print_y, print_x);
	if (fd) {
		while ((fdir = readdir(fd)) != NULL) {
			getyx(info_win, cur_y, cur_x);
			if (cur_x == 0) {
				wmove(info_win, ++print_y, print_x);
			}

			if (cur_y > INFO_HEIGHT - 2) {
				break;
			}

			fd_name = fdir->d_name;
			if (strcmp(fd_name, ".") != 0 && strcmp(fd_name, "..") != 0) {
				if (is_file(fd_name)) {
					mvwprintw(info_win, print_y, print_x, "%s\n", fd_name);
				} else {
					colored_print(info_win, print_y, print_x, fd_name, 2);
				}
			}
		}

		closedir(fd);
	}
}

void end()
{
	delwin(main_win);
	delwin(info_win);
	delwin(control_win);
	endwin();
}

