/*
 * HisDbSave.cpp
 *
 *   创建日期: 2013-2-27
 *   作         者: mzh
 */

#include "HisDbSave.h"
#include "Device.h"
#include "MemDb.h"

HisDbSave* HisDbSave::m_hisDb = NULL;
HisDbSave::HisDbSave()
{
	// TODO 构造函数
	//历史数据配置信息
	m_warningSave = 7;//报警和操作数据保存天数
	m_level1Cycle = 300;//一级数据保存周期，单位s
	m_level1Save = 7;//一级数据保存时间，单位天
	m_level2Cycle = 300;//二级数据保存周期，单位s
	m_level2Save = 7;//二级数据保存时间，单位天
	m_level3Cycle = 300;//三级数据保存周期，单位s
	m_level3Save = 7;//三级数据保存时间，单位天
	memset(m_hisDev, '\0', sizeof(HisDev) * ARRAY_LENGTH_128);

	m_hisDevNum = 15;//目前设备类型15种
	m_hisDev[0].dev = "env";
	m_hisDev[1].dev = "ammete";
	m_hisDev[2].dev = "dido";
	m_hisDev[3].dev = "pvs";
	m_hisDev[4].dev = "pcs";
	m_hisDev[5].dev = "box";
	m_hisDev[6].dev = "dcdc";
	m_hisDev[7].dev = "wt";
	m_hisDev[8].dev = "bms";
	m_hisDev[9].dev = "bms_temp";
	m_hisDev[10].dev = "bms_v";
	m_hisDev[11].dev = "eis";
	m_hisDev[12].dev = "acpower";
	m_hisDev[13].dev = "gen";
	m_hisDev[14].dev = "load";

}

HisDbSave::~HisDbSave()
{
	// TODO 析构函数

}

HisDbSave* HisDbSave::GetInstance()
{
	if (NULL == HisDbSave::m_hisDb)
	{
		HisDbSave::m_hisDb = new HisDbSave();
	}

	return HisDbSave::m_hisDb;
}

void HisDbSave::FreeInstanse()
{
	if (NULL != HisDbSave::m_hisDb)
	{
		delete HisDbSave::m_hisDb;
		HisDbSave::m_hisDb = NULL;
	}

	return;
}

bool HisDbSave::Init()
{
	zlog_info(Util::m_zlog, "配置信息初始化");
	bool ret = false;

	ret = this->InitHisConfig();

	zlog_info(Util::m_zlog, "配置信息初始化%s",ret?"成功":"失败");
	return ret;
}

bool HisDbSave::InitHisConfig()
{
	zlog_info(Util::m_zlog, "从配置数据库中，读取历史数据配置信息");

	//局部变量
	ConfigDb* db = (ConfigDb*) ConfigDb::GetInstance();//配置数据库实例

	//1、打开数据库
	if (!db->IsOpen())
	{
		if (db->Open())
		{
			zlog_warn(Util::m_zlog, "配置数据库打开失败");
			return false;
		}
	}

	string type;
	const string tb = "sys_his";//历史数据配置信息表

	int value;
	bool ret = true;
	bool ret1 = false;

	//1、报警和操作数据保存天数
	type = "his_warning_save";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_warningSave = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//2、一级数据保存周期，单位s
	type = "his_level_1_cycle";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level1Cycle = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//3、一级数据保存时间，单位天
	type = "his_level_1_save";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level1Save = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//4、二级数据保存周期，单位s
	type = "his_level_2_cycle";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level2Cycle = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//5、二级数据保存时间，单位天
	type = "his_level_2_save";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level2Save = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//6、三级数据保存周期，单位s
	type = "his_level_3_cycle";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level3Cycle = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//7、三级数据保存时间，单位天
	type = "his_level_3_save";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level3Save = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//读取设备保存级别
	int i = 0;
	for (i = 0; i < m_hisDevNum; i++)
	{
		zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",m_hisDev[i].dev.c_str());
		ret1 = db->GetValue(value, m_hisDev[i].dev, tb);
		ret = ret && ret1;
		if (ret1)
		{
			m_hisDev[i].level = value;
			switch (m_hisDev[i].level)
			{
			case 1:
				m_hisDev[i].save = m_level1Save;
				m_hisDev[i].cycle = m_level1Cycle;
				break;
			case 2:
				m_hisDev[i].save = m_level2Save;
				m_hisDev[i].cycle = m_level2Cycle;
				break;
			case 3:
				m_hisDev[i].save = m_level3Save;
				m_hisDev[i].cycle = m_level3Cycle;
				break;
			default:
				m_hisDev[i].save = m_level1Save;
				m_hisDev[i].cycle = m_level1Cycle;
				break;
			}
		}
		else
		{
			zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",m_hisDev[i].dev.c_str());
		}
	}

	//关闭数据库
	db->Close();

	return ret;
}

