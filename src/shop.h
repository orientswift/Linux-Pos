#ifndef SHOP_H
#define SHOP_H

#include<ncurses.h>
#include<list>
#include<string>

using namespace std;

class Shop {
public:
	Shop(char *userid, char *username);
	~Shop();
	void show();
	
private:
	char *userid;
	char *username;
	
	char *shopno;//交易单号
	char *shoptime;//交易时期
	void getTimeAndNo();//获取单号和时间
	
	void getList();
	void showList(WINDOW *win, int win_x, int win_y, int page);
	
	double shopprice;//当前交易总价
	list<string> sales;
	list<string>::iterator myiterator;
	
	void showTips(char *tips);//显示提示信息
	void showDeleteDialog();//显示窗口删除销售条目
	void showCheckoutDialog();//收银结单
	void undo();//撤单
	
	
};

#endif
