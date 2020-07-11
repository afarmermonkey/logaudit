/************************FILE INFOMATION***********************************
**
** Project Project       : C++Mysql8.0数据库跨平台编程实战
** Contact               : 913902738@qq.com
** Author                : 朱文泉
** Function              ：读取日志，存到数据库
***************************************************************************/
#include <iostream>
#include "CAgent.h"
using namespace std;
int main(int argc,char *argv[])
{
    //1 初始化agent 连接数据库
    if (argc < 2)
    {
        cout << "please set ip" << endl;
        cout << "./agent 127.0.0.1" << endl;
    }
    if (!CAgent::Get()->init(argv[1]))
    {
        cout << "Agent init failed!" << endl;
    }
    std::cout << "agent start!" << endl;
    CAgent::Get()->Run();
    return 0;
}
