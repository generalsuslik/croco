#include "../inc/fmanager.h"

#ifdef _WIN32
	perror("Not linux os\n");
	exit(EXIT_FAILURE);
#endif

#ifdef _WIN64
	perror("Not linux os\n");
	exit(EXIT_FAILURE);
#endif


WINDOW *main_win, *linfo_win, *rinfo_win, *control_win;
/*
  LINFO_WIN:
  linfo_win = left info win
  there will be printed parent folder data
  of the cwd
 
  MAIN_WIN:
  main_win = main win
  there will be printed files/folders stored in cwd
 
  RINFO_WIN:
  rinfo_win = right info win
  there will be printed highlighed node info
      for file:
          it's text
 
      for folder:
          it's childen nodes
 
 
  +-----------+--------------+--------------+
  |           |              |              |
  |           |              |              |
  |           |              |              |
  | linfo_win |   main_win   |  rinfo_win   |
  |           |              |              |
  |           |              |              |
  |           |              |              |
  +-----------+--------------+--------------+
  |             control_win                 |
  +-----------------------------------------+
*/

/*
 * in dirs[] we'll store
 * cwd's files and folders 's name 
*/
char *dirs[DIRS_MAX];
size_t ndirs = 0;

/*
 * in prev_dirs[] we'll store
 * cwd's parent's data (files & folders)
*/
char *prev_dirs[DIRS_MAX];
size_t nprev_dirs = 0;

char cwd[PATH_MAX]; // current working directory
size_t ncwd = 0;

char prev_cwd[PATH_MAX]; // parent of cwd
size_t nprev_cwd = 0;

size_t top_index = 0;  // cwd[top_index] will be shown on top of the main_win. it's for scrolling

char new_dir[TITLE_MAX];
size_t nnew_dir = 0;

int ch;
size_t highlight = 1;
size_t choice = 0;

int main()
{
	init();
	init_colors();
	init_wins();

	get_cwd();
	open_wd(cwd, dirs, &ndirs);
	open_wd(prev_cwd, prev_dirs, &nprev_dirs);
	
	print_main();
	while (1) {
		process_kb();
		
		assign_ndir(new_dir, --choice);
		change_cwd(new_dir);
		open_wd(cwd, dirs, &ndirs);
		open_wd(prev_cwd, prev_dirs, &nprev_dirs);

		highlight = 1;
		choice = 0;
		print_main();
	
		refresh();
	}

	end();
	return 0;
}

/*
 * init curses
 */
void init()
{
	initscr();
	cbreak();
	start_color();	
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	
	mvprintw(START_X, START_Y, "Press any key to start. Press F1 to exit");	
	refresh();
}

