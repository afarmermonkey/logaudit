#define LOGPATH "/var/log/auth.log"
#include "CAgent.h"
#include<string>
#include<iostream>
#include<thread>
#include<string.h>
using namespace std;
using namespace cc;
//获取本机ip地址
#ifndef _WIN32
#include<ifaddrs.h>
#include<arpa/inet.h>
#endif // !_WIN32
string CAgent::GetLocalIp()
{
	char ip[16] = { 0 };
#ifndef _WIN32
	ifaddrs* ifadd = 0;
	if (getifaddrs(&ifadd) != 0)return "";
	//遍历地址
	ifaddrs* iter = ifadd;
	while (iter!=NULL)
	{
		//ipv4
		if(iter->ifa_addr->sa_family==AF_INET)
		if (strcmp(iter->ifa_name, "lo" )!= 0)//去掉回环地址 127.0.0.1
		{
			//转为整形ip维字符串			
			void *tmp = &((sockaddr_in*)iter->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmp, ip, INET_ADDRSTRLEN);
			break;
		}
		iter = iter->ifa_next;
	}
	freeifaddrs(ifadd);
#endif // !_WIN32
	return ip;
}
bool CAgent::init(string ip)
{
	local_ip = GetLocalIp();
	if (ip.empty())
	{
		cout << "CAgent::init failed! ip is empty!" << endl;
		return false;
	}
	mysql = new CCMysql();
	if (!mysql->Connect(ip.c_str(), "root", "123456", "zwq"))
	{
		cerr << "CAgent::init failed! Connect failed!" << endl;
		return false;
	}
	cout << "Connect success!" << endl;

	//读取日志
	fp = fopen(LOGPATH,"rb");
	if (!fp)
	{
		cerr << "fopen log " << LOGPATH << " failed!" << endl;
		return false;
	}
	cout << "fopen log " << LOGPATH << " success!" << endl;
	//只审计系统开始运行之后的事件

	//文件移到结尾处
	fseek(fp, 0, SEEK_END);

	//创建日志表t_log
	//string sql = "CREATE TABLE IF NOT EXISTS `t_log`(\
	//			`id` INT AUTO_INCREMENT,\
	//			`ip` VARCHAR(16),\
	//			`log` VARCHAR(4096),\
	//			`log_time` datetime,\
	//			PRIMARY KEY(`id`))";
	//mysql->Query(sql.c_str());
	return true;
}
//主循环
void CAgent::Run()
{
	//读取最新日志
	string log;
	for (;;)
	{
		char c = fgetc(fp);
		
		//太耗cpu
		if (c <= 0)//没有字符或读到结尾
		{
			this_thread::sleep_for(200ms);  //节约cpu
			continue;
		}
		if (c == '\n')  
		{
			SaveLog(log);
			cout << log << endl;
			log = "";
			continue;
		}
		log += c;
	}
}
// 日志写入数据库
bool CAgent::SaveLog(std::string log)
{
	SQLDATA logData;
	logData["log"] = log.c_str();
	logData["ip"] = local_ip.c_str();

	//插入时间，mysql now()
	//@表示 字段内容不加引号 @会自动去除
	logData["@log_time"] = "now()";
	mysql->Insert(logData,"t_log");
	return true;
}
CAgent::~CAgent()
{
	mysql->Close();
	delete mysql;
}
CAgent::CAgent()
{
	
}