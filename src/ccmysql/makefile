all:libccmysql.so  install
libccmysql.so:CCData.cpp ccmysql.cpp ccmysql.h CCData.h
	g++ -fPIC -shared $^ -o $@ -I/usr/include/mysql -lmysqlclient
install:
	cp libccmysql.so /usr/lib/
	@echo "install libccmysql.so success!"
clean:
	rm -rf *.so
	rm -rf /usr/lib/libccmysql.so
