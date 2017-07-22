#ifndef LOGIN_H
#define LOGIN_H
#include<string>

class Login {
public:
	Login();
	~Login();
	int show();//显示整个登录窗口；返回1表示登录成功、0表示退出系统
	
	int isLogined;//是否登录成功标志
	char *username;//登录成功的用户姓名
	int usertype;//登录成功的用户类型
	char *userid;//登录成功的用户id
	
private:
	int showMiddleWin(const char *wintitle);//显示登录窗口中间的登录框
	void doLogin(const char *userid, const char *password);//连接数据库处理登录请求
};

#endif
