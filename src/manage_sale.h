#ifndef MANAGE_SALE_H
#define MANAGE_SALE_H
#include<list>
#include<string>
#include<ncurses.h>

using namespace std;

class Manage_Sale {
public:
	Manage_Sale();
	~Manage_Sale();
	void show();
	
private:
	void getList(const char *sql);//获取销售表列表数据
	void showList(WINDOW *win, int win_x, int win_y, int page);//指定窗口显示销售列表
	
	void showSearchDialog();//显示条件搜索输入窗口
	
	string sql;
	
	list<string> sales;
	list<string>::iterator myiterator;
};

#endif
