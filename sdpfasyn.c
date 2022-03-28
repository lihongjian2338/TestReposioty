#ifndef SDPF_API_H
#define SDPF_API_H

typedef enum _ErrCode
{
    Err_ok =                                (0),  //正确.
    Err_incorrectSecureMessageVersion =     (1),  //安全消息版本号信息非法. 
    Err_incorrectSignerType =               (2),  //签名者信息中签名方式非法.
    Err_incorrectCertVersion =              (3),  //数字证书版本号信息非法.
    Err_incorrectCertIssueDigest =          (4),  //数字证书签发者信息非法. 
    Err_incorrectCertSubjectInfo =          (5),  //数字证书中待签主题信息非法.
    Err_incorrectCertSubjectAttribute =     (6),  //数字证书中待签主题属性信息非法.
    Err_incorrectCertValidityPeriod =       (7),  //数字证书中有效限定信息非法. （根据CA时间）
    Err_incorrectCertTimeStartAndEnd =      (8),  //数字证书中有效时间信息非法. 
    Err_incorrectSubcertAuthority =         (9),  //数字证书父子关系非法.
    Err_incorrectCertChain =                (10), //证书链非法.
    Err_incorrectCertSignature =            (11), //数字证书签名信息非法.
    Err_incorrectTbsDataGenTime =           (12), //待签数据中数据产生时间信息非法.
    Err_incorrectTbsDataHashAlg =           (13), //待签数据中杂凑算法信息非法.
    Err_incorrectTbsDataItsAid =            (14), //待签数据中AID信息非法.
    Err_incorrectSignedMessageSignature =   (15), //安全消息中签名信息非法.
    Err_incorrectCertType =                 (16), //证书类型错误(显性|隐性) 
    Err_incorrectCertId =                   (17), //linkvalue或hostname错误 
    Err_incorrectCertPubkey =               (18), //公钥信息错误 
    Err_noCertInSecureMessage=              (19), //安全消息中无证书信息
    Err_signedMessageSignWay =              (20), //签名方式非法          
    Err_incorrectCertHash =                 (21), //数字证书的hash算法非法
    Err_incorrectCertSignatureType =        (22), //数字证书的签名类型非法

    Err_initFailed =                        (50), //库初始化失败
    Err_noInitialized =                     (51), //库未初始化
    Err_incorrectParam =                    (52), //无效参数
    Err_certType =                          (53), //签发SecData时，参数中的证书类型错误
    Err_oerEncodeFailed =                   (54),  //OER编码失败
    Err_oerDecodeFailed =                   (55),  //OER解码失败
    Err_computeDigestFailed =               (56),  //计算摘要失败
    Err_invalidSignCert =                   (57),  //无有效的签名证书
    Err_signedFailed =                      (58),  //签名失败
    Err_repeatedMsg =                       (59),  //重复消息
    Err_pcHasBeenRevoked =                  (60),  //签名证书已被撤销
    Err_notFoundCorrespondPc =              (61),  //未发现小模式对应的PC证书【小模式】
    Err_notFoundSpecifyCert =               (62),  //签名时未找到对应j值的证书【签发时】
    Err_signedMessageExpired =              (63),  //安全消息过期
    Err_bufOverflow =                       (64),  //解出的数据长度大于缓存长度
    Err_incorrectConfigFile =               (65),  //无效的配置文件
    Err_timeExpire =                        (66),  //时间过期
    Err_timeNotStart =                      (67),  //时间未生效
    Err_asynVerifyTxFailed=                 (68),  //异步验签失败
    Err_innerSpaceLack=                     (69),  //内部空间不足
//无效的配置文件
}SDPF_ErrCode;

typedef struct{
    unsigned char dataHash[32];
    unsigned char pubkey[64];
    unsigned char sign[64];
}SignInfo_t;

typedef struct{
    SignInfo_t certVerify;              //证书dataHash全为0表示证书已信任
    SignInfo_t spduVerify;
    unsigned char certHashid8[8];
}ToBeVerifyInfo_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
    * @brief      初始化
    * @param[in]  tspsDataRootPath SCMF数据根目录（同I-APP-SCMF-03，SCMF初始化时填入的路径，因为SDPF依赖SCMF中的TDCL）
    * @return     错误码
    * @retval     0为正确，其他为错误
    */ 
int ITS_SDPF_Init(
    const char* tspsDataRootPath
    );

/***
    * @brief 回收资源
    *
    * @return 错误码
    * @retval 0为正确，其他为错误
    */ 
