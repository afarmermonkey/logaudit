#ifndef _CCENTER_H
#define _CCENTER_H
#include<string>
#include "ccmysql.h"
class CCenter
{
public:
	static CCenter* Get()
	{
		static CCenter center;
		return &center;
	}

	
	~CCenter();
	bool Install(std::string ip);
	//初始化数据库
	bool Inint();
	//添加设备
	bool AddDevice(std::string ip,std::string name);
	void Run();
private:
	CCenter();
	cc::CCMysql *mysql = 0;
};

#endif