bool HisDbSave::SaveWarn()
{
//	zlog_info(Util::m_zlog, "保存报警/故障数据");
//
//	//判断是否有报警数据，没有就返回
//	if(!MemDb::HasWarnData())
//	{
//		return true;
//	}
//
	bool ret = true;
//	WarnData warnData;
//	string name;
//	char tmp[ARRAY_LENGTH_512];
//
//	//得到历史数据库类的实例并打开历史数据库
//	HisDb* db = HisDb::GetInstance();
//	if (!db->IsOpen())
//	{
//		if (db->Open())
//		{
//			zlog_warn(Util::m_zlog, "历史数据库打开失败");
//			return false;
//		}
//	}
//
//	//事物开始
//	if(!db->Begin())
//	{
//		zlog_warn(Util::m_zlog, "事务开始失败");
//		db->Close();
//		return false;
//	}
//	//预处理
//	string sql="insert into [data_warn]([time],[time_t],[addr],[fault],[name]) values(datetime( 'now', 'localtime' ),0,%d,%d,'%s'),";
//	if(!db->Prepare(sql))
//	{
//		zlog_warn(Util::m_zlog, "预处理失败");
//		db->Commit();
//		db->Close();
//		return false;
//	}
//
//	//循环读取报警/故障数据
//	while (MemDb::GetWarnData(warnData))
//	{
//		sprintf(tmp,"%s",warnData.name);
//		name=tmp;
//		db->BindInt(0,warnData.addr);
//		db->BindInt(1,warnData.fault);
//		db->BindText(2,name);
//		db->Step();
//	}
//
//	if (!db->Commit())
//	{
//		zlog_warn(Util::m_zlog, "事务提交失败");
//		ret=false;
//	}
//
//	db->Close();

	return ret;
}

bool HisDbSave::SaveLevelData(int level)
{
	zlog_info(Util::m_zlog, "保存1级设备数据");
	bool ret = true;
//	bool ret1 = false;

//	Device* dev = Device::GetInstance();
//	PtrArray* slaveDevArray = dev->GetSlaveDevArray();
//
//	Device::SlaveDev* slaveDev = NULL;//实时数据指针
//	PtrArray::iterator iter;
//	string sql;
//	string temp;
//	string tb;
//	int i=0;
//
//	//得到历史数据库类的实例并打开历史数据库
//	HisDb* db = HisDb::GetInstance();
//	if (!db->IsOpen())
//	{
//		if (db->Open())
//		{
//			zlog_warn(Util::m_zlog, "历史数据库打开失败");
//			return false;
//		}
//	}
//
//	//保存历史数据
//	for (iter = slaveDevArray->begin(); iter != slaveDevArray->end(); iter++)
//	{
//		slaveDev = (Device::SlaveDev*) (*iter);
//
//		tb = Device::m_devType[i].tb;
//		//防止表名为空并且设备级别为设备定级别
//		if ((tb.length() > 0) && (Device::m_devType[29].level == level))
//		{
//			ret1 = SaveData(tb, slaveDev->sn, slaveDev->start,
//					Device::m_devType[29].dataLength);
//			ret = ret && ret1;
//		}
//		i++;
//	}
//
//	db->Close();

	return ret;
}

