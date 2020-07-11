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
	termios tm_new, tm_old;//����̨��ͬ����ʾģʽ
	int fd=0;
	if (tcgetattr(fd, &tm_old) < 0) //��ʾģʽ��old ����
		return -1;
	//����Ϊԭʼģʽ��û�л���
	cfmakeraw(&tm_new);  //ԭʼģʽ��new
	if (tcsetattr(fd, TCSANOW, &tm_new) < 0) //��ʾģʽ��Ϊnew�������ԣ�
	{
		return -1;
	}
	char c = getchar();
	if (tcsetattr(fd, TCSANOW, &tm_old) < 0) //�ָ�ԭ������ʾģʽ
	{
		return -1;
	}
	return c;
}
#endif // _WIN32
// test 10000 ����һ������������

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

		//����ʱ�䣬��mysql now����
		//@��ʾ �ֶ����ݲ������ţ�@���Զ�ȥ��
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

		//����ʱ�䣬��mysql now����
		//@��ʾ �ֶ����ݲ������ţ�@���Զ�ȥ��
		data["@log_time"] = "now()";
		mysql->Insert(data, "t_log");

	}
	mysql->Commit();
	mysql->StopTransaction();

}
//search 127.0.0.1 ����ip
void CClient::c_search(std::vector<std::string>cmds)
{
	cout << "c_search" << endl;
	if (cmds.size() < 2)return;
	string key = cmds[1];
	//��¼��ѯʱ��


	//��¼��ʼʱ��
	auto start = system_clock::now();
	string sql = "select * from t_log ";
	string where = " where ip='";
	where += key;
	where += "'";
	sql += where;
	//��������û������Ķ�  �������������ݲ���ʱ�伸��Ϊ��
	auto rows = mysql->GetResult(sql.c_str());
	//����ÿһ��
	for (auto row : rows)
	{
		//����ÿһ��
		for (auto c : row)
		{
			if (c.data)
				cout << c.data << " ";
		}
		cout << endl;
	}

	//��¼����ʱ�� -�ó���ʱ
	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start); //΢��
	cout << "time  sec =" << double(duration.count()) * microseconds::period::num / microseconds::period::den << " sec" << endl;

	//ͳ������
	sql = "select count(*) from t_log ";
	sql += where;
	rows = mysql->GetResult(sql.c_str());

	int total = 0;
	if (rows.size() > 0 && rows[0][0].data)
		total = atoi(rows[0][0].data);
	cout << "total :" << total << endl;


}
//��ȡ�û�����
string CClient::Input()
{
	//��ջ���
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
//��������
string CClient::InputPassword()
{
	//��ջ���
	cin.ignore(4096,'\n');
	string passwd = "";
	for (;;)
	{
		//��ȡ�����ַ�����ʾ
		char a = _getch();
		if (a <= 0 || a == '\n' || a == '\r')
			break;
		cout << "*" << flush;
		passwd += a;
	}
	return passwd;
}
//����û�������Ϣ������ע�빥��true ��ȫ false Σ��
bool CClient::CheckInput(const string& in)
{
	//��������ֵ��ַ�
	string str = "'\"()";
	for(char a:str)
	{
		size_t found=in.find(a);
		if (found != string::npos)//����Υ���ַ�
			return false;
	}
	return true;
}
//��ʼ��
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
//�û���¼
bool CClient::Login()
{
	bool isLogin = false;
	for (int i=0; i < maxLoginTimes; i++)
	{
		string username = "";
		//�����û���
		cout << "input username: " << flush;
		cin >> username;
		cout << "[" << username << "]" << endl;
		//sqlע�빥��
		//���֧�����ж���sql��䣬���Խ��������䣬����Լ�����䣨ɾ�⣩
		//������ֱ�ӵ�¼
		
		//�����û�Ȩ�ޣ�����root�û�
		//��Ԥ�������stmt
		//����û�����

		//select id from t_user where user='root' and pass=md5('123456')
		//ģ��sqlע�빥��
		//usename=	
		//passwd=1')or'c4ca4238a0b923820dcc509a6f75849b'=md5('1
		//select id from t_user where user='1'or'1'='1' and pass=md5('1')or'c4ca4238a0b923820dcc509a6f75849b'=md5('1')

		//������������
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

//cmds=��log 1 10 ����һҳÿһҳ10��
void CClient::c_log(std::vector<std::string>cmds)
{
	int pagecount = 10;
	int page = 1;
	if (cmds.size() > 1)
		page = atoi(cmds[1].c_str());
	if (cmds.size() > 2)
		pagecount = atoi(cmds[2].c_str());
	//limit 0 10 ��0��ʼȡ10��
	string sql = "select * from t_log limit ";
	stringstream ss;
	ss << sql;
	ss << (page - 1) * pagecount;
	ss << ",";
	ss << pagecount;
	sql = ss.str();
	auto rows = mysql->GetResult(sql.c_str());
	//����ÿһ��
	for (auto row : rows)
	{
		//����ûһ��
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
//like ����ip������־���� like���� ������Ч
void CClient::c_like(std::vector<std::string>cmds)
{
	cout << "c_like" << endl;
	if (cmds.size() < 2)return;
	string key = cmds[1];
	//��¼��ѯʱ��
	//��¼��ʼʱ��
	auto start = system_clock::now();
	string sql = "select * from t_log ";
	string where = " where log like '%";
	where += key;
	where += "%'";
	sql += where;
	//һ�������� ������ 0.47�� ������ 0.000687
	auto rows = mysql->GetResult(sql.c_str());
	//����ÿһ��
	for (auto row : rows)
	{
		//����ÿһ��
		for (auto c : row)
		{
			if (c.data)
				cout << c.data << " ";
		}
		cout << endl;
	}

	//��¼����ʱ�� -�ó���ʱ
	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start); //΢��
	cout << "time  sec =" << double(duration.count()) * microseconds::period::num / microseconds::period::den << " sec" << endl;

	//ͳ������
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
	//1 �û���¼
	if (!Login())return;
	//2 ��ҳ��ʾt_log
	//��ȡ�û�����
	for (;;)
	{
		cout << "input:" << flush;
		string cmd = Input();
		//log 1 10 ��һҳ һҳ10��
		//�и�ո�
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