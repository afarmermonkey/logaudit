client:client.cpp CClient.cpp CClient.h
	g++ $^ -o $@ -I ../ccmysql -lccmysql
	./$@ 127.0.0.1
clean:
	rm -rf *.o
	rm -rf client
