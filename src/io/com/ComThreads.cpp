/*
 * ComThreads.cpp
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#include "ComThreads.h"
#include "../../Manager.h"
#include "ComThread.h"
#include "ComCtrlThread.h"

ComThreads::ComThreads()
{
	// TODO 构造函数
	m_comThreads.clear();
	m_comCtrlThreads.clear();
	m_coms.clear();
	m_Manager = NULL;
}

ComThreads::~ComThreads()
{
	// TODO 析构函数
	m_Manager = NULL;

	CloseThreads();
}

//保存IO管理类指针
void ComThreads::SetManager(Manager* manager)
{
	//判断指针是否为空
	assert(manager!=NULL);

	m_Manager = manager;
}

//为每个串口创建串口线程
int ComThreads::OpenThreads()
{
	//得到实例化并初始化
	Device* device = Device::GetInstance();

	//获取串口设备信息指针
	PtrArray* devArray = device->GetComDevArray();

	//获取从设备信息指针
	PtrArray* slaveArray = device->GetSlaveDevArray();

	//读取通讯类型指针
	PtrArray* constCommTypeArray = device->GetConstCommTypeArray();
	//串口485类型
	Device::ConstCommType* commType =
			(Device::ConstCommType*) constCommTypeArray->at(0);


	//存取设备信息数组
	Device::ComDev* comDev = NULL;

	//存取从设备信息数组
	Device::SlaveDev* slaveDev = NULL;

	PtrArray::iterator comIter;
	PtrArray::iterator slaveIter;

	ComThread* comThread = NULL;
	ComCtrlThread* comCtrlThread = NULL;
	Com* com = NULL;
	m_coms.clear();

	//获取本控制器地址
	int controllerAddr = device->GetControllerAddr();

	bool success=false;

	//判断通信类型
	zlog_warn(Util::m_zlog, "判断通信类型（串口、网口、CAN口）,如果是串口类型就创建串口线程");
	for (comIter = devArray->begin(); comIter != devArray->end(); comIter++)
	{
		comDev = reinterpret_cast<Device::ComDev*> (*comIter);
		if (NULL == comDev)
		{
			continue;
		}
		for (slaveIter = slaveArray->begin(); slaveIter != slaveArray->end(); slaveIter++)
		{
			slaveDev = reinterpret_cast<Device::SlaveDev*> (*slaveIter);

			if ((NULL != slaveDev)
					&& (slaveDev->commType == commType->type)
					&& (comDev->port == slaveDev->commNo)
					&& (slaveDev->own == controllerAddr))//485通信
			{
				zlog_warn(Util::m_zlog, "通信方式为:%s,%s",
						commType->name.c_str(), commType->desc.c_str());

				//打开串口
				com = new Com();
				success=com->Open(comDev->port, comDev->baudrate, comDev->databits,
						comDev->parity, comDev->stopbits);
				m_coms.push_back(com);
				zlog_warn(Util::m_zlog, "打开串口=%d%s, %d,%c,%d,%d",comDev->port,success?"成功":"失败",comDev->baudrate,
						comDev->parity, comDev->databits,comDev->stopbits);

				//创建串口查询数据线程
				zlog_warn(Util::m_zlog, "创建串口查询数据线程");
				comThread = new ComThread();
				comThread->SetCom(com);
				comThread->SetComThreads(this);
				comThread->Start();
				m_comThreads.push_back(comThread);

				//创建串口控制线程
				zlog_warn(Util::m_zlog, "创建串口控制线程");
				comCtrlThread = new ComCtrlThread();
				comCtrlThread->SetCom(com);
				comCtrlThread->SetComThreads(this);
				comCtrlThread->Start();
				m_comCtrlThreads.push_back(comCtrlThread);
				break;
			}
		}
	}

	return ErrorInfo::ERR_OK;
}

//将串口线程关闭
int ComThreads::CloseThreads()
{
	int i;

	//关闭串口控制线程
	ComCtrlThread* comCtrlThread = NULL;
	for (i = 0; i < (int)m_comCtrlThreads.size(); i++)
	{
		comCtrlThread = (ComCtrlThread*) m_comCtrlThreads[i];

		//判断串口线程是否打开
		if (NULL != comCtrlThread)
		{
			if (comCtrlThread->IsAlive())
			{
				zlog_warn(Util::m_zlog,"关闭串口线程");
				comCtrlThread->Stop();
			}

			delete comCtrlThread;
			comCtrlThread = NULL;
		}
	}

	m_comCtrlThreads.clear();

	ComThread* comThread = NULL;
	for (i = 0; i < (int)m_comThreads.size(); i++)
	{
		comThread = (ComThread*) m_comThreads[i];

		//判断串口线程是否打开
		if (NULL != comThread)
		{
			if (comThread->IsAlive())
			{
				zlog_warn(Util::m_zlog,"关闭串口线程");
				comThread->Stop();
			}

			delete comThread;
			comThread = NULL;
		}
	}

	m_comThreads.clear();

	Com* com = NULL;
	for (i = 0; i < (int)m_coms.size(); i++)
	{
		com = (Com*) m_coms[i];

		//判断串口线程是否打开
		if (NULL != com)
		{
			if (com->IsOpen())
			{
				zlog_warn(Util::m_zlog,"关闭串口线程");
				com->Close();
			}

			delete com;
			com = NULL;
		}
	}
	m_coms.clear();

	return ErrorInfo::ERR_OK;
}
