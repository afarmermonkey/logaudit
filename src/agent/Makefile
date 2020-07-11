agent:agent.cpp CAgent.cpp CAgent.h
	g++ $^ -o $@ -I ../ccmysql -lccmysql
	./$@ 127.0.0.1
clean:
	rm -rf *.o
	rm -rf agent
