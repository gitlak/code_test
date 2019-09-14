

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include<linux/delay.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include<asm/uaccess.h>
#include <linux/slab.h>


#define NETLINK_USER  22
#define USER_MSG    (NETLINK_USER + 1)
#define MAX_PLOAD 1024
#define CLINUM 10
#define MSG_LEN 512

static char msg[MSG_LEN]={0};
static char *str = NULL;
char str2[MSG_LEN] = {0};

int index1 = 0;
int client_num = 0;
int cli[CLINUM] = {0};
//char* (*pf)(char *arg);
int var = 1;

char confg[CLINUM]={'n','n','n','n','n','n','n','n','n','n'};
int confg2[CLINUM]={0};


int characters_num = 0;
char *ch[3];

static struct sock *netlinkfd = NULL;

//toupper
char* strupr(char* src)
{
    while (*src != '\0')
    {
        if(*src >= 'a' && *src <= 'z')
        {
            *src -= 32;
            msleep_interruptible(200);
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
            msleep_interruptible(200);
        }
        src++;
    }
    return src;
}

int send_msg(int8_t *pbuf, uint16_t len, int pid)
{

    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;

    int ret;
    nl_skb = nlmsg_new(NLMSG_SPACE(len), GFP_ATOMIC);
    if(!nl_skb)
    {
		printk("netlink_alloc_skb error\n");
		return -1;
    }
	nlh = nlmsg_put(nl_skb, 0, 0, USER_MSG, len, 0);
	if (nlh == NULL)
	{
		printk("nlmsg_put() error\n");
		nlmsg_free(nl_skb);
		return -1;
	}
	memcpy(nlmsg_data(nlh), pbuf, len);
	ret = netlink_unicast(netlinkfd, nl_skb, pid, MSG_DONTWAIT);
	return ret;
}

int findzero(int arr[])
{
	int i;
	for(i = 1; i < CLINUM;i++)
	{
		if(arr[i] == 0)
			return i;
	}
	return -1;
}

static void recv_cb(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    char *data = NULL;
    char *data2 = NULL;
    int length = 0;
    int pid = 0;
    int msg_len = 0;

    int i;
    int ind = 0;

    printk("skb->len:%u\n", skb->len);
    if(skb->len >= nlmsg_total_size(0))
    {
        nlh = nlmsg_hdr(skb);
        msg_len =  nlh->nlmsg_len - sizeof(struct nlmsghdr);
        printk("msg_len----%u\n",msg_len);
        pid = nlh-> nlmsg_pid;

        for (i = 1; i < CLINUM; i++) {
			//printk("888888888888888888888888\n");
			if (cli[i] == pid)
			{
				ind = i;
				break;
			}
			else
				continue;
		}
		if (i == CLINUM){
			printk("888888888888888888888888\n");
			//cli[var] = pid;
			//ind = var++;
			ind = findzero(cli);
			cli[ind] = pid;
			client_num++;
		}
        data = NLMSG_DATA(nlh);
        data2 = data;
        length  = strlen(data2);
        characters_num += length;

        printk("data length----%u\n",length);
        printk("nlmsg_pid-----------%d\n",pid);

        if(data)
        {
        	if(strncmp(data,"quit",4) == 0)
        	{
        		printk("345--------------\n");
        		printk("Disconnected from the Client %d\n",pid);
        		client_num--;
        		cli[i] = 0;
        		confg[i] = 'n';
        	}
        	else
        	{
				if (confg[ind] == 'u')
				{
					strupr(data2);
					send_msg(data2, length+1, pid);
				}
				else if (confg[ind] == 'l')
				{
					strlwr(data2);
					send_msg(data2, length+1, pid);
				}
				else if (confg[ind] == 'n')
					send_msg(data2, length+1, pid);
				else
				{
					if (confg[0] == 'u')
					{
						strupr(data2);
						send_msg(data2, length+1, pid);
					}
					else if (confg[0] == 'l')
					{
						strlwr(data2);
						send_msg(data2, length+1, pid);
					}

					else if (confg[0] == 'n')
						send_msg(data2, length+1, pid);
					else
					{
						//char *buf = NULL;
						printk("mode is not set,please wait for a minute!\n");
						//sprintf(buf, "%s","mode is not set,please wait for a minute!\n");
						//send_msg(buf, strlen(buf)+1,pid);
					}
				}
        	}

			printk("kernel receive data: %s\n",data);
			//pf(data2);            // tolower   toupper
			//send_msg(data, nlmsg_len(nlh));
            //send_msg(data2, length+1, pid);
        }
        //memset(NLMSG_DATA(nlh), 0, msg_len);
        memset(NLMSG_DATA(nlh), 0, nlh->nlmsg_len);
        //bzero(NLMSG_DATA(nlh),nlh->nlmsg_len);
        //nlmsg_free(skb);
    }
}

