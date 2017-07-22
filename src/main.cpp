#include"login.h"
#include"manage.h"
#include"shop.h"
#include<ncurses.h>
#include<locale.h>

#define TOPTITLE "超市收银系统"
#define BOTTOMTITLE "Linux POS version 1.0 Author Long"

void showTitle(int bg);

int main() 
{
	setlocale(LC_ALL,"");//开启中文 编译时用-lncursesw代替-lncurses
	initscr();
	start_color();  /* 开启颜色管理功能 */
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_BLACK, COLOR_RED);
	init_pair(3, COLOR_WHITE, COLOR_BLUE);
	init_pair(4, COLOR_RED, COLOR_WHITE);
	
	cbreak();
	noecho();
	//keypad(stdscr, TRUE);
	
	
	while(1) {
		showTitle(3);
		
		//弹出登录窗口
		Login login;
		if(login.show() == 1) {
			//成功登录
			//mvprintw(2,1,login.username);
			//mvprintw(3,1,login.usertype);
			//refresh();
			
			showTitle(0);
			switch(login.usertype) {
			case 0:
			case 1: {
				Shop shop(login.userid,login.username);
				shop.show();
				break;
				}
			case 2: {
				Manage manage(login.username);
				manage.show();
				break;
				}
			}
			
		} else {
			break;//退出系统
		}
		
	}
	
	endwin();

	return 0;
}

void showTitle(int bg) {//设置窗口背景颜色、绘制标题、刷新

	wbkgd(stdscr,COLOR_PAIR(bg));
		
	mvprintw(0,0,TOPTITLE);
	mvchgat(0, 0, COLS, A_BLINK, 1, NULL);
	
	mvprintw(LINES-1,0,BOTTOMTITLE);
	mvchgat(LINES-1, 0, COLS, A_BLINK, 1, NULL);
	
	refresh();
}

