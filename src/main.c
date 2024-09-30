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
#define HEIGHT getmaxy(stdscr) - 2

DIR *d;
WINDOW *main_win, *info_win;

#define DIRS_MAX 320000
char *dirs[DIRS_MAX];
int ndirs = 0;

#define PATH_MAX 16000
char cwd[PATH_MAX]; // current working directory
int ncwd = 0;

#define TITLE_MAX 1000
char new_dir[TITLE_MAX];

int startx = 0;
int starty = 1;

void init();
void init_colors();
void init_wins();
void get_cwd();
void change_cwd();
void open_cwd();
void assign_ndir(char *new_dir, int choice);
bool is_file();
void print_main(int highlight);
void colored_print(WINDOW *win, int y, int x, char *text, int color);
void end();

int main()
{
	init();
	init_colors();
	
	init_wins();

	get_cwd();
	open_cwd();
	
	int ch = 0;
	int highlight = 1;
	int choice = 0;
	print_main(highlight);
	while (ch != KEY_F(1)) {
		while (1) {
			ch = wgetch(main_win);
			switch (ch) {	
				case KEY_UP:
					if (highlight == 1) {
						highlight = ndirs;
					} else {
						--highlight;
					}
					break;

				case KEY_DOWN:
					if (highlight == ndirs) {
						highlight = 1;
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
		
			print_main(highlight);
			if (choice != 0) {
				break;
			}
		}
		
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

void init_colors() 
{
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK); // for binaries
	init_pair(3, COLOR_BLUE, COLOR_BLACK); 	// for folders
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

void init_wins() 
{
	main_win = newwin(HEIGHT, WIDTH / 2, starty, startx);
	keypad(main_win, TRUE);
	box(main_win, 0, 0);
	refresh();
	wrefresh(main_win);

	info_win = newwin(HEIGHT, WIDTH / 2, starty, startx + WIDTH / 2);
	keypad(info_win, FALSE);
	box(info_win, 0, 0);
	refresh();
	wrefresh(info_win);
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
		while (cwd[ncwd - 1] != '/') {
			cwd[--ncwd] = '\0';
		}
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
		printf("%s\n", cwd);
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

bool is_file(int index)
{
	char ccwd[PATH_MAX] = { '\0' };
	strcpy(ccwd, cwd);
	strcat(ccwd, dirs[index]);
	struct stat path_stat;
	if (stat(ccwd, &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
		return true;	
	}

	return false;
}

void print_main(int highlight)
{
	wclear(main_win);
	for (int i = 0; i < ndirs; ++i) {
		if (highlight == i + 1) {
			wattron(main_win, A_REVERSE);
			if (is_file(i)) {
				mvwprintw(main_win, 1 + i, 1, dirs[i]);
			} else {
				colored_print(main_win, 1 + i, 1, dirs[i], 2);
			}
			wattroff(main_win, A_REVERSE);
		} else {
			if (is_file(i)) {
				mvwprintw(main_win, 1 + i, 1, dirs[i]);
			} else {
				colored_print(main_win, 1 + i, 1, dirs[i], 2);
			}
		}
	}

	wrefresh(main_win);
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

void end()
{
	endwin();
}