int ITS_SDPF_Destroy();

/**
    * @brief 查看版本信息
    * @param[out]       version         版本信息
    * @param[in/out]    versionLen      版本信息长度
    * @return 错误码
    * @retval 0为正确，其他为错误
    */
 
int ITS_SDPF_Version(
    char* version, 
    int* versionLen
    );

/**
    * @brief设置日志文件存储位置和大小,在初始化前调用, 若不设置，默认在/tmp路径下，大小10MB.
    * 
    *日志路径下包含scmf、ssf、xdjakey三个日志文件,容量至少4MB，3个类型日志容量分别为2M、1M、1M
    *
    *
    * @param[in]    logFilePath    日志文件存储路径
    * @param[in]    capacity_mb    日志文件大小，单位：MB
    * @param[in]    mode           日志模式：0 关闭错误日志；1 打开错误日志；
    *
    */
void ITS_SDPF_SetLogConf(
     const char *logFilePath, 
     int capacity_mb, 
     int mode
     );

/**
    * @brief 基于IC签发安全消息
    * @param[in]  mode              模式：0 小模式,1 大模式
    * @param[in]  aid               应用ID
    * @param[in]  pdu               待签明文数据
    * @param[in]  pduLen            待签明文数据长度
    * @param[out] spdu              签名后安全消息数据
    * @param[in/out] spduLen        签名后安全消息数据长度
    *
    * @return 错误码
    * @retval 0-成功 其他-错误
    */
int ITS_SDPF_BaseIcGenSPDU(
    int mode,
    long aid,
    unsigned char *pdu, 
    int pduLen, 
    unsigned char *spdu, 
    int *spduLen
    );

/**
    * @brief 基于AC签发安全消息
    * @param[in]  mode              模式：0 小模式,1 大模式
    * @param[in]  aid               应用ID
    * @param[in]  pdu               待签明文数据
    * @param[in]  pduLen            待签明文数据长度
    * @param[out] spdu              签名后安全消息数据
    * @param[in/out] spduLen        签名后安全消息数据长度
    *
    * @return 错误码
    * @retval 0-成功 其他-错误
    */
int ITS_SDPF_BaseAcGenSPDU(
    int mode,
    long aid,
    unsigned char *pdu, 
    int pduLen, 
    unsigned char *spdu, 
    int *spduLen
    );

/**
    * @brief 基于PC签发安全消息
    * @param[in]  index             使用假名证书，取值范围（0~19）；
    * @param[in]  mode              模式：0 小模式,1 大模式
    * @param[in]  aid               应用ID
    * @param[in]  pdu               待签明文数据
    * @param[in]  pduLen            待签明文数据长度
    * @param[out] spdu              签名后安全消息数据
    * @param[in/out] spduLen        签名后安全消息数据长度
    *
    * @return 错误码
    * @retval 0-成功 其他-错误
    */
int ITS_SDPF_BasePcGenSPDU(
    int index,
    int mode,
    long aid,
    unsigned char *pdu, 
    int pduLen, 
    unsigned char *spdu, 
    int *spduLen
    );

/**
    * @brief 验证安全消息
    *
    * @param[in] aid            应用ID
    * @param[in] spdu           签名安全消息数据
    * @param[in] spduLen        签名安全消息数据长度
    * @param[out] pdu           明文数据
    * @param[in/out] pduLen     明文数据长度指针
    * @return 错误码
    * @retval 0-成功其他-错误
    */
int ITS_SDPF_VerifySPDU(
    long aid,
    unsigned char *spdu, 
    int spduLen, 
    unsigned char *pdu,
    int *pduLen
    );

/**
    * @brief 获取证书信息(有效期信息)
    *
    * @param[in] certType       证书类型(1-PC 2-AC 3-IC 4-EC)
    * @param[in] index          证书序号
    * @param[out] certData      输出证书数据
    * @param[out] certDataLen   证书长度
    * @param[out] startTime     证书生效时间
    * @param[out] endTime       证书截止日期
    * @return 错误码
    * @retval 0-成功其他-错误
    */
int ITS_SDPF_GetCertInfo(
    int certType, 
    int index, 
    unsigned char* certData, 
    int* certDataLen, 
    long long* startTime, 
    long long* endTime
    );

