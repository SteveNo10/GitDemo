
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <math.h>
#include <iostream>

using namespace std;

#define MYPORT 8888
const char* SERVERIP = "192.168.1.32";

#define ERR_EXIT(m) \
    do \
{ \
    perror(m); \
    exit(EXIT_FAILURE); \
} while(0)

void echo_cli(int sock)
{
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVERIP);

    int ret;
    char sendbuf[8192] = {0};
    char recvbuf[8192] = {0};
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        printf("向服务器发送：%s\n",sendbuf);
        sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("recvfrom");
        }
        string strRecv(recvbuf, ret);
        printf("从服务器接收：%s\n",recvbuf);

        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sock);
}

int main(void)
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket");

    int recvbuf = 0;
    int len = sizeof(recvbuf);
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, (socklen_t*)&len);
    recvbuf *= 2;
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)&recvbuf, sizeof(recvbuf));

    echo_cli(sock);

    return 0;
}

