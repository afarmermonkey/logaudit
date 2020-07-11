/************************FILE INFOMATION***********************************
**
** Project Project       : C++Mysql8.0数据库跨平台编程实战
** Contact               : 913902738@qq.com
** Author                : 朱文泉
** Function              ：Linux平台的日志审计
***************************************************************************/
#include <iostream>
#include<string>
#include "CCenter.h"
using namespace std;
void Usage()
{
    cout << "=========Center Usage===========" << endl;
    cout << "./center install 127.0.0.1" << endl;
    cout << "./center add 127.0.0.1 filesever1" << endl;
}
int main(int argc,char* argv[])
{
    string cmd;
    if (argc > 1)
    {
        cmd = argv[1];
    }
    //安装系统
    if (cmd == "install")
    {
        cout << "begin install center" << endl;
        //./center install 127.0.0.1
        if (argc < 3)
        {
            Usage();
            return 0;
        }
        CCenter::Get()->Install(argv[2]);
        return 0;
    }
    if (!CCenter::Get()->Inint())
    {
        cerr << "CCenter::Get()->Inint() failed!" << endl;
        return -1;
    }
   if (cmd == "add")
    {
        cout << "add device" << endl;
        //./center add 127.0.0.1 filesever1
        if (argc < 4)
        {
            Usage();
            return 0;
        }
        CCenter::Get()->AddDevice(argv[2],argv[3]);
    }
    std::cout << "Center start\n";

    CCenter::Get()->Run();
    return 0;
}
