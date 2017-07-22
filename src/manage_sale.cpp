#include"manage_sale.h"
#include<form.h>
#include<cstring>
#include"DB_Manager.h"
#include"unit.h"

Manage_Sale::Manage_Sale() {
	sql = string("select * from sale_total");
}

Manage_Sale::~Manage_Sale() {

}

void Manage_Sale::show() {
	int win_x = COLS-2;
	int win_y = LINES-4;
	WINDOW *sale_win = newwin(win_y, win_x, 2, 1);
	
	keypad(sale_win, TRUE);
	int ch, exit=0;
	int page = 1;
	
	getList(sql.c_str());
	
	while(exit == 0) {
		showList(sale_win, win_x, win_y, page);
		
		ch = wgetch(sale_win);
		switch(ch) {
		case KEY_DOWN:
			page++;
			wclear(sale_win);
			wrefresh(sale_win);
			break;
		case KEY_UP:
			if(page > 1) {
				wclear(sale_win);
				wrefresh(sale_win);
				page--;
			}
			break;
		/*弹出搜索窗口*/
		case KEY_F(2):
			keypad(sale_win, FALSE);
			showSearchDialog();
			getList(sql.c_str());
			wclear(sale_win);
			wrefresh(sale_win);
			keypad(sale_win, TRUE);
			break;
		case KEY_F(3):
			sql = string("select * from sale_total");
			getList(sql.c_str());
			wclear(sale_win);
			wrefresh(sale_win);
			break;
		case KEY_F(5):
			getList(sql.c_str());
			wclear(sale_win);
			wrefresh(sale_win);
			break;
		case 0x1b:
			exit = 1;
			break;
		default:
			break;
		}
		
	}
	
	wclear(sale_win);
	wrefresh(sale_win);
	
	keypad(sale_win, FALSE);
}

void Manage_Sale::showList(WINDOW *win, int win_x, int win_y, int page) {
	
	mvwprintw(win, 0, 0, "F2:条件搜索  F3默认排序  F5:刷新数据  ESC:返回上级  ↑↓:换页");
	wprintw(win, " <");   
	char t[10] = "0";
	sprintf(t, "%d", page);
	wprintw(win, t);
	wprintw(win, ">");
	mvwhline(win, 1, 0, '-', win_x);
	mvwprintw(win, 2, 0, "ID:雇员帐号:     单号     :     日期     :应付:实收:找零");
	
	
	//(page-1)*(win_y-3) 为当前页起始数从0开始，其中win_y-3 为当前能容纳行数
	int start = (page-1)*(win_y-3);
	
	if(start >= sales.size() || page < 1){//越数没有数据显示
		return;
	}
	
	int i = 0;
	for(myiterator = sales.begin(); myiterator != sales.end() && i < start; myiterator++) {	
		i++;
	}
	
	int y = 3;
	for(; myiterator != sales.end() && y < win_y; myiterator++){
		mvwprintw(win, y, 0, (*myiterator).c_str());
		y++;
	}
	
	wrefresh(win);
}

int salelist_callback(void *arg, int nr, char **values, char **names) {
	list<string> *mylist = (list<string> *)arg;
	
	string sale_total_id = values[0];//销售id
	string user_id = values[1];//雇员id
	string sale_total_number = values[2];//销售单号
	string sale_total_date = values[3];//销售日期
	string sale_total_price = values[4];//应付金额
	string sale_total_money = values[5];//实收金额
	string sale_total_change = values[6];//找零金额
	
	string sale_item = sale_total_id + ":" + user_id + ":" + sale_total_number + ":" + sale_total_date + ":" + sale_total_price + ":" + sale_total_money + ":" + sale_total_change;
	
 	mylist->push_back(sale_item);
	return 0;
}

void Manage_Sale::getList(const char *sql) {
	sales.clear();
	
	DB_Manager db;
	db.db_open("pos.db");
	
	db.db_exec(sql, salelist_callback, &sales);
	db.db_close();
}

void Manage_Sale::showSearchDialog() {
	WINDOW *win = newwin(5, 20, (LINES-5)/2, (COLS-20)/2);
	keypad(win, TRUE);
	wbkgd(win,COLOR_PAIR(1));//设置窗口背景颜色
	box(win, 0, 0);//绘制窗口边框
	
	mvwprintw(win, 1, 6, "条件搜索");//输出提示
	mvwprintw(win, 3, 2, "雇员帐号:");//输出提示
	mvwchgat(win, 3, 11, 6, A_BLINK, 2, NULL);//模拟出输入框
	wrefresh(win);//刷新显示以上几条操作
	
	char temp[7] = {0};
	echo();//打开回显
	mvwgetnstr(win, 3, 11, temp, 6);//获取用户输入
	noecho();//输入完关闭回显
	
	string id = string(temp);
	if(id.length() == 6) {
		string s = "select * from sale_total where user_id='"+id+"'";
		sql = s;
	}
	
	//清空窗口取消监听
	wbkgd(win,COLOR_PAIR(0));//设置窗口背景颜色
	wclear(win);
	wrefresh(win);
	keypad(win, FALSE);
}
