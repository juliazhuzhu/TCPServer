/**
  event_udpserver.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event.h>
#include <event2/listener.h>

#define PORT 8111
#define MSG_LEN 1024

void read_cb(int fd, short event, void *arg) {
    char buf[MSG_LEN];
    int len;
    socklen_t addr_len = sizeof(struct sockaddr);
    struct sockaddr_in cli_addr;

    memset(buf, 0, sizeof(buf));
    len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cli_addr, &addr_len);

    if (len == -1) {
        perror("recvfrom");
    } else if (len == 0) {
        printf("Connection Closed\n");
    } else {
        buf[len] = '\0';
        printf("recv[%s:%d]\n", buf, len);
        sendto(fd, buf, len, 0, (struct sockaddr *)&cli_addr, addr_len);
    }
}

int bind_socket(struct event *ev) {
    int sock_fd;
    int flag = 1;
    struct sockaddr_in local_addr;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
        perror("setsockopt");
        return 1;
    }

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) < 0) {
        perror("bind");
        return -1;
    } else {
        printf("bind success, port[%d]\n", PORT);
    }

    event_set(ev, sock_fd, EV_READ | EV_PERSIST, &read_cb, NULL);
    if (event_add(ev, NULL) == -1) {
        perror("event_set");
    }

    return 0;
}

int main()
{
    struct event ev;

    if (event_init() == NULL) {
        perror("event_init");
        _exit(-1);
    }

    if (bind_socket(&ev) != 0) {
        perror("bind_socket");
        _exit(-1);
    }

    event_dispatch();

    return 0;
}

