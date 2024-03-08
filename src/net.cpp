#include"net.h"

void service_init(SOCKET& s, const char *ip, int port){
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.S_un.S_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(s, (sockaddr *)&addr, sizeof(sockaddr));
}

void client_init(SOCKET &c, const char *sip, int port)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.S_un.S_addr = inet_addr(sip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    connect(c, (sockaddr *)&addr, sizeof(sockaddr));
}
