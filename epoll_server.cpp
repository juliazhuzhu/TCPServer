#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8111
#define MESSAGE_SIZE 1024
#define MAX_EVENTS 20
#define TIMEOUT 500
#define MAX_PROCESS 9

int main() {
//    std::cout << "Hello, World!" << std::endl;
    int socket_fd = -1;
    int accept_fd = -1;
    int ret = -1;
    int flags = 1;
    int backlog = 10;

    char in_buf[MESSAGE_SIZE] = {0,};
    struct sockaddr_in local_addr, remote_addr;
    int epoll_fd;
    struct epoll_event ev, events[MAX_EVENTS];
    int event_number;
    pid_t pid = -1;


    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cout << "create socket error" << std::endl;
        exit(1);
    }

    flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

    ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));
    if (ret == -1) {
        std::cout << "setsocket error" << std::endl;
    }

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(PORT);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(local_addr.sin_zero), 8);

    ret = bind(socket_fd, (struct sockaddr *) &local_addr, sizeof(struct sockaddr_in));
    if (ret == -1) {
        std::cout << "bind error" << std::endl;
        exit(1);
    }

    ret = listen(socket_fd, backlog);
    if (ret == -1) {
        std::cout << "listen error" << std::endl;
        exit(1);
    }

    epoll_fd = epoll_create(256);

    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);


    for (int i = 0; i < MAX_PROCESS; i++) {
        if (pid != 0) {
            pid = fork();
        }
    }

   if (pid == 0) {
       epoll_fd = epoll_create(256);

       ev.events = EPOLLIN;
       ev.data.fd = socket_fd;
       epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

       for (;;) {
           event_number = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
           for (int i = 0; i < event_number; i++) {
               if (events[i].data.fd == socket_fd) {
                   std::cout << "listening events:" << i << std::endl;
                   socklen_t addr_len = sizeof(struct sockaddr_in);
                   accept_fd = accept(socket_fd, (struct sockaddr *) &remote_addr, &addr_len);

                   flags = fcntl(accept_fd, F_GETFL, 0);
                   fcntl(accept_fd, F_SETFL, flags | O_NONBLOCK);
                   ev.events = EPOLLIN | EPOLLET;
                   ev.data.fd = accept_fd;
                   epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
               } else if (events[i].events & EPOLLIN) {
                   do {

                       memset(in_buf, 0, MESSAGE_SIZE);
                       ret = recv(events[i].data.fd, (void *) in_buf, MESSAGE_SIZE, 0);
                       if (ret == 0) {
                           close(events[i].data.fd);
                       }

                       if (ret == MESSAGE_SIZE) {
                           std::cout << "" << std::endl;
                       }
                   } while (ret < 0 && errno == EINTR);//这个操作被中断

                   if (ret < 0) {
                       switch (errno) {
                           //没有数据了
                           case EAGAIN:
                               break;
                           default:
                               break;


                       }
                   }

                   if (ret > 0) {
                       std::cout << "receive message:" << in_buf << std::endl;
                       send(events[i].data.fd, (void *) in_buf, MESSAGE_SIZE, 0);
                   }
               }
           }


       }
   }else {

       do {
           pid = waitpid(-1, NULL, 0);

       } while (pid != -1);

       return 0;


   }

    std::cout << "quit server..." << std::endl;
    close(socket_fd);

    return 0;

}
