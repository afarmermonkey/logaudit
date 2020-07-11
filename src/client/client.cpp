/************************FILE INFOMATION***********************************
**
** Project Project       : C++Mysql8.0数据库跨平台编程实战
** Contact               : 913902738@qq.com
** Author                : 朱文泉
** Function              ：跨平台的日志审计
***************************************************************************/
#include <iostream>
#include "CClient.h"
using namespace std;
int main(int argc,char*argv[])
{
    string ip = "127.0.0.1";
    if (!CClient::Get()->Init(ip))
    {
        return 0;
    }
    std::cout << "Client start!\n";
    CClient::Get()->Run();
    return 0;
}
