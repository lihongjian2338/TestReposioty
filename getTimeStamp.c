//This is the get timestamp way, use gettimeofday()
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

int main()
{
    struct timeval tv;
    long int time;
    double time_d;

    gettimeofday(&tv, NULL);
    time = tv.tv_sec *1000 + tv.tv_usec/1000;
    printf("tv_sec:%lu\n",tv.tv_sec);
    printf("tv_usec:%lu\n",tv.tv_usec);
    printf("tv_usec+tv_sec:%lu\n",time);
    printf("time sizeof:%d\n", sizeof(time));
    printf("tv_usec+tv_sec(x):%lx\n",time);
    printf("tv_usec+tv_sec(2x):%lx\n",time&0xFF);
    uint8_t tmp[8]={0};

    memcpy(tmp, &time, 8);
    printf("tmp[0]:%x\n", tmp[0]);
    printf("tmp[7]:%x\n", tmp[7]);

    return 0;
}
