#pragma once

#ifndef _NET_H_
#define _NET_H_

#include <winsock2.h>

#define BEGIN_NET    \
    WSADATA wsadata; \
    WSAStartup(MAKEWORD(2, 2), &wsadata)

#define END_NET WSACleanup()

void service_init(SOCKET& s, const char *ip, int port);
void client_init(SOCKET& c, const char *sip, int port);

#endif