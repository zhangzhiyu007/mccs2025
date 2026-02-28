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
#include <time.h>

SocketThreads::SocketThreads()
{
	// TODO Auto-generated constructor stub
	m_serverManager = NULL;
	m_socketTypeDevs.clear();
	m_socketThreads.clear();
	m_socketCtrlThreads.clear();
	m_tcpClients.clear();
}

SocketThreads::~SocketThreads()
{
	// TODO Auto-generated destructor stub
	m_serverManager = NULL;

	if (!m_socketThreads.empty() || !m_socketCtrlThreads.empty() || !m_tcpClients.empty() || !m_socketTypeDevs.empty())
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
	if (NULL == device)
	{
		zlog_error(Util::m_zlog, "创建设备通讯线程失败: Device实例为空");
		return ErrorInfo::ERR_NULL;
	}

	//获取网口从设备信息指针
	PtrArray* slaveArray = device->GetSlaveDevArray();
	if (NULL == slaveArray)
	{
		zlog_error(Util::m_zlog, "创建设备通讯线程失败: 从设备数组为空");
		return ErrorInfo::ERR_NULL;
	}

	//读取通讯类型指针
	PtrArray* constCommTypeArray = device->GetConstCommTypeArray();
	if ((NULL == constCommTypeArray) || (constCommTypeArray->size() <= 1))
	{
		zlog_error(Util::m_zlog, "创建设备通讯线程失败: 通讯类型配置无效");
		return ErrorInfo::ERR_FAILED;
	}

	//以太网类型
	Device::ConstCommType* commType =
			(Device::ConstCommType*) constCommTypeArray->at(1);
	if (NULL == commType)
	{
		zlog_error(Util::m_zlog, "创建设备通讯线程失败: 以太网通讯类型为空");
		return ErrorInfo::ERR_NULL;
	}

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
	int createdCount = 0;
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
		m_tcpClients.push_back(client);

		zlog_warn(Util::m_zlog, "创建以太网通讯线程,通信参数:IP=%s,port=%d",
				socketAddress->ip.c_str(), socketAddress->port);
		socketThread = new SocketThread();
		socketThread->SetSocketTypeDev(socketAddress);
		socketThread->SetSocketThreads(this);
		socketThread->SetTcpClent(client);
		socketThread->Start();
		if (socketThread->GetState() != Thread::RUNNING)
		{
			zlog_error(Util::m_zlog, "采集线程启动失败: IP=%s,port=%d",
					socketAddress->ip.c_str(), socketAddress->port);
			delete socketThread;
			socketThread = NULL;
			delete client;
			m_tcpClients.pop_back();
			continue;
		}
		m_socketThreads.push_back(socketThread);

		//创建控制线程
		socketCtrlThread = new SocketCtrlThread();
		socketCtrlThread->SetSocketTypeDev(socketAddress);
		socketCtrlThread->SetSocketThreads(this);
		socketCtrlThread->SetTcpClent(client);
		socketCtrlThread->Start();
		if (socketCtrlThread->GetState() != Thread::RUNNING)
		{
			zlog_error(Util::m_zlog, "控制线程启动失败: IP=%s,port=%d",
					socketAddress->ip.c_str(), socketAddress->port);
			delete socketCtrlThread;
			socketCtrlThread = NULL;
			if (socketThread->IsAlive())
			{
				socketThread->Stop();
			}
			delete socketThread;
			socketThread = NULL;
			m_socketThreads.pop_back();
			delete client;
			m_tcpClients.pop_back();
			continue;
		}
		m_socketCtrlThreads.push_back(socketCtrlThread);
		createdCount++;
	}

	zlog_warn(Util::m_zlog,"创建以太网通讯结束,成功创建线程组=%d", createdCount);

	if ((m_socketTypeDevs.size() > 0) && (createdCount == 0))
	{
		return ErrorInfo::ERR_FAILED;
	}

	return ErrorInfo::ERR_OK;
}

