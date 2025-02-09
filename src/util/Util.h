/*
 * Util.h
 *
 *   创建日期: 2013-9-27
 *   作          者: 马中华
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <libgen.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <dirent.h>
#include <pthread.h>
#include <termios.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <exception>
#include <sys/mman.h>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <memory.h>
#include <dirent.h>
#include <cassert>

#include "../etc/zlog/zlog.h"
#include "StatTimer.h"
#include "WatchDog.h"
#include "FirstOrderLagFilter.h"

using namespace std;

#ifndef MAX_VALUE
#define MAX_VALUE 0xFFFF
#endif

typedef unsigned int        DWORD;
typedef unsigned long       ULONG;
typedef bool                BOOL;
typedef char                CHAR;
typedef unsigned char       BYTE;
typedef unsigned char       UCHAR;
typedef unsigned short      WORD;
typedef unsigned short      USHORT;//16位有符号整型
typedef unsigned int        UINT;//32位无符号整型
typedef long                HANDLE;//句柄
typedef float               FLOAT;//浮点型
typedef int                 INT;//
typedef long                LONG;//
typedef long long           LONGLONG;//

#ifndef ERROR
#define ERROR          -1
#endif
#ifndef SUCCESS
#define SUCCESS         0
#endif
#ifndef FAILED
#define FAILED          -1
#endif

#ifndef USED
#define USED            1
#endif
#ifndef UNUSED
#define UNUSED          0
#endif

#define msleep(x)  usleep((x)*1000)

//数组
#ifndef ARRAY_LENGTH_2
#define ARRAY_LENGTH_2      2
#endif
#ifndef ARRAY_LENGTH_8
#define ARRAY_LENGTH_8      8
#endif
#ifndef ARRAY_LENGTH_16
#define ARRAY_LENGTH_16     16
#endif
#ifndef ARRAY_LENGTH_20
#define ARRAY_LENGTH_20     20
#endif
#ifndef ARRAY_LENGTH_32
#define ARRAY_LENGTH_32     32
#endif
#ifndef ARRAY_LENGTH_128
#define ARRAY_LENGTH_128    128
#endif
#ifndef ARRAY_LENGTH_256
#define ARRAY_LENGTH_256    256
#endif
#ifndef ARRAY_LENGTH_512
#define ARRAY_LENGTH_512    512
#endif
#ifndef ARRAY_LENGTH_1024
#define ARRAY_LENGTH_1024   1024
#endif
#ifndef ARRAY_LENGTH_2048
#define ARRAY_LENGTH_2048   2048
#endif

#ifndef DEFAULT_NUM
#define DEFAULT_NUM ARRAY_LENGTH_1024
#endif

//无效句柄
#ifndef INVALID_HANDLE
#define INVALID_HANDLE       -1
#endif

#ifndef SYSTEM_LEVEL
#define SYSTEM_LEVEL
#define LEVEL_1 1
#define LEVEL_2 2
#define LEVEL_3 3

#endif

#define CK_TIME 5 //计算CPU使用率间隔

/*名称：DefaultType
 *描述：通用结构体
 *创建人：mzh
 *创建日期：2013-2-13
 */
typedef struct DefaultType
{
	int sn;
	char* name;
	char* desc;
} DefaultType;

typedef vector<string>	strings;
typedef vector<UCHAR>   String;
typedef vector<UCHAR>   ByteArray;
typedef vector<string>  StringArray;
typedef vector<void*>   PtrArray;
typedef vector<int>     IntArray;
typedef vector<USHORT>  UshortArray;
typedef vector<short>   ShortArray;
typedef vector<double>  DoubleArray;
typedef vector<float>  FloatArray;
/*名称：ErrorInfo
 *描述：错误码
 *创建人：mzh
 *创建日期：2013-2-13
 */
class ErrorInfo
{
public:
	const static int ERR_OK = 0; //正常/无错误
	const static int ERR_FAILED = 1; //未知错误/失败
	const static int ERR_OPENED = 2; //打开失败/没有打开
	const static int ERR_TIMEOUT = 3; //超时错误
	const static int ERR_NULL = 4; //无效指针/句柄
	const static int ERR_INV_RESP = 5;//无效应答
	const static int ERR_NO_DATA = 6; //无数据
	const static int ERR_OUT_OF_RANGE = 7; //越界
	const static int ERR_WRITE = 8; //写入失败
	const static int ERR_READ = 9; //读取失败
	const static int ERR_CHECK = 10; //校验失败
	const static int ERR_UNANBLE = 11;//不可控/不使能
	const static int ERR_CONFIG = 12;//未初始化

};

class Top;

/*名称:
 *描述:
 *用法:
 *创建人:马中华
 *创建日期:2013-9-27
 */
class Util
{
	//方法
public:

	/*
	* 名称: Init
	* 功能: 初始化系统信息，如日志，读取系统配置信息
	* 输入:无
	* 输出: 返回
	*/
	static bool Init();

	/*
	* 名称: Uninit
	* 功能: 反初始化
	* 输入:无
	* 输出: 无
	*/
	static void Uninit();

	/*
	* 名称: GetMemUsage
	* 功能: 读取当前内存占用率
	* 输入:mem:[out]float& 内存占用率%
	* 输出: 返回成功/失败
	*/
	static bool GetMemUsage(float& mem);

