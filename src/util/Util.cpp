/*
 * Util.cpp
 *
 *   创建日期: 2013-9-27
 *   作          者: 马中华
 */

#include "Util.h"
#include "Top.h"

//日志
zlog_category_t* Util::m_zlog = NULL;
Top* Util::m_top = NULL;
zlog_category_t* Util::m_zlog_sysStatus = NULL;

Util::Util()
{
	// TODO 构造函数

}

Util::~Util()
{
	// TODO 析构函数
}

bool Util::GetCurDir(char* dir, int num)
{
	//getcwd(dir, num);

	//获取当前程序绝对路径
	int cnt = readlink("/proc/self/exe", dir, num);
	if ((cnt < 0) || (cnt >= num))
	{
	    printf("GetCurDir Failed\n");
	    return false;
	}
	//获取当前目录绝对路径，即去掉程序名
	int i;
	for (i = cnt; i >=0; --i)
	{
	    if (dir[i] == '/')
	    {
	        dir[i+1] = '\0';
	        break;
	    }
	}

	return true;
}

string Util::GetCurDir()
{
	string str;
	str.clear();
	char buffer[ARRAY_LENGTH_1024];
	if(GetCurDir(buffer,ARRAY_LENGTH_1024))
	{
		str = buffer;
	}

	return str;
}

bool Util::Init()
{
	bool ret = false;
	//初始化
	ret = Util::InitZlog();
//	//日志打开成功后
//	if (ret)
//	{
//		//启动查询CPU占用率线程
//		if (NULL == m_top)
//		{
//			m_top = new Top;
//			m_top->Start();
//		}
//	}

	return ret;
}

void Util::Uninit()
{
//	//停止Top
//	if (NULL != m_top)
//	{
//		if (m_top->IsAlive())
//		{
//			m_top->Stop();
//		}
//
//		delete m_top;
//		m_top = NULL;
//	}

	//关闭日志
	zlog_fini();
}

bool Util::InitZlog()
{
	printf("InitZlog\n");
	int rc;

	zlog_category_t *c;
	zlog_category_t *sysc;

	char curFile[ARRAY_LENGTH_1024];
	char curDir[ARRAY_LENGTH_1024];

	//读取当前目录
	Util::GetCurDir(curDir, ARRAY_LENGTH_1024);
	sprintf(curFile, "%sconf/log.conf", curDir);

	//读取配置文件
	printf("InitZlog file path = %s\n",curFile);
	rc = zlog_init(curFile);
	if (rc)
	{
		printf("init failed\n");
		return false;
	}

	//初始化日志
	c = zlog_get_category("mccs");
	sysc = zlog_get_category("syslog");
	if ( !c )
	{
		printf("get cat fail\n");
		zlog_fini();
		return false;
	}


	Util::m_zlog = c;
	Util::m_zlog_sysStatus = sysc;

	printf("InitZlog success\n");
	return true;
}


bool Util::GetMemUsage(float& mem)
{
	mem = 0.0;

	FILE *fp;
	char buf[128];
	char name[20];
	char name1[20];
	long int memTotal;//所有可用RAM大小
	long int memFree;//未使用的内存
	long int buffers;//用来给文件做缓冲的大小
	long int cached;//高速缓冲存储器用的内存的大小
	long int used;
	float usage;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL)
	{
		zlog_warn(Util::m_zlog,"打开/proc/meminfo文件失败");
		return false;
	}
	fgets(buf, sizeof(buf), fp);
	sscanf(buf, "%s %ld %s", name, &memTotal, name1);

	fgets(buf, sizeof(buf), fp);
	sscanf(buf, "%s %ld %s", name, &memFree, name1);

	fgets(buf, sizeof(buf), fp);
	sscanf(buf, "%s %ld %s", name, &buffers, name1);

	fgets(buf, sizeof(buf), fp);
	sscanf(buf, "%s %ld %s", name, &cached, name1);

	used = memTotal - memFree;
	usage = (float) used / memTotal * 100;

	mem = usage;
	return true;
}

bool Util::GetCpuUsage(float& cpu)
{
	cpu = 0.0;
	bool success = false;
	if (NULL != m_top)
	{
		success = m_top->GetCpuUsage(cpu);
	}

	return success;
}

int Util::GetSystemTime()
{
	int tm = 0;//精确时间
	//strut timeval {long tv_sec; /* 秒数 */long tv_usec; /* 微秒数 */};
	struct timeval tv;//时间结构体

	//获得精确计时时间
	gettimeofday(&tv, NULL);

	//转换为时间，微妙
	tm = tv.tv_sec;

	return tm;
}

