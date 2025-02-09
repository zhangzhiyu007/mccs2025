/*
 * Option.h
 *
 *  Created on: 2013-3-26
 *      Author: mzh
 */

#ifndef OPTION_H_
#define OPTION_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>

/*名称: Option
 *描述: socket参数类
 *用法: 由Socket类调用
 *创建人: mzh
 *创建日期:2013-3-26
 */
class Option
{
	friend class Socket;

public:
	virtual ~Option() throw()
	{
	}

	//返回设备级别
	virtual int Level() const throw() = 0;
	//返回设备ID
	virtual int Id() const throw() = 0;

protected:
	virtual void* Pointer() throw() = 0; ///< 写入参数
	virtual const void* Pointer() const throw() = 0; //读取参数
	virtual socklen_t Size() const throw() = 0; //参数大小
};

/*名称: OptionTemplate
 *描述: socket参数类模板
 *用法: 由Socket类调用，可以应用于简单类型
 *创建人: mzh
 *创建日期:2013-3-26
 */
template<int LEVEL, int ID, class TYPE>
class OptionTemplate: public Option
{
public:
	OptionTemplate() throw()
	{
	}
	OptionTemplate(const TYPE& val) throw() :
		m_value(val)
	{
	}
	const TYPE& operator=(const TYPE& val) throw()
	{
		return m_value = val;
	}
	operator TYPE&() throw()
	{
		return m_value;
	}
	operator const TYPE&() const throw()
	{
		return m_value;
	}

	virtual int Level() const throw()
	{
		return LEVEL;
	}
	virtual int Id() const throw()
	{
		return ID;
	}

protected:
	virtual void* Pointer() throw()
	{
		return reinterpret_cast<void*> (&m_value);
	}
	virtual const void* Pointer() const throw()
	{
		return reinterpret_cast<const void*> (&m_value);
	}
	virtual socklen_t Size() const throw()
	{
		return sizeof(TYPE);
	}

private:
	TYPE m_value;
};

/// Socketspecific options
namespace SocketOption
{
#ifdef SO_KEEPALIVE
typedef OptionTemplate<SOL_SOCKET, SO_KEEPALIVE, int> KeepAlive;
#endif
#ifdef SO_OOBINLINE
typedef OptionTemplate<SOL_SOCKET, SO_OOBINLINE, int> OOBInline;
#endif
#ifdef SO_RCVLOWAT
typedef OptionTemplate<SOL_SOCKET, SO_RCVLOWAT, int> RcvLOWAT;
#endif
#ifdef SO_SNDLOWAT
typedef OptionTemplate<SOL_SOCKET, SO_SNDLOWAT, int> SndLOWAT;
#endif
#ifdef SO_RCVTIMEO
typedef OptionTemplate<SOL_SOCKET, SO_RCVTIMEO, int> RcvTimeO;
#endif
#ifdef SO_SNDTIMEO
typedef OptionTemplate<SOL_SOCKET, SO_SNDTIMEO, int> SndTimeO;
#endif
#ifdef SO_BSDCOMPAT
typedef OptionTemplate<SOL_SOCKET, SO_BSDCOMPAT, int> BSDCompat;
#endif
#ifdef SO_PASSCRED
typedef OptionTemplate<SOL_SOCKET, SO_PASSCRED, int> PassCred;
#endif
#ifdef SO_DEBUG
typedef OptionTemplate<SOL_SOCKET, SO_DEBUG, int> Debug;
#endif
#ifdef SO_REUSEADDR
typedef OptionTemplate<SOL_SOCKET, SO_REUSEADDR, int> ReuseAddr;
#endif
#ifdef SO_TYPE
typedef OptionTemplate<SOL_SOCKET, SO_TYPE, int> Type;
#endif
#ifdef SO_DONTROUTE
typedef OptionTemplate<SOL_SOCKET, SO_DONTROUTE, int> DontRoute;
#endif
#ifdef SO_BROADCAST
typedef OptionTemplate<SOL_SOCKET, SO_BROADCAST, int> Broadcast;
#endif
#ifdef SO_SNDBUF
typedef OptionTemplate<SOL_SOCKET, SO_SNDBUF, int> SNDBuf;
#endif
#ifdef SO_RCVBUF
typedef OptionTemplate<SOL_SOCKET, SO_RCVBUF, int> RCVBuf;
#endif
#ifdef SO_PRIORITY
typedef OptionTemplate<SOL_SOCKET, SO_PRIORITY, int> Priority;
#endif
#ifdef SO_ERROR
typedef OptionTemplate<SOL_SOCKET, SO_ERROR, int> Error;
#endif
#ifdef SO_LINGER
typedef OptionTemplate<SOL_SOCKET, SO_LINGER, linger> Linger;
#endif

#ifdef SO_BINDTODEVICE
// This option uses a c-string as type, so template can't be used.
class BindToDevice: public Option
{
public:
	BindToDevice(const char* device) throw()
	{
		m_deviceName = new char[strlen(device) + 1];
		strcpy(m_deviceName, device);
	}

