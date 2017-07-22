#ifndef MANAGE_GOODS_H
#define MANAGE_GOODS_H
#include<list>
#include<string>
#include<ncurses.h>

using namespace std;

class Manage_Goods {
public:
	Manage_Goods();
	~Manage_Goods();
	void show();
	
private:
	void showAddGoodsDialog();//显示添加商品对话框
	/*两个函数基本相似，重复代码比较多*/
	void showDeleteGoodsDialog();//显示删除商品对话框
	
	void getList();//获取商品列表数据
	void showList(WINDOW *win, int win_x, int win_y, int page);//指定窗口显示商品列表
	
	list<string> goods;
	list<string>::iterator myiterator;
};

#endif
