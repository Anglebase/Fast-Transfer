#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include "send.h"
#include "recv.h"
#include "spre.h"
#include "asyncIO.hpp"
using namespace std;

bool as_service = true;
void commandCheck(int argc, char **args);

int main(int argc, char **args)
{
    // fasttrans -[send|recv|spre] <path> <port> [ip]
    system("chcp 65001");

    // 检查选项
    commandCheck(argc, args);

    if (string("-send") == args[1])
    {
        sendtoOther(argc, args);
    }
    else if (string("-recv") == args[1])
    {
        recvfromOther(argc, args);
    }
    else if (string("-spre") == args[1])
    {
        if (argc == 5)
        {
            cout << "广播模式下必须由接收方指定IP" << endl;
            return 0;
        }
        spreadtoOthers(argc, args);
    }

    return 0;
}

void printHelp();

void commandCheck(int argc, char **args)
{
    if (argc >= 1 && args[1] == string("-help"))
    {
        printHelp();
        exit(0);
    }
    if (args[1][0] != '-' || argc > 5)
    {
        cout << "参数不正确，请以选项'-help'查看使用方法" << endl;
        exit(0);
    }
    if (argc < 4)
    {
        cout << "参数不完整，请以选项'-help'查看使用方法" << endl;
        exit(0);
    }
    if (to_string(atoi(args[3])) != args[3] ||
        atoi(args[3]) <= 1024 ||
        atoi(args[3]) > 65535)
    {
        cout << "指定的端口号'" << args[3] << "'不合法" << endl;
        exit(0);
    }
    if (argc == 5)
    {
        as_service = false;
    }
}

void printHelp()
{
    cout << "命令格式： fasttrans <option> <path> <port> [ip]" << endl;
    cout << "option 可以为以下值之一：" << endl;
    cout << "\t-send\t作为文件的发送者" << endl;
    cout << "\t-recv\t作为文件的接收者" << endl;
    cout << "\t-spre\t以广播的形式发送文件" << endl;
    cout << "path 用于指定要发送/保存文件所在路径" << endl;
    cout << "port 用于指定文件传输所使用的端口，取值为 1025-65535" << endl;
    cout << "ip 为可选项，文件发送方与接受方仅有一方必须指定，指定值为对方的局域网IP" << endl;
    cout << "注意事项：" << endl;
    cout << "\t*文件的发送方与接收方必须处于同一局域网下(相同WiFi或相同手机热点)" << endl;
    cout << "\t*指定IP的一方应优先于另一方执行程序" << endl;
}