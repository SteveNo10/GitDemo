#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>

#define MYPORT 8888
#define ROUTEPORT 8889
const char* ROUTEIP = "192.168.1.34";

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

void echo_ser(int sock)
{
    struct sockaddr_in routeaddr;
    memset(&routeaddr, 0, sizeof(routeaddr));
    routeaddr.sin_family = AF_INET;
    routeaddr.sin_port = htons(ROUTEPORT);
    routeaddr.sin_addr.s_addr = inet_addr(ROUTEIP);

    char recvbuf[1024] = {0};
    char sendbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;

    while (1)
    {
        bzero(recvbuf, 1024);
        bzero(sendbuf, 1024);

        peerlen = sizeof(peeraddr);
        memset(recvbuf, 0, sizeof(recvbuf));
        n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,
                (struct sockaddr *)&peeraddr, &peerlen);
        if (n <= 0)
        {
            if (errno == EINTR)
                continue;

            ERR_EXIT("recvfrom error");
        }
        else if(n > 0)
        {
            printf("接收到的数据：%s\n",recvbuf);

            memcpy(sendbuf, &peeraddr, peerlen);
            memcpy(sendbuf + peerlen, recvbuf, strlen(recvbuf));

            sendto(sock, sendbuf, peerlen + strlen(recvbuf), 0, (struct sockaddr*)&routeaddr, sizeof(routeaddr));
            printf("转发的数据：%s\n", sendbuf + peerlen);
        }
    }
    close(sock);
}

int main(void)
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket error");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("监听%d端口\n",MYPORT);
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");

    echo_ser(sock);

    return 0;
}
