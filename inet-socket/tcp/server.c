/*
//  tcp 测试
//  server.cpp 
//  linux_socket_api_server
//
//  Created by jet on 2019-6-29.
//  Copyright (c) 2019年 jet. All rights reserved.
*/

// #include<stdio.h>
// int main(int argc, char const *argv[])
// {
// 	printf("hello world----->\n");
// 	return 0;
// }

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
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

int tsocket(int argc, const char * argv[]);
static void *msg_handle(void * sock_fd);   //Only can be seen in the file


int main(int argc, const char * argv[]) {
    (void)tsocket(argc,argv);
    return 0;
}
int tsocket(int argc, const char * argv[]) {
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = atoi(argv[3]);
    int fd;
    int check_ret;

    if(argc < 3) {
    	printf("please input arguments! \n");
        exit(-1);
    }

    cout << "ip=" << ip << " port="<<port << " backlog=" << backlog<<endl;
    // printf("ip=%s  port=%d  backlog=%d\n", ip, port, backlog);
    fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        printf("Request socket failed!\n");
        return -1;
    }

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    //转换成网络地址
    address.sin_port = htons(port);
    address.sin_family = AF_INET;
    //地址转换
    inet_pton(AF_INET, ip, &address.sin_addr);   //指定IP
    // address.sin_addr.s_addr = htonl(INADDR_ANY);    //不指定IP
    //设置socket buffer大小
    int recvbuf = 4096;
    int len = sizeof( recvbuf );
    setsockopt( fd, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof( recvbuf ) );
    getsockopt( fd, SOL_SOCKET, SO_RCVBUF, &recvbuf, ( socklen_t* )&len );
    printf( "the receive buffer size after settting is %d\n", recvbuf );

    int reuse=1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)) < 0) {
     	perror("setsockopet error\n");
  		return -1;
 	}

    //绑定ip和端口
    check_ret = bind(fd,(struct sockaddr*)&address,sizeof(address));
    if(check_ret < 0) {
    	printf("bind failed!\n");
    	return -1;
    }

    //创建监听队列，用来存放待处理的客户连接
    check_ret = listen(fd, backlog);
    if(check_ret < 0) {
    	printf("bind failed!\n");
    	return -1;
    }


    struct sockaddr_in addressClient;
    socklen_t clientLen = sizeof(addressClient);
    int connfd;
    char showData[INET_ADDRSTRLEN];
    char sockBuf[BUFFER_SIZE];
	size_t ret;
	pthread_t thread_id;
    while(1) {
	    //接受连接，阻塞函数
	    connfd = accept(fd, (struct sockaddr*)&addressClient, &clientLen);
        cout<<"connfd === "<<connfd<<endl;
	    if(connfd < 0) {
	        // std::cout << "accept error";
	        printf("accept error\n");
	        return -1;
	    } else {
	        //打印客户端信息    
	        cout <<"client:"<<inet_ntop(AF_INET,&addressClient.sin_addr,showData, 
                    INET_ADDRSTRLEN)<<":" <<ntohs(addressClient.sin_port)<<" connected!"<<endl;
	        if(pthread_create(&thread_id, NULL, msg_handle, (void *)(&connfd)) == -1) {
	            fprintf(stderr,"pthread_create error!\n");
	            break;                                  //break while loop
	        }
	    }
	    // close(fd);
	}
    close(fd);
    return 0;
}

static void *msg_handle(void *sock_fd) {
	int connfd, i;
    char sockBuf[BUFFER_SIZE];
	size_t ret;
    struct sockaddr_in tmpAddress;
    socklen_t clientLen = sizeof(tmpAddress);
    char showData[INET_ADDRSTRLEN];

	connfd = *((int *)sock_fd);
    // //获取远端socket信息
    // getpeername(connfd,(struct sockaddr*)&tmpAddress, &clientLen );
    // cout <<"client:"<<inet_ntop(AF_INET, &tmpAddress.sin_addr, showData, 
    //          INET_ADDRSTRLEN)<<":" <<ntohs(tmpAddress.sin_port)<<endl;
    // cout<<endl;
    while(1) {
	    memset(sockBuf, 0, BUFFER_SIZE);
        ret = recv(connfd, sockBuf, BUFFER_SIZE-1, 0);   
        cout<<"收到消息："<<endl;
        printf("msg_len = %ld, msg = %s", ret, sockBuf);
        if (!strncmp(sockBuf,"quit",4)) {
	        break;
	    }

	    for(i = 0; i < ret-1; i++) {
            sockBuf[i] = toupper(sockBuf[i]);
        }
        // write(connfd, sockBuf, ret-1);
        printf("发送信息：\n");
        // puts(sockBuf);
        fputs(sockBuf, stdout);
        cout<<endl;
        send(connfd, sockBuf, strlen(sockBuf),0);
    }
    getpeername(connfd,(struct sockaddr*)&tmpAddress, &clientLen);
    cout<<"client:"<<inet_ntop(AF_INET, &tmpAddress.sin_addr, showData, 
             INET_ADDRSTRLEN)<<":" <<ntohs(tmpAddress.sin_port)<<"  exit!"<<endl;
    close(connfd);            //close a file descriptor.
    pthread_exit(NULL);   //terminate calling thread!
}


//g++ server.c -o server -lpthread 
//./server 192.168.2.106 2019 99 
