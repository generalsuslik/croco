#include <dirent.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
void assign_ndir(char *new_dir, char *str);
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
					choice = highlight - 1;
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
		
		// now we have a choice to go
		//new_dir = dirs[choice];
		assign_ndir(new_dir, dirs[choice]);
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
}

void change_cwd() 
{
	int last_slash;
	for (int i = 0; cwd[i] != '\0'; ++i) {
		if (cwd[i] == '/') {
			last_slash = i;
		}
	}

	int new_index = 0;
	int index = last_slash;
	while (new_dir[new_index] != '\0') {
		cwd[++index] = new_dir[new_index++];
	}
}

void open_cwd(char *str)
{
	for (int i = 0; i < ndirs; ++i) {
		dirs[i] = "";
	}
	ndirs = 0;
	d = opendir(str);
	struct dirent *dir;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			dirs[ndirs++] = dir->d_name;
		}
	}
}

void assign_ndir(char *new_dir, char *str)
{
	int index = 0;
	while (str[index] != '\0') {
		new_dir[index] = str[index];
		++index;
	}
}

void print_main(int highlight)
{
	wclear(main_win);
	for (int i = 0; i < ndirs; ++i) {
		if (highlight == i + 1) {
			wattron(main_win, A_REVERSE);
			colored_print(main_win, 1 + i, 1, dirs[i], 2);
			wattroff(main_win, A_REVERSE);
		} else {
			colored_print(main_win, 1 + i, 1, dirs[i], 2);
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