	virtual ~BindToDevice() throw()
	{
		delete[] m_deviceName;
	}

	virtual int Level() const throw()
	{
		return SOL_SOCKET;
	}
	virtual int Id() const throw()
	{
		return SO_BINDTODEVICE;
	}

protected:
	// virtual char* Pointer() throw()
	// {
	// 	return m_deviceName;
	// }
	// virtual const char* Pointer() const throw()
	// {
	// 	return m_deviceName;
	// }
	virtual socklen_t Size() const throw()
	{
		return strlen(m_deviceName) + 1;
	}

private:
	char* m_deviceName;
};
#endif
}

/// IP-specific options
namespace IPOption
{
#ifdef IP_PKTINFO
typedef OptionTemplate<SOL_IP, IP_PKTINFO, in_pktinfo> PaketInfo;
#endif
#ifdef IP_OPTIONS
typedef OptionTemplate<SOL_IP, IP_OPTIONS, ip_opts> Options;
#endif
#ifdef IP_RECVOPTS
typedef OptionTemplate<SOL_IP, IP_RECVOPTS, int> RecvOpts;
#endif
#ifdef IP_RETOPTS
typedef OptionTemplate<SOL_IP, IP_RETOPTS, int> RetOpts;
#endif
#ifdef IP_RECVTOS
typedef OptionTemplate<SOL_IP, IP_RECVTOS, int> RecvTOS;
#endif
#ifdef IP_RECVTTL
typedef OptionTemplate<SOL_IP, IP_RECVTTL, int> RecvTTL;
#endif
#ifdef IP_TOS
typedef OptionTemplate<SOL_IP, IP_TOS, int> TOS;
#endif
#ifdef IP_TTL
typedef OptionTemplate<SOL_IP, IP_TTL, int> TTL;
#endif
#ifdef IP_ROUTER_ALERT
typedef OptionTemplate<SOL_IP, IP_ROUTER_ALERT, int> RouterAlert;
#endif
#ifdef IP_MTU_DISCOVER
typedef OptionTemplate<SOL_IP, IP_MTU_DISCOVER, int> MTUDiscover;
#endif
#ifdef IP_MULTICAST_IF
typedef OptionTemplate<SOL_IP, IP_MULTICAST_IF, in_addr> MulticastIF;
#endif
#ifdef IP_MULTICAST_TTL
typedef OptionTemplate<SOL_IP, IP_MULTICAST_TTL, int> MulticastTTL;
#endif
#ifdef IP_MULTICAST_LOOP
typedef OptionTemplate<SOL_IP, IP_MULTICAST_LOOP, int> MulticastLoop;
#endif
#ifdef IP_ADD_MEMBERSHIP
typedef OptionTemplate<SOL_IP, IP_ADD_MEMBERSHIP, ip_mreq> AddMembership;
#endif
#ifdef IP_DROP_MEMBERSHIP
typedef OptionTemplate<SOL_IP, IP_DROP_MEMBERSHIP, ip_mreq> DropMembership;
#endif
//    typedef OptionPointerTemplate< SOL_IP, IP_RECVERR, void* > RecvErr;
}

/// TCP-specific options
namespace TCPOption
{
#ifdef TCP_NODELAY
typedef OptionTemplate<SOL_TCP, TCP_NODELAY, int> NoDelay;
#endif
#ifdef TCP_MAXSEG
typedef OptionTemplate<SOL_TCP, TCP_MAXSEG, int> MaxSeg;
#endif
#ifdef TCP_CORK
typedef OptionTemplate<SOL_TCP, TCP_CORK, int> Cork;
#endif
}

#endif /* OPTION_H_ */
