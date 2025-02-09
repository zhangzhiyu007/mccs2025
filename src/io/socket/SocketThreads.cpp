/*
 * SocketThreads.cpp
 *
 *  Created on: 2013-4-10
 *      Author: liuhaitao
 */

#include "SocketThreads.h"
#include "../../Manager.h"
#include "SocketCtrlThread.h"
#include "SocketThread.h"

SocketThreads::SocketThreads()
{
	// TODO Auto-generated constructor stub
	m_serverManager = NULL;
	m_socketTypeDevs.clear();
	m_socketThreads.clear();
}

SocketThreads::~SocketThreads()
{
	// TODO Auto-generated destructor stub
	m_serverManager = NULL;

	if (!m_socketThreads.empty())
	{
		CloseThreads();
	}

	m_socketThreads.clear();
}

//保存IO管理类指针
void SocketThreads::SetServerManager(Manager* manager)
{
	//判断指针是否为空
	assert(manager != NULL);
	m_serverManager = manager;
}

//为每个IP和port口创建一个线程
int SocketThreads::OpenThreads()
{
	//得到实例并初始化
	Device* device = Device::GetInstance();

	//获取网口从设备信息指针
	PtrArray* slaveArray = device->GetSlaveDevArray();

	//读取通讯类型指针
	PtrArray* constCommTypeArray = device->GetConstCommTypeArray();
	//以太网类型
	Device::ConstCommType* commType =
			(Device::ConstCommType*) constCommTypeArray->at(1);

	//存取从设备信息数组
	Device::SlaveDev* slaveDev = NULL;

	PtrArray::iterator slaveIter;
	PtrArray::iterator socketIter;

	SocketThread* socketThread = NULL;
	SocketAddress* socketAddress = NULL;
	SocketCtrlThread* socketCtrlThread = NULL;

	//获取本控制器地址
	int controllerAddr = device->GetControllerAddr();

	//判断通信类型
	zlog_warn(Util::m_zlog,"创建以太网通讯，通信类型%d,通讯方式%s",commType->type,commType->name.c_str());

	//1、读取相同IP和端口设备
	bool isExist = false;
	m_socketTypeDevs.clear();
	for (slaveIter = slaveArray->begin(); slaveIter != slaveArray->end(); slaveIter++)
	{
		slaveDev = reinterpret_cast<Device::SlaveDev*> (*slaveIter);

		if ((NULL != slaveDev)
				&& (slaveDev->commType == commType->type)
				&& (slaveDev->own == controllerAddr))
		{
			zlog_warn(Util::m_zlog, "通信方式为:%s,%s",
					commType->name.c_str(), commType->desc.c_str());
			isExist = false;

			//查找是否已经存在该IP和端口设备
			for (socketIter = m_socketTypeDevs.begin(); socketIter
					!= m_socketTypeDevs.end(); socketIter++)
			{
				socketAddress = reinterpret_cast<SocketAddress*> (*socketIter);
				if ((socketAddress->ip.compare(slaveDev->ip) == 0)
						&& (socketAddress->port == slaveDev->port))
				{
					isExist = true;
					break;
				}
			}

			//不存在就保存
			if (!isExist)
			{
				socketAddress = new SocketAddress();
				socketAddress->ip = slaveDev->ip;
				socketAddress->port = slaveDev->port;
				zlog_warn(Util::m_zlog, "以太网通讯,通信参数:IP=%s,port=%d",
						socketAddress->ip.c_str(), socketAddress->port);
				m_socketTypeDevs.push_back(socketAddress);
			}
		}
	}

	//为每个IP和端口开辟通讯线程
	zlog_warn(Util::m_zlog,"为每个IP和端口开辟通讯线程");
	TcpClient* client;
	for (socketIter = m_socketTypeDevs.begin(); socketIter
			!= m_socketTypeDevs.end(); socketIter++)
	{
		socketAddress = reinterpret_cast<SocketAddress*> (*socketIter);
		if (NULL == socketAddress)
		{
			continue;
		}

		//创建线程
		client = new TcpClient();

		zlog_warn(Util::m_zlog, "创建以太网通讯线程,通信参数:IP=%s,port=%d",
				socketAddress->ip.c_str(), socketAddress->port);
		socketThread = new SocketThread();
		socketThread->SetSocketTypeDev(socketAddress);
		socketThread->SetSocketThreads(this);
		socketThread->SetTcpClent(client);
		socketThread->Start();
		m_socketThreads.push_back(socketThread);

		//创建控制线程
		socketCtrlThread = new SocketCtrlThread();
		socketCtrlThread->SetSocketTypeDev(socketAddress);
		socketCtrlThread->SetSocketThreads(this);
		socketCtrlThread->SetTcpClent(client);
		socketCtrlThread->Start();
		m_socketCtrlThreads.push_back(socketCtrlThread);

	}

	zlog_warn(Util::m_zlog,"创建以太网通讯结束");

	return ErrorInfo::ERR_OK;
}

//关闭socket线程
int SocketThreads::CloseThreads()
{
	zlog_info(Util::m_zlog,"关闭socket线程");

	UINT i;

	//关闭控制线程
	SocketCtrlThread* socketCtrlThread = NULL;
	for (i = 0; i < m_socketCtrlThreads.size(); i++)
	{
		socketCtrlThread = (SocketCtrlThread*) m_socketCtrlThreads[i];

		//判断socket线程是否打开
		if (NULL != socketCtrlThread)
		{
			if (socketCtrlThread->IsAlive())
			{
				zlog_warn(Util::m_zlog,"关闭socket控制线程");
				socketCtrlThread->Stop();
			}
			delete socketCtrlThread;
			socketCtrlThread = NULL;
		}
	}
	m_socketCtrlThreads.clear();

	//关闭线程
	SocketThread* socketThread = NULL;
	for (i = 0; i < m_socketThreads.size(); i++)
	{
		socketThread = (SocketThread*) m_socketThreads[i];

		//判断socket线程是否打开
		if (NULL != socketThread)
		{
			if (socketThread->IsAlive())
			{
				zlog_warn(Util::m_zlog,"关闭socket线程");
				socketThread->Stop();
			}
			delete socketThread;
			socketThread = NULL;
		}
	}
	m_socketThreads.clear();

	//关闭以太网设备
	SocketAddress* socketAddress = NULL;
	for (i = 0; i < m_socketTypeDevs.size(); i++)
	{
		socketAddress = (SocketAddress*) m_socketTypeDevs[i];
		if (NULL != socketAddress)
		{
			delete socketAddress;
			socketAddress = NULL;
		}
	}

	return ErrorInfo::ERR_OK;
}