int HisDbSave::GetLevel1Cycle()
{

	return this->m_level1Cycle;
}

int HisDbSave::GetLevel2Cycle()
{
	return this->m_level2Cycle;
}

int HisDbSave::GetLevel3Cycle()
{
	return this->m_level3Cycle;
}

bool HisDbSave::SaveData(string tb,int sn,int start,int len)
{
	bool ret=false;

	int i = 0;
	string sql;
	string temp;
	RealData data;
	char tmp[ARRAY_LENGTH_512];

	//得到历史数据库类的实例并打开历史数据库
	HisDb* db = HisDb::GetInstance();

	//组装sql语句
	sprintf(tmp, "insert into [%s] values(datetime( 'now', 'localtime' ),0,%d",
			tb.c_str(), sn);
	sql = tmp;

	for (i = 0; i < len; i++)
	{
		ret = MemDb::GetRealData(data, start + i);//读取实时数据

		if (!ret)
		{
			sprintf(tmp,",0");
		}
		else
		{
			if (data.pv.type == RealData::INT)
			{
				sprintf(tmp,",%d", data.pv.data.iValue);
			}
			else
			{
				sprintf(tmp,",%.3f", data.pv.data.dbValue);
			}
		}
		sql += tmp;
	}

	//预编译
	ret = db->Prepare(sql);
	if (!ret)
	{
		zlog_warn(Util::m_zlog, "预处理sql语句失败");
		return false;
	}

	//执行
	if (DB_DONE != db->Step())
	{
		ret = false;
		zlog_warn(Util::m_zlog, "执行sql语句失败");
	}

	//结束
	db->Finalize();
	return ret;
}

HisDbSaveThread::HisDbSaveThread()
{
	m_running = false;
	m_flagExit = false;
	m_handle = INVALID_HANDLE;
	//初始化互斥对象
	pthread_mutex_init(&m_mutex, NULL);
}

HisDbSaveThread::~HisDbSaveThread()
{
	//释放互斥锁
	pthread_mutex_destroy(&m_mutex);
}

/*
 *  Start
 *  功能:启动线程
 *  输入：无
 *  输出：成功/失败
 */
bool HisDbSaveThread::Start()
{
	zlog_info(Util::m_zlog, "开启线程");

	//局部变量
	int ret = 0;

	if (m_running)
	{
		//线程已经运行
		zlog_info(Util::m_zlog, "线程已经运行");
		return true;
	}

	//1、开启线程
	ret = pthread_create((pthread_t*) &m_handle, NULL, HisDbSaveThread::Run,
			(void*) this);
	if (ret != 0)
	{
		//开启线程失败
		zlog_warn(Util::m_zlog, "开启线程失败");
		return false;
	}

	//开启线程成功
	zlog_info(Util::m_zlog, "开启线程成功");

	//加锁
	pthread_mutex_lock(&m_mutex);
	//给线程唤醒信号
	pthread_cond_signal(&m_cond);
	//解锁
	pthread_mutex_unlock(&m_mutex);

	return true;
}

/*
 *  Stop
 *  功能:终止线程运行
 *  输入：无
 *  输出：无
 */
void HisDbSaveThread::Stop()
{
	//停止线程
	zlog_info(Util::m_zlog, "停止线程");

	//1、判断线程是否正在运行
	if (!m_running)
	{
		//线程已经停止
		zlog_info(Util::m_zlog, "线程已经停止");
		return;
	}

	//2、停止线程,设置运行标志为停止运行
	m_flagExit = true;

	//等待线程结束，该方法为阻塞式，直到线程结束
	pthread_join(m_handle, NULL);

	m_handle = INVALID_HANDLE;
	zlog_info(Util::m_zlog, "停止线程成功");
}

