#include <dirent.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH getmaxx(stdscr)
#define HEIGHT getmaxy(stdscr) - 2

DIR *d;
WINDOW *main_win, *info_win;

char *dirs[320000];
int ndirs = 0;

int startx = 0;
int starty = 1;

void init();
void init_colors();
void init_wins();
void open_dir(char *str);
void colored_print(WINDOW *win, int y, int x, char *text, int color);
void end();

int main()
{
	init();
	init_colors();
	
	init_wins();
	open_dir(".");
	
	int ch;
	while ((ch = getch()) != KEY_F(1)) {
		for (int i = 0; i < ndirs; ++i) {
			if (strchr(dirs[i], '.')) {
				colored_print(main_win, 1 + i, 1, dirs[i], 3);
			} else {
				colored_print(main_win, 1 + i, 1, dirs[i], 2);	
			}
			wrefresh(main_win);
			refresh();
		}
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

void open_dir(char *str)
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

void colored_print(WINDOW *win, int y, int x, char *text, int color)
{
	wattron(win, COLOR_PAIR(color));
	mvwprintw(win, y, x, "%s\n", text);
	wattroff(win, COLOR_PAIR(color));
	box(win, 0, 0);
}

void end()
{
	endwin();
}

