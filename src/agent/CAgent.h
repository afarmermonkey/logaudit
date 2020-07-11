#ifndef _CAGENT_H
#define _CAGENT_H
#include "ccmysql.h"
#include<string>
class CAgent
{
public:
   inline static CAgent* Get()
	{
		static CAgent agent;
		return &agent;
	}
	//agent模块初始化，只能调用一次，在所有接口调用之前调用
	bool init(std::string ip);
	
	//主循环
	void Run();
	// 日志写入数据库
	bool SaveLog(std::string log);
	//获取本机ip地址
	static std::string GetLocalIp();
	~CAgent();
private:
	CAgent();
	//数据库对象
	cc::CCMysql *mysql=0;
	//打开日志文件
	FILE* fp = 0;
	std::string local_ip;
};
#endif