/*
* 名称: GetSystemTime
* 功能: 获取系统时间
* tm[0]:年 [2000]
* tm[1]:月 [1-12]
* tm[2]:日 [1-31]
* tm[3]:时 [0-23]
* tm[4]:分 [0-59]
* tm[5]:秒 [0-59]
* 输入:
* tm       :[in/out]  UshortArray& 当前系统时间
* 输出: 成功/失败
*/
bool Util::GetSystemTime(UshortArray& tm)
{
	bool success = true;

	//	注释：time_t是一个在time.h中定义好的结构体。而tm结构体的原形如下：
	//	struct tm
	//	{
	//	int tm_sec;//seconds 0-61
	//	int tm_min;//minutes 1-59
	//	int tm_hour;//hours 0-23
	//	int tm_mday;//day of the month 1-31
	//	int tm_mon;//months since jan 0-11
	//	int tm_year;//years from 1900
	//	int tm_wday;//days since Sunday, 0-6
	//	int tm_yday;//days since Jan 1, 0-365
	//	int tm_isdst;//Daylight Saving time indicator
	//	};

	time_t now; //实例化time_t结构
	struct tm *timenow; //实例化tm结构指针
	//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	time(&now);
	//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
	timenow = localtime(&now);

	tm.clear();
	tm.push_back(timenow->tm_year+1900);//年
	tm.push_back(timenow->tm_mon);//月
	tm.push_back(timenow->tm_mday);//日
	tm.push_back(timenow->tm_hour);//时
	tm.push_back(timenow->tm_min);//分
	tm.push_back(timenow->tm_sec);//秒
	if(timenow->tm_wday>0)
	{
		tm.push_back(timenow->tm_wday);//周
	}
	else
	{
		tm.push_back(7);//周
	}

	return success;
}

/*
 * 名称: SetSystemTime
 * 功能: 设置系统时间
 * tm[0]:年 [2000]
 * tm[1]:月 [1-12]
 * tm[2]:日 [1-31]
 * tm[3]:时 [0-23]
 * tm[4]:分 [0-59]
 * tm[5]:秒 [0-59]
 * 输入:
 * tm       :[in/out]  UshortArray& 当前系统时间
 * 输出: 成功/失败
 */
bool Util::SetSystemTime(UshortArray& tm)
{
	bool success = true;

	struct tm _tm;
	struct timeval tv;
	time_t timep;
	_tm.tm_year = tm[0] - 1900;
	_tm.tm_mon = tm[1] - 1;
	_tm.tm_mday = tm[2];
	_tm.tm_hour = tm[3];
	_tm.tm_min = tm[4];
	_tm.tm_sec = tm[5];

	timep = mktime(&_tm);
	tv.tv_sec = timep;
	tv.tv_usec = tm[6] * 1000;
	if (settimeofday(&tv, (struct timezone *) 0) < 0)
	{
		printf("Set system datatime error!\n");
		success = false;
	}

	return success;
}

StringArray Util::SplitString(string str,string split)
{
	StringArray array;
	array.clear();

	char* tmpStr = new char[str.size() + 1];
	char* tmpSplit = new char[split.size() + 1];
	memset(tmpStr,0,str.size() + 1);
	memset(tmpSplit,0,split.size() + 1);
	strcpy(tmpStr, str.c_str());
	strcpy(tmpSplit, split.c_str());
	//分割字符串
	char * p;
	p = strtok(tmpStr, tmpSplit);
	while (p != NULL)
	{
		string str1(p);
		array.push_back(str1);
		p = strtok(NULL, tmpSplit);
	}

	delete[] tmpStr;
	delete[] tmpSplit;

	return array;
}

string Util::GetIpBroadcast(string ip)
{
	StringArray array = Util::SplitString(ip, ".");
	string ipBroadcast;
	ipBroadcast.clear();

	char tmp[ARRAY_LENGTH_32];
	if (array.size() >= 4)
	{
		sprintf(tmp,"%s.%s.%s.255",array[0].c_str(),array[1].c_str(),array[2].c_str());
		ipBroadcast=tmp;
	}

	return ipBroadcast;
}

bool Util::SetBitValue(int& data,int pos,bool value)
{
	if ((pos < 0) || (pos > 31))
	{
		return false;
	}

	DWORD dataTmp = data;

	if (value)
	{
		dataTmp |= 0x01 << pos;
	}
	else
	{
		dataTmp &= ~(0x01 << pos);
	}
	data = dataTmp;

	return true;
}


int Util::GetBitValue(int data, int pos)
{
	if ((pos < 0) || (pos > 31))
	{
		return -1;
	}

	int dataTmp;

	dataTmp = (data>>pos) & 0x01;

	return dataTmp;
}

bool Util::CreateMem(void** shmt, int& shmid, int id, int len)
{
	zlog_warn(Util::m_zlog, "创建内存共享:ID=%d,len=%d",id,len);
	if ((shmid = shmget(id, len, IPC_CREAT|0777)) < 0)
	{
		zlog_warn(Util::m_zlog, "申请内存共享失败");
		return false;
	}

	if ((*shmt = shmat(shmid, (char*) 0, 0)) == (void *) -1)
	{
		zlog_warn(Util::m_zlog, "连接共享内存失败");
		shmt = NULL;
		return false;
	}
	zlog_warn(Util::m_zlog, "连接共享内存成功:内存地址=%d",(int)*shmt);

	return true;
}

