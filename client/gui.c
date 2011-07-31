#include "gui.h"

void initGui() {  
  char mesg[] = "Just a string";
  char str[80];
  int row, col;
  
  initscr();    // Start curses mode
  raw();        // Disable line buffering
  noecho();     // Suppress automatic key echo
  keypad(stdscr, TRUE); // Enable reading of function keys, arrow keys, etc
  nodelay(stdscr, TRUE); // Don't block on getch();
  
  
  getmaxyx(stdscr, row, col); // get the number of rows and columns
  mvprintw(row/2, (col-strlen(mesg))/2, "%s", mesg);
  // 
  // getstr(str);
  // mvprintw(LINES - 2, 0, "You entered: %s", str);
}