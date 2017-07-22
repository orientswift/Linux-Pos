#ifndef MANAGE_H
#define MANAGE_H

#include<ncurses.h>

class Manage {
public:
	Manage(char *username);
	~Manage();
	void show();
	
private:
	char *username;
	void showMenu();
	void print_menu(WINDOW *menu_win, int highlight, char**choices, int n_choices);
};

#endif
