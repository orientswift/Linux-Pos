#include"manage_goods.h"
#include<form.h>
#include<cstring>
#include"DB_Manager.h"
#include"unit.h"

Manage_Goods::Manage_Goods() {
	
}

Manage_Goods::~Manage_Goods() {

}

void Manage_Goods::show() {
	int win_x = COLS-2;
	int win_y = LINES-4;
	WINDOW *goods_win = newwin(win_y, win_x, 2, 1);
	
	getList();
	
	keypad(goods_win, TRUE);
	int ch, exit=0;
	int page = 1;
	
	while(exit == 0) {
		showList(goods_win, win_x, win_y, page);
		
		ch = wgetch(goods_win);
		switch(ch) {
		case KEY_DOWN:
			page++;
			wclear(goods_win);
			wrefresh(goods_win);
			break;
		case KEY_UP:
			if(page > 1) {
				wclear(goods_win);
				wrefresh(goods_win);
				page--;
			}
			break;
		/*弹出添加商品窗口*/
		case KEY_F(2):
			keypad(goods_win, FALSE);
			//显示添加商品窗口
			showAddGoodsDialog();
			getList();
			wclear(goods_win);
			wrefresh(goods_win);
			keypad(goods_win, TRUE);
			break;
		case KEY_F(4):
			keypad(goods_win, FALSE);
			//显示删除商品窗口
			showDeleteGoodsDialog();
			getList();
			wclear(goods_win);
			wrefresh(goods_win);
			keypad(goods_win, TRUE);
			break;
			break;	
		case KEY_F(5):
			getList();
			wclear(goods_win);
			wrefresh(goods_win);
			break;
		case 0x1b:
			exit = 1;
			break;
		default:
			break;
		}
		
	}
	
	wclear(goods_win);
	wrefresh(goods_win);
	
	keypad(goods_win, FALSE);
}

void Manage_Goods::showList(WINDOW *win, int win_x, int win_y, int page) {
	
	mvwprintw(win, 0, 0, "F2:添加商品  F4:删除商品  F5:刷新数据  ESC:返回上级  ↑↓:换页");
	wprintw(win, " <");   
	char t[10] = "0";
	sprintf(t, "%d", page);
	wprintw(win, t);
	wprintw(win, ">");
	
	mvwhline(win, 1, 0, '-', win_x);
	mvwprintw(win, 2, 0, "条形码 : 名称 :单位:规格:价格:成本:库存:折扣");
	
	//(page-1)*(win_y-3) 为当前页起始数从0开始，其中win_y-3 为当前能容纳行数
	int start = (page-1)*(win_y-3);
	
	if(start >= goods.size() || page < 1){//越数没有数据显示
		return;
	}
	
	int i = 0;
	for(myiterator = goods.begin(); myiterator != goods.end() && i < start; myiterator++) {	
		i++;
	}
	
	int y = 3;
	for(; myiterator != goods.end() && y < win_y; myiterator++){
		mvwprintw(win, y, 0, (*myiterator).c_str());
		y++;
	}
	
	wrefresh(win);
}

int goodslist_callback(void *arg, int nr, char **values, char **names) {
	list<string> *mylist = (list<string> *)arg;
	string code = values[0];
	string name = values[1];
	string unit = values[2];
	string size = values[3];
	string price = values[4];
	string cost = values[5];
	string num = values[6];
	string discount = values[7];
	string goods_item = code + ":" + name + ":" + unit + ":" + size + ":" + price + ":" + cost + ":" + num + ":" + discount;
 	mylist->push_back(goods_item);
	return 0;
}

void Manage_Goods::getList() {
	goods.clear();
	
	DB_Manager db;
	db.db_open("pos.db");
	
	db.db_exec("select * from goods", goodslist_callback, &goods);
	db.db_close();
}

