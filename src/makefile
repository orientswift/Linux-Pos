main : main.o DB_Manager.o login.o manage.o manage_goods.o manage_sale.o manage_user.o shop.o unit.o
	g++ -o main main.o DB_Manager.o login.o manage.o manage_goods.o manage_sale.o manage_user.o shop.o unit.o -lform -lsqlite3 -lncursesw

main.o : main.cpp manage.h shop.h
	g++ -c main.cpp

DB_Manager.o : DB_Manager.cpp DB_Manager.h
	g++ -c DB_Manager.cpp
	
login.o : login.cpp login.h DB_Manager.h unit.h
	g++ -c login.cpp

manage.o : manage.cpp manage.h manage_user.h manage_sale.h manage_goods.h
	g++ -c manage.cpp

manage_goods.o : manage_goods.cpp manage_goods.h DB_Manager.h unit.h
	g++ -c manage_goods.cpp

manage_sale.o : manage_sale.cpp manage_sale.h DB_Manager.h unit.h
	g++ -c manage_sale.cpp

manage_user.o : manage_user.cpp manage_user.h DB_Manager.h unit.h
	g++ -c manage_user.cpp

shop.o : shop.cpp shop.h DB_Manager.h unit.h
	g++ -c shop.cpp

unit.o : unit.cpp unit.h
	g++ -c unit.cpp

clean :
	rm main main.o DB_Manager.o login.o manage.o manage_goods.o manage_sale.o manage_user.o shop.o unit.o

#g++ main.cpp -o main manage.cpp login.cpp DB_Manager.cpp manage_user.cpp manage_goods.cpp manage_sale.cpp shop.cpp -lform -lsqlite3 -lncursesw