/**
    * @brief 获取正则公钥及正则ID
    *
    * @param[out] pubkeyHex         正则公钥（为x||y的hex字符串）
    * @param[in/out] pubkeyHexLen   正则公钥长度(缓冲区至少128字节)
    * @param[out] pubkeyIdHex       正则ID（正则公钥的Hash值，散列输出的低位16位字节hex字符串）
    * @param[in/out] pubkeyIdHexLen     正则ID长度(缓冲区至少32字节)
    * @return 错误码
    * @retval 0为正确，其他为错误
*/ 
int ITS_SDPF_GetPublicKeyInfo(
    char* pubkeyHex,  
    int* pubkeyHexLen,  
    char* pubkeyIdHex, 
    int* pubkeyIdHexLen
    );

/**
    * @brief 安全消息验证预处理
    *
    * @param[in] aid            应用ID
    * @param[in] spdu           签名安全消息数据
    * @param[in] spduLen        签名安全消息数据长度
    * @param[out] pdu           明文数据
    * @param[out] pduLen        明文数据长度
    * @param[out] isSmallMode   该条安全消息是否为小模式|1为小模式，0为大模式
    * @param[out] tbvInfo       待验签名信息
    * @return 错误码
    * @retval 0-成功其他-错误
    */
int ITS_SDPF_VerifySPDUPreprocess(
    long aid, 
    const unsigned char *spdu, 
    int spduLen, 
    unsigned char *pdu,
    int *pduLen,
    int *isSmallMode,
    ToBeVerifyInfo_t *tbvInfo
    );

/**
    * @brief 安全消息的签名验证,与ITS_SDPF_VerifySPDUPreprocess配合使用
    *
    * @param[in] tbvInfo       待验签名信息
    * @return 错误码
    * @retval 0-成功其他-错误
    */
int ITS_SDPF_VerifySPDUSignature(const ToBeVerifyInfo_t *tbvInfo);

/**
    * @brief 回调接口-获取PDU数据
    *
    * @param[in] result         错误码(0-成功，其他错误见:SDPF_ErrCode)  
    * @param[in] pdu            明文数据
    * @param[in] pduLen         明文数据长度
    * @return 错误码
    * @retval 0-成功其他-错误
    */
typedef void (*CallBackFetchPdu)(int result, unsigned char *pdu, int pdulen, void *ctx);

/**
    * @brief 为异步接口设置回调函数,与ITS_SDPF_AsynVerifySPDU配合使用
    *
    * @param[in] cbFetchPdu     回调函数接口，用于获取PDU数据
    * @return 错误码
    * @retval 0-成功其他-错误
    */
void ITS_SDPF_SetCallbackForAsynVerifySPDU(
    CallBackFetchPdu cbFetchPdu
    );

/**
    * @brief 验证安全消息-异步接口
    *
    * @param[in] aid            应用ID
    * @param[in] spdu           签名安全消息数据
    * @param[in] spduLen        签名安全消息数据长度
    * @return 错误码
    * @retval 0-成功其他-错误
    */
int ITS_SDPF_AsynVerifySPDU(
    long aid,
    unsigned char *spdu, 
    int spduLen,
    void *ctx 
    );

#ifdef __cplusplus
}
#endif

#endif

//////////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "sdpf_api.h"

///重要:SDPF使用依赖TSPS下载的证书信息，故而需要指定TSPS服务程序根目录
const static char* kTspsRootPath = "../tsps";
static int cntTest = 0;
static int cntTestSuccess = 0;
static long long finishTxTick;
static long long finishAllTick;

///可配置log路径和大小，在初始化前调用ITS_SDPF_SetLogConf接口实现；
///若不调用，日志默认存储在{kTspsRootPath}/file/log/sdpf/目录下,大小10M。此demo中未调用,请根据需要选择

/* 设置签名参数 */
#define CERT_KEYID  0      //0~19 [0~jmax-1]
#define OBU_USR_AID     111
#define RSU_USR_AID     3618

#define SELEC_PC_BIG        1
#define SELEC_AC_BIG        2
#define SELEC_IC_BIG        3
#define SELEC_PC_LITTLE     4
#define SELEC_AC_LITTLE     5
#define SELEC_IC_LITTLE     6

typedef struct {
    int pduLen;
    unsigned char pdu[1024];
}Data_t;

/**************************************************************************************************/
//数据队列
#define TBV_CACHE 100
#define RingQueueIsFull(rq)         (((rq.Tail + 1) % rq.DataBufLen) == rq.Head)
#define RingQueueIsEmpty(rq)        (rq.Head == rq.Tail)
#define RingQueueTailInc(rq)        (rq.Tail = (rq.Tail + 1) % rq.DataBufLen) // update
#define RingQueueHeadInc(rq)        (rq.Head = (rq.Head + 1) % rq.DataBufLen) // update