/*
 *  Work
 *  功能:线程运行
 *  输入：无
 *  输出：无
 */
void HisDbSaveThread::Work()
{
	m_flagExit = false;
	m_running = true;

	HisDbSave* db = HisDbSave::GetInstance();
	if(!db->Init())
	{
		m_flagExit = false;
		m_running = false;
		return;
	}

	int level1Cycle = db->GetLevel1Cycle();
	int level2Cycle = db->GetLevel2Cycle();
	int level3Cycle = db->GetLevel3Cycle();
	int level1CycleTmp = Util::GetSystemTime();
	int level2CycleTmp = Util::GetSystemTime();
	int level3CycleTmp = Util::GetSystemTime();
	int level1CycleTmp1 = level1CycleTmp;
	int level2CycleTmp1 = level2CycleTmp;
	int level3CycleTmp1 = level3CycleTmp;

	while (true)
	{
		//1、判断是不是退出线程
		if (m_flagExit)
		{
			//退出线程
			zlog_warn(Util::m_zlog, "线程退出");
			break;
		}

		//2、保存报警数据
		db->SaveWarn();


		//3、判断保存一级数据时间是否到，保存一级数据
		level1CycleTmp = Util::GetSystemTime();
		level2CycleTmp = Util::GetSystemTime();
		level3CycleTmp = Util::GetSystemTime();

		if ((level1CycleTmp - level1CycleTmp1) >= level1Cycle)//时间到
		{
			level1CycleTmp1 = level1CycleTmp;
			db->SaveLevelData(LEVEL_1);
		}

		//4、保存二级数据
		if ((level2CycleTmp - level2CycleTmp1) >= level2Cycle)//时间到
		{
			level2CycleTmp1 = level2CycleTmp;
			db->SaveLevelData(LEVEL_2);
		}

		//5、保存三级数据
		if ((level3CycleTmp - level3CycleTmp1) >= level3Cycle)//时间到
		{
			level3CycleTmp1 = level3CycleTmp;
			db->SaveLevelData(LEVEL_3);
		}

		sleep(1);
	}

	//退出线程
	db->FreeInstanse();

	m_flagExit = false;
	m_running = false;
	return;
}

/*
 *  Work
 *  功能:线程运行
 *  输入：无
 *  输出：无
 */
void* HisDbSaveThread::Run(void* param)
{
	zlog_info(Util::m_zlog, "线程运行");

	//1、基本判断
	HisDbSaveThread* thread = (HisDbSaveThread*) param;

	if (NULL == thread)
	{
		zlog_warn(Util::m_zlog, "NULL == thread");
		return NULL;
	}

	//2、线程挂起，防止线程句柄为无效句柄
	pthread_mutex_lock(&(thread->m_mutex)); //这个mutex主要是用来保证pthread_cond_wait的并发性
	//这个while要特别说明一下，单个pthread_cond_wait功能很完善，
	//为何这里要有一个while (head == NULL)呢？因为pthread_cond_wait
	//里的线程可能会被意外唤醒，如果这个时候head != NULL，则不是我们想要的情况。
	//这个时候，应该让线程继续进入pthread_cond_wait
	while (thread->m_handle == INVALID_HANDLE)
	{
		// pthread_cond_wait会先解除之前的pthread_mutex_lock锁定的mtx，
		//然后阻塞在等待对列里休眠，直到再次被唤醒（大多数情况下是等待
		//的条件成立而被唤醒，唤醒后，该进程会先锁定先pthread_mutex_lock(&mtx);，
		//再读取资源
		//用这个流程是比较清楚的/*block-->unlock-->wait() return-->lock*/
		pthread_cond_wait(&(thread->m_cond), &(thread->m_mutex));
	}
	pthread_mutex_unlock(&(thread->m_mutex)); //临界区数据操作完毕，释放互斥锁

	thread->Work();

	zlog_info(Util::m_zlog, "线程退出");
	return (void*) 0;
}
