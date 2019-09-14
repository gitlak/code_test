/**
	  udp 测试 -- chat
	  jet 2019-7.6 10:30  zhonghaihuanyu 
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/un.h>
#include<errno.h>
#include<stddef.h>
#include<unistd.h>
#include<ctype.h>
#include <pthread.h>


#define MAX_CONNECT_NUM 2
#define BUFFER_SIZE 1024
#define SERVER_PORT 8888

#define CLIENT_IP "192.168.2.107"
#define CLIENT_PORT 9999


static void *send_msg(void *arg);

typedef struct 
{
    struct sockaddr_in addr;
    int fd;
} arg_t;

int main()
{
    int fd,new_fd,len,i,n,ret;
    char buffer[BUFFER_SIZE];

    fd = socket(AF_INET, SOCK_DGRAM, 0); ////TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(fd < 0) {
        printf("Request socket failed!\n");
        return -1;
    }

    struct sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //监听任意IP
    ser_addr.sin_port = htons(SERVER_PORT);  //指定监听端口

    struct sockaddr_in clientAddr;
    int clientAddrLen;

    arg_t send_arg;
    send_arg.addr = clientAddr;
    send_arg.fd = fd;
    pthread_t thread_id;
    if(pthread_create(&thread_id, NULL, send_msg, (void *)(&send_arg)) == -1) {
        // fprintf(stderr,"pthread_create error!\n");
        printf("pthread_create error!\n");
        return -1;
    }
    
    if(bind(fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0 ) {
        printf("bind failed!\n"); 
        return -1;
    }
    
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddrLen = sizeof(clientAddr);
    while(1) {
        printf("Waiting for message...\n");
        bzero(buffer, BUFFER_SIZE);   //不可少
        ret = recvfrom(fd, buffer, 512, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if(ret < 0) {
            printf("recv failed\n");
        }
        printf("收到信息：\n");
        // puts(buffer);
        fputs(buffer, stdout);
    }
    close(fd);
    return 0;
}

static void *send_msg(void *arg) {
    char buffer[BUFFER_SIZE];
    int fd;
    struct sockaddr_in client_addr;
    // arg_t *send_arg;

    // send_arg = (arg_t *)arg;
    // client_addr = send_arg->addr;
    // fd = send_arg->fd;
    fd = socket(AF_INET, SOCK_DGRAM, 0); ////TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(fd < 0) {
        printf("Request socket failed!\n");
        return NULL;
    }

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);  //向客户端发信息
    // inet_pton(AF_INET, SERVER_IP, &ser_addr.sin_addr); 
    // ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    client_addr.sin_port = htons(CLIENT_PORT);  //指定发送端口

    while(1) {
        printf("发送信息：\n");
        // gets(buffer);
        fgets(buffer, sizeof(buffer), stdin);
        if (!strncmp(buffer,"quit",4)) {
            break;
        }
        sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, 
                sizeof(client_addr));
    }
    pthread_exit(NULL);
}
