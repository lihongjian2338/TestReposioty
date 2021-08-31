//This is the demo about the poll usage
#include  <unistd.h>
#include  <sys/types.h>       /* basic system data types */
#include  <sys/socket.h>      /* basic socket definitions */
#include  <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include  <arpa/inet.h>       /* inet(3) functions */
 
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
 
 
#include <poll.h> /* poll function */
#include <limits.h>
 
#define MAXLINE 1024
 
#ifndef OPEN_MAX
#define OPEN_MAX 4096
#endif

/*注：原始代码可以查看UNIX网络编程poll使用*/
int main(int argc, char **argv)
{
    int                 i, maxi, listenfd, connfd, sockfd;
    int                 nready;
    ssize_t             n;
    char                buf[MAXLINE];
    socklen_t           clilen;
    struct pollfd       client[OPEN_MAX];
    struct sockaddr_in  cliaddr, servaddr;
 
    listenfd = socket(AF_INET, SOCK_STREAM, 0);//监听fd
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(6888);
 
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
 
    listen(listenfd, 1024);
 
    client[0].fd = 0;
    client[0].events = POLLIN;
    client[1].fd = listenfd;
    client[1].events = POLLIN;  /* 监听标准输入，下列循环从2开始 */
    for (i = 2; i < OPEN_MAX; i++)
    {
        client[i].fd = -1;      /* -1 indicates available entry */
    }
    maxi = 1;                   /* max index into client[] array */
 
    for ( ; ; ) 
    {
        nready = poll(client, maxi+1, 2*1000);//maxi表示client数组大小,
        /* timeout是等待毫秒数, 负数表示无限等待， 为0表示调用后立即返回 */
        /* 执行结果：为0表示超时前没有任何事件发生；-1表示失败；成功则返回结构体中revents不为0的文件描述符个数 */

        /*
        *new client connection
        *每次有新连接都会执行这个if语句，然后将新添加的链接调用accept来接受链接
        */

        if(nready<0)
        {
            printf("poll fail\n");
        }
        else if(nready == 0)
        {
            printf("timeout\n");
        }
        else
        {
            if(client[0].revents & POLLIN)
            {
                printf("this is the stdin");
            }
            if (client[1].revents & POLLIN) 
            {   
                clilen = sizeof(cliaddr);
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);//accept函数返回了一个socketfd，

    
                for (i = 2; i < OPEN_MAX; i++)//监视connfd是否可读、可写
                {
                    if (client[i].fd < 0) 
                    {
                        client[i].fd = connfd;  /* save descriptor */
                        break;
                    }
                }
                if (i == OPEN_MAX)
                {
                    printf("too many clients\n");
                }
    
                client[i].events = POLLIN;//检测connfd是否可读
                if (i > maxi)
                    maxi = i;               /* max index in client[] array */
    
                if (--nready <= 0)/*如果除了listen的client[0]被激活，其他事件没有没有被激活则nready是1
                    *自减1后，为0，表示此次处理poll结束。继续下次监视。
                    */
                    continue;               /* no more readable descriptors */
            }
    
            for (i = 2; i <= maxi; i++) 
            {    /* 第0个元素是处理listen的，处理其余accept的所有可读的connfd */
                if ((sockfd = client[i].fd) < 0)//无效的fd
                    continue;
                if (client[i].revents & (POLLIN | POLLERR)) 
                {//处理可读的connfd
                    if ( (n = read(sockfd, buf, MAXLINE)) < 0) 
                    {
                        if (errno == ECONNRESET) /*connection reset by client */
                        {
                            close(sockfd);
                            client[i].fd = -1;
                        } 
                        else
                        {
                            printf("read error");
                            //err_sys("read error");
                        }
                    } 
                    else if (n == 0) /*connection closed by client */
                    {
                        close(sockfd);
                        client[i].fd = -1;
                    } 
                    else
                    {
                        //writen(sockfd, buf, n);
                        write(sockfd, buf, n);
                    }
                    if (--nready <= 0)
                        break;              /* no more readable descriptors */
                }
            }   
        }
    }
}
