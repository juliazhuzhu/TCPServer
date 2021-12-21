#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>

#define PORT 8112
#define MSG_LEN 1024

int main(int argc, char* argv[])
{
    int  sockfd;
    char send_buf[MSG_LEN] = { 0 };
    char recv_buf[MSG_LEN + 1] = { 0 };
    struct sockaddr_in serv_addr;

    if (argc != 2) {
        printf("usage: udpclient <ip address>\n");
        _exit(-1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    while (fgets(send_buf, MSG_LEN -1 , stdin) != NULL) {
        sendto(sockfd,
               send_buf,
               strlen(send_buf),
               0,
               (struct sockaddr *)&serv_addr,
               sizeof(serv_addr));
        int n = recvfrom(sockfd, recv_buf, MSG_LEN, 0, NULL, NULL);
        if (n > 0) {
            recv_buf[n] = '\0';
            printf("recv[%s|%d]\n", recv_buf, n);
        }
    }

    return 0;
}