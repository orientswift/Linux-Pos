#include"shop.h"
#include<cstring>
#include"DB_Manager.h"
#include<form.h>
#include<time.h>
#include<cstdlib>
#include"unit.h"

Shop::Shop(char *userid, char *username) {
	this->userid = new char[strlen(userid)+1];
	strcpy(this->userid, userid);
	
	this->username = new char[strlen(username)+1];
	strcpy(this->username, username);
	
	
	this->shopno = 0;
	this->shoptime = 0;
	//this->shopprice = new char[30];
	
	getTimeAndNo();
}

Shop::~Shop() {
	if(userid != NULL) {
		delete[] userid;
		userid = 0;
	}
	if(username != NULL) {
		delete[] username;
		username = 0;
	}
	if(shoptime != 0) {
		delete[] shoptime;
		username = 0;
	}
	if(shopno != 0) {
		delete[] shopno;
		shopno = 0;
	}
	//if(shopprice != 0) {
	//	delete[] shopprice;
	//	shopprice = 0;
	//}
}

void Shop::getTimeAndNo() {
	/*获取时间*/
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char t1[15] = {0};
	sprintf(t1, "%04d%02d%02d%02d%02d%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	
	if(shoptime != 0)
		delete[] shoptime;
	shoptime = new char[15];
	strcpy(shoptime, t1);
	
	/*获取单号*/
	srand(time(NULL));
	int no = rand()%9999+1;
	if(shopno != 0)
		delete[] shopno;
	char t2[5] = {0};
	sprintf(t2, "%04d", no);
	shopno = new char[5];
	strcpy(shopno, t2);
	
	DB_Manager db;
	db.db_open("pos.db");
	char sql[100] = {0};
	sprintf(sql, "insert into sale_total values(%s,'%s','%s%s','%s',0,0,0,0)", shopno, userid, shoptime, shopno, shoptime);
	
	if(db.db_exec(sql, 0, 0) != SQLITE_OK) {
		getTimeAndNo();//若随机获取的单号已存在则再次获取一次
	}
	
	db.db_close();
}

void Shop::show() {
	if(this->username == NULL)
		return;
	
	//绘制界面外框
	WINDOW *win = newwin(LINES-2, COLS, 1, 0);
	wbkgd(win,COLOR_PAIR(0));//设置窗口背景颜色
	box(win, 0, 0);
	wrefresh(win);
	
	//界面内框显示信息
	int win_x = COLS-2;
	int win_y = LINES-4;
	WINDOW *shop_win = newwin(win_y, win_x, 2, 1);
	keypad(shop_win, TRUE);
	
	getList();
	
	//输入框表单
	FIELD *field[3];
	FORM *my_form;
	int rows, cols;
	field[0] = new_field(1, 8, 0, 10, 0, 0);
	field[1] = new_field(1, 5, 0, 28, 0, 0);
	field[2] = NULL;
	
	/* 设置表单域 */
	set_field_type(field[0], TYPE_ALNUM, 8);
	set_field_fore(field[0], COLOR_PAIR(4));
	set_field_back(field[0], COLOR_PAIR(4));
	field_opts_on(field[0], O_AUTOSKIP);
	set_field_buffer(field[0], 0, "CM");
	
	set_field_type(field[1], TYPE_NUMERIC, 1);
	set_field_fore(field[1], COLOR_PAIR(4));
	set_field_back(field[1], COLOR_PAIR(4));
	field_opts_off(field[1], O_AUTOSKIP);
	
	/* 创建并递送表单 */
	my_form = new_form(field);
	scale_form(my_form, &rows, &cols);
	
	/* 设置主窗口和子窗口 */
	set_form_win(my_form, shop_win);
	set_form_sub(my_form, derwin(shop_win, rows, cols, win_y-3, 0));
	
	post_form(my_form);
	wrefresh(win);
	
	int ch, exit=0;
	int page = 1;
	
	while(exit == 0) {
		showList(shop_win, win_x, win_y, page);
		
		//刷新完切回到输入框里
		form_driver(my_form, REQ_END_LINE);
		
		ch = wgetch(shop_win);
		switch(ch) {
		case KEY_DOWN:
			page++;
			wclear(shop_win);
			wrefresh(shop_win);
			//刷新完输入框被清空显示，设置一下可恢复颜色
			set_field_buffer(field[0], 0, field_buffer(field[0],0));
			set_field_buffer(field[1], 0, field_buffer(field[1],0));
			break;
		case KEY_UP:
			if(page > 1) {
				page--;
				wclear(shop_win);
				wrefresh(shop_win);
				//刷新完输入框被清空显示，设置一下可恢复颜色
				set_field_buffer(field[0], 0, field_buffer(field[0],0));
				set_field_buffer(field[1], 0, field_buffer(field[1],0));
			}
			break;
		case KEY_RIGHT:
			/* 跳到下一个域 */
			form_driver(my_form, REQ_NEXT_FIELD);
			/* 跳到当前缓冲区的末尾 */
			/* 在最后一个字符处跳出 */
			form_driver(my_form, REQ_END_LINE);
			break;
		case KEY_LEFT:
			/* 跳到上一个域 */
			form_driver(my_form, REQ_PREV_FIELD);
			form_driver(my_form, REQ_END_LINE);
			break;
		case KEY_BACKSPACE:
			form_driver(my_form, REQ_DEL_LINE);
			break;
		case KEY_F(2):
			{//删除条目
			if(sales.size() < 1) {
				keypad(shop_win, FALSE);
				showTips("当前没有销售商品条目");
				keypad(shop_win, TRUE);
				break;
			}
			keypad(shop_win, FALSE);
			showDeleteDialog();
			keypad(shop_win, TRUE);
			
			wclear(shop_win);
			wrefresh(shop_win);
			//刷新完输入框被清空显示，设置一下可恢复颜色
			set_field_buffer(field[0], 0, field_buffer(field[0],0));
			set_field_buffer(field[1], 0, field_buffer(field[1],0));
			break;
			}
		case KEY_F(4):
			{//收银
			if(sales.size() < 1) {
				keypad(shop_win, FALSE);
				showTips("当前没有销售商品");
				keypad(shop_win, TRUE);
				break;
			}
			keypad(shop_win, FALSE);
			showCheckoutDialog();
			keypad(shop_win, TRUE);
						
			wclear(shop_win);
			wrefresh(shop_win);
			//刷新完输入框被清空显示，设置一下可恢复颜色
			set_field_buffer(field[0], 0, field_buffer(field[0],0));
			set_field_buffer(field[1], 0, field_buffer(field[1],0));
			break;
			}
		case KEY_F(5):
			{//撤单
			undo();
			
			wclear(shop_win);
			wrefresh(shop_win);
			//刷新完输入框被清空显示，设置一下可恢复颜色
			set_field_buffer(field[0], 0, field_buffer(field[0],0));
			set_field_buffer(field[1], 0, field_buffer(field[1],0));
			break;
			}
		case KEY_F(7):
			{//退出登录
			//若当前有商品条目则先撤单或收银后才能退出
			if(sales.size() > 0) {
				keypad(shop_win, FALSE);
				showTips("有未完成交易不能退出");
				keypad(shop_win, TRUE);
				break;
			}
			//退出时取消当前的交易单号数据库信息
			DB_Manager db;
			db.db_open("pos.db");
			char sql1[128] = {0};
			sprintf(sql1, "delete from sale_total where sale_total_id=%s", shopno);
			db.db_exec(sql1, 0, 0);
			db.db_close();
			exit = 1;
			break;
			}
		case 10:
			{//按回车键添加销售条目
			string code_temp(field_buffer(field[0],0));
			string num_temp(field_buffer(field[1],0));
			
			set_field_buffer(field[1], 0, "");
			set_field_buffer(field[0], 0, "CM");
			set_current_field(my_form, field[0]);
			
			string code = Unit::trim(code_temp);
			string num = Unit::trim(num_temp);
			if(code.length() != 8 || num.length() == 0) {
				break;
			}
			
			DB_Manager db;
			db.db_open("pos.db");
			//查询并添加条目
			char sql[256] = {0};
			sprintf(sql, "insert into sale_detail select null,%s,g.goods_code,%d,g.goods_price*g.goods_discount*%d from goods as g where g.goods_code='%s'", shopno, atoi(num.c_str()), atoi(num.c_str()), code.c_str());
			db.db_exec(sql, 0, 0);
			db.db_close();
			
			getList();//刷新数据
			break;
			}
		default:
			/* 如果是一个普通的字符，则打印出来 */
			form_driver(my_form, ch);
			break;
		}
		
	}
	
	wclear(shop_win);
	wrefresh(shop_win);
	wclear(win);
	wrefresh(win);
	keypad(shop_win, FALSE);
}

void Shop::showCheckoutDialog() {
	FIELD *field[2];
	FORM *my_form;
	WINDOW *my_form_win;
	int ch, rows, cols;

	//两个按钮的状态
	int highlight = 0;//高亮
	int choice = -1;//选中
	
	/* 初始化域 */
	field[0] = new_field(1, 8, 1, 5, 0, 0);//id
	field[1] = NULL;
	
	/* 设置表单域选项*/
	set_field_type(field[0], TYPE_NUMERIC, 8);/*检查输入类型为数字且最少有8位数*/
	set_field_fore(field[0], COLOR_PAIR(2));
	set_field_back(field[0], COLOR_PAIR(2));
	
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
	mvwprintw(my_form_win,1,2,"总价:");
	char price_temp[20] = "0";
	sprintf(price_temp, "%.2f", shopprice);
	wprintw(my_form_win, price_temp);
	mvwprintw(my_form_win,3,2,"实收:");

	mvwprintw(my_form_win,5,10,"取消");
	wattroff(my_form_win, COLOR_PAIR(1));
	//默认选中确认键
	wattron(my_form_win, COLOR_PAIR(3));
	mvwprintw(my_form_win,5,3,"收银");
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
			break;
		default:
			/* 如果是一个普通的字符，则打印出来 */
			form_driver(my_form, ch);
			break;
		}
		
		//刷新按钮状态
		if(highlight == 1) {
			mvwchgat(my_form_win, 5, 3, 4, A_BLINK, 1, NULL);
			mvwchgat(my_form_win, 5, 10, 4, A_BLINK, 3, NULL);
		} else {
			mvwchgat(my_form_win, 5, 3, 4, A_BLINK, 3, NULL);
			mvwchgat(my_form_win, 5, 10, 4, A_BLINK, 1, NULL);
		}
		//刷新完按钮指针切回到输入框里
		form_driver(my_form, REQ_END_LINE);
		
		//处理取消或收银事件
		if(choice == 1) {
			break;//取消跳出循环
		} else if(choice == 0) {
			choice = -1;
			
			std::string cost_temp = std::string(field_buffer(field[0],0));
			string cost = Unit::trim(cost_temp);
			double cost_d = atof(cost.c_str());
			//输入为空或收款小于总价
			if(cost.length() == 0 || cost_d < shopprice) {
				continue;
			}
			double change_d = cost_d - shopprice;
			char change[20] = {0};
			sprintf(change, "%.2f", change_d);
			
			std::string sql = "update sale_total set sale_total_price="+string(price_temp)+",sale_total_money="+cost+",sale_total_change="+string(change)+",sale_total_state=1 where sale_total_id="+shopno;
			
			DB_Manager db;
			db.db_open("pos.db");
			
			db.db_exec(sql.c_str(), 0, 0);
			db.db_close();
			
			//操作完显示一个迷你信息框
			wclear(my_form_win);//先清空输入窗口
			box(my_form_win, 0, 0);//绘制窗口边框
			mvwprintw(my_form_win,1,2,"总价:");
			wprintw(my_form_win, price_temp);
			mvwprintw(my_form_win,3,2,"实收:");
			wprintw(my_form_win, cost.c_str());
			mvwprintw(my_form_win,5,2,"找零:");
			wprintw(my_form_win, change);
			wrefresh(my_form_win);//刷新显示以上几条操作
			wgetch(my_form_win);
			
			//获取刷新后的数据
			getTimeAndNo();
			getList();
			break;
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

void Shop::showDeleteDialog() {
	FIELD *field[2];
	FORM *my_form;
	WINDOW *my_form_win;
	int ch, rows, cols;

	//两个按钮的状态
	int highlight = 0;//高亮
	int choice = -1;//选中
	
	/* 初始化域 */
	field[0] = new_field(1, 8, 1, 3, 0, 0);//id
	field[1] = NULL;
	
	/* 设置表单域选项*/
	set_field_type(field[0], TYPE_NUMERIC, 8);/*检查输入类型为数字且最少有8位数*/
	set_field_fore(field[0], COLOR_PAIR(2));
	set_field_back(field[0], COLOR_PAIR(2));
	
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
	mvwprintw(my_form_win,1,(cols+5)/2-4,"删除条目");
	mvwprintw(my_form_win,3,2,"id:");

	mvwprintw(my_form_win,5,8,"取消");
	wattroff(my_form_win, COLOR_PAIR(1));
	//默认选中确认键
	wattron(my_form_win, COLOR_PAIR(3));
	mvwprintw(my_form_win,5,3,"删除");
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
			break;
		default:
			/* 如果是一个普通的字符，则打印出来 */
			form_driver(my_form, ch);
			break;
		}
		
		//刷新按钮状态
		if(highlight == 1) {
			mvwchgat(my_form_win, 5, 3, 4, A_BLINK, 1, NULL);
			mvwchgat(my_form_win, 5, 8, 4, A_BLINK, 3, NULL);
		} else {
			mvwchgat(my_form_win, 5, 3, 4, A_BLINK, 3, NULL);
			mvwchgat(my_form_win, 5, 8, 4, A_BLINK, 1, NULL);
		}
		//刷新完按钮指针切回到输入框里
		form_driver(my_form, REQ_END_LINE);
		
		//处理取消或删除事件
		if(choice == 1) {
			break;//取消跳出循环
		} else if(choice == 0) {
			choice = -1;
			
			std::string detailid = std::string(field_buffer(field[0],0));
			
			//输入为空
			if(Unit::trim(detailid).length() == 0) {
				continue;
			}
			
			std::string sql = "delete from sale_detail where sale_detail_id="+detailid + " and sale_total_id=" + shopno;
			
			DB_Manager db;
			db.db_open("pos.db");
			
			//mvprintw(1,1,sql.c_str());//测试输出sql语句
			//refresh();
			
			db.db_exec(sql.c_str(), 0, 0);
			db.db_close();
			
			getList();//获取刷新后的数据
			break;
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

void Shop::undo() {
	if(sales.size() < 1) {//当前没有交易无需任何操作
		return;
	}
	
	DB_Manager db;
	db.db_open("pos.db");
	
	char sql[128] = {0};
	sprintf(sql, "delete from sale_detail where sale_total_id=%d", atoi(shopno));
	db.db_exec(sql, 0, 0);
	db.db_close();
	
	getList();
}

//分页显示销售条目
void Shop::showList(WINDOW *win, int win_x, int win_y, int page) {
	//销售单信息栏
	mvwprintw(win, 0, 0, "收银员:");
	wprintw(win, username);
	wprintw(win, "   交易号:");
	wprintw(win, shopno);
	wprintw(win, "   交易日期:");
	wprintw(win, shoptime);
	wprintw(win, "  总价:");
	char price_temp[20] = "0";
	sprintf(price_temp, "%.2f", shopprice);
	wprintw(win, price_temp);
	
	//分割线
	mvwhline(win, 1, 0, '_', win_x);
	//提示
	//1|CM000001|洗衣粉|1000克|袋|20|1|1|20
	mvwprintw(win, 2, 0, "id:商品条码-名称-规格:单价*折扣*数量=总价");
	wprintw(win, "  <第");   
	char t[10] = "0";
	sprintf(t, "%d", page);
	wprintw(win, t);
	wprintw(win, "页>");
	
	mvwhline(win, win_y-4, 0, '-', win_x);
	//输入栏
	mvwprintw(win, win_y-3, 0, "商品条码:");
	mvwprintw(win, win_y-3, 23, "数量:");
	mvwprintw(win, win_y-3, 33, "   (按回车添加)");
	//菜单提示栏
	mvwhline(win, win_y-2, 0, '=', win_x);
	mvwprintw(win, win_y-1, 0, "F2:删除条目  F4:收银  F5:撤单  F7:退出  ↑↓:换页");
	
	//(page-1)*(win_y-7) 为当前页起始数从0开始，其中win_y-3 为当前能容纳行数
	int start = (page-1)*(win_y-7);
	
	if(start >= sales.size() || page < 1){//越数没有数据显示
		return;
	}
	
	int i = 0;
	for(myiterator = sales.begin(); myiterator != sales.end() && i < start; myiterator++) {	
		i++;
	}
	
	int y = 3;
	for(; myiterator != sales.end() && y < win_y-4; myiterator++){
		mvwprintw(win, y, 0, (*myiterator).c_str());
		y++;
	}
	
	wrefresh(win);
}
//获取当前交易单号的销售商品总价
int shopprice_callback(void *arg, int nr, char **values, char **names) {
	double *price = (double *)arg;
	*price = atof(values[0]);
	return 0;
}

int shoplist_callback(void *arg, int nr, char **values, char **names) {
	list<string> *mylist = (list<string> *)arg;
	//1|CM000001|洗衣粉|1000克|袋|20|1|1|20
	string id = values[0];
	string code = values[1];
	string name = values[2];
	string size = values[3];
	string unit = values[4];
	string price = values[5];
	string discount = values[6];
	string count = values[7];
	string total_price = values[8];
	string sale_item = id + ":" + code + "-" + name + "-" + size + "/" + unit + ":" + price + "*" + discount + "*" + count + "=" + total_price;
 	mylist->push_back(sale_item);
	return 0;
}

void Shop::getList() {
	sales.clear();
	
	DB_Manager db;
	db.db_open("pos.db");
	
	char sql[256] = {0};
	sprintf(sql, "select s.sale_detail_id,s.goods_code,g.goods_name,g.goods_size,g.goods_unit,g.goods_price,g.goods_discount,s.sale_detail_count,s.sale_detail_price from sale_detail as s,goods as g where s.sale_total_id=%d and s.goods_code=g.goods_code", atoi(shopno));
	db.db_exec(sql, shoplist_callback, &sales);

	//获取商品总价，注意ifnull，sql中sum函数没有结果也会返回null所以会执行回调函数，ifnull函数使其若为null时返回0
	char sql1[128] = {0};
	sprintf(sql1, "select ifnull(sum(s.sale_detail_price),0) from sale_detail as s where s.sale_total_id=%d", atoi(shopno));
	db.db_exec(sql1, shopprice_callback, &shopprice);
	
	db.db_close();
}

void Shop::showTips(char *tips) {
	/* 创建窗口 */
	WINDOW *win = newwin(3, strlen(tips)-5, (LINES-4)/2, (COLS-(strlen(tips)-5))/2);
	keypad(win, TRUE);
	wbkgd(win,COLOR_PAIR(1));//设置窗口背景颜色
	box(win, 0, 0);/* 在窗口的四周显示边框 */
	
	/* 在窗口显示提示信息 */
	wattron(win, COLOR_PAIR(1));
	mvwprintw(win,1,1,tips);
	wattroff(win, COLOR_PAIR(1));
	wrefresh(win);
	
	wgetch(win);
		
	//清空窗口取消监听
	wbkgd(win,COLOR_PAIR(0));//设置窗口背景颜色
	wclear(win);
	wrefresh(win);
	keypad(win, FALSE);
}