//关闭socket线程
int SocketThreads::CloseThreads()
{
	const time_t begin = time(NULL);
	zlog_info(Util::m_zlog,"关闭socket线程: ctrl=%d, io=%d, client=%d, addr=%d",
			(int)m_socketCtrlThreads.size(), (int)m_socketThreads.size(),
			(int)m_tcpClients.size(), (int)m_socketTypeDevs.size());

	UINT i;
	int ret = ErrorInfo::ERR_OK;

	// 先关闭TCP客户端连接，帮助阻塞IO快速返回（对象延后删除）
	TcpClient* client = NULL;
	for (i = 0; i < m_tcpClients.size(); i++)
	{
		client = (TcpClient*) m_tcpClients[i];
		if (NULL != client)
		{
			client->Close();
		}
	}

	// 关闭控制线程，超时未退出的线程保留对象，避免悬挂访问
	PtrArray aliveCtrlThreads;
	SocketCtrlThread* socketCtrlThread = NULL;
	for (i = 0; i < m_socketCtrlThreads.size(); i++)
	{
		socketCtrlThread = (SocketCtrlThread*) m_socketCtrlThreads[i];
		if (NULL == socketCtrlThread)
		{
			continue;
		}

		if (socketCtrlThread->IsAlive())
		{
			zlog_warn(Util::m_zlog,"关闭socket控制线程");
			socketCtrlThread->Stop();
		}

		if (socketCtrlThread->IsAlive())
		{
			zlog_error(Util::m_zlog, "socket控制线程未能在关闭阶段退出，保留对象: idx=%u", i);
			aliveCtrlThreads.push_back(socketCtrlThread);
			ret = ErrorInfo::ERR_FAILED;
			continue;
		}

		delete socketCtrlThread;
		socketCtrlThread = NULL;
	}
	m_socketCtrlThreads.swap(aliveCtrlThreads);

	// 关闭采集线程，超时未退出的线程保留对象，避免悬挂访问
	PtrArray aliveSocketThreads;
	SocketThread* socketThread = NULL;
	for (i = 0; i < m_socketThreads.size(); i++)
	{
		socketThread = (SocketThread*) m_socketThreads[i];
		if (NULL == socketThread)
		{
			continue;
		}

		if (socketThread->IsAlive())
		{
			zlog_warn(Util::m_zlog,"关闭socket线程");
			socketThread->Stop();
		}

		if (socketThread->IsAlive())
		{
			zlog_error(Util::m_zlog, "socket采集线程未能在关闭阶段退出，保留对象: idx=%u", i);
			aliveSocketThreads.push_back(socketThread);
			ret = ErrorInfo::ERR_FAILED;
			continue;
		}

		delete socketThread;
		socketThread = NULL;
	}
	m_socketThreads.swap(aliveSocketThreads);

	// 仍有线程活跃时，必须保留client和地址对象，避免线程访问已释放内存
	if (!m_socketCtrlThreads.empty() || !m_socketThreads.empty())
	{
		zlog_error(Util::m_zlog,
				"关闭socket线程未完全成功: ctrl_alive=%d, io_alive=%d，保留client/addr对象",
				(int)m_socketCtrlThreads.size(), (int)m_socketThreads.size());
		const time_t end = time(NULL);
		zlog_info(Util::m_zlog, "关闭socket线程结束(部分保留),耗时=%d s", (int)(end - begin));
		return ErrorInfo::ERR_FAILED;
	}

	// 所有线程已退出后再释放TCP客户端对象
	for (i = 0; i < m_tcpClients.size(); i++)
	{
		client = (TcpClient*) m_tcpClients[i];
		if (NULL != client)
		{
			delete client;
			client = NULL;
		}
	}
	m_tcpClients.clear();

	// 释放以太网设备地址对象
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
	m_socketTypeDevs.clear();

	const time_t end = time(NULL);
	zlog_info(Util::m_zlog, "关闭socket线程结束,耗时=%d s", (int)(end - begin));
	return ret;
}

