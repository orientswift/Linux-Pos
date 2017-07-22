#include"manage.h"
#include"manage_user.h"
#include"manage_goods.h"
#include"manage_sale.h"
#include<cstring>


Manage::Manage(char *username) {
	if(username != NULL) {
		this->username = new char[strlen(username)+1];
		strcpy(this->username, username);
	} else {
		this->username = 0;
	}
}

Manage::~Manage() {
	if(this->username != 0) {
		delete[] this->username;
		this->username = 0;
	}
}

void Manage::show() {
	if(this->username == 0)
		return;
	
	//绘制管理后台界面外框
	WINDOW *win = newwin(LINES-2, COLS, 1, 0);
	wbkgd(win,COLOR_PAIR(0));//设置窗口背景颜色
	box(win, 0, 0);
	wrefresh(win);
	
	showMenu();
}

void Manage::showMenu() {
	char *choices[] = {
		"用户管理",
		"商品管理",
		"销售记录",
		"退出登录",
	};
	int n_choices = sizeof(choices) / sizeof(char *);
	int highlight = 1;//当前选中菜单项
	int choice = 0;//用户确定选择项
	int c;//用户输入
	int WIDTH = 14;//菜单框长度
	int HEIGHT = 9;//菜单框高度
	int startx = (COLS - WIDTH) / 2;
	int starty = (LINES - HEIGHT) / 2;
	
	//绘制后台菜单框
	WINDOW *menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	wbkgd(menu_win,COLOR_PAIR(1));//设置窗口背景颜色
	keypad(menu_win, TRUE);
	
	//绘制管理员用户姓名
	mvwprintw(menu_win, 1, (WIDTH-strlen(username)/3)/2-1, username);
	
	print_menu(menu_win, highlight, choices, n_choices);
	while(1){
		
		c = wgetch(menu_win);
		switch(c) {  
		case KEY_UP:
			if(highlight == 1)
				highlight = n_choices;
			else
				--highlight;
			break;
		case KEY_DOWN:
			if(highlight == n_choices)
				highlight = 1;
			else
				++highlight;
			break;
		case 10:
			choice = highlight;
			break;
		default:
			break;
		}
		print_menu(menu_win, highlight, choices, n_choices);
		if(choice != 0) {
			keypad(menu_win, FALSE);//关闭键盘监听
			
			if(choice == 4) {
				break;
			} else if(choice == 1) {
				Manage_User mu;
				mu.show();
				
			} else if(choice == 2) {
				Manage_Goods mg;
				mg.show();
				
			} else if(choice == 3) {
				Manage_Sale ms;
				ms.show();
			}
			print_menu(menu_win, highlight, choices, n_choices);//恢复菜单显示
			keypad(menu_win, TRUE);//开启键盘监听
			choice = 0;//恢复0
		}
		
	}
}

void Manage::print_menu(WINDOW *menu_win, int highlight, char**choices, int n_choices) {
	int x, y, i; 
	x = 3;
	y = 3;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i) {  
		if(highlight == i + 1) /* 使选择的字符串高亮显示 */ {  
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		} else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
	
	//隐藏输入点
	mvprintw(0, COLS-1, "");
	clrtoeol();
	refresh();
}
