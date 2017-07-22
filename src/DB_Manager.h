#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include<sqlite3.h>

//sqlite3_open(const char *filename, sqlite3 **ppDb);  
//sqlite3_close(sqlite3 *db);  
//sqlite3_exec(sqlite3*, const char *sql, int (*callback)(void*,int,char**,char**), void *, char **errmsg); 

class DB_Manager {
public:
	DB_Manager();
	
	~DB_Manager();
	
	int db_open(const char *db_name);
	
	int db_close();
	
	int db_exec(const char *sql, int (*callback)(void*, int , char**, char**), void *arg);
	
private:	
	sqlite3 *db;
};

#endif