void init_colors()
{
	init_pair(d_COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(d_COLOR_GREEN, COLOR_GREEN, COLOR_BLACK); 
	init_pair(d_COLOR_BLUE, COLOR_BLUE, COLOR_BLACK); 	
	init_pair(d_COLOR_RED, COLOR_RED, COLOR_BLACK); 
	init_pair(d_COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);	
}

/*
 * initializes windows:
 * main, left, right. - i.e. non-modal windows
 * these windows are gonna be shown to
 * user on startup
 */
void init_wins() 
{
	main_win = newwin(MAIN_HEIGHT, MAIN_WIDTH, START_Y, START_X + LEFT_WIDTH);
	keypad(main_win, TRUE);
	box(main_win, 0, 0);
	refresh();
	wrefresh(main_win);

	linfo_win = newwin(LEFT_HEIGHT, LEFT_WIDTH, START_Y, START_X);
	keypad(linfo_win, FALSE);
	box(linfo_win, 0, 0);
	refresh();
	wrefresh(linfo_win);

	rinfo_win = newwin(RIGHT_HEIGHT, RIGHT_WIDTH, START_Y, START_X + MAIN_WIDTH + LEFT_WIDTH);
	keypad(rinfo_win, TRUE);
	box(rinfo_win, 0, 0);
	refresh();
	wrefresh(rinfo_win);

	control_win = newwin(CONTROL_HEIGHT, CONTROL_WIDTH, MAIN_HEIGHT + CONTROL_MARGIN, START_X);
	keypad(control_win, FALSE);
	refresh();
	wrefresh(control_win);

	check_win_err();
}

void check_win_err()
{
	if (main_win == NULL) {
		end();
		fprintf(stderr, "main win err\n");
		exit(EXIT_FAILURE);
	}

	if (linfo_win == NULL) {
		end();
		fprintf(stderr, "left info win err\n");
		exit(EXIT_FAILURE);
	}

	if (rinfo_win == NULL) {
		end();
		fprintf(stderr, "right info win err\n");
		exit(EXIT_FAILURE);
	}

	if (control_win == NULL) {
		end();
		fprintf(stderr, "control win err\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * process keyboard
 * reads user input and peroforms
 * action depending on it
 */
void process_kb()
{
	while (1) {
		print_main();
		print_linfo();
		print_rinfo();

		ch = wgetch(main_win);
		switch(ch) {
			case KEY_UP:
				process_kup();
				break;

			case KEY_DOWN:
				process_kdown();
				break;
			
			case KEY_F(1):
				end();
				exit(EXIT_SUCCESS);
				break;

			case KKEY_ENTER: 
				choice = highlight;
				break;
			
			case ':':
				waddch(control_win, ':');
				print_cursor(control_win, 0, 1);
				process_control();
				wrefresh(control_win);
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

/*
 * processing keyup event
 * for the main win
 */
void process_kup()
{
	if (highlight == 1) {
		highlight = ndirs;
		top_index += (HEIGHT - MARGIN_TOP) * (ndirs / (HEIGHT - MARGIN_TOP));
	} else if (highlight % (HEIGHT - MARGIN_TOP + 1) == 0) {
		top_index -= (HEIGHT - MARGIN_TOP);
		--highlight;
	} else {
		--highlight;
	}
}

/*
 * processing keydown event
 * for the main win
 */
void process_kdown()
{
	if (highlight == ndirs) {
		highlight = 1;
		top_index = 0;
	} else if (highlight % (HEIGHT - MARGIN_TOP) == 0) {
		++highlight;
		top_index += (HEIGHT - MARGIN_TOP);
	} else {
		++highlight;
	}
}

void process_control()
{
	keypad(control_win, TRUE);
	
	char   buffer[1024];
	size_t buffer_len = 0;
	
	int cch;
	while ((cch = wgetch(control_win)) != KKEY_ENTER) {
		switch(cch) {
			case KEY_BACKSPACE:
				if (buffer_len > 0) {
					mvwaddch(control_win, 0, buffer_len + 1, ' ');
					mvwaddch(control_win, 0, --buffer_len + 1, ' ');
					print_cursor(control_win, 0, buffer_len + 1);
					buffer[buffer_len] = '\0';
				}
				break;

			case KEY_F(1):
				end();
				exit(EXIT_SUCCESS);
				break;

			case KKEY_ESC:
				wclear(control_win);
				keypad(control_win, FALSE);
				return;

			case KEY_UP:
				wclear(control_win);
				keypad(control_win, FALSE);
				return;

			case KEY_DOWN:
				wclear(control_win);
				keypad(control_win, FALSE);
				return;

			default:
				if (isalnum(cch) || cch == ' ' || cch == '.' || cch == '/') {
					mvwaddch(control_win, 0, buffer_len + 1, cch);
					print_cursor(control_win, 0, buffer_len + 2);
					assert(cch < CHAR_MAX);
					buffer[buffer_len++] = (char)cch;
				}
				break;
		}
	}

	buffer[buffer_len] = '\0';
	process_command(cwd, buffer);
	
	// updating arrays of dirs & files
	open_wd(cwd, dirs, &ndirs);
	open_wd(prev_cwd, prev_dirs, &nprev_dirs);
	
	print_main();
	wclear(control_win);

	keypad(control_win, FALSE);

}

void get_cwd()
{
	if (getcwd(cwd, PATH_MAX) == NULL) {
		end();
		perror("getcwd error\n");
		exit(EXIT_FAILURE);
	}

	while (cwd[ncwd] != '\0') {
		++ncwd;
	}

	cwd[ncwd++] = '/';
	cwd[ncwd] = '\0';

	upd_prev_cwd(cwd);
}

void change_cwd(const char *new_dir) 
{
	if (strcmp(new_dir, ".") == 0) {
		return;
	}

	if (strcmp(new_dir, "..") == 0) {
		strcpy(cwd, prev_cwd);
		upd_prev_cwd(cwd);
		return;
	}

	strcat(cwd, new_dir);
	ncwd = strlen(cwd);
	cwd[ncwd++] = '/';
	cwd[ncwd] = '\0';
	upd_prev_cwd(cwd);
}

/*
 * adds dir & file 's names to dirs_arr
 */
void open_wd(const char *wd, char **dirs_arr, size_t *ndirs_arr)
{
	for (size_t i = 0; i < *ndirs_arr; ++i) {
		memset(dirs_arr[i], '\0', strlen(dirs_arr[i]));
	}
	*ndirs_arr = 0;

	DIR *d;
	d = opendir(wd);
	struct dirent *dir;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			refresh();
			dirs_arr[*ndirs_arr] = malloc(PATH_MAX * sizeof(char));
			strcpy(dirs_arr[(*ndirs_arr)++], dir->d_name);
		}
	} else {
		end();
		perror("Could not open directory: ");
		fprintf(stderr, "%s\n", wd);
		exit(EXIT_FAILURE);
	}

	closedir(d);

	refresh();
}

/*
 * updates prev_cwd
 * depending on wd
 */
void upd_prev_cwd(char *wd) 
{
	size_t n = strlen(wd);
	
	int last_slash = 0;
	int prev_slash = 0;
	
	for (size_t i = 0; i < n; ++i) {
		if (wd[i] == '/') {
			prev_slash = last_slash;
			last_slash = i;
		}
	}

	nprev_cwd = prev_slash + 1;
	for (size_t i = 0; i <= nprev_cwd; ++i) {
		prev_cwd[i] = wd[i];
	}
	prev_cwd[nprev_cwd] = '\0';
}

/*
 * assignes chosen option to a new_dir
 */
void assign_ndir(char *new_dir, int choice)
{
	memset(new_dir, '\0', nnew_dir);	
	
	nnew_dir = 0;
	while (dirs[choice][nnew_dir] != '\0') {
		new_dir[nnew_dir] = dirs[choice][nnew_dir];
		++nnew_dir;
	}
}

bool is_file(char *src, char *fname)
{
	char ccwd[PATH_MAX] = { '\0' };
	create_path(ccwd, src, fname);
	struct stat path_stat;
	if (stat(ccwd, &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
		return true;	
	}

	return false;
}

/*
 *  path like: 
 * fullpath = src + fname
 */
void create_path(char *fullpath, char *src, char *fname)
{
	strcpy(fullpath, src);
	strcat(fullpath, fname);
}

/*
 * prints main window contend:
 * list of files and folders 
 * in cwd (current working directory)
 */
void print_main()
{
	wclear(main_win);

	/* printing colored cwd */
	colored_print(main_win, 1, 0, cwd, d_COLOR_CYAN);

	/* adding a line delimeter */
	mvwhline(main_win, 2, 0, 0, WIDTH / 2);

	size_t pos_i = 0;
	for (size_t i = top_index; i < ndirs; ++i) {
		pos_i = 3 + i - top_index;
		if (highlight == i + 1) {
			wattron(main_win, A_REVERSE);
			if (is_file(cwd, dirs[i])) {
				mvwprintw(main_win, pos_i, 1, dirs[i]);
			} else {
				colored_print(main_win, pos_i, 1, dirs[i], FOLDER_COLOR);
			}
			wattroff(main_win, A_REVERSE);
		} else {
			if (is_file(cwd, dirs[i])) {
				mvwprintw(main_win, pos_i, 1, dirs[i]);
			} else {
				colored_print(main_win, pos_i, 1, dirs[i], FOLDER_COLOR);
			}
		}
	}

	wrefresh(main_win);
}

/*
 * prints prevoius 
 * folder info
 */
void print_linfo()
{
	wclear(linfo_win);
	
	/* printing prev dir name */
	colored_print(linfo_win, 1, 1, prev_cwd, FOLDER_COLOR);
	
	/* adding line delimeter */
	mvwhline(linfo_win, 2, 1, 0, LEFT_WIDTH);

	for (size_t i = 0; i < nprev_dirs; ++i) {
		if (is_file(prev_cwd, prev_dirs[i])) {
			mvwprintw(linfo_win, i + 3, 1, prev_dirs[i]);
		} else {
			colored_print(linfo_win, i + 3, 1, prev_dirs[i], FOLDER_COLOR);
		}
	}

	wrefresh(linfo_win);	
}

/*
 * prints info window content
 * depending on what is highlighted 
 * in the main window
 */
void print_rinfo()
{
	wclear(rinfo_win);
	
	/* printing file/dir name in color in info win */	
	if (is_file(cwd, dirs[highlight - 1])) {
		colored_print(rinfo_win, 1, 1, dirs[highlight - 1], INFO_FILE_COLOR);
	} else {
		colored_print(rinfo_win, 1, 1, dirs[highlight - 1], INFO_FOLDER_COLOR);
	}
	
	/* adding a line delimeter */
	mvwhline(rinfo_win, 2, 1, 0, WIDTH / 2 - 2);
	
	if (is_file(cwd, dirs[highlight - 1])) {
		print_file(dirs[highlight - 1]);
	} else {
		print_folder(dirs[highlight - 1]);
	}

	wrefresh(rinfo_win);
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

/*
 * prints content of the file <fname>
 * to the info window
 */
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
	wmove(rinfo_win, print_y, print_x);
	while ((ch = fgetc(f_ptr)) != EOF) {
		getyx(rinfo_win, cur_y, cur_x);
		if (cur_x == 0) {
			wmove(rinfo_win, ++print_y, print_x);
		}
		
		if (cur_y > LEFT_HEIGHT - 2) {
			break;
		}

		wprintw(rinfo_win, "%c", ch);
	}

	fclose(f_ptr);
	box(rinfo_win, 0, 0);
}

/*
 * prints content of the folder <fname>
 * to the info window
 */
void print_folder(char *fname)
{
	char fpath[PATH_MAX] = { '\0' };
	create_path(fpath, cwd, fname);

	int len = strlen(fpath);
	fpath[len++] = '/';
	
	DIR *fd;
	struct dirent *fdir;
	fd = opendir(fpath);

	char *fd_name;
	int cur_y = 3;
	int cur_x = 2;

	int print_y = 3;
	int print_x = 1;
	wmove(rinfo_win, print_y, print_x);
	if (fd) {
		while ((fdir = readdir(fd)) != NULL) {
			getyx(rinfo_win, cur_y, cur_x);
			if (cur_x == 0) {
				wmove(rinfo_win, ++print_y, print_x);
			}

			if (cur_y > LEFT_HEIGHT - 2) {
				break;
			}

			fd_name = fdir->d_name;
			// not goin to print . and .. folders in info window
			if (strcmp(fd_name, ".") != 0 && strcmp(fd_name, "..") != 0) {
				if (is_file(fpath, fd_name)) {
					mvwprintw(rinfo_win, print_y, print_x, "%s\n", fd_name);
				} else {
					colored_print(rinfo_win, print_y, print_x, fd_name, FOLDER_COLOR);
				}
			}
		}

		closedir(fd);
	}
}

void print_cursor(WINDOW *win, int y, int x)
{
	wattron(win, A_REVERSE);
	mvwaddch(win, y, x, ' ');
	wattroff(win, A_REVERSE);				
}

void end()
{
	delwin(main_win);
	delwin(linfo_win);
	delwin(rinfo_win);
	delwin(control_win);
	endwin();
}

