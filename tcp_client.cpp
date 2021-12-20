#include <iostream>
// accept
#include <sys/socket.h>
// connect
#include <sys/types.h>
//close
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>

//使用宏，定义一个端口
#define PORT 8112
//sendbuf length
#define MESSAGE_LEN 1024

int main(int argc, char const *argv[])
{
    int ret = -1;
    // define this socket param use to send and receive data
    int socket_fd;

    struct sockaddr_in serveraddr;
    //set sendbuf array is empty,this is output
    char sendbuf[MESSAGE_LEN] = {
            0,
    };
    //set receive buffer, this is input
    char recvbuf[MESSAGE_LEN] = {
            0,
    };

    /// Step1: 创建Socket //
    //PF_INET         Internet version 4 protocols,
    //SOCK_STREAM TCP
    //SOCK_DGRAM UDP
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        std::cout << "Failed to create socket!" << std::endl;
        exit(-1);
    }

    /// Step2: 连接 //
    // int connect(int socket,
    //             const struct sockaddr *address, //要传一个地址
    //             socklen_t address_len);

//    serveraddr.sin_family = AF_INET;
//    serveraddr.sin_port = PORT;
//    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //inet_addr 将字符串转为数字
//    ret = connect(socket_fd,
//                  (struct sockaddr *)&serveraddr, //是一个地址
//                  sizeof(struct sockaddr));
//    if (ret < 0)
//    {
//        //connect error
//        std::cout << "Failed to connect !" << std::endl;
//        exit(1);
//    }

    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    char server_ip[] = "127.0.0.1";
    if( inet_pton(AF_INET,server_ip,&server_addr.sin_addr) <=0 ) {
        printf("inet_pton error for %s\n",server_ip);
        exit(0);
    }

    if( connect(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }



    // send message from client
    while (1)
    {
        // Send message to server///
        //the first is 0,but the second maybe isn't 0,
        //so need set 0 at this.
        memset(sendbuf, 0, MESSAGE_LEN);
        //get input data from terminal
        fgets(sendbuf,MESSAGE_LEN-1, stdin);

        // while exit if input "quit"
        if (strcmp(sendbuf, "quit") == 0)
        {
            break;
        }

        // send information by terminal
        ret = send(socket_fd, sendbuf, strlen(sendbuf), 0);
        if (ret <= 0)
        {
            //send message to server is error
            std::cout << "Failed to send data!" << std::endl;
            break;
        }

        // receive message form server///
        ret = recv(socket_fd, recvbuf, MESSAGE_LEN, 0);
        // how to know the message is end? The zero at the end
        recvbuf[ret] = '\0';
        std::cout << "recv:" << recvbuf << std::endl;
    }
    close(socket_fd);
    return 0;
}