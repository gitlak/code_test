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
const char *filename="uds-tmp";

int main()
{
    struct sockaddr_un un;
    int sock_fd,ret;
    char buffer[BUFFER_SIZE];
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path,filename);
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);  //TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(sock_fd < 0){
        printf("Request socket failed\n");
        return -1;
    }
    if(connect(sock_fd,(struct sockaddr *)&un,sizeof(un)) < 0){
        printf("connect socket failed\n");
        return -1;
    }
    while(1){
        printf("发送信息：\n");
        // gets(buffer);
        fgets(buffer, sizeof(buffer), stdin);
        if (!strncmp(buffer,"quit",4)){
            break;
        }
        send(sock_fd,buffer,strlen(buffer),0);
        ret = recv(sock_fd,buffer,BUFFER_SIZE,0);
        if(ret < 0){
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