	/*
	* 名称: GetCpuUsage
	* 功能: 读取CPU占用率
	* 输入:cpu:[out]float& CPU占用率%
	* 输出: 返回成功/失败
	*/
	static bool GetCpuUsage(float& cpu);

	/*
	* 名称: GetCurDir
	* 功能: 读取当前目录，格式/tmp/media/nanddisk/mccs/
	* 输入:dir:[in/out]char 保存当前程序所在目录
	*	   num:[in]int dir数组大小
	* 输出: 返回成功true/失败false
	*/
	static bool GetCurDir(char* dir,int num);

    /*
    * 名称: GetCurDir
    * 功能: 读取当前目录，格式/tmp/media/nanddisk/mccs/
    * 输入:无
    * 输出: 返回当前程序所在目录,失败则返回字符串长度为0
    */
	static string GetCurDir();

    /*
    * 名称: GetSystemTime
    * 功能: 读取精确时间,相对时间
    * 输入:无
    * 输出: 时间s
    */
	static int  GetSystemTime();

	/*
    * 名称: GetSystemTime
    * 功能: 获取系统时间
    * tm[0]:年 [2000]
    * tm[1]:月 [1-12]
    * tm[2]:日 [1-31]
    * tm[3]:时 [0-23]
    * tm[4]:分 [0-59]
    * tm[5]:秒 [0-59]
    * tm[6]:周几 [周一到周日分别是1-7]
    * 输入:
    * tm       :[in/out]  UshortArray& 当前系统时间
    * 输出: 成功/失败
    */
	static bool  GetSystemTime(UshortArray& tm);

    /*
    * 名称: SetSystemTime
    * 功能: 设置系统时间
    * tm[0]:年 [2014]
    * tm[1]:月 [1-12]
    * tm[2]:日 [1-31]
    * tm[3]:时 [0-23]
    * tm[4]:分 [0-59]
    * tm[5]:秒 [0-59]
    * tm[6]:毫秒[0-999]
    * 输入:
    * tm       :[in/out]  UshortArray& 当前系统时间
    * 输出: 成功/失败
    */
	static bool  SetSystemTime(UshortArray& tm);

    /*
    * 名称: SplitString
    * 功能: 分割字符串
    * 输入:
    * str      :[in]string 被分割字符串
    * split    :[in]string 分隔符
    * 输出: 返回分割后的字符数组,如果不成功字符数组为0
    */
	static StringArray SplitString(string str,string split);

    /*
    * 名称: GetIpBroadcast
    * 功能: 根据IP地址计算广播地址
    * 输入:
    * str      :[in]string 被分割字符串
    * 输出: 广播地址,失败返回字节长度为0
    */
	static string GetIpBroadcast(string ip);

	/*
	 * 名称：SetBitValue
	 * 功能：设置某个数据的任意一位的值
	 * 输入：
	 * data		 :[in/out]int& 原数据
	 * pos       :[in]const int 按位位置，从右向左（从零开始）
	 * value     :[in]const bool 位属性，0/1
	 * 输出：unsigned short 返回CRC校验结果
	 */
	static bool SetBitValue(int& data,const int pos,const bool value);

	/*
	 * 名称：GetBitValue
	 * 功能：获取某个数据的任意一位的值
	 * 输入：
	 * data		 :[in]int 原数据
	 * pos       :[in]const int 按位位置，从右向左(从零开始)
	 * 输出：unsigned short 返回数据结果
	 */
	static int GetBitValue(int data, int pos);

	/*
	 * 名称: OpenMem
	 * 功能: 进程开启内存共享,第一次打开内存共享时调用
	 * 输入:无
	 * 输出: 成功/失败
	 */
	static bool CreateMem(void** shmt,int& shmid,int id,int len);

	/*
	 * 名称: OpenMem
	 * 功能: 进程开启内存共享,除第一次以外打开内存共享时调用
	 * 输入:无
	 * 输出: 成功/失败
	 */
	static bool OpenMem(void** shmt,int& shmid,int id,int len);

	/*
	 * 名称: CloseMem
	 * 功能: 断开共享内存
	 * 输入:无
	 * 输出: 成功/失败
	 */
	static bool CloseMem(void* shmt);

	/*
	 * 名称: OpenMem
	 * 功能: 释放共享内存
	 * 输入:无
	 * 输出: 成功/失败
	 */
	static bool DelMem(int shmid);

	//初始化互斥
	static bool InitMutex(pthread_mutex_t* mutex);
	//互斥加锁
	static void Lock(pthread_mutex_t* mutex);
	//互斥解锁
	static void Unlock(pthread_mutex_t* mutex);

protected:
private:
	Util();
	virtual ~Util();

	/*
	* 名称: InitZlog
	* 功能: 初始化zlog日志
	* 输入:无
	* 输出: 返回
	*/
	static bool InitZlog();

	/*
	* 名称: InitControllerInfo
	* 功能: 读取控制器配置信息
	* 输入:无
	* 输出: 返回
	*/
	static bool InitControllerInfo();

	//变量
public:
	//日志
		static zlog_category_t* m_zlog;
		static zlog_category_t* m_zlog_sysStatus; // 系统重启的状态
		static Top* m_top;

protected:
private:

};

#endif // UTIL_H_
