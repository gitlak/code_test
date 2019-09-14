/**
    udp 测试
    jet 2019-6-30
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include<sys/un.h>
#include<errno.h>
#include<stddef.h>
#include<unistd.h>

#define SERVER_PORT 8888
#define SERVER_IP "192.168.2.106"
#define BUFFER_SIZE 1024

int main()
{
    struct sockaddr_in ser_addr;
    int sock_fd, ret;
    char buffer[BUFFER_SIZE];
   
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  //TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(sock_fd < 0) {
        printf("Request socket failed\n");
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    // inet_pton(AF_INET, SERVER_IP, &ser_addr.sin_addr); 
    // ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //注意网络序转换
    ser_addr.sin_port = htons(SERVER_PORT);  //注意网络序转换

    struct sockaddr_in src;
    int ServerAddrLen = sizeof(src);

    FILE *fp = NULL;
    fp = fopen("send_info.txt","r");
    if (fp == NULL) {
        printf("open file error!\n");
        exit(1);
    }

//==============
  
        ret = fread(buffer, 1, sizeof(buffer), fp);
        sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&ser_addr,sizeof(ser_addr));

        memset(buffer, 0, BUFFER_SIZE);
        ret = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&src, &ServerAddrLen);
        if(ret < 0) {
            printf("recv failed\n");
        }
        printf("收到信息：\n");
        fputs(buffer, stdout);  //输出到控制台
//================

    while(1) {
        printf("发送信息：\n");
        fgets(buffer, sizeof(buffer), stdin);
        sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&ser_addr,sizeof(ser_addr));
        if (!strncmp(buffer,"quit",4)) {
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        ret = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&src, &ServerAddrLen);
        if(ret < 0) {
            printf("recv failed\n");
        }
        printf("收到信息：\n");
        fputs(buffer, stdout);  //输出到控制台
        fputs(buffer, fp);      //输出到文件
    }
    close(sock_fd);
    fclose(fp);    //文件关闭后才能用cat查看
    fp = NULL;
    return 0;
}

// 1，单独编译
// gcc client.c -o client
// gcc server.c -o server
// 开两个终端
// ./server
// ./client
// 2，makefile编译
// make



