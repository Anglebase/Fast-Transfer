#include "send.h"
#include "net.h"
#include <iostream>
#include <fstream>
using namespace std;
#include "asyncIO.hpp"
namespace aio = pack_asyncIO;

extern bool as_service;

void asServiceSend(int argc, char **args);
void asClientSend(int argc, char **args);

void sendtoOther(int argc, char **args)
{
    if (as_service)
    {
        asServiceSend(argc, args);
    }
    else
    {
        asClientSend(argc, args);
    }
}

// 作为服务端发送数据
void asServiceSend(int argc, char **args)
{
    // 打开文件准备发送
    ifstream ifs(args[2], ios::binary | ios::in);
    if (!ifs.is_open())
    {
        cout << "打开文件时异常" << endl;
        exit(0);
    }
    aio::out << "正在读取文件信息...\n";
    ifs.seekg(0, ios::end);
    size_t lenth = ifs.tellg();
    ifs.seekg(0);
    aio::out << "lenth:" << lenth << '\n';

    // 建立网络连接
    BEGIN_NET;
    SOCKET service;
    service_init(service, "0.0.0.0", atoi(args[3]));
    listen(service, 1);

    sockaddr caddr;
    int size = sizeof(sockaddr);

    aio::out << "正在等待连接...\n";
    SOCKET client = accept(service, &caddr, &size);

    // 发送长度信息
    aio::out << "正在发送...\n";
    send(client, (char *)&lenth, sizeof(size_t), 0);

    // 发送文件内容
    size_t count = 0;
    char buff[1024];
    double curr = 0., last = 0.;
    while (count + 1024 < lenth)
    {
        memset(buff, 0, 1024);
        ifs.read(buff, 1024);
        send(client, buff, 1024, 0);
        count += 1024;
        curr = count * 100. / lenth;
        if (curr - last > 0.1)
        {
            aio::out << "\r共计" << lenth << "字节，已发送" << count << "字节(" << curr << "%)        ";
            last = curr;
        }
    }
    memset(buff, 0, 1024);
    ifs.read(buff, lenth - count);
    send(client, buff, lenth - count, 0);
    aio::out << "\r共计" << lenth << "字节，已发送" << lenth << "字节(100%)        \n";

    aio::out << "传输已完成...\n";
    closesocket(client);
    closesocket(service);
    END_NET;
    ifs.close();

    while (!asyncIO.finish())
    {
        this_thread::sleep_for(300ms);
    }
    exit(0);
}

void asClientSend(int argc, char **args)
{
    // 打开文件准备发送
    ifstream ifs(args[2], ios::binary | ios::in);
    if (!ifs.is_open())
    {
        cout << "打开文件时异常" << endl;
    }
    aio::out << "正在读取文件信息...\n";
    ifs.seekg(0, ios::end);
    size_t lenth = ifs.tellg();
    ifs.seekg(0);

    BEGIN_NET;
    SOCKET client;
    client_init(client, args[4], atoi(args[3]));

    send(client, (char *)&lenth, sizeof(size_t), 0);

    // 发送文件内容
    size_t count = 0;
    char buff[1024];
    double curr = 0., last = 0.;
    while (count + 1024 < lenth)
    {
        memset(buff, 0, 1024);
        ifs.read(buff, 1024);
        send(client, buff, 1024, 0);
        count += 1024;
        curr = count * 100. / lenth;
        if (curr - last > 0.1)
        {
            aio::out << "\r共计" << lenth << "字节，已发送" << count << "字节(" << curr << "%)        ";
            last = curr;
        }
    }
    memset(buff, 0, 1024);
    ifs.read(buff, lenth - count);
    send(client, buff, lenth - count, 0);
    aio::out << "\r共计" << lenth << "字节，已发送" << lenth << "字节(100%)        \n";

    aio::out << "传输已完成...\n";

    closesocket(client);
    END_NET;
    ifs.close();
    while (!asyncIO.finish())
    {
        this_thread::sleep_for(300ms);
    }
    exit(0);
}
