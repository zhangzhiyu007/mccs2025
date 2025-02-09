/*
 * CanThread.cpp
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#include "CanThread.h"

CanThread::CanThread()
{
	// TODO 构造函数
	m_canDev = NULL;
	m_canThreads = NULL;
	m_can = NULL;
}

CanThread::~CanThread()
{
	// TODO 析构函数
	m_canDev = NULL;
	m_canThreads = NULL;

	//释放内存
	if (NULL != m_can)
	{
		//关闭CAN口
		if(m_can->IsOpen())
		{
			m_can->Close();
		}

		delete m_can;
		m_can = NULL;
	}
}

//保存串口设备指针
void CanThread::SetCan(Can* can)
{
	assert(can !=NULL);
	m_can = can;
}

//保存CAN口线程池指针
void CanThread::SetCanThreads(CanThreads* canThreads)
{
	assert(canThreads != NULL);
	m_canThreads = canThreads;
}

//保存串口设备指针
void CanThread::SetCanDev(Device::CanDev *canDev)
{
	assert(canDev !=NULL);
	m_canDev = canDev;
}

//线程运行
void CanThread::Run()
{
	//得到实例并初始化
	Device* device = Device::GetInstance();
	int ret = ErrorInfo::ERR_OK;
	Can::FrameInfo frameRead;

	if (m_can == NULL)
	{
		m_can = new Can;
	}

	//获取从设备信息指针
	PtrArray* slaveArray = device->GetSlaveDevArray();
	PtrArray runArray;
	runArray.clear();

	//CAN类型
	PtrArray* constCommTypeArray = device->GetConstCommTypeArray();
	Device::ConstCommType* commType =
			(Device::ConstCommType*) constCommTypeArray->at(2);

	//获取从设备信息数组
	Device::SlaveDev* slaveDev = NULL;
	//获取本控制器地址
	int controllerAddr = device->GetControllerAddr();

	PtrArray::iterator slaveIter;

	for (slaveIter = slaveArray->begin(); slaveIter != slaveArray->end(); slaveIter++)
	{
		slaveDev = reinterpret_cast<Device::SlaveDev*> (*slaveIter);

		if ((NULL != slaveDev)
				&& (slaveDev->commType == commType->type)
				&& (m_canDev->port == slaveDev->commNo)
				&&(slaveDev->own == controllerAddr))//CAN口通信
		{
			runArray.push_back(*slaveIter);
		}
	}

	//BMS
	BMS bms;
	bms.SetCan(this->m_can);

	while (true)
	{
		//判断线程是否结束
		if (m_state == Thread::STOPPING)
		{
			zlog_warn(Util::m_zlog, "CAN口操作失败，CAN口线程结束");
			break;
		}

		//判断CAN口是否打开
		if (!m_can->IsOpen())
		{
			//如果CAN口关闭，则重新打开
			zlog_warn(Util::m_zlog, "CAN口关闭，重新打开CAN口");

			m_can->Open();

			continue;
		}

		//从CAN口读取数据
		zlog_info(Util::m_zlog,"判断CAN是否有数据可读");
		if (m_can->IsReadable() < 0)
		{
			zlog_warn(Util::m_zlog,"没有数据可读");
			break;
		}

		//从CAN口读取数据
		ret = m_can->Read(frameRead);

#if 0
		printf("帧ID：%X:",frameRead.id);
		for(int i = 0; i < frameRead.data.size(); i++)
		{
			printf("%02x ",frameRead.data[i]);
		}
		printf("\n");
#endif

		if(ret != ErrorInfo::ERR_OK)
		{
			zlog_warn(Util::m_zlog,"接收数据失败");
			break;
		}

		//获取从设备信息
		zlog_info(Util::m_zlog, "获取从设备信息");

		for (slaveIter = runArray.begin(); slaveIter != runArray.end(); slaveIter++)
		{
			slaveDev = reinterpret_cast<Device::SlaveDev*> (*slaveIter);

			if (NULL == slaveDev)
			{
				continue;
			}

			//判断设备名称
			switch (slaveDev->devType)
			{
			//判断设备厂商和型号来确定具体通信协议
			case DeviceType::DEVICE_TYPE_BMS://BMS
				ret = bms.Read(slaveDev,frameRead);
//				ret = bms.Preset(slaveDev);
				break;
			default:
				break;
			}
			msleep(2);
		}
		msleep(5);
	}
}
