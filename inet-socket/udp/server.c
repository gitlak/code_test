/**
	  udp 测试
	  jet 2019-6.30 15:30  zhonghaihuanyu 
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
#include<ctype.h>


#define MAX_CONNECT_NUM 2
#define BUFFER_SIZE 1024
#define SERVER_PORT 8888
#define IPP         "192.168.2.106"

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
    // ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // ser_addr.sin_addr.s_addr = inet_addr(IPP);
    inet_pton(AF_INET, IPP, &ser_addr.sin_addr);
    ser_addr.sin_port = htons(SERVER_PORT);

    if(bind(fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0 ) {
        printf("bind failed!\n"); 
        return -1;
    }

    FILE *fp = NULL;
    fp = fopen("recv_info.txt","a");
    if (fp == NULL) {
        printf("open file error!\n");
        exit(1);
    }
    char str[] = "This is runoob.com\n";
    fwrite(str, sizeof(str), 1, fp);


 	struct sockaddr_in clientAddr;
 	int clientAddrLen;
    clientAddrLen = sizeof(clientAddr);
    memset(&clientAddr, 0, sizeof(clientAddr));
    while(1) {
        printf("Waiting for message...\n");
        bzero(buffer, BUFFER_SIZE);   //不可少
        ret = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if(ret < 0) {
            printf("recv failed\n");
        }
        printf("收到信息：\n");
        fputs(buffer, stdout);
        // fputs(buffer, fp);

        if (!strncmp(buffer,"quit",4)) {
            break;
        }

        n = strlen(buffer);
        printf("n ==== %d\n", n);
        int k = fwrite(buffer, 1, n, fp);     //把接收的信息写入文件recv_info.txt
        printf("k ========= %d\n", k);
        // fwrite(buffer, n, 1, fp);     //把接收的信息写入文件recv_info.txt
        for(i = 0; i < n; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        // write(new_fd, buffer, n);
        printf("转换为大写：\n");
        fputs(buffer, stdout);
        sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&clientAddr, clientAddrLen);

        // break;
    }
    close(fd);
    fclose(fp);
    return 0;
}
