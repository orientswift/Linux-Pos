#include"DB_Manager.h"
#include<iostream>

//sqlite3_open(const char *filename, sqlite3 **ppDb);  
//sqlite3_close(sqlite3 *db);  
//sqlite3_exec(sqlite3*, const char *sql, int (*callback)(void*,int,char**,char**), void *, char **errmsg); 

int DB_Manager::db_open(const char *db_name) {
	if(db_name == 0) return -1;
	return sqlite3_open(db_name, &db);
}

int DB_Manager::db_close() {
	int result;
	if(db == 0) {
		return 1;
	}
	//std::cout<<"close..."<<std::endl;
	result = sqlite3_close(db);
	db = 0;
	return result;
}
	
int DB_Manager::db_exec(const char *sql, int (*callback)(void *arg, int nr, char **values, char **names), void *arg) {
	if(sql == 0) return -1;
	if(db == 0) return -1;
	return sqlite3_exec(db, sql, callback, arg, 0);
}

DB_Manager::DB_Manager() {
	db = 0;
}

DB_Manager::~DB_Manager() {
	db_close();
}
