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
	//用户登录
	bool Login();
	//最大登录失败次数
	int maxLoginTimes = 10;
	//密码输入
	std::string InputPassword();
	//获取用户输入
	std::string Input();
	//检查用户输入信息，避免注入攻击
	bool CheckInput(const std::string& in);
	// test 10000 插入一万条测试数据
	void c_test(std::vector<std::string>cmds);
	//搜索search 127.0.0.1
	void c_search(std::vector<std::string>cmds);
	//like 搜索ip，和日志内容 like
	void c_like(std::vector<std::string>cmds);
private:
	CClient();
	void c_log(std::vector<std::string>cmds);
	cc::CCMysql *mysql=0;
};
#endif // !_CCLIENT_H



