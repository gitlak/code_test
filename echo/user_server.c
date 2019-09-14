

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include<errno.h>
#include<pthread.h>
#include<regex.h>
#include <signal.h>
#include <sys/msg.h>

#define QLEN   10
char *ch[3];
char *str = NULL;
char conf[QLEN]={'a','a','a','a','a','a','a','a','a','a'};


//int (*pf)(int arg);
int flag = 0;
int readlen = 0;

int client_num = 0;
int characters = 0;

//struct acp{
//	int cli_num;
//	int sockfd;
//};

//toupper
char* strupr(char* src)
{
    while (*src != '\0')
    {
        if(*src >= 'a' && *src <= 'z')
        {
            *src -= 32;
            usleep(200000);
        }
        src++;
    }
    return src;
}

// tolower
char* strlwr(char* src)
{
    while (*src != '\0')
    {
        if (*src >='A' && *src <= 'Z'){
            *src += 32;
            usleep(200000);
        }
        src++;
    }
    return src;
}



void *recvmessage(void *arg)
{
	int i,clfd,ind = 0;
	char buf[BUFSIZ];

	//client_num++;
	clfd = *(int *)arg;

	sprintf(buf, "Client ID is %d",client_num);
	send(clfd, buf, sizeof(buf),0);

	char *cha,*chb;
	char delims[] = " ";
	int len_str = 0;


	while(1)
	{
		memset(buf, 0, sizeof(buf));
		readlen = recv(clfd, buf, sizeof(buf), 0);
		//printf("readlen==%d\n", readlen);
		if (!readlen){
			printf("Disconnected from the client %d.\n",ind);
			client_num--;
			break;
		}

		characters += readlen;

		len_str = strlen(buf);
		chb = &buf[len_str-1];

		//printf("buf===========================%c\n",*chb);

		ind = atoi(chb);
		//printf("ind--------------%d\n",ind);
		buf[len_str-1] = '\0';
		int j;
		if (conf[ind] == 'u') {
			strupr(buf);

		} else if (conf[ind] == 'l') {
			strlwr(buf);

		} else if (conf[ind] == 'n')
			send(clfd, buf, sizeof(buf),0);
		else
		{
			if (conf[0] == 'u') {
				strupr(buf);

			} else if (conf[0] == 'l') {
				strlwr(buf);

			} else if (conf[0] == 'n')
				send(clfd, buf, sizeof(buf),0);
			else
			{
				printf("mode is not set,please wait for a minute!\n");
				sprintf(buf, "%s","mode is not set,please wait for a minute!\n");
				send(clfd, buf, sizeof(buf),0);
				continue;
			}
		}
		send(clfd, buf, sizeof(buf),0);
	}
}

int regx(char *buf)
{
	int status, i;
	regmatch_t pmatch[1];
	const size_t nmatch = 1;
	regex_t reg;
	const char *pattern = "^mode\\s[0-9]\\s(upper|lower|normal)\\s$";

	regcomp(&reg, pattern, REG_EXTENDED);
	status = regexec(&reg, buf, nmatch, pmatch, 0);
	if (status == REG_NOMATCH)
		printf("no match.\n");
	else if (0 == status) {
		printf("set mode successfully:  ");
		for (i = pmatch[0].rm_so; i < pmatch[0].rm_eo; ++i)
			putchar(buf[i]);
		printf("\n");
	}
	regfree(&reg);
	return status;
}

void *config(void *arg)
{
	printf("please set a  conversion mode first!\nthe format is :mode [] upper/lower/normal\n");
	for(;;)
	{
		char buff[128];
		printf("SERVER> ");
		fgets(buff, sizeof(buff), stdin);

		if (strncmp(buff, "show", 4) != 0)
		{
			int ret = regx(buff);
			if (ret == REG_NOMATCH)
			{
				printf("input error,please input again!\n");
			}
			else if (0 == ret)
			{
				//printf("match successfully!\n");
				char delims[] = " ";
				ch[0] = strtok(buff, delims);
				ch[1] = strtok(NULL, delims);
				ch[2] = strtok(NULL, delims);

				flag = 1;

				//printf("mode-------%s\n", ch[2]);
				int index = atoi(ch[1]);
				char *chp = ch[2];
				conf[index] = *chp;
				//printf("ch[1]-------%c\n", conf[index]);
			}
		}
		else
		{
			if (!flag)
				printf("no configuration information to show!\n");
			else {
				printf("Server info:\n");
				printf("		Global mode:%c\n", conf[0]);
				printf("		Characters:%d\n", characters);
				printf("		Clients:%d\n", client_num);
			}
			continue;
		}
	}
}


int main(){
	setbuf(stdout,NULL);
	unlink("server_socket");
	int server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, "server_socket");


	bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(listen(server_sockfd,QLEN)==-1)
	{
		perror("listen");
		exit(1);
	}

	int client_sockfd;
	struct sockaddr_un client_addr;
	socklen_t len = sizeof(client_addr);

	int pc;
	pthread_t recvtid,conftid;

	if((pc = pthread_create(&conftid, NULL, config, NULL)) != 0)
		perror("config  thread create  fails");

	while (1) {
		if(readlen >= 0)
		{
			if((client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr,&len)) < 0)
				perror("accept error\n");

			client_num++;
			if (client_num == QLEN)
				break;
			else
			{
				if ((pc = pthread_create(&recvtid, NULL, recvmessage,
					&client_sockfd)) != 0)
					perror("recvmessage thread create  fails");
			}
		}

	}
	close(client_sockfd);
	return 0;
}

