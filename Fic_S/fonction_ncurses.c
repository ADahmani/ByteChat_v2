#include "fonction_ncurses.h"


void initfenetres(){
	initscr();
	cbreak();			
	keypad(stdscr, TRUE);		
	if(has_colors() == FALSE)
	{	
		printf("votre terminal ne support pas les couleurs (ncurses)\n");
		
	}
	else{
	start_color();			
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	}
}

 

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	
	box(local_win, 0 , 0);		
	wrefresh(local_win);		

	return local_win;
}

void destroy_win(WINDOW *local_win)
{	
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}
