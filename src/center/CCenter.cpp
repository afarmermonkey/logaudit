#include "CCenter.h"
#include<iostream>
#include<fstream>
#include<thread>
#include<regex>
#define CENTER_CONF "ip" //Install函数生成的配置文件 Init（）读这个配置文件
using namespace std;
using namespace cc;
bool CCenter::Inint()
{
	mysql = new CCMysql();
	string ip = "";
	//读取数据库IP地址
	ifstream fs;
	fs.open(CENTER_CONF);
	if (!fs.is_open())
	{
		cerr << "please install center!" << endl;
		return false;
	}
	fs >> ip;
	fs.close();
	if (ip.empty())
	{
		cerr << "please install center!" << endl;
		return false;
	}
	cout << "Inint center " << ip << endl;

	if (!mysql->Connect(ip.c_str(), "root", "123456", "zwq"))
	{
		cout << "db connect failed!" << endl;
		return false;
	}
	cout << "db connect success!" << endl;
	return mysql->Query("set names utf8");
}
bool CCenter::Install(string ip)
{
	//1 生成配置文件 数据库IP
	ofstream of;
	of.open(CENTER_CONF);
	if (!of.is_open())
	{
		cout << "open conf" << CENTER_CONF << "failed! " << endl;
		return false;
	}
	of << ip;
	of.close();

	//2 初始化表和数据
	if (!Inint())
	{
		return false;
	}
	cout << "CCenter::Install " << ip << endl;

	
	mysql->StartTransaction();
	//创建策略表
	//清空原来的数据，防止数据污染
	mysql->Query("DROP TABLE IF EXISTS `t_strategy`");
	string sql = "CREATE TABLE IF NOT EXISTS `t_strategy`(\
		`id` INT AUTO_INCREMENT,\
		`name` VARCHAR(256) CHARACTER SET `utf8` COLLATE`utf8_bin`,\
		`strategy` VARCHAR(4096),\
		 PRIMARY KEY(`id`))";
	bool re = mysql->Query(sql.c_str());
	if (!re)
	{
		mysql->RollBack();//事务回滚
		return false;
	}
	{
		SQLDATA data;
		data["name"] = "用户登录失败";
		//Jul 11 01:06:48 ubuntu sshd[18552]: Failed password for zwq from 192.168.37.1 port 58384 ssh2

		data["strategy"] = ".*Failed password for(.+) from ([0-9.]+) port ([0-9]+).*";
		mysql->Insert(data, "t_strategy");

		data["name"] = "用户登录成功";
		//Jul 11 01:06:48 ubuntu sshd[18552]: Failed password for zwq from 192.168.37.1 port 58384 ssh2
		//Accepted password for zwq from 192.168.37.1 port 58384 ssh2
		data["strategy"] = ".*Accepted password for(.+) from ([0-9.]+) port ([0-9]+).*";
		mysql->Insert(data, "t_strategy");
	}
	//创建用户表，初始化管理元用户 root 123456 md5
	//t_user id user pass
	mysql->Query("DROP TABLE IF EXISTS `t_user`");
	sql = "CREATE TABLE IF NOT EXISTS `t_user`(\
		`id` INT AUTO_INCREMENT,\
		`user` VARCHAR(256) CHARACTER SET `utf8` COLLATE`utf8_bin`,\
		`pass` VARCHAR(1024),\
		 PRIMARY KEY(`id`))";
	re = mysql->Query(sql.c_str());
	if (!re)
	{
		mysql->RollBack();//事务回滚
		return false;
	}
	{

		SQLDATA data;
		data["user"] = "root";
		//Jul 11 01:06:48 ubuntu sshd[18552]: Failed password for zwq from 192.168.37.1 port 58384 ssh2

		data["@pass"] = "md5('123456')";
		mysql->Insert(data, "t_user");
	}
	//创建设备表 Agent t_device
	mysql->Query("DROP TABLE IF EXISTS `t_device`");
	sql = "CREATE TABLE IF NOT EXISTS `t_device`(\
				`id` INT AUTO_INCREMENT,\
				`ip` VARCHAR(16),\
				`name` VARCHAR(2048),\
				`last_heart` datetime,\
				PRIMARY KEY(`id`))";
	mysql->Query(sql.c_str());
	//创建日志表  t_log
	mysql->Query("DROP TABLE IF EXISTS `t_log`");
	sql = "CREATE TABLE IF NOT EXISTS `t_log`(\
				`id` INT AUTO_INCREMENT,\
				`ip` VARCHAR(16),\
				`log` VARCHAR(2048),\
				`log_time` datetime,\
				PRIMARY KEY(`id`))";
	mysql->Query(sql.c_str());
	//创建审计结果表  t_audit
	mysql->Query("DROP TABLE IF EXISTS `t_audit`");
	sql = "CREATE TABLE IF NOT EXISTS `t_audit`(\
				`id` INT AUTO_INCREMENT,\
				`name` VARCHAR(2048),\
				`context` VARCHAR(2048),\
				`user` VARCHAR(256),\
				`device_ip` VARCHAR(16),\
				`from_ip` VARCHAR(16),\
				`port` int,\
				`last_heart` datetime,\
				PRIMARY KEY(`id`))";
	mysql->Query(sql.c_str());


	mysql->Commit();
	mysql->StopTransaction();
		
	return true;
}
//添加设备
bool CCenter::AddDevice(std::string ip, std::string name)
{
	/*"CREATE TABLE IF NOT EXISTS `t_device`(\
				`id` INT AUTO_INCREMENT,\
				`ip` VARCHAR(16),\
				`name` VARCHAR(2048),\
				`last_heart` datetime,\
				PRIMARY KEY(`id`))";*/
	SQLDATA data;
	data["ip"] = ip.c_str();
	data["name"] = name.c_str();
	return mysql->Insert(data, "t_device");
}
void CCenter::Run()
{
	//只审计运行之后的事件
	//找到最后一个事件，取到id号
	int lastid = 0;
	auto rows = mysql->GetResult("select max(id) from t_log");
	if (rows[0][0].data)
	{
		lastid = atoi(rows[0][0].data);
	}
	cout << "lastid is " << lastid << endl;
	//取审计策略
	/*"CREATE TABLE IF NOT EXISTS `t_strategy`(\
		`id` INT AUTO_INCREMENT,\
		`name` VARCHAR(256) CHARACTER SET `utf8` COLLATE`utf8_bin`,\
		`strategy` VARCHAR(4096),\
		 PRIMARY KEY(`id`))";*/
	rows = mysql->GetResult("SELECT * from t_strategy");
	//正则表达式map key 对应审计事件名称
	map<string, regex> strategys;
	for (auto row:rows)
	{
		if (row[1].data && row[2].data)
			strategys[row[1].data] = regex(row[2].data);
	}
	for (;;)
	{
		char buf[1024] = { 0 };
		//获取agent存储的最新数据
		sprintf(buf, "select * from t_log where id>%d", lastid);
		auto rows = mysql->GetResult(buf);
		if (rows.empty())
		{
			this_thread::sleep_for(200ms);
		}
		
		//遍历最新日志，对每条日志进行审计
		for (auto row : rows)
		{
			lastid = atoi(row[0].data);
			if (!row[2].data) //row[2].data 日志
				continue;
			cout << row[2].data << endl;
			//遍历审计规则
			for (auto strategy: strategys)
			{
				//正则结果
				smatch match;
				string data = row[2].data;
				//匹配正则，返回结果到match
				bool ret = regex_match(data,match,strategy.second);
				if (!ret||match.size() <= 0)
				{
					continue;
				}
				cout << strategy.first.c_str() << endl;//审计成功事件名称
				/*"CREATE TABLE IF NOT EXISTS `t_audit`(\
				`id` INT AUTO_INCREMENT,\
				`name` VARCHAR(2048),\
				`context` VARCHAR(2048),\
				`user` VARCHAR(256),\
				`device_ip` VARCHAR(16),\
				`from_ip` VARCHAR(16),\
				`port` int,\
				`last_heart` datetime,\
				PRIMARY KEY(`id`))";*/
				SQLDATA data1;
				//审计成功事件的名称 
				data1["name"] = strategy.first.c_str();
				data1["context"] = data.c_str();
				if (row[1].data)
					data1["device_ip"] = row[1].data;
				//匹配结果，下表0是整个字符串 1是第一个匹配结果
				string user = match[1];
				string from_ip = match[2];
				string port = match[3];
				data1["user"] = user.c_str();
				data1["from_ip"] = from_ip.c_str();
				data1["port"] = port.c_str();
				mysql->Insert(data1, "t_audit");
			}
		}

	}
}
CCenter::CCenter()
{

}
CCenter::~CCenter()
{
	mysql->Close();
	delete mysql;
}