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
const char *filename="uds-tmp";

int main()
{
    int fd,new_fd,len,i,n,ret;
    struct sockaddr_un un;
    struct sockaddr_un client_addr;
    char buffer[BUFFER_SIZE];

    fd = socket(AF_UNIX, SOCK_STREAM, 0); ////TCP:SOCK_STREAM ; UDP:SOCK_DGRAM
    if(fd < 0){
        printf("Request socket failed!\n");
        return -1;
    }
    un.sun_family = AF_UNIX;
    unlink(filename);
    strcpy(un.sun_path, filename);
    if(bind(fd,(struct sockaddr *)&un,sizeof(un)) < 0 ){
        printf("bind failed!\n"); 
        return -1;
    }
    if(listen(fd, MAX_CONNECT_NUM) < 0){
        printf("listen failed!\n");
        return -1;
    }
    while(1){
        printf("Waiting for message...\n");
        
        len = sizeof(client_addr);
        new_fd = accept(fd,(struct sockaddr *)&client_addr,&len);
        // new_fd = accept(fd,NULL,NULL);
        if(new_fd < 0){
            printf("accept failed\n");
            return -1;
        }
        while(1) {
            bzero(buffer,BUFFER_SIZE);   //不可少
            ret = recv(new_fd,buffer,BUFFER_SIZE,0);
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
            send(new_fd,buffer,BUFFER_SIZE,0);
        }
        close(new_fd);
    }
    close(fd);
    return 0;
}
