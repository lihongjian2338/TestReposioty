//This is the demo for thread max num test in a process, use top watch the thread status.
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

void *thread(void *vargp)
{
    //让线程循环，不退出
    while(1)
    {
        sleep(1);
    }
}

int main()
{
    int err = 0, count = 0;
    pthread_t tid;
    //持续创建线程，直到创建失败
    while(err==0)
    {
        //创建线程
        err = pthread_create(&tid,NULL,thread,NULL);
        count++;
        printf("count=%d\n",count);
    }
    printf("create thread error:%s\n",strerror(errno));
    printf("Maximum number of thread within a process is :%d\n",count);

    //输入回车键，程序才推出，可以方便观察创建完线程后，进程的top情况
    getchar();

    return 0;
}
