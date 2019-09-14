

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include<errno.h>
#include <signal.h>
#include <sys/msg.h>

#include <sys/msg.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>


//#define NETLINK_USER 22
//#define USER_MSG    (NETLINK_USER + 1)
#define MSG_LEN 100
#define MAX_PLOAD 1024
#define NETLINK_TEST 23

struct _my_msg
{
    struct nlmsghdr hdr;
    char  data[MSG_LEN];
};

int com_kernel(void)
{
	struct sockaddr_nl local, dest_addr;
	int skfd;
	struct nlmsghdr *nlh = NULL;
	struct _my_msg info;
	int ret;
	char send_buff[BUFSIZ];

	skfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
	if (skfd == -1) {
		printf("create socket error...%s\n", strerror(errno));
		return -1;
	}

	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0;

	if (bind(skfd, (struct sockaddr *) &local, sizeof(local)) != 0) {
		printf("bind() error\n");
		close(skfd);
		return -1;
	}

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; // to kernel
	dest_addr.nl_groups = 0;

	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
	memset(nlh, 0, sizeof(struct nlmsghdr));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
	nlh->nlmsg_flags = 0;
	nlh->nlmsg_type = 0;
	nlh->nlmsg_seq = 0;
	nlh->nlmsg_pid = getpid(); //self port

	printf("Client ID is %d.\n", local.nl_pid);

	while (1)
	{
		printf("CLIENT> ");
		fgets(send_buff, sizeof(send_buff), stdin);

		if (strncmp(send_buff, "quit", 4) == 0) {
			printf("Disconnected from the server！\n");
			bzero(NLMSG_DATA(nlh),nlh->nlmsg_len);
			memcpy(NLMSG_DATA(nlh), send_buff, strlen(send_buff));
			sendto(skfd, nlh, nlh->nlmsg_len, 0,
					(struct sockaddr *)&dest_addr, sizeof(struct sockaddr_nl));
			break;
		}

		bzero(NLMSG_DATA(nlh),nlh->nlmsg_len);
		memcpy(NLMSG_DATA(nlh), send_buff, strlen(send_buff));
		ret = sendto(skfd, nlh, nlh->nlmsg_len, 0,
				(struct sockaddr *)&dest_addr, sizeof(struct sockaddr_nl));

		if (!ret)
		{
			perror("sendto error1\n");
			close(skfd);
			exit(-1);
		}

		//memset(&info, 0, sizeof(info));
		ret = recvfrom(skfd, &info, sizeof(struct _my_msg), 0,
				(struct sockaddr *)&dest_addr, (socklen_t *)sizeof(dest_addr));
		if (!ret)
		{
			perror("recv form kernel error\n");
			close(skfd);
			exit(-1);
		}
		printf("ECHO: %s\n", info.data);
	}
	close(skfd);
    //free(nlh);
    return 0;
}



void com_user(void)
{
	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	char buf[BUFSIZ];
	int n;

	struct sockaddr_un address;
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, "server_socket");

	int result = connect(sockfd, (struct sockaddr *) &address, sizeof(address));
	if (result == -1) {
		perror("Unable to establish a connection with the server！");
		exit(1);
	}
	recv(sockfd, buf, sizeof(buf), 0);
	printf("%s\n", buf);

	char *ch;
	int last_ch;
	char delims[] = " ";
	ch = strtok(buf, delims);
	ch = strtok(NULL, delims);
	ch = strtok(NULL, delims);
	ch = strtok(NULL, delims);

	last_ch = atoi(ch);
	//printf("last_ch------%d\n",last_ch);

	int buf_len = 0;
	char cha = *ch;
	while (1) {
		memset(buf, 0, sizeof(buf));
		printf("CLIENT>");
		fgets(buf, sizeof(buf), stdin);

		buf_len = strlen(buf);
		buf[buf_len] = cha;
		buf[buf_len+1] = '\0';
		//printf("buf===========================%c\n",buf[buf_len]);

		if (strncmp(buf, "quit", 4) == 0) {
			printf("Disconnected from the server！\n");
			//send(sockfd, buf, strlen(buf), 0);
			//sleep(1);
			break;
		}
		int r = send(sockfd, buf, strlen(buf), 0);
		//printf("Write r ===== %d\n", r);
		n = recv(sockfd, buf, sizeof(buf), 0);
		//printf("Read len =========== %d\n", n);
		printf("ECHO: %s", buf);
		//write(STDOUT_FILENO, buf, n);
	}

	//char *tp = "Disconnected from the client";
	//sprintf(tp, "%s--%d!",tp,pid);
	//write(sockfd, tp, strlen(tp));
	close(sockfd);
}


int main(int argc, char *argv[])
{
	//signal(SIGQUIT, signal_handle);
	if(argc == 1)
	{
		printf("too few arguments,please input again!\n");
		exit(0);
	}
	if(strcmp(argv[1],"-u") == 0)
		com_user();
	else if(strcmp(argv[1],"-k") == 0)
		com_kernel();
	else
	{
		printf("format error,please input again!\n");
		return 1;
	}
	return 0;
}

