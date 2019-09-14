/**
  AF_UNIX 测试
  jet 2019-6.23 11:30  zhonghaihuanyu 
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
#include<ctype.h>


#define MAX_CONNECT_NUM 2
#define BUFFER_SIZE 1024
char* server_file = "server.sock";


int main()
{
    int fd,new_fd,len,i,n,ret;
    struct sockaddr_un un;
    char buffer[BUFFER_SIZE];

    fd = socket(AF_UNIX, SOCK_DGRAM, 0); ////TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(fd < 0){
        printf("Request socket failed!\n");
        return -1;
    }

    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    unlink(server_file);
    strcpy(un.sun_path, server_file);

    if(bind(fd, (struct sockaddr *)&un, sizeof(un)) < 0 ) {
        printf("bind failed!\n"); 
        return -1;
    }
   
    struct sockaddr_un clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    while(1) {
        printf("Waiting for message...\n");
        
        bzero(buffer, BUFFER_SIZE);   //不可少
        memset(&clientAddr, 0, sizeof(clientAddr));
        
        ret = recvfrom(fd, buffer, 512, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if(ret < 0){
            printf("recv failed\n");
        }
        printf("收到信息：\n");
        // puts(buffer);
        fputs(buffer, stdout);
        
        n = strlen(buffer);
        printf("n ==== %d\n", n);
        for(i = 0; i < n; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        // write(new_fd, buffer, n);
        printf("发送信息：\n");
        // puts(buffer);
        fputs(buffer, stdout);
        sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&clientAddr, clientAddrLen);
       
    }
    close(fd);
    return 0;
}