void Manage_Goods::showAddGoodsDialog() {
	FIELD *field[7];
	FORM *my_form;
	WINDOW *my_form_win;
	int ch, rows, cols;

	//两个按钮的状态
	int highlight = 0;//高亮
	int choice = -1;//选中
	
	/* 初始化域 */
	field[0] = new_field(1, 8, 1, 4, 0, 0);//条形码
	field[1] = new_field(1, 16, 2, 4, 0, 0);//规格
	field[2] = new_field(1, 7, 3, 4, 0, 0);//价格
	field[3] = new_field(1, 7, 4, 4, 0, 0);//成本
	field[4] = new_field(1, 7, 5, 4, 0, 0);//库存
	field[5] = new_field(1, 7, 6, 4, 0, 0);//折扣
	field[6] = NULL;
	
	/* 设置表单域选项*/
	set_field_type(field[0], TYPE_ALNUM, 8);/*检查输入类型为字母或数字且最少有8位数*/
	set_field_fore(field[0], COLOR_PAIR(3));
	set_field_back(field[0], COLOR_PAIR(3));
	set_field_buffer(field[0], 0, "CM");
	field_opts_on(field[0], O_AUTOSKIP);/*如果当前表单域已满，自动跳到下一个表单域*/
	
	set_field_type(field[1], TYPE_ALNUM, 1);/*检查输入类型为字母或数字且最少有1位数*/
	set_field_fore(field[1], COLOR_PAIR(2));
	set_field_back(field[1], COLOR_PAIR(2));
	field_opts_off(field[1], O_AUTOSKIP);/*如果当前表单域已满，不自动跳到下一个表单域 */
	
	set_field_fore(field[2], COLOR_PAIR(3));
	set_field_back(field[2], COLOR_PAIR(3));
	field_opts_off(field[2], O_AUTOSKIP);/*如果当前表单域已满，不自动跳到下一个表单域 */
	
	set_field_fore(field[3], COLOR_PAIR(2));
	set_field_back(field[3], COLOR_PAIR(2));
	field_opts_off(field[3], O_AUTOSKIP);/*如果当前表单域已满，自动跳到下一个表单域*/
	
	set_field_fore(field[4], COLOR_PAIR(3));
	set_field_back(field[4], COLOR_PAIR(3));
	field_opts_off(field[4], O_AUTOSKIP);/*如果当前表单域已满，不自动跳到下一个表单域 */
	
	set_field_fore(field[5], COLOR_PAIR(2));
	set_field_back(field[5], COLOR_PAIR(2));
	field_opts_off(field[5], O_AUTOSKIP);/*如果当前表单域已满，不自动跳到下一个表单域 */
	
	/* 创建表单并传递 */
	my_form = new_form(field);
	/* 计算表单所需要的面积大小 */
	scale_form(my_form, &rows, &cols);
	
	/* 创建与表单相关联的窗口 */
	my_form_win = newwin(rows + 5, cols + 3, LINES/2-(rows+5)/2, COLS/2-(cols+1)/2);
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
	mvwprintw(my_form_win,1,(cols+3)/2-4,"添加商品");
	mvwprintw(my_form_win,3,1,"条码:");
	mvwprintw(my_form_win,4,1,"规格:");
	mvwprintw(my_form_win,5,1,"售价:");
	mvwprintw(my_form_win,6,1,"成本");
	mvwprintw(my_form_win,7,1,"库存");
	mvwprintw(my_form_win,8,1,"折扣");
	mvwprintw(my_form_win,10,13,"取消");
	wattroff(my_form_win, COLOR_PAIR(1));
	//默认选中确认键
	wattron(my_form_win, COLOR_PAIR(3));
	mvwprintw(my_form_win,10,5,"添加");
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
			mvwchgat(my_form_win, 10, 5, 4, A_BLINK, 1, NULL);
			mvwchgat(my_form_win, 10, 13, 4, A_BLINK, 3, NULL);
		} else {
			mvwchgat(my_form_win, 10, 5, 4, A_BLINK, 3, NULL);
			mvwchgat(my_form_win, 10, 13, 4, A_BLINK, 1, NULL);
		}
		//刷新完按钮指针切回到输入框里
		form_driver(my_form, REQ_END_LINE);
		
		//处理添加或取消事件
		if(choice == 1) {
			break;//取消
		} else if(choice == 0) {
			choice = -1;
			
			std::string code = std::string(field_buffer(field[0],0));
			std::string size = std::string(field_buffer(field[1],0));
			std::string price = std::string(field_buffer(field[2],0));
			std::string cost = std::string(field_buffer(field[3],0));
			std::string num = std::string(field_buffer(field[4],0));
			std::string discount = std::string(field_buffer(field[5],0));
			
			//输入为空
			if(Unit::trim(code).length() < 8 || Unit::trim(size).length() == 0 || Unit::trim(price).length() == 0 || Unit::trim(cost).length() == 0 || Unit::trim(num).length() == 0 || Unit::trim(discount).length() == 0) {
				set_field_buffer(field[0], 0, "empty!");
				continue;
			}
			
			std::string sql = "insert into goods values('"+code+"','无名','个','"+Unit::trim(size)+"',"+Unit::trim(price)+","+Unit::trim(cost)+","+Unit::trim(num)+","+Unit::trim(discount)+")";
			
			DB_Manager db;
			db.db_open("pos.db");
			
			//mvprintw(1,1,sql.c_str());//测试输出sql语句
			//refresh();
			
			if(db.db_exec(sql.c_str(), 0, 0) != SQLITE_OK) {
				set_field_buffer(field[0], 0, "exist!");
			} else {
			
				/*ncurses输入中文不能用field
				     因为上面循环默认wgetch单个字符
					所以单独mvwgetnstr输入中文
				)*/
				//输入商品名称
				wclear(my_form_win);//先清空输入窗口
				box(my_form_win, 0, 0);//绘制窗口边框
				mvwprintw(my_form_win, 2, 1, "输入商品名称并回车:");//输出提示
				mvwchgat(my_form_win, 3, 2, 15, A_BLINK, 2, NULL);//模拟出输入框
				wrefresh(my_form_win);//刷新显示以上几条操作
				
				char temp1[15] = {0};
				echo();//打开回显
				mvwgetnstr(my_form_win, 3, 2, temp1, 15);//获取用户输入
				noecho();//输入完关闭回显
				/*以上可封装成一个组件:char *test(WINDOW *win, char *title, int n)*/
				std::string name = std::string(temp1);
				std::string sql1 = "update goods set goods_name='"+Unit::trim(name)+"' where goods_code='"+code+"'";
				db.db_exec(sql1.c_str(), 0, 0);
				

				mvwprintw(my_form_win, 5, 1, "输入商品单位并回车:");//输出提示
				mvwchgat(my_form_win, 6, 2, 15, A_BLINK, 2, NULL);//模拟出输入框
				wrefresh(my_form_win);//刷新显示以上几条操作
				
				char temp2[15] = {0};
				echo();//打开回显
				mvwgetnstr(my_form_win, 6, 2, temp2, 15);//获取用户输入
				noecho();//输入完关闭回显

				std::string unit = std::string(temp2);
				std::string sql2 = "update goods set goods_unit='"+Unit::trim(unit)+"' where goods_code='"+code+"'";
				db.db_exec(sql2.c_str(), 0, 0);
				
				
				db.db_close();
				break;
			}
			
			db.db_close();
		}
	}
	/* 取消表单递送并释放内存 */
	unpost_form(my_form);
	free_form(my_form);
	free_field(field[0]);
	free_field(field[1]);
	free_field(field[2]);
	
	//清空窗口取消监听
	wbkgd(my_form_win,COLOR_PAIR(0));//设置窗口背景颜色
	wclear(my_form_win);
	wrefresh(my_form_win);
	keypad(my_form_win, FALSE);
}

