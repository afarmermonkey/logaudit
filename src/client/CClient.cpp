#include "CClient.h"
#include<iostream>
#include"ccmysql.h"
#include<vector>
#include<sstream>
#include<string.h>
using namespace std;
using namespace cc;
#ifdef _WIN32
#include<conio.h>
#else
#include<termio.h>
char _getch()
{
	termios tm_new, tm_old;//控制台不同的显示模式
	int fd=0;
	if (tcgetattr(fd, &tm_old) < 0) //显示模式给old 备份
		return -1;
	//跟改为原始模式，没有回显
	cfmakeraw(&tm_new);  //原始模式给new
	if (tcsetattr(fd, TCSANOW, &tm_new) < 0) //显示模式设为new（不回显）
	{
		return -1;
	}
	char c = getchar();
	if (tcsetattr(fd, TCSANOW, &tm_old) < 0) //恢复原来的显示模式
	{
		return -1;
	}
	return c;
}
#endif // _WIN32
// test 10000 插入一万条测试数据

#include <chrono>
using namespace chrono;
void CClient::c_test(std::vector<std::string>cmds)
{
	int count = 10000;
	if (cmds.size() > 1)
		count = atoi(cmds[1].c_str());
	mysql->StartTransaction();
	for (int i = 0; i < count; i++)
	{
		SQLDATA data;
		stringstream ss;
		ss << "testlog";
		ss << (i + 1);
		string tmp = ss.str();
		data["log"] = tmp.c_str();
		data["ip"] = "127.0.0.1";

		//插入时间，用mysql now（）
		//@表示 字段内容不加引号，@会自动去除
		data["@log_time"] = "now()";
		mysql->Insert(data, "t_log");

	}
	{
		SQLDATA data;
		stringstream ss;
		ss << "search001";
		string tmp = ss.str();
		data["log"] = tmp.c_str();
		data["ip"] = "10.0.0.1";

		//插入时间，用mysql now（）
		//@表示 字段内容不加引号，@会自动去除
		data["@log_time"] = "now()";
		mysql->Insert(data, "t_log");

	}
	mysql->Commit();
	mysql->StopTransaction();

}
//search 127.0.0.1 搜索ip
void CClient::c_search(std::vector<std::string>cmds)
{
	cout << "c_search" << endl;
	if (cmds.size() < 2)return;
	string key = cmds[1];
	//记录查询时间


	//记录开始时间
	auto start = system_clock::now();
	string sql = "select * from t_log ";
	string where = " where ip='";
	where += key;
	where += "'";
	sql += where;
	//有索引比没索引快的多  有索引百万数据查找时间几乎为零
	auto rows = mysql->GetResult(sql.c_str());
	//遍历每一行
	for (auto row : rows)
	{
		//遍历每一列
		for (auto c : row)
		{
			if (c.data)
				cout << c.data << " ";
		}
		cout << endl;
	}

	//记录结束时间 -得出耗时
	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start); //微秒
	cout << "time  sec =" << double(duration.count()) * microseconds::period::num / microseconds::period::den << " sec" << endl;

	//统计总数
	sql = "select count(*) from t_log ";
	sql += where;
	rows = mysql->GetResult(sql.c_str());

	int total = 0;
	if (rows.size() > 0 && rows[0][0].data)
		total = atoi(rows[0][0].data);
	cout << "total :" << total << endl;


}
//获取用户输入
string CClient::Input()
{
	//清空缓冲
	//cin.ignore(4096, '\n');
	string input = "";
	for (;;)
	{
		char a = getchar();
		if (a <= 0 || a == '\n' || a == '\r')
			break;
		
		input += a;
	}
	return input;
}
//密码输入
string CClient::InputPassword()
{
	//清空缓冲
	cin.ignore(4096,'\n');
	string passwd = "";
	for (;;)
	{
		//获取输入字符不显示
		char a = _getch();
		if (a <= 0 || a == '\n' || a == '\r')
			break;
		cout << "*" << flush;
		passwd += a;
	}
	return passwd;
}
//检查用户输入信息，避免注入攻击true 安全 false 危险
bool CClient::CheckInput(const string& in)
{
	//不允许出现的字符
	string str = "'\"()";
	for(char a:str)
	{
		size_t found=in.find(a);
		if (found != string::npos)//发现违规字符
			return false;
	}
	return true;
}
//初始化
bool CClient::Init(string ip)
{
	cout << "ip " << ip << endl;
	mysql = new CCMysql();
	if (!mysql->Connect(ip.c_str(), "root", "123456", "zwq"))
	{
		cerr << "db connect failed!" << endl;
		return false;
	}
	cout << "db connect success!" << endl;
	return mysql->Query("set names utf8");
}
//用户登录
bool CClient::Login()
{
	bool isLogin = false;
	for (int i=0; i < maxLoginTimes; i++)
	{
		string username = "";
		//接收用户名
		cout << "input username: " << flush;
		cin >> username;
		cout << "[" << username << "]" << endl;
		//sql注入攻击
		//如果支持运行多条sql语句，可以结束你的语句，添加自己的语句（删库）
		//无密码直接登录
		
		//限制用户权限，不用root用户
		//用预处理语句stmt
		//检查用户输入

		//select id from t_user where user='root' and pass=md5('123456')
		//模拟sql注入攻击
		//usename=	
		//passwd=1')or'c4ca4238a0b923820dcc509a6f75849b'=md5('1
		//select id from t_user where user='1'or'1'='1' and pass=md5('1')or'c4ca4238a0b923820dcc509a6f75849b'=md5('1')

		//接收密码输入
		cout << "input password: " << flush;
		string passwd =InputPassword();
		if (!CheckInput(passwd) || !CheckInput(passwd))
		{
			cout << "Injection attacks!" << endl;
			continue;
		}
		string sql = "select id from t_user where user='";
		sql += username;
		sql += "' and pass=md5('";
		sql += passwd;
		sql += "')";
		//cout << sql << endl;
		auto rows = mysql->GetResult(sql.c_str());
		if (rows.size() > 0)
		{
			cout << "\nlogin success!" << endl;
			isLogin = true;
			break;
		}
		cout << "login failed!" << endl;
	}
	return isLogin;
}

