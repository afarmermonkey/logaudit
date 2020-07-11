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
	//agentģ���ʼ����ֻ�ܵ���һ�Σ������нӿڵ���֮ǰ����
	bool init(std::string ip);
	
	//��ѭ��
	void Run();
	// ��־д�����ݿ�
	bool SaveLog(std::string log);
	//��ȡ����ip��ַ
	static std::string GetLocalIp();
	~CAgent();
private:
	CAgent();
	//���ݿ����
	cc::CCMysql *mysql=0;
	//����־�ļ�
	FILE* fp = 0;
	std::string local_ip;
};
#endif
