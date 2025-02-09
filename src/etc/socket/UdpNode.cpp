/*
 * UdpNode.cpp
 *
 *   创建日期: 2013-3-27
 *   作         者: mzh
 */

#include "UdpNode.h"

UdpNode::UdpNode()
{
	// TODO 构造函数
	m_isServer = false;//服务器/客户端
	m_isBroadcast = false;//广播模式/点对点
	m_port=0;
}

UdpNode::~UdpNode()
{
	// TODO 析构函数
}

void UdpNode::SetServer(bool server)
{
	this->m_isServer = server;
}

bool UdpNode::IsServer()
{
	return m_isServer;
}

void UdpNode::SetBroadcast(bool broadcast)
{
	m_isBroadcast = broadcast;
}

bool UdpNode::IsBroadcast()
{
	return m_isBroadcast;
}

void UdpNode::SetRemoteAddr(const Address& addr)
{
	m_remoteAddr = addr;
}

Address UdpNode::GetRemoteAddr()
{
	return m_remoteAddr;
}

void UdpNode::SetHostAddr(const Address& addr)
{
	m_hostAddr = addr;
}

Address UdpNode::GetHostAddr()
{
	return m_hostAddr;
}

bool UdpNode::Open(const bool isServer,const bool isBroadcast,const int port)
{
	m_isServer = isServer;//服务器/客户端
	m_isBroadcast = isBroadcast;//广播模式/点对点
	m_port = port;//绑定端口

	return Open();
}

bool UdpNode::Open()
{
	zlog_info(Util::m_zlog, "打开UDP");
	bool ret = true;
	SocketOption::Broadcast broadcast(1);

	if (this->IsOpened())
	{
		//已经打开
		zlog_warn(Util::m_zlog, "UDP已经打开");
		return true;
	}

	if (!Socket::Open(Socket::UDP))
	{
		zlog_warn(Util::m_zlog, "打开socket失败");
		return false;
	}

	Address addr(Address::ANY_IP,m_port);

	if (m_isServer)//服务器
	{
		//绑定本地端口
		zlog_warn(Util::m_zlog, "绑定端口=%d",m_port);
		ret = this->Bind(addr);

	}
	else//客户端
	{
		if (m_isBroadcast)
		{
			ret = this->Setsockopt(broadcast);
		}

		if(m_port>0)
		{
			ret = this->Bind(addr);
		}
	}

	zlog_info(Util::m_zlog, "打开socket%s",ret?"成功":"失败");

	return ret;
}

int UdpNode::Recvfrom(Address& addr, String& data, int len, int flags)
{
	int ret = Socket::Recvfrom(addr, data, len, flags);
	this->m_remoteAddr = addr;
	return ret;
}

int UdpNode::Recvfrom(Address& addr, String& data, int len, int seconds, int useconds,
		int flags)
{
	int ret = Socket::Recvfrom(addr, data, len, seconds, useconds, flags);
	this->m_remoteAddr = addr;
	return ret;
}




