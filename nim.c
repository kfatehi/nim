/*
 * Nim client + ncurses, written in C
 */
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

void initialize(void);

int main()
{	
  char mesg[] = "Just a string";
  char str[80];
  int row, col;
  
  initialize();
  getmaxyx(stdscr, row, col); // get the number of rows and columns
	mvprintw(row/2, (col-strlen(mesg))/2, "%s", mesg);

  getstr(str);
  mvprintw(LINES - 2, 0, "You entered: %s", str);

  getch();			/* Wait for user input */
	endwin();			/* End curses mode		  */
	return 0;
}

void initialize()
{
  initscr();    // Start curses mode
  raw();        // Disable line buffering
  noecho();     // Suppress automatic key echo
  keypad(stdscr, TRUE); // Enable reading of function keys, arrow keys, etc
}