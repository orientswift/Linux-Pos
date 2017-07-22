#include"login.h"
#include<ncurses.h>
#include<form.h>
#include<cstring>
#include"DB_Manager.h"
#include<unistd.h>
#include<stdlib.h>
#include"unit.h"

Login::Login() {
	//initscr();
	isLogined = 0;
	username = 0;
	userid = 0;
	usertype = -1;
}

Login::~Login() {
	//endwin();
	if(username != 0) {
		delete[] username;
		username = 0;
	}
	
	if(userid != 0) {
		delete[] userid;
		userid = 0;
	}
}

int Login::show() {
	wbkgd(stdscr,COLOR_PAIR(3));//设置窗口背景颜色
	

	return showMiddleWin("欢迎来到登录窗口");
}

int Login::showMiddleWin(const char *wintitle) {
	FIELD *field[3];
	FORM *my_form;
	WINDOW *my_form_win;
	int ch, rows, cols;
	
	//两个按钮的状态
	int highlight = 0;//高亮
	int choice = -1;//选中

	/* 初始化域 */
	field[0] = new_field(1, 15, 1, 7, 0, 0);
	field[1] = new_field(1, 15, 3, 7, 0, 0);
	field[2] = NULL;
	
	/* 设置表单域选项*/
	set_field_type(field[0], TYPE_ALNUM, 1);/*检查输入类型为字母或数字且最少有1位数*/
	set_field_fore(field[0], COLOR_PAIR(2));
	set_field_back(field[0], COLOR_PAIR(2));
	field_opts_off(field[0], O_AUTOSKIP);/*如果当前表单域已满，不自动跳到下一个表单域*/
	
	set_field_type(field[1], TYPE_ALNUM, 1);/*检查输入类型为字母或数字且最少有1位数*/
	set_field_fore(field[1], COLOR_PAIR(2));
	set_field_back(field[1], COLOR_PAIR(2));
	field_opts_off(field[1], O_AUTOSKIP);/*如果当前表单域已满，不自动跳到下一个表单域 */
	field_opts_off(field[1], O_PUBLIC);//关闭回显变为密码框
	
	/* 创建表单并传递 */
	my_form = new_form(field);
	/* 计算表单所需要的面积大小 */
	scale_form(my_form, &rows, &cols);
	
	/* 创建与表单相关联的窗口 */
	my_form_win = newwin(rows + 6, cols + 6, LINES/2-(rows+6)/2, COLS/2-(cols+6)/2);
	keypad(my_form_win, TRUE);
	wbkgd(my_form_win,COLOR_PAIR(1));//设置窗口背景颜色
	
	/* 设置主窗口和子窗口 */
	set_form_win(my_form, my_form_win);
	set_form_sub(my_form, derwin(my_form_win, rows, cols, 2, 2));
	
	/* 在窗口的四周显示边框 */
	box(my_form_win, 0, 0);
	post_form(my_form);
	
	/* 在窗口显示标题和标签 */
	wattron(my_form_win, COLOR_PAIR(1));
	mvwprintw(my_form_win,1,(cols+6)/2-strlen(wintitle)/3,wintitle);
	mvwprintw(my_form_win,3,4,"帐号:");
	mvwprintw(my_form_win,5,4,"密码:");
	mvwprintw(my_form_win,7,18,"退出");
	wattroff(my_form_win, COLOR_PAIR(1));
	//默认选中确认键
	wattron(my_form_win, COLOR_PAIR(3));
	mvwprintw(my_form_win,7,7,"登录");
	wattroff(my_form_win, COLOR_PAIR(3));
	
	wrefresh(my_form_win);
	//刷新完按钮指针切回到输入框里
	form_driver(my_form, REQ_END_LINE);
	
	/* 循环以获取用户请求 */
	while(1) {
		ch = wgetch(my_form_win);
		switch(ch) {
		case KEY_DOWN:
			/* 跳到下一个域 */
			form_driver(my_form, REQ_NEXT_FIELD);
			/* 跳到当前缓冲区的末尾 */
			/* 在最后一个字符处跳出 */
			form_driver(my_form, REQ_END_LINE);
			break;
		case KEY_UP:
			/* 跳到上一个域 */
			form_driver(my_form, REQ_PREV_FIELD);
			form_driver(my_form, REQ_END_LINE);
			break;
		case KEY_LEFT:
			if(highlight == 0)
				highlight = 1;
			else
				highlight = 0;
			break;
		case KEY_RIGHT:
			if(highlight == 1)
				highlight = 0;
			else
				highlight = 1;
			break;
		case 10:
			choice = highlight;
			break;
		case KEY_BACKSPACE:
			form_driver(my_form, REQ_DEL_LINE);
			break;
		default:
			/* 如果是一个普通的字符，则打印出来 */
			form_driver(my_form, ch);
			break;
		}
		
		//刷新按钮状态
		if(highlight == 1) {
			mvwchgat(my_form_win, 7, 7, 4, A_BLINK, 1, NULL);
			mvwchgat(my_form_win, 7, 18, 4, A_BLINK, 3, NULL);
		} else {
			mvwchgat(my_form_win, 7, 7, 4, A_BLINK, 3, NULL);
			mvwchgat(my_form_win, 7, 18, 4, A_BLINK, 1, NULL);
		}
		//刷新完按钮指针切回到输入框里
		form_driver(my_form, REQ_END_LINE);
		
		//处理登录或退出事件
		if(choice == 1) {
			break;//退出系统跳出循环
		} else if(choice == 0) {
			choice = -1;
			
			char *userid = field_buffer(field[0],0);
			char *password = field_buffer(field[1],0);
			//set_field_buffer(field[0], 0, password);//测试显示出来
			
			std::string uid = std::string(field_buffer(field[0],0));
			std::string upsw = std::string(field_buffer(field[1],0));
			
			std::string id = Unit::trim(uid);
			//输入为空
			if(id.length()==0 || Unit::trim(upsw).length()==0) {
				set_field_buffer(field[0], 0, "empty!");
				continue;
			}
			
			
			doLogin(userid, password);
			
			sleep(1);//尽可能等sqlite3回调完成
			
			if(isLogined == 1) {
				this->userid = new char[id.length()+1];
 				strcpy(this->userid, id.c_str());
				break;//成功登录跳出循环
			} else {
				set_field_buffer(field[0], 0, "wrong password");
			}
		}
	}
	/* 取消表单递送并释放内存 */
	unpost_form(my_form);
	free_form(my_form);
	free_field(field[0]);
	free_field(field[1]);
	
	keypad(my_form_win, FALSE);
	
	if(isLogined == 1) {
		//登录成功
		return 1;
	} else {
		return 0;
	}
}

int login_callback(void *arg, int nr, char **values, char **names) {
	Login *login = (Login *)arg;
 	if(nr > 0) {
 		login->isLogined = 1;
 		//values为局部变量，重新分配内存
 		login->username = new char[strlen(values[0])+1];
 		strcpy(login->username, values[0]);
 		login->usertype = atoi(values[1]);
 		//mvprintw(2,1,login->username);
		//mvprintw(3,1,login->usertype);
		//refresh();
 	}
	return 0;
}

void Login::doLogin(const char *userid, const char *password) {
	DB_Manager db;
	db.db_open("pos.db");
	std::string uid = std::string(userid);
	std::string upsw = std::string(password);
	std::string sql = "select user_name,user_type from user where user_id='" + Unit::trim(uid) + "' and user_password='" + Unit::trim(upsw) + "'";
	//mvprintw(1,1,sql.c_str());
	//refresh();
	db.db_exec(sql.c_str(), login_callback, this);
	db.db_close();
}
