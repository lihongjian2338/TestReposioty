/*************************************************
  名称: milliseconds_sleep
  描述: 毫秒级定时器
  输入参数: mSec 毫秒数
  返回值: 无
**************************************************/
void milliseconds_sleep(unsigned long mSec){
    struct timeval tv;
    tv.tv_sec=mSec/1000;
    tv.tv_usec=(mSec%1000)*1000;
    int err;
    do{
       err=select(0,NULL,NULL,NULL,&tv);
    }while(err<0 && errno==EINTR);
}
