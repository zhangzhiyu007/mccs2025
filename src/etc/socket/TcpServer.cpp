/*
 * TcpServer.cpp
 *
 *   创建日期: 2013-3-28
 *   作         者: mzh
 */

#include "TcpServer.h"

TcpServer::TcpServer()
{
	// TODO 构造函数

}

TcpServer::~TcpServer()
{
	// TODO 析构函数
}

bool TcpServer::Start(const int port, int queue)
{
	zlog_info(Util::m_zlog, "启动服务器:IP=ANY_IP,端口=%d,队列=%d",port,queue);

	Address addr(Address::ANY_IP, port);
	return Start(addr, queue);
}

bool TcpServer::Start(const Address& addr, int queue)
{
	zlog_info(Util::m_zlog, "启动服务器:IP=%s,端口=%d,队列=%d",
			addr.GetIPString(0).c_str(),
			addr.GetPort(),
			queue);

	if (this->IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket已经打开");
		this->Close();
	}

	if (!this->Open(TCP))
	{
		zlog_warn(Util::m_zlog, "socket打开失败");
		return false;
	}

	if (!this->Bind(addr))
	{
		zlog_warn(Util::m_zlog, "服务器绑定失败");
		this->Close();
		return false;
	}

	if (!this->Listen(queue))
	{
		zlog_warn(Util::m_zlog, "服务器绑定失败");
		this->Close();
		return false;
	}

	return true;
}

void TcpServer::Stop()
{
	zlog_warn(Util::m_zlog, "关闭服务器");
	this->Close();
}

Socket* TcpServer::GetConnection(Address& addr)
{
	return new Socket(this->Accept(addr));
}

bool TcpServer::Wait4Connection(int seconds , int useconds )
{
	bool read = true;
	bool write = false;
	bool except = false;

	bool ret = this->Wait(read, write, except, seconds, useconds);

	return (ret && read);
}

//TcpThreadedServer
void TcpThreadedServer::TcpListen::Run()
{
	Socket* sock;
	Address clientAddress;
	Address serverAddress(Address::ANY_IP,m_server->m_port);

	if (m_server != 0)
	{
		m_server->m_server.Start(serverAddress);

		while (m_listen)
		{
			//每隔1s等待连接
			if(!m_server->m_server.IsOpened())
			{
				m_server->m_server.Start(serverAddress);
				sleep(10);
			}

			if (m_server->m_server.Wait4Connection(1))
			{
				sock = (TcpServer*)m_server->m_server.GetConnection(clientAddress);

				TcpService* service =
						m_server->m_serviceFactory->CreateService(sock,this->m_server->m_param);
				service->SetAutoDel(true);
				service->Start();//启动线程

				--(m_server->m_counterSemaphore);
				m_server->m_clientCounter++;
				++(m_server->m_counterSemaphore);
			}
		}

		m_server->m_server.Stop();
	}
}

TcpThreadedServer::TcpThreadedServer() :
	m_listener(NULL), m_serviceFactory(NULL)
{
	m_clientCounter = 0;
	m_param = NULL;
}

TcpThreadedServer::~TcpThreadedServer()
{

}

//设置从站管理线程
void TcpThreadedServer::SetParam(void* param)
{
	this->m_param = param;
}

void TcpThreadedServer::Start(TcpServiceFactory & services, Address::Port port)
{
	m_serviceFactory = &services;
	m_port = port;

	m_clientCounter = 0;

	m_listener = new TcpListen;
	m_listener->m_server = this;
	m_listener->m_listen = true;
	m_listener->Start();

	//    m_listener->Join();
}

void TcpThreadedServer::Stop()
{
	m_listener->m_listen = false;
	m_listener->Join();
	m_listener = NULL;

	m_serviceFactory = NULL;
}

Address TcpThreadedServer::GetAddress()
{
	return m_server.Getpeername();
}

int TcpThreadedServer::GetClients()
{
	int count;

	--m_counterSemaphore;
	count = m_clientCounter;
	++m_counterSemaphore;

	return count;
}
