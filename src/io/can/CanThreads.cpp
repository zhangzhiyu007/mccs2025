/*
 * CanThreads.cpp
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#include "CanThreads.h"
#include "../../Manager.h"

CanThreads::CanThreads()
{
	// TODO 构造函数
	m_canThread.clear();
}

CanThreads::~CanThreads()
{
	// TODO 析构函数
	if(!m_canThread.empty())
	{
		CloseThreads();
	}

	m_canThread.clear();
}

//保存IO管理类指针
void CanThreads::SetManager(Manager* manager)
{
	this->m_Manager = manager;
}

//为每个CAN口创建线程
int CanThreads::OpenThreads()
{
	//得到实例并初始化
	zlog_info(Util::m_zlog,"得到实例并初始化");
	Device* device = Device::GetInstance();

	//获取CAN口指针
	zlog_info(Util::m_zlog,"获取CAN口指针");
	PtrArray* devArray = device->GetCanDevArray();

	//获取从设备指针
	zlog_info(Util::m_zlog,"获取从设备指针");
	PtrArray* slaveArray = device->GetSlaveDevArray();

	//读取通讯类型指针
	PtrArray* constCommTypeArray = device->GetConstCommTypeArray();
	//CAN类型
	Device::ConstCommType* commType =
			(Device::ConstCommType*) constCommTypeArray->at(2);

	//存取设备信息数组
	Device::CanDev* canDev = NULL;

	//存取从设备信息数组
	Device::SlaveDev* slaveDev = NULL;

	PtrArray::iterator canIter;
	PtrArray::iterator slaveIter;

	//获取本控制器地址
	int controllerAddr = device->GetControllerAddr();

	CanThread* canThread = NULL;
	Can* can = NULL;
	m_cans.clear();

	bool success = false;

	//判断通信类型
	zlog_info(Util::m_zlog, "判断通信类型（串口、网口、CAN口）");
	for (canIter = devArray->begin(); canIter != devArray->end(); canIter++)
	{
		canDev = reinterpret_cast<Device::CanDev*> (*canIter);
		if (NULL == canDev)
		{
			continue;
		}

		for (slaveIter = slaveArray->begin(); slaveIter != slaveArray->end(); slaveIter++)
		{
			slaveDev = reinterpret_cast<Device::SlaveDev*> (*slaveIter);

			if ((NULL != slaveDev)
					&&(slaveDev->commType == commType->type)
					&&(canDev->port== slaveDev->commNo)
					&&(slaveDev->own == controllerAddr))//CAN口通信
			{
				zlog_warn(Util::m_zlog, "通信方式为:%s,%s", commType->name.c_str(), commType->desc.c_str());

				//打开CAN口
				can = new Can();
				success = can->Open(canDev->port,canDev->baudrate);
				m_cans.push_back(can);

				//创建线程
				canThread = new CanThread();
				canThread->SetCanDev(canDev);
				canThread->SetCan(can);
				canThread->SetCanThreads(this);
				canThread->Start();
				m_canThread.push_back(canThread);

				break;
			}
		}
	}

	return ErrorInfo::ERR_OK;
}

//将已经启动CAN口线程关闭
int CanThreads::CloseThreads()
{
	unsigned int i;
	CanThread* canThread = NULL;
	for (i = 0; i < m_canThread.size(); i++)
	{
		canThread = (CanThread*) m_canThread[i];

		//判断CAN口线程是否打开
		if (NULL != canThread)
		{
			if (canThread->IsAlive())
			{
				zlog_warn(Util::m_zlog,"关闭CAN口线程");
				canThread->Stop();
			}

			delete canThread;
			canThread = NULL;
		}
	}

	m_canThread.clear();

	Can* can = NULL;
	for(i = 0;i<(int)m_cans.size();i++)
	{
		can = (Can*)m_cans[i];

		//判断CAN口线程是否打开
		if(NULL!=can)
		{
			if(can->IsOpen())
			{
				zlog_info(Util::m_zlog,"关闭CAN口线程");
				can->Close();
			}

			delete can;
			can = NULL;
		}
	}
	m_cans.clear();
	return ErrorInfo::ERR_OK;
}
