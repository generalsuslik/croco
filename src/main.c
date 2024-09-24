#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIDTH getmaxx(stdscr)
#define HEIGHT getmaxy(stdscr) - 2

WINDOW *main_win, *info_win;

int startx = 0;
int starty = 1;

void init();
void init_wins();
void end();

int main()
{
	init();
	
	printw("To exit type F1\n");	
	#if 0
	WINDOW *win = newwin(HEIGHT, WIDTH, 10, 10);
	keypad(win, TRUE);
	mvwprintw(win, 1, 1, "asd");
	refresh();
	
	wrefresh(win);
	box(win, 0, 0);
	getch();
	
	delwin(win);
	endwin();
	#endif

	init_wins();
	getch();

	end();
	return 0;
}

void init()
{
	initscr();
	clear();
	cbreak();
	noecho();
	//keypad(stdscr, TRUE);
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

void end()
{
	endwin();
}