struct netlink_kernel_cfg cfg =
{
    .input = recv_cb,
};

int str2int(char *str)
{
	int temp = 0;
	while(*str != 0)
	{
		temp = temp * 10 + (*str-'0');
		str++;
	}
	return temp;
}

static ssize_t my_write(struct file *file, const char *buffer, unsigned long count,
		void *data) {
	char *cp;
	char *r;
	int i;
	char delims[] = " ";
	unsigned long len = sizeof(msg);
	if (count >= len)
		count = len - 1;
	if (copy_from_user(msg, (void*) buffer, count))
		return -EFAULT;
	msg[count] = '\0';
	str = msg;

	printk("strlen(msg)-----%lu\n",strlen(msg));


	strcpy(str2,msg);

	printk("str in write-----------%s\n", str);

	ch[0] = strsep(&str, delims);
	ch[1] = strsep(&str, delims);
	ch[2] = strsep(&str, delims);

	r = ch[1];
	index1 = str2int(r);
	cp = ch[2];
	if(!index1)
	{
		for(i = 1;i < 10;i++)
			if(!confg2[i])
				confg[i] = *cp;
	}
	else
	{
		confg[index1] = *cp;
		confg2[index1] = 1;
	}
	confg[index1] = *cp;
	printk("index1----------%d\n",index1);
	printk("ch[1]------%s,ch[2]------%c\n", r, confg[index1]);

	return count;
}

static int hello_proc_show(struct seq_file *m, void *v)
{
	//seq_printf(m, "Hello proc!\n");
	//seq_printf(m, "str2******************%s\n",str2);
	int j;
	seq_printf(m,"Config info:\n");
	seq_printf(m, "     index1      cli_id     configure\n");
	for (j = 0; j < CLINUM; j++)
		seq_printf(m, "       %d          %d         %c\n", j, cli[j],confg[j]);

	seq_printf(m,"Server info:\n");
	seq_printf(m,"         Global mode: %c\n",confg[0]);
	seq_printf(m,"         Characters:  %d\n",characters_num);
	seq_printf(m,"         Clients:     %d\n", client_num);
	return 0;
}
static int hello_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, hello_proc_show, NULL);
}
static const struct file_operations fops = {
		.owner = THIS_MODULE,
		.open =  hello_proc_open,
		.read =  seq_read,
		.write = my_write,
		.llseek = seq_lseek,
		.release = single_release,
};

static int __init test_netlink_init(void)
{

    printk("init netlink_demo!\n");
    proc_create("hello_proc", 0, NULL, &fops);

    netlinkfd = netlink_kernel_create(&init_net, USER_MSG, &cfg);
    if(!netlinkfd)
    {
        printk(KERN_ERR "can not create a netlink socket!\n");
        return -1;
    }
    printk("netlink demo init ok!\n");
    return 0;
}

static void __exit test_netlink_exit(void)
{
	remove_proc_entry("hello_proc", NULL);

    sock_release(netlinkfd->sk_socket);
    printk(KERN_DEBUG "netlink exit\n!");
}


MODULE_AUTHOR("root");
MODULE_DESCRIPTION("kernel_to_user_demo");
MODULE_LICENSE("Dual BSD/GPL");
module_init(test_netlink_init);
module_exit(test_netlink_exit);





