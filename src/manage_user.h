#ifndef MANAGE_USER_H
#define MANAGE_USER_H
#include<list>
#include<string>
#include<ncurses.h>

using namespace std;

class Manage_User {
public:
	Manage_User();
	~Manage_User();
	void show();
	
private:
	void showAddUserDialog();//显示添加雇员对话框
	/*两个函数基本相似，重复代码比较多*/
	void showDeleteUserDialog();//显示删除雇员对话框
	
	void getList();//获取雇员列表数据
	void showList(WINDOW *win, int win_x, int win_y, int page);//指定窗口显示雇员列表
	
	list<string> users;
	list<string>::iterator myiterator;
};

#endif
