/*
//  client.cpp
//  linux_socket_api_client
//
//  Created by jet on 2019-6-29.
//  Copyright (c) 2019年 jet. All rights reserved.
*/
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

using namespace std;
#define BUFFER_SIZE 1024

void tserver(int argc, const char *argv[]);

int main(int argc, const char * argv[]) {
    tserver(argc,argv);
    return 0;
}
void tserver(int argc, const char *argv[]) {
    std::cout << "tserver start..." << std::endl;
    if(argc < 3) {
    	cout<< "please input arguments!" <<endl;
        exit(-1);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = atoi(argv[3]);

    std::cout << "server ip=" << ip << " port="<<port << " backlog=" << backlog  << std::endl;

    int fd;
    int check_ret, ret;
    char buffer[BUFFER_SIZE];

    fd = socket(PF_INET,SOCK_STREAM , 0);
    assert(fd >= 0);

    int sendbuf = 4096;
    int len = sizeof( sendbuf );
    setsockopt( fd, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof( sendbuf ) );
    getsockopt( fd, SOL_SOCKET, SO_SNDBUF, &sendbuf, ( socklen_t* )&len );
    printf( "the send buffer size after settting is %d\n", sendbuf );

    struct sockaddr_in address;
    bzero(&address,sizeof(address));

    //转换成网络地址
    address.sin_port = htons(port);
    address.sin_family = AF_INET;
    //地址转换
    inet_pton(AF_INET, ip, &address.sin_addr);
    check_ret = connect(fd, (struct sockaddr*) &address, sizeof(address));
    assert(check_ret >= 0);
    //发送数据
    // const char* oob_data = "abc";
    // const char* normal_data = "my boy!";
    // send(fd, normal_data, strlen(normal_data), 0);
    // send(fd, oob_data, strlen(oob_data), MSG_OOB);//MSG_OOB
    // send(fd, normal_data, strlen(normal_data), 0);
    

    while(1) {
	    printf("发送信息：\n");
	    // gets(buffer);
	    fgets(buffer, sizeof(buffer), stdin);
	    send(fd, buffer, strlen(buffer), 0);
	    // ret = recv(sock_fd,buffer,BUFFER_SIZE,0);

	    if (!strncmp(buffer,"quit",4)){
	        break;
	    }

	    ret = recv(fd, buffer, BUFFER_SIZE-1, 0);
        assert(ret != -1);
	    // if(ret < 0){
	    //     printf("recv failed\n");
	    // }
	    printf("收到信息：\n");
	    // puts(buffer);
	    fputs(buffer, stdout);
	}
    close(fd);
}

//g++ client.c -o client
//./client 192.168.2.106 2019 88 