#include "recv.h"
#include "net.h"
#include <fstream>
#include <iostream>
#include "asyncIO.hpp"
using namespace std;
namespace aio = pack_asyncIO;

extern bool as_service;

void asServiceRecv(int argc, char **args);
void asClientRecv(int argc, char **args);

void recvfromOther(int argc, char **args)
{
    if (as_service)
    {
        asServiceRecv(argc, args);
    }
    else
    {
        asClientRecv(argc, args);
    }
}

void asServiceRecv(int argc, char **args)
{
    ofstream ofs(args[2], ios::out | ios::binary);
    if (!ofs.is_open())
    {
        cout << "打开文件时异常" << endl;
        exit(0);
    }

    BEGIN_NET;
    SOCKET service;
    service_init(service, "0.0.0.0", atoi(args[3]));
    listen(service, 1);

    sockaddr caddr;
    int size = sizeof(sockaddr);
    
    aio::out << "正在等待接收文件...\n";
    SOCKET client = accept(service, &caddr, &size);

    aio::out << "正在获取文件信息文件...\n";
    size_t lenth;
    recv(client, (char *)&lenth, sizeof(lenth), 0);
    char buff[1024];
    size_t count = 0;
    double curr = 0., last = 0.;
    
    aio::out << "正在接收文件...\n";
    while (count + 1024 < lenth)
    {
        memset(buff, 0, 1024);
        recv(client, buff, 1024, 0);
        ofs.write(buff, 1024);
        curr = count * 100. / lenth;
        if (curr - last > 0.1)
        {
            aio::out << "\r共计" << lenth << "字节，已接收" << count << "字节(" << curr << "%)        ";
            last = curr;
        }
    }
    memset(buff, 0, 1024);
    recv(client, buff, lenth - count, 0);
    ofs.write(buff, lenth - count);
    aio::out << "\r共计" << lenth << "字节，已接收" << lenth << "字节(100%)        \n";

    aio::out << "传输已完成...\n";
    closesocket(client);
    closesocket(service);
    END_NET;
    ofs.close();
    while (!asyncIO.finish())
    {
        this_thread::sleep_for(300ms);
    }
    exit(0);
}

void asClientRecv(int argc, char **args)
{
    ofstream ofs(args[2], ios::out | ios::binary);
    if (!ofs.is_open())
    {
        cout << "打开文件时异常" << endl;
        exit(0);
    }

    BEGIN_NET;
    SOCKET client;
    client_init(client, args[4], atoi(args[3]));
    
    aio::out << "正在获取文件信息...\n";
    size_t lenth;
    recv(client, (char *)&lenth, sizeof(lenth), 0);
    char buff[1024];
    size_t count = 0;
    double curr = 0., last = 0.;
    aio::out << "正在接收文件...\n";
    while (count + 1024 < lenth)
    {
        memset(buff, 0, 1024);
        recv(client, buff, 1024, 0);
        ofs.write(buff, 1024);
        curr = count * 100. / lenth;
        if (curr - last > 0.1)
        {
            aio::out << "\r共计" << lenth << "字节，已接收" << count << "字节(" << curr << "%)        ";
            last = curr;
        }
    }
    memset(buff, 0, 1024);
    recv(client, buff, lenth - count, 0);
    ofs.write(buff, lenth - count);
    aio::out << "\r共计" << lenth << "字节，已接收" << lenth << "字节(100%)        \n";

    aio::out << "传输已完成...\n";
    closesocket(client);
    END_NET;
    ofs.close();
    while (!asyncIO.finish())
    {
        this_thread::sleep_for(300ms);
    }
    exit(0);
}