#include "spre.h"
#include "net.h"

#include <map>
#include <thread>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>
#include "asyncIO.hpp"
namespace aio = pack_asyncIO;
using namespace std;

map<string, thread *> tasklist;
queue<string> freequeue;

void spreadtoOthers(int argc, char **args)
{
    unsigned long long int recvcount = 0;
    unsigned long long int recved = 0;
    // 资源回收线程
    thread freethread{
        [&]()
        {
            while (1)
            {
                if (freequeue.empty())
                {
                    this_thread::sleep_for(500ms);
                    continue;
                }
                tasklist[freequeue.front()]->join();
                delete tasklist[freequeue.front()];
                tasklist.erase(freequeue.front());
                freequeue.pop();

                recved++;
            }
        }};

    thread print{
        [&]()
        {
            aio::out << "\r共有" << recvcount << "个申请，已完成" << recved << "个，剩余"
                     << recvcount - recved << "个正在传输";
            this_thread::sleep_for(500ms);
        }};

    ifstream ifs(args[2], ios::in | ios::binary);
    if (!ifs.is_open())
    {
        cout << "打开文件时异常" << endl;
        exit(0);
    }
    ifs.seekg(0, ios::end);
    size_t lenth = ifs.tellg();
    ifs.seekg(0);
    ifs.close();

    // 建立网络连接
    BEGIN_NET;
    SOCKET service;
    service_init(service, "0.0.0.0", atoi(args[3]));
    listen(service, 5);

    sockaddr caddr;
    int size = sizeof(sockaddr);

    unsigned long long int lr = 0;
    unsigned long long int lrd = 0;
    while (1)
    {
        SOCKET client = accept(service, &caddr, &size);
        tasklist["ClientID" + to_string(recvcount)] = new thread{
            [&, client, lenth, recvcount]()
            {
                // 发送长度信息
                send(client, (char *)&lenth, sizeof(size_t), 0);
                ifstream tifs(args[2], ios::in | ios::binary);
                // 发送文件内容
                size_t count = 0;
                char buff[1024];
                while (count + 1024 < lenth)
                {
                    memset(buff, 0, 1024);
                    tifs.read(buff, 1024);
                    send(client, buff, 1024, 0);
                    count += 1024;
                }
                memset(buff, 0, 1024);
                tifs.read(buff, lenth - count);
                send(client, buff, lenth - count, 0);
                closesocket(client);
                tifs.close();

                freequeue.push("ClientID" + to_string(recvcount));
            }};

        recvcount++;
    }

    closesocket(service);
    END_NET;
    freethread.join();
}