typedef struct tagRingQueue{
  unsigned short    Head;                       // Queue header
  unsigned short    Tail;                       // Queue tail
  unsigned short    DataBufLen;                 // Queue Depth
}TRingQ;

#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
sem_t _sem;
pthread_mutex_t _mtx;
static TRingQ ringQ;
static Data_t tbvDat[TBV_CACHE];

void initTbv(void)
{
    sem_init(&_sem, 0, TBV_CACHE-1);
    ringQ.Head = 0;
    ringQ.Tail = 0;
    ringQ.DataBufLen = TBV_CACHE;
}

bool writeTbv(const Data_t *tbvinfo)
{
    sem_wait(&_sem);
    pthread_mutex_lock(&_mtx);
    if(true == RingQueueIsFull(ringQ))
        printf( "write error" );
    memcpy(&tbvDat[ringQ.Tail], tbvinfo, sizeof(Data_t));
    RingQueueTailInc(ringQ);
    pthread_mutex_unlock(&_mtx);
    return true;
}

bool readTbv(Data_t *tbvinfo)
{
    bool rc = true;
    pthread_mutex_lock(&_mtx);
    if(true == RingQueueIsEmpty(ringQ))
    {
        printf("read error");
        rc = false;
    }
    else
    {
        memcpy(tbvinfo, &tbvDat[ringQ.Head], sizeof(Data_t));
        RingQueueHeadInc(ringQ);
        sem_post(&_sem);
        rc = true;
    }
    pthread_mutex_unlock(&_mtx);
    return rc;
}
bool isEmptyTbv()
{
    return RingQueueIsEmpty(ringQ);
}

unsigned long long nowUs(void)                                                                                                                     
{                                                                                                   
    struct timeval tv;                                                                              
    gettimeofday(&tv, NULL);                                                                        
    return (((unsigned long long)tv.tv_sec * 1000000LL) + tv.tv_usec);                              
                
} 
  
void waitInBlocking(int mSec)                                                                       
{                                                                                                                                                        
     struct timeval v;                                                                               
     v.tv_sec = 0;                                                                                   
     v.tv_usec = mSec*1000;                                                                          
     select(0, NULL, NULL, NULL, &v);                                                       
}

/* 打印16进制 */
void hexDump(const char *str,unsigned char *data, int datalen)
{
    int i;

    printf("%s L=%d | ",str,datalen);
    for(i=0;i<datalen;i++)
    {
        if(0 == i%16) printf("\n  ");
        printf("%02x ", data[i]);
    }
    printf("\n");
}

static int isRun = 1;
void *dataHandle(void *para)
{
    while(isRun)
    {
        if(false == isEmptyTbv())
        {
            Data_t data;
            readTbv(&data);
            //To Do User Logic
        }
        else
        {
            extern void waitInBlocking(int mSec);
            waitInBlocking(20);
        }
    }
    return NULL;
}
void stopAsynAssitThread(void)
{
    isRun = 0;
}

void startAsynAssitThread(void)
{
    pthread_t td;
    pthread_create(&td, NULL, dataHandle, NULL);
    pthread_detach(td);
}
void fetchPduResult(int result, unsigned char *pdu, int pdulen, void *ctx)
{
    //printf("sn:%d ret:%d cntTest:%d\n", serialNum, result, cntTest);
    //hexDump("pdu", pdu, pdulen);
    Data_t data;
    int *sn;
    if(!result)
    {
        cntTestSuccess++;
        if((int)sizeof(data.pdu) > pdulen)
        {
            memcpy(data.pdu, pdu, pdulen);
            data.pduLen = pdulen;
        }
        writeTbv(&data);
	sn = (int *)ctx;
	if (cntTestSuccess-1 != *sn)
            printf("cnTestSuccess : %d, recv sn: %d\n", cntTestSuccess-1, *sn);
    }
    else
        printf("ret %d\n", result);
    cntTest--;
}