void Manage_Goods::showDeleteGoodsDialog() {
	FIELD *field[2];
	FORM *my_form;
	WINDOW *my_form_win;
	int ch, rows, cols;

	//两个按钮的状态
	int highlight = 0;//高亮
	int choice = -1;//选中
	
	/* 初始化域 */
	field[0] = new_field(1, 8, 1, 8, 0, 0);//id
	field[1] = NULL;
	
	/* 设置表单域选项*/
	set_field_type(field[0], TYPE_ALNUM, 8);/*检查输入类型为数字且最少有6位数*/
	set_field_fore(field[0], COLOR_PAIR(2));
	set_field_back(field[0], COLOR_PAIR(2));
	set_field_buffer(field[0], 0, "CM");
	
	/* 创建表单并传递 */
	my_form = new_form(field);
	/* 计算表单所需要的面积大小 */
	scale_form(my_form, &rows, &cols);
	
	/* 创建与表单相关联的窗口 */
	my_form_win = newwin(rows + 5, cols + 5, LINES/2-(rows+5)/2, COLS/2-(cols+5)/2);
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
	mvwprintw(my_form_win,1,(cols+5)/2-4,"删除商品");
	mvwprintw(my_form_win,3,2,"条形码:");

	mvwprintw(my_form_win,5,11,"取消");
	wattroff(my_form_win, COLOR_PAIR(1));
	//默认选中确认键
	wattron(my_form_win, COLOR_PAIR(3));
	mvwprintw(my_form_win,5,5,"删除");
	wattroff(my_form_win, COLOR_PAIR(3));
	
	wrefresh(my_form_win);
	//刷新完按钮指针切回到输入框里
	form_driver(my_form, REQ_END_LINE);
	
	/* 循环以获取用户请求 */
	while(1) {
		ch = wgetch(my_form_win);
		switch(ch) {
		
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
			set_field_buffer(field[0], 0, "CM");
			break;
		default:
			/* 如果是一个普通的字符，则打印出来 */
			form_driver(my_form, ch);
			break;
		}
		
		//刷新按钮状态
		if(highlight == 1) {
			mvwchgat(my_form_win, 5, 5, 4, A_BLINK, 1, NULL);
			mvwchgat(my_form_win, 5, 11, 4, A_BLINK, 3, NULL);
		} else {
			mvwchgat(my_form_win, 5, 5, 4, A_BLINK, 3, NULL);
			mvwchgat(my_form_win, 5, 11, 4, A_BLINK, 1, NULL);
		}
		//刷新完按钮指针切回到输入框里
		form_driver(my_form, REQ_END_LINE);
		
		//处理删除或取消事件
		if(choice == 1) {
			break;//取消
		} else if(choice == 0) {
			choice = -1;
			
			std::string goods_code = std::string(field_buffer(field[0],0));
			
			//输入有误
			if(Unit::trim(goods_code).length() < 8) {

				continue;
			}
			
			std::string sql = "delete from goods where goods_code='"+goods_code+"'";
			
			DB_Manager db;
			db.db_open("pos.db");
			
			//mvprintw(1,1,sql.c_str());//测试输出sql语句
			//refresh();
			
			if(db.db_exec(sql.c_str(), 0, 0) != SQLITE_OK) {
				set_field_buffer(field[0], 0, "wrong!");
			} else {
				db.db_close();
				break;
			}
			
			db.db_close();
		}
	}
	/* 取消表单递送并释放内存 */
	unpost_form(my_form);
	free_form(my_form);
	free_field(field[0]);
	
	//清空窗口取消监听
	wbkgd(my_form_win,COLOR_PAIR(0));//设置窗口背景颜色
	wclear(my_form_win);
	wrefresh(my_form_win);
	keypad(my_form_win, FALSE);
}