//cmds=“log 1 10 ”第一页每一页10条
void CClient::c_log(std::vector<std::string>cmds)
{
	int pagecount = 10;
	int page = 1;
	if (cmds.size() > 1)
		page = atoi(cmds[1].c_str());
	if (cmds.size() > 2)
		pagecount = atoi(cmds[2].c_str());
	//limit 0 10 从0开始取10条
	string sql = "select * from t_log limit ";
	stringstream ss;
	ss << sql;
	ss << (page - 1) * pagecount;
	ss << ",";
	ss << pagecount;
	sql = ss.str();
	auto rows = mysql->GetResult(sql.c_str());
	//遍历每一行
	for (auto row : rows)
	{
		//遍历没一列
		for (auto c : row)
		{
			cout << c.data << endl;
		}
		cout << endl;
	}
	//int total = 0;
	//cout << "Total" << total << endl;
	cout << "Page = " << page << " Pagecount" << pagecount << endl;
}
//like 搜索ip，和日志内容 like查找 索引无效
void CClient::c_like(std::vector<std::string>cmds)
{
	cout << "c_like" << endl;
	if (cmds.size() < 2)return;
	string key = cmds[1];
	//记录查询时间
	//记录开始时间
	auto start = system_clock::now();
	string sql = "select * from t_log ";
	string where = " where log like '%";
	where += key;
	where += "%'";
	sql += where;
	//一百万数据 无索引 0.47秒 有索引 0.000687
	auto rows = mysql->GetResult(sql.c_str());
	//遍历每一行
	for (auto row : rows)
	{
		//遍历每一列
		for (auto c : row)
		{
			if (c.data)
				cout << c.data << " ";
		}
		cout << endl;
	}

	//记录结束时间 -得出耗时
	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start); //微秒
	cout << "time  sec =" << double(duration.count()) * microseconds::period::num / microseconds::period::den << " sec" << endl;

	//统计总数
	sql = "select count(*) from t_log ";
	sql += where;
	rows = mysql->GetResult(sql.c_str());

	int total = 0;
	if (rows.size() > 0 && rows[0][0].data)
		total = atoi(rows[0][0].data);
	cout << "total :" << total << endl;

}
void CClient::Run()
{
	//1 用户登录
	if (!Login())return;
	//2 分页显示t_log
	//获取用户输入
	for (;;)
	{
		cout << "input:" << flush;
		string cmd = Input();
		//log 1 10 第一页 一页10行
		//切割空格
		vector<string> cmds;
		char* p = strtok((char*)cmd.c_str(), " ");
		while (p)
		{
			cmds.push_back(p);
			p = strtok(0, " ");
		}
		string type = cmd;
		if (cmds.size() > 0)
		{
			type = cmds[0];
		}
		if (type == "log")
		{
			c_log(cmds);
		}
		else if (type == "test")
		{
			c_test(cmds);
		}
		else if (type == "search")
		{
			c_search(cmds);
		}
		else if (type == "like")
		{
			c_like(cmds);
		}
	}
}
CClient::CClient() 
{

}
CClient::~CClient()
{

}