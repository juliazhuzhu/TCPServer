/**
  uv_udpserver.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <uv.h>
#include <pthread.h>

#define PORT 8111
#define MSG_LEN 1024
#define THREAD_NUM 4

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);

void on_read(uv_udp_t* handle, ssize_t nread,
             const uv_buf_t* buf, const struct sockaddr * addr, unsigned flags);

void on_send(uv_udp_send_t* req, int status);

void* run(void *arg){

    int port = *(int*)arg;
    uv_loop_t* loop = NULL;
    loop = uv_loop_new();
    //loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_udp_t serv_socket;
    uv_udp_t cli_socket;
    struct sockaddr_in serv_addr;
    uv_ip4_addr("0.0.0.0", port, &serv_addr);
    uv_udp_init(loop, &serv_socket);
    uv_udp_bind(&serv_socket, (struct sockaddr *)&serv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&serv_socket, alloc_buffer, on_read);

    uv_udp_init(loop, &cli_socket);

    uv_run(loop, UV_RUN_DEFAULT);
}



int main(int argc, char* argv[])
{

    pthread_t thid[THREAD_NUM];
    int port[THREAD_NUM];
    void *ret;

    for (int i=0; i<THREAD_NUM; i++){
        port[i] = PORT + i;
        printf("port %d \n", port[i]);
        if (pthread_create(&thid[i], NULL, run, &port[i]) != 0) {
            perror("pthread_create() error");
            exit(1);
        }
    }

    printf("join.... \n");
    for (int i=0; i<THREAD_NUM; i++) {
        if (pthread_join(thid[i], &ret) != 0) {
            perror("pthread_join() error");
            exit(3);
        }
    }





}

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    printf("alloc size[%d]\n", suggested_size);
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

void on_read(uv_udp_t* handle, ssize_t nread,
             const uv_buf_t* buf, const struct sockaddr * addr, unsigned flags)
{
    printf("on_read, nread[%d]\n", nread);
    uv_udp_send_t* req = NULL;
    if (nread == 0) {
        return;
    }

    req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    if (req == NULL) {
        return;
    }

    uv_buf_t send_buf = uv_buf_init(buf->base, nread);
    uv_udp_send(req, handle, &send_buf, 1, addr, on_send);

    free(buf->base);
}

void on_send(uv_udp_send_t* req, int status)
{
    if (status != 0) {
        fprintf(stderr, "uv_udp_send error:%s\n", uv_strerror(status));
    }
}

