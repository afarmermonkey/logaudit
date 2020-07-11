#define LOGPATH "/var/log/auth.log"
#include "CAgent.h"
#include<string>
#include<iostream>
#include<thread>
#include<string.h>
using namespace std;
using namespace cc;
//��ȡ����ip��ַ
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
	//������ַ
	ifaddrs* iter = ifadd;
	while (iter!=NULL)
	{
		//ipv4
		if(iter->ifa_addr->sa_family==AF_INET)
		if (strcmp(iter->ifa_name, "lo" )!= 0)//ȥ���ػ���ַ 127.0.0.1
		{
			//תΪ����ipά�ַ���			
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

	//��ȡ��־
	fp = fopen(LOGPATH,"rb");
	if (!fp)
	{
		cerr << "fopen log " << LOGPATH << " failed!" << endl;
		return false;
	}
	cout << "fopen log " << LOGPATH << " success!" << endl;
	//ֻ���ϵͳ��ʼ����֮����¼�

	//�ļ��Ƶ���β��
	fseek(fp, 0, SEEK_END);

	//������־��t_log
	//string sql = "CREATE TABLE IF NOT EXISTS `t_log`(\
	//			`id` INT AUTO_INCREMENT,\
	//			`ip` VARCHAR(16),\
	//			`log` VARCHAR(4096),\
	//			`log_time` datetime,\
	//			PRIMARY KEY(`id`))";
	//mysql->Query(sql.c_str());
	return true;
}
//��ѭ��
void CAgent::Run()
{
	//��ȡ������־
	string log;
	for (;;)
	{
		char c = fgetc(fp);
		
		//̫��cpu
		if (c <= 0)//û���ַ��������β
		{
			this_thread::sleep_for(200ms);  //��Լcpu
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
// ��־д�����ݿ�
bool CAgent::SaveLog(std::string log)
{
	SQLDATA logData;
	logData["log"] = log.c_str();
	logData["ip"] = local_ip.c_str();

	//����ʱ�䣬mysql now()
	//@��ʾ �ֶ����ݲ������� @���Զ�ȥ��
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