bool Util::OpenMem(void** shmt, int& shmid, int id, int len)
{
	zlog_warn(Util::m_zlog, "打开内存共享:ID=%d,len=%d",id,len);
	if ((shmid = shmget(id, len, 0777)) < 0)
	{
		zlog_warn(Util::m_zlog, "申请内存共享失败");
		return false;
	}

	if ((*shmt = shmat(shmid, (char*) 0, 0)) == (void *) -1)
	{
		zlog_warn(Util::m_zlog, "连接共享内存失败");
		shmt = NULL;
		return false;
	}
	zlog_warn(Util::m_zlog, "连接共享内存成功:内存地址=%d",(int)*shmt);

	return true;
}

bool Util::CloseMem(void* shmt)
{
	bool ret = false;
	if (shmdt(shmt) == 0)
	{
		ret = true;
	}

	return ret;
}

bool Util::DelMem(int shmid)
{
	bool ret = false;
	if (shmctl(shmid, IPC_RMID, 0) == 0)
	{
		ret = true;
	}

	return ret;
}

//初始化互斥
bool Util::InitMutex(pthread_mutex_t* mutex)
{
	mutex = NULL;
	zlog_info(Util::m_zlog, "初始化进程间互斥");
	int ret = 0;
	//m_mutex一定要是进程间可以共享的，否则无法达到进程间互斥
	mutex = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ
			| PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (MAP_FAILED == mutex)
	{
		mutex = NULL;
		zlog_warn(Util::m_zlog, "内存共享失败导致初始化进程间互斥失败");
		return false;
	}

	//设置attr的属性
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	//一定要设置为PTHREAD_PROCESS_SHARED
	//具体可以参考http://blog.chinaunix.net/u/22935/showart_340408.html
	ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if (ret != 0)
	{
		mutex = NULL;
		zlog_warn(Util::m_zlog, "初始化pthread_mutexattr_setpshared失败");
		return false;
	}
	pthread_mutex_init(mutex, &attr);

	return true;
}

//互斥加锁
void Util::Lock(pthread_mutex_t* mutex)
{
	int ret = pthread_mutex_lock(mutex);
	if (ret != 0)
	{
		zlog_warn(Util::m_zlog, "加锁失败");
	}
}

//互斥解锁
void Util::Unlock(pthread_mutex_t* mutex)
{
	int ret = pthread_mutex_unlock(mutex);
	if (ret != 0)
	{
		zlog_warn(Util::m_zlog, "解锁失败");
	}
}


//a)    CString s;  //生成一个空字符串s
//b)    CString s(str) //拷贝构造函数 生成str的复制品
//c)    CString s(str,stridx) //将字符串str内“始于位置stridx”的部分当作字符串的初值
//d)    CString s(str,stridx,strlen) //将字符串str内“始于stridx且长度顶多strlen”的部分作为字符串的初值
//e)    CString s(cstr) //将C字符串作为s的初值
//f)    CString s(chars,chars_len) //将C字符串前chars_len个字符作为字符串s的初值。
//g)    CString s(num,c) //生成一个字符串，包含num个c字符
//h)    CString s(beg,end) //以区间beg;end(不包含end)内的字符作为字符串s的初值
//i)    s.~CString() //销毁所有字符，释放内存
//都很简单，我就不解释了。
//2．字符串操作函数
//   这里是C++字符串的重点，我先把各种操作函数罗列出来，不喜欢把所有函数都看完的人可以在这里找自己喜欢的函数，再到后面看他的详细解释。
//a) =,assign()   //赋以新值
//b) swap()   //交换两个字符串的内容
//c) +=,append(),push_back() //在尾部添加字符
//d) insert() //插入字符
//e) erase() //删除字符
//f) clear() //删除全部字符
//g) replace() //替换字符
//h) + //串联字符串
//i) ==,!=,<,<=,>,>=,compare()  //比较字符串
//j) size(),length()  //返回字符数量
//k) max_size() //返回字符的可能最大个数
//l) empty()  //判断字符串是否为空
//m) capacity() //返回重新分配之前的字符容量
//n) reserve() //保留一定量内存以容纳一定数量的字符
//o) [ ], at() //存取单一字符
//p) >>,getline() //从stream读取某值
//q) <<  //将谋值写入stream
//r) copy() //将某值赋值为一个C_CString
//s) c_str() //将内容以C_String返回
//t) data() //将内容以字符数组形式返回
//u) substr() //返回某个子字符串
//v)查找函数
//w)begin() end() //提供类似STL的迭代器支持
//x) rbegin() rend() //逆向迭代器
//y) get_allocator() //返回配置器