int verifySpduByAsynApi(int count, unsigned char *spdu, int spdulen)
{
    int ret = 0;
    printf("\n--------------------------\n");
    /* 验证S P D U */
    finishTxTick = nowUs();
    finishAllTick = nowUs();
    int *p = (int *)malloc(sizeof(int) * count);
    for(int i=0; i<count; i++)
    {
	p[i] = i;
        ret = ITS_SDPF_AsynVerifySPDU(OBU_USR_AID, spdu, spdulen, &p[i]); //serialNum建议采用累加方式,短时间内出现重复serialNum会导致数据丢失。
        if(Err_ok != ret)
        {
            printf("verify sPDU fail (%d)\n", ret);
            cntTest = 0;
            break;
        }
        else
        {
            //hexDump("decode PDU from S-PDU",userPdu, userPduLen);
        }
    }
    
    finishTxTick = nowUs() - finishTxTick;

    while(cntTest)
    {
        waitInBlocking(10);
    }
    finishAllTick = nowUs() - finishAllTick;

    if(count)
    {
        printf("Verify SPDU == test cnt %d(%d), cost total %lld us | (%lld us * %lld次/秒)\n", count, cntTestSuccess, finishAllTick, (long long)(finishAllTick/count), (long long)(count)*1000000/finishAllTick); 
    }

    return ret;
}

int genSpdu(int select, unsigned char *spdu, int *len)
{
    int ret = 0;
    unsigned char userPdu[1500];
    memset(userPdu, 0x77, sizeof(userPdu));
    hexDump("PDU", userPdu, sizeof(userPdu));

    switch(select){
    case SELEC_PC_BIG:
        ret = ITS_SDPF_BasePcGenSPDU( CERT_KEYID, 1, OBU_USR_AID, userPdu, sizeof(userPdu), spdu, len);
        break;

    case SELEC_AC_BIG:
        ret = ITS_SDPF_BaseAcGenSPDU( 1, RSU_USR_AID, userPdu, sizeof(userPdu), spdu, len);
        break;

    case SELEC_IC_BIG:
        ret = ITS_SDPF_BaseIcGenSPDU( 1, OBU_USR_AID, userPdu, sizeof(userPdu), spdu, len);
        break;

    case SELEC_PC_LITTLE:
        ret = ITS_SDPF_BasePcGenSPDU( CERT_KEYID, 0, OBU_USR_AID, userPdu, sizeof(userPdu), spdu, len);
        break;

    case SELEC_AC_LITTLE:
        ret = ITS_SDPF_BaseAcGenSPDU( 0, RSU_USR_AID, userPdu, sizeof(userPdu), spdu, len);
        break;

    case SELEC_IC_LITTLE:
        ret = ITS_SDPF_BaseIcGenSPDU( 0, OBU_USR_AID, userPdu, sizeof(userPdu), spdu, len);
        break;

    default:
        break;
    }
    
    return ret;
}
 
int main(int argc, char** argv)
{
    int ret = 0;
    char version[50];
    int versionlen = sizeof(version);
    int kCount = 0; 
    int kSelect = 0; 
    
    if(argc < 3)
    {
        printf("usage:./sdpf_demo.asyn [option] [count]\n");
        printf("\toption: 1-PC 2-AC\n");
        printf("\tcount: 0~1000000000\n");
        printf("  ./sdpf_demo.asyn 1 10000 表示使用PC证书签名，验签10000次\n");
        return 0;
    }
    else
    {
        if(argc == 3)
        {
            cntTest = atoi(argv[2]); 
            kCount = cntTest;
            printf("test count %d\n", kCount);
        }
    }
    kSelect = atoi(argv[1]); 
    printf("select cert type: %d\n", kSelect);

    ret = ITS_SDPF_Init(kTspsRootPath);
    if(Err_ok != ret)
    {
        printf("init fail\n");
        return -1;
    }

    /* 获取版本 */
    ret = ITS_SDPF_Version(version, &versionlen);
    if(Err_ok != ret)
    {
        printf("get version fail\n");
    }
    else
    {
        printf("version:%s\n\n",version);
    }

    // 签发S P D U 
    unsigned char userSpduData[2048];
    int userSpduLen = sizeof(userSpduData);
     
    ret = genSpdu(kSelect, userSpduData, &userSpduLen);
    if(Err_ok != ret)
    {
        printf("generate sPDU fail (%d)\n", ret);
        return -2;
    }
    hexDump("S-PDU",userSpduData, userSpduLen);

    //设置异步验签接口的回调函数
    ITS_SDPF_SetCallbackForAsynVerifySPDU(fetchPduResult);

    //初始化数据队列
    initTbv();
    
    //启动业务处理逻辑
    startAsynAssitThread();

    //执行异步验签测试
    verifySpduByAsynApi(kCount, userSpduData, userSpduLen);

    //关闭业务处理逻辑
    stopAsynAssitThread();
    
    /* 注销服务 */
    ret = ITS_SDPF_Destroy();
    if(Err_ok != ret)
    {
        printf("Destroy sDPF module\n");
        return -3;
    }

    return 0;
}


