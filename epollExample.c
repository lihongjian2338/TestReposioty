#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAX 50  //此MAX用来表示存放文件描述符的位置
#define SER_PORT 5555
 
void write_to_all(char *wbuf);    //此函数用于把一个客户端发来的消息转发给其他客户端
int client_fd[MAX];       //此数组用于装连接进来的客户端的套接字文件描述符

int main()
{
	int sockfd,newsockfd;
	struct sockaddr_in saddr;
 
	int size = sizeof(struct sockaddr_in);
	
	//创建2个epoll_event结构体
	struct epoll_event event;	//用于往epoll句柄加入新的待检测套接字
	struct epoll_event wait_event;   //用于装epoll_wait（）返回的响应的套接字的消息，判断哪个套接字发生响应
 
	int on = 1;
	int ret;
	
	//设置TCP套接字的一些属性
	bzero(&saddr,size);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SER_PORT);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));	//设置端口复用，不设也可以，对功能没影响
	bind(sockfd,(struct sockaddr*)&saddr,sizeof(struct sockaddr));
	listen(sockfd,10);
 
	//epoll 相关参数
	int max = 0;//此max表示需要监控有效文件描述符个数
	memset(client_fd,-1,sizeof(client_fd));
 
	int epfd;
	//创建epoll句柄epfd
	epfd = epoll_create(10);   //这是10没啥意义，只要是正数即可。
 
	//加入监听套接字
	event.data.fd = sockfd;
	event.events = EPOLLIN;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&event);
 
	//加入标准输入，使服务端可以发信息
	event.data.fd = 0;
	event.events = EPOLLIN;
	epoll_ctl(epfd,EPOLL_CTL_ADD,0,&event);
 
	int i;
	while(1)
	{
		//等待套接字发生某种响应，否则一直阻塞
        //(max+2)maxevents告之内核这个events有多大，这个 maxevents的值不能大于创建epoll_create()时的size，
		ret = epoll_wait(epfd,&wait_event,max+2,-1);
		//判断是否有新客户端连入
		if((wait_event.data.fd ==sockfd) && 
			(wait_event.events &EPOLLIN == EPOLLIN))
		{
			int len = sizeof(struct sockaddr);
			struct sockaddr_in caddr;
			newsockfd = accept(sockfd,(struct sockaddr*)&caddr,
				&len);
			//newsockfd加入client_fd
			for(i=0;i<MAX;i++)
			{
				if(client_fd[i] != -1)
					continue;
				else
				{
					client_fd[i] = newsockfd;
					break;
				}
			}		
 
			//newsockfd加入epoll句柄 epfd
			event.data.fd = newsockfd;
			event.events = EPOLLIN;
			epoll_ctl(epfd,EPOLL_CTL_ADD,newsockfd,&event);
			max+=1;
 
			ret --;//处理完一个响应后，ret减一
			if(ret<=0)//若ret已经为0，则说明已经处理完所有的发生响应的套接字，就可以继续epoll_wait（）等待套接字发生再响应
			{
				continue;
			}
		}
		if((wait_event.data.fd ==0)&&(wait_event.events & EPOLLIN==EPOLLIN))
		{
			char wbuf[50]={0};
			bzero(wbuf,50);
			scanf("%s",wbuf);
			write_to_all(wbuf);
			ret-=1;
		}
	
		//判断有没有其他客户端的数据发来
		for(i=0;i<MAX;i++)
		{
			char rbuf[50]={0};
			if((wait_event.data.fd ==client_fd[i])
				&&(wait_event.events&EPOLLIN==EPOLLIN))
			{
				int recv_num;
				bzero(rbuf,50);
				recv_num = recv(client_fd[i],rbuf,50,0);
				if(recv_num == 0)
				{
					client_fd[i] = -1;
					close(client_fd[i]);
					max -=1;
				}
				else
				{
					printf("%s\n",rbuf);
					bzero(rbuf,50);
				}
				ret-=1;
				if(ret<=0)
					break;
			}
		}
	}
}
 
void write_to_all(char *wbuf)
{
	int i;
	for(i=0;i<MAX;i++)
	{
		if(client_fd[i] !=-1)
		{
			write(client_fd[i],wbuf,50);
		}
	}
}
