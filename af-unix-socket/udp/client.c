/**
jet 2019-6-23
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<errno.h>
#include<stddef.h>
#include<unistd.h>
#define BUFFER_SIZE 1024

char* server_file = "server.sock";
char* client_file = "client.sock";

int main()
{
    struct sockaddr_un un;
    int sock_fd, ret;
    char buffer[BUFFER_SIZE];
   
    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);  //TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(sock_fd < 0) {
        printf("Request socket failed\n");
        return -1;
    }
  
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    unlink(client_file);
    strcpy(un.sun_path, client_file);


    if(bind(sock_fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
        perror("bind fail");
        printf("bind failed!\n"); 
        return -1;
    }

    struct sockaddr_un serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, server_file);
    int ServerAddrLen = sizeof(serveraddr);
    while(1) {
        printf("发送信息：\n");
        // gets(buffer);
        fgets(buffer, sizeof(buffer), stdin);
        if (!strncmp(buffer,"quit",4)) {
            break;
        }
        
        sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&serveraddr, ServerAddrLen);
        ret = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serveraddr, 
                  &ServerAddrLen);
        if(ret < 0) {
            printf("recv failed\n");
        }
        printf("收到信息：\n");
        // puts(buffer);
        fputs(buffer, stdout);
    }
    close(sock_fd);
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



