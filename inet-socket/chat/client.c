/**
    udp 测试 -- chat
    jet 2019-7-6
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
#include <pthread.h>


#define SERVER_PORT 8888
#define SERVER_IP "192.168.2.106"
#define BUFFER_SIZE 1024
#define CLIENT_PORT 9999

static void *recv_msg(void *arg);

int main()
{
    struct sockaddr_in ser_addr;
    int sock_fd, ret;
    char buffer[BUFFER_SIZE];

    // 创建一个线程来接收服务器发送的信息
    pthread_t thread_id;
    if(pthread_create(&thread_id, NULL, recv_msg, NULL) == -1) {
        // fprintf(stderr,"pthread_create error!\n");
        printf("pthread_create error!\n");
        return -1;
    }
   
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  //TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(sock_fd < 0) {
        printf("Request socket failed\n");
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  //向服务器发信息
    // inet_pton(AF_INET, SERVER_IP, &ser_addr.sin_addr); 
    // ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //注意网络序转换
    ser_addr.sin_port = htons(SERVER_PORT);  //指定发送端口

    while(1) {
        printf("发送信息：\n");
        // gets(buffer);
        fgets(buffer, sizeof(buffer), stdin);
        if (!strncmp(buffer,"quit",4)){
            break;
        }
        sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&ser_addr,sizeof(ser_addr));
    }
    close(sock_fd);
    pthread_exit(NULL);
    return 0;
}

static void *recv_msg(void *arg) {
	printf("recv_msg running...\n");
    int ret;
    char buffer[BUFFER_SIZE];
    int fd;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);  //TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(fd < 0) {
        printf("Request socket failed\n");
        return NULL;
    }

    //自己作为server
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //监听任意IP
    cli_addr.sin_port = htons(CLIENT_PORT);        //指定监听端口
    if(bind(fd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0 ) {
        printf("bind failed!\n"); 
        return NULL;
    }

    struct sockaddr_in server;
    int ServerAddrLen = sizeof(server);
    while(1) {
        // printf("666====\n");
        memset(buffer, 0, BUFFER_SIZE);
        ret = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server, &ServerAddrLen);
        if(ret < 0){
            printf("recv failed\n");
        }
        printf("收到信息：\n");
        // puts(buffer);
        fputs(buffer, stdout);
    }
}

// 1，单独编译
// gcc client.c -o client
// gcc server.c -o server
// 开两个终端
// ./server
// ./client
// 2，makefile编译
// make



