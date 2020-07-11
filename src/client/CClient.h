#ifndef _CCLIENT_H
#define _CCLIENT_H
#include<string>
#include"ccmysql.h"
#include<vector>
class CClient
{
public:
	static CClient* Get()
	{
		static CClient client;
		return &client;
	}
	bool Init(std::string ip);
	~CClient();
	void Run();
	//�û���¼
	bool Login();
	//����¼ʧ�ܴ���
	int maxLoginTimes = 10;
	//��������
	std::string InputPassword();
	//��ȡ�û�����
	std::string Input();
	//����û�������Ϣ������ע�빥��
	bool CheckInput(const std::string& in);
	// test 10000 ����һ������������
	void c_test(std::vector<std::string>cmds);
	//����search 127.0.0.1
	void c_search(std::vector<std::string>cmds);
	//like ����ip������־���� like
	void c_like(std::vector<std::string>cmds);
private:
	CClient();
	void c_log(std::vector<std::string>cmds);
	cc::CCMysql *mysql=0;
};
#endif // !_CCLIENT_H



