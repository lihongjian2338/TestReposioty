#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
/* 经多测试得到 */
/* getpid()即进程ID，线程组ID, 如：38640 */
/* pid = syscall(SYS_gettid)即每个线程task_struct中pid参数，每个线程不一样，如两个线程：38640, 38641 */
/* pthread_self() 和 pthread_create(&tid,NULL,thread_run,NULL)中tid
   线程各自栈的起始地址，如：0x7fe3e13f6700*/

void* thread_run(void* arg)
{
    (void)arg;
    pid_t pid = syscall(SYS_gettid);

    while(1){
        printf("I am thread->进程ID：%d ->内核线程ID：%d ->用户态线程ID：%lx\n",getpid(),pid,pthread_self());
        sleep(1);
    }

}
int main()
{
    pid_t pid = syscall(SYS_gettid);//获取内核中的线程ID

    int ret;
    pthread_t tid;

    ret = pthread_create(&tid,NULL,thread_run,NULL);
    if(ret != 0){
        perror("pthread_create");
        exit(1);
    }

    while(1){
        printf("I am main  ->进程ID：%d ->内核线程ID：%d ->用户态线程ID：%lx ->创建tid:%lx\n",getpid(),pid,pthread_self(),tid);
        sleep(1);
    }

    return 0;
}
