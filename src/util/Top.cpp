/*
 * Top.cpp
 *
 *   创建日期: 2013-11-29
 *   作          者: 马中华
 */

#include "Top.h"

Top::Top() :
	m_interval(5),m_success(false),m_cpuUsage(0.0)
{
	// TODO 构造函数
}

Top::~Top()
{
	// TODO 析构函数
}

bool Top::GetCpuUsage(float& cpu)
{
	cpu = m_cpuUsage;
	return m_success;
}

void Top::Run()
{
	FILE* fp;
	char buf[128];
	char cpu[5];
	long int user;
	long int nice;
	long int sys;
	long int idle;
	long int ioWait;
	long int irq;
	long int softIrq;
	long int all1;
	long int all2;
	long int idle1;
	long int idle2;
	float usage;

	fp = fopen("/proc/stat", "r");
	if (fp == NULL)
	{
		m_success = false;
		zlog_warn(Util::m_zlog,"打开/proc/stat文件失败，返回");
		return;
	}

	m_success = true;

	while(true)
	{
		//判断线程是否结束
		if (m_state == Thread::STOPPING)
		{
			zlog_warn(Util::m_zlog, "线程结束");
			break;
		}

		//计算CPU占用率
		memset(buf, 0, 128);
		cpu[0] = '\0';
		user = 0;
		nice = 0;
		sys = 0;
		idle = 0;
		ioWait = 0;
		irq = 0;
		softIrq = 0;

		//第一次读取CPU数据
		fgets(buf, sizeof(buf), fp);
		sscanf(buf, "%s%ld%ld%ld%ld%ld%ld%ld", cpu, &user, &nice, &sys, &idle,
				&ioWait, &irq, &softIrq);

		all1 = user + nice + sys + idle + ioWait + irq + softIrq;
		idle1 = idle;

		rewind(fp);

		/*第二次取数据*/
		sleep(m_interval);

		memset(buf, 0, 128);
		cpu[0] = '\0';
		user = 0;
		nice = 0;
		sys = 0;
		idle = 0;
		ioWait = 0;
		irq = 0;
		softIrq = 0;

		fgets(buf, sizeof(buf), fp);
		sscanf(buf, "%s%ld%ld%ld%ld%ld%ld%ld", cpu, &user, &nice, &sys, &idle,
				&ioWait, &irq, &softIrq);

		all2 = user + nice + sys + idle + ioWait + irq + softIrq;
		idle2 = idle;

		usage = (float) (all2 - all1 - (idle2 - idle1)) / (all2 - all1) * 100;

		m_cpuUsage = usage;
		sleep(1);
	}
}
