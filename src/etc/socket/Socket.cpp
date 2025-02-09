/*
 * Socket.cpp
 *
 *   创建日期: 2013-3-27
 *   作         者: mzh
 */
#include "Socket.h"

//错误返回码
namespace ErrorReturnValues
{
const int SOCKET_ERROR = -1;
const int BIND_ERROR = -1;
const int CONNECT_ERROR = -1;
const int SEND_ERROR = -1;
const int RECV_ERROR = -1;
const int LISTEN_ERROR = -1;
const int ACCEPT_ERROR = -1;
const int GETSOCKNAME_ERROR = -1;
const int GETPEERNAME_ERROR = -1;
const int GETSOCKOPT_ERROR = -1;
const int SETSOCKOPT_ERROR = -1;
const int SHUTDOWN_ERROR = -1;
}

//错误返回信息
namespace ErrorMessages
{
const string NOT_OPENED = "Socket not opened yet";
const string OPENED = "Socket already opened";
}

//异常抛出的方法名
namespace MethodNames
{
const string OPEN = "Socket::Open()";
const string CLOSE = "Socket::Close()";
const string BIND = "Socket::Bind()";
const string CONNECT = "Socket::Connect()";
const string SEND = "Socket::Send()";
const string SENDTO = "Socket::Sendto()";
const string RECV = "Socket::Recv()";
const string RECVFROM = "Socket::Recvfrom()";
const string LISTEN = "Socket::Listen()";
const string ACCEPT = "Socket::Accept()";
const string GETSOCKNAME = "Socket::Getsockname()";
const string GETPEERNAME = "Socket::Getpeername()";
const string GETSOCKOPT = "Socket::Getsockopt()";
const string SETSOCKOPT = "Socket::Setsockopt()";
const string SHUTDOWN = "Socket::Shutdown()";
const string WAIT = "Socket::Wait()";
}

using namespace ErrorReturnValues;
using namespace ErrorMessages;
using namespace MethodNames;

Socket::Socket() :
	m_socket(INVALID_HANDLE)
{
	//按缺省的属性初始化互斥体变量mutex
	pthread_mutex_init(&m_mutex, NULL);
}

Socket::Socket(int sfd) :
	m_socket(sfd)
{
}

Socket::Socket(Protocol protocol)
{
	Open(protocol);
}

Socket::~Socket()
{
	if (IsOpened())
	{
		::close(m_socket);//关闭端口
	}
	//销毁互斥变量
	pthread_mutex_destroy(&m_mutex);
}

//加锁
void Socket::Lock()
{
	// 给互斥体变量加锁
	pthread_mutex_lock(&m_mutex);
}

//解锁
void Socket::Unlock()
{
	// 给互斥体变量解除锁
	pthread_mutex_unlock(&m_mutex);
}

bool Socket::Open(Protocol protocol)
{
	zlog_info(Util::m_zlog, "打开socket");
	if (IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket已经打开");
		return true;
	}

	m_socket = socket(PF_INET, protocol, 0);
	if (m_socket == SOCKET_ERROR)
	{
		zlog_warn(Util::m_zlog, "socket打开失败");
		return false;
	}

	zlog_info(Util::m_zlog, "打开socket成功");
	return true;
}

void Socket::Close()
{
	zlog_info(Util::m_zlog, "关闭socket");
	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket已经关闭");
		return;
	}

	::close(this->m_socket);

	m_socket = INVALID_HANDLE;
	return;
}
using namespace std;
bool Socket::Bind(const sockaddr_in& addr)
{
	zlog_info(Util::m_zlog, "绑定IP地址");
	int error;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return false;
	}

	const sockaddr* socketAddress = reinterpret_cast<const sockaddr*> (&addr);

	error = ::bind(m_socket, socketAddress, sizeof(addr));
	if (error == BIND_ERROR)
	{
		zlog_warn(Util::m_zlog, "绑定失败");
		return false;
	}

	zlog_info(Util::m_zlog, "绑定IP地址成功");
	return true;
}

bool Socket::Bind(const Address& addr)
{
	zlog_info(Util::m_zlog, "绑定IP地址=%s,port=%d",addr.GetIPString(0).c_str(),addr.GetPort());
	return Bind(static_cast<const sockaddr_in> (addr));
}

bool Socket::Connect(const sockaddr_in& addr)
{
	zlog_info(Util::m_zlog, "连接网络上其他设备IP");
	int error;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return false;
	}

	const sockaddr* socketAddress = reinterpret_cast<const sockaddr*> (&addr);
	error = ::connect(m_socket, socketAddress, sizeof(addr));
	if (error == CONNECT_ERROR)
	{
		zlog_warn(Util::m_zlog, "连接失败");
		return false;
	}

	zlog_info(Util::m_zlog, "连接网络上其他设备成功");
	return true;
}

bool Socket::Connect(const Address& addr)
{
	zlog_info(Util::m_zlog, "连接IP地址=%s,port=%d",addr.GetIPString(0).c_str(),addr.GetPort());
	return Connect(static_cast<const sockaddr_in> (addr));
}

int Socket::Send(const char* data, size_t size, int flags)
{
	zlog_debug(Util::m_zlog, "向网络上其他设备发送TCP数据");
	int bytes = 0;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return bytes;
	}

	bytes = ::send(m_socket, data, size, flags);
	if (bytes == SEND_ERROR)
	{
		zlog_warn(Util::m_zlog, "向网络上其他设备发送TCP数据失败");
		return bytes;
	}
	else if (bytes == 0)
	{
		zlog_warn(Util::m_zlog, "向网络上其他设备发送TCP数据失败");
		bytes = SEND_ERROR;
	}

	zlog_debug(Util::m_zlog, "向网络上其他设备发送TCP数据成功,数量=%d",bytes);

	return bytes;
}

int Socket::Sendto(const sockaddr_in& addr, const char* data, size_t size,
		int flags)
{
	zlog_debug(Util::m_zlog, "向网络上其他设备发送UDP数据");
	int bytes = 0;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return bytes;
	}

	const sockaddr* socketAddress = reinterpret_cast<const sockaddr*> (&addr);

	bytes = ::sendto(m_socket, data, size, flags, socketAddress, sizeof(addr));
	if (bytes == SEND_ERROR)
	{
		zlog_warn(Util::m_zlog, "向网络上其他设备发送UDP数据失败");
		return bytes;
	}
	else if (bytes == 0)
	{
		zlog_warn(Util::m_zlog, "向网络上其他设备发送UDP数据失败");
		bytes = SEND_ERROR;
	}

	zlog_debug(Util::m_zlog, "向网络上其他设备发送UDP数据成功,数量=%d",bytes);
	return bytes;
}


int Socket::Recv(char* data, size_t size, int flags)
{
	zlog_debug(Util::m_zlog, "从网络上其他设备读取TCP数据");
	int bytes = -1;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return bytes;
	}

	bytes = ::recv(m_socket, data, size, flags);
	if (bytes == RECV_ERROR)
	{
		zlog_warn(Util::m_zlog, "从网络上其他设备读取TCP数据失败");
		return bytes;
	}
	else if (bytes == 0)
	{
		zlog_warn(Util::m_zlog, "从网络上其他设备读取TCP数据失败");
		bytes = RECV_ERROR;
	}

	zlog_debug(Util::m_zlog, "从网络上其他设备读取TCP数据成功,数量=%d",bytes);
	return bytes;
}

int Socket::Recvfrom(sockaddr_in& addr, char* data, size_t size, int flags)
{
	zlog_debug(Util::m_zlog, "从网络上其他设备读取UDP数据");
	int bytes = -1;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return bytes;
	}

	socklen_t addrLen = sizeof(addr);
	sockaddr* socketAddress = reinterpret_cast<sockaddr*> (&addr);
	bytes = ::recvfrom(m_socket, data, size, flags, socketAddress, &addrLen);
	if (bytes == RECV_ERROR)
	{
		zlog_warn(Util::m_zlog, "从网络上其他设备读取UDP数据失败");
		return bytes;
	}
	else if (bytes == 0)
	{
		zlog_warn(Util::m_zlog, "从网络上其他设备读取UDP数据失败");
		bytes = RECV_ERROR;
	}

	zlog_debug(Util::m_zlog, "从网络上其他设备读取UDP数据成功,数量=%d",bytes);

	return bytes;
}

int Socket::Send(const String& data, int flags)
{
	zlog_debug(Util::m_zlog, "向网络上其他设备发送TCP数据");
	char* tmp = new char[data.size() + 1];

	for (int i = 0; i < data.size(); i++)
	{
		tmp[i] = data[i];
	}
	int bytes = Send((const char*) tmp, data.size(), flags);

	delete[] tmp;
	return bytes;
}

int Socket::Sendto(const Address& addr, const String& data, int flags)
{
	zlog_debug(Util::m_zlog, "向网络上其他设备发送UDP数据");
	char* tmp = new char[data.size() + 1];
	for (int i = 0; i < data.size(); i++)
	{
		tmp[i] = data[i];
	}
	int bytes = Sendto(addr, (const char*) tmp, data.size(), flags);

	delete[] tmp;
	return bytes;
}

int Socket::SendFlush(const String& data, int flags)
{
	zlog_debug(Util::m_zlog, "向网络上其他设备读取UDP发送TCP数据");
	int bytes = 0;
	char* tmp = new char[data.size() + 1];
	TCPOption::NoDelay option(1);
	TCPOption::NoDelay optionDefault;
	if (this->Getsockopt(optionDefault))
	{
		if (this->Setsockopt(option))
		{
			for (int i = 0; i < data.size(); i++)
			{
				tmp[i] = data[i];
			}
			bytes = Send(tmp, data.size(), flags);
		}

		this->Setsockopt(optionDefault);
	}

	delete[] tmp;
	return bytes;
}

int Socket::Recv(String& data, int len, int flags)
{
	int bytes = 0;
	//2016.6.22修改
	char* tmp = new char[len + 1];
//	char* tmp = new char[65535];

	bytes = Recv(tmp, len, flags);

	if (bytes > 0)
	{
		data.clear();
		for (int i = 0; i < bytes; i++)
		{
			data.push_back(tmp[i]);
		}
	}

	delete[] tmp;

	return bytes;
}

int Socket::Recvfrom(Address& addr, String& data, int len, int flags)
{
	zlog_debug(Util::m_zlog, "从网络上其他设备读取UDP数据");
	int bytes = 0;
	char* tmp = new char[len + 1];
	sockaddr_in address;
	bytes = Recvfrom(address, tmp, len, flags);

	if (bytes > 0)
	{
		data.clear();
		for (int i = 0; i < bytes; i++)
		{
			data.push_back(tmp[i]);
		}
		addr = address;
	}

	delete[] tmp;

	return bytes;
}

int Socket::Recv(String& data, int len, int seconds, int useconds, int flags)
{
	zlog_debug(Util::m_zlog, "在超时时间内从网络上其他设备读取TCP数据");

	bool isRead = true;
	bool isWrite = false;
	bool isException = false;
	int num = 0;
	int ret = 0;
	String dataRead;
	data.clear();

	ret = Wait(isRead, isWrite, isException, seconds, useconds);
	while (ret && isRead)
	{
		dataRead.clear();
		//读取socket数据
		num = this ->Recv(dataRead, len - data.size(), flags);
		if (num > 0)
		{
			for (int i = 0; i < num; i++)
			{
				data.push_back(dataRead[i]);

				//433~436 8.22添加，针对网口数据有结束符的协议
				if((dataRead[i] == 0x0A) && (i == (num - 1)))
				{
					break;
				}
			}

			num = data.size();
		}
		else
		{
			//无数据跳出循环
			break;
		}

		if (data.size() < len)
		{
			ret = Wait(isRead, isWrite, isException, seconds, useconds);
		}
		else
		{
			num = data.size();
			break;
		}
	}

	// zlog_debug(Util::m_zlog, "在超时时间内从网络上其他设备读取TCP数据：期望=%d,实际=%d",len,data.size());

	return num;
}

int Socket::Recvfrom(Address& addr, String& data, int len, int seconds,
		int useconds, int flags)
{
	zlog_debug(Util::m_zlog, "在超时时间内从网络上其他设备读取UDP数据");
	bool isRead = true;
	bool isWrite = false;
	bool isException = false;
	bool ret = false;
	int num = 0;
	char* tmp = new char[len + 1];

	ret = Wait(isRead, isWrite, isException, seconds, useconds);

	if (ret && isRead)
	{
		num = this ->Recv(tmp, len, flags);

		if (num > 0)
		{
			data.clear();
			for (int i = 0; i < num; i++)
			{
				data.push_back(tmp[i]);
			}
		}
	}
	else
	{
		num = 0;
	}

	delete[] tmp;
	return num;
}

bool Socket::Listen(int queuelen)
{
	zlog_info(Util::m_zlog, "启动服务器监听,客户端最大队列=%d",queuelen);
	int error = 0;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return false;
	}

	error = ::listen(m_socket, queuelen);
	if (error == LISTEN_ERROR)
	{
		zlog_warn(Util::m_zlog, "打开监听失败");
		return false;
	}

	zlog_info(Util::m_zlog, "启动服务器监听成功");
	return true;
}

Socket::Descriptor Socket::Accept(sockaddr_in& addr)
{
	zlog_info(Util::m_zlog, "接受客户端新建连接");
	Descriptor sfd;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return INVALID_HANDLE;
	}

	socklen_t addrLen = sizeof(addr);

	sockaddr* socketAddress = reinterpret_cast<sockaddr*> (&addr);

	sfd = ::accept(m_socket, socketAddress, &addrLen);
	if (sfd == ACCEPT_ERROR)
	{
		zlog_warn(Util::m_zlog, "接受客户端新建连接失败");
		return INVALID_HANDLE;
	}

	zlog_info(Util::m_zlog, "启动服务器监听成功");
	return sfd;
}

Socket::Descriptor Socket::Accept(Address& addr)
{
	zlog_error(Util::m_zlog, "接受客户端新建连接%s",addr.GetIPString(0).c_str());
	Descriptor tempSocket;

	sockaddr_in address = addr;
	tempSocket = Accept(address);
	addr = address;

	return tempSocket;
}

Address Socket::Getsockname()
{
	zlog_info(Util::m_zlog, "返回主机的地址");
	int error = 0;
	Address addr;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return addr;
	}

	sockaddr_in address;
	socklen_t addrLen = sizeof(address);

	sockaddr* socketAddress = reinterpret_cast<sockaddr*> (&address);

	error = ::getsockname(m_socket, socketAddress, &addrLen);
	if (error == GETSOCKNAME_ERROR)
	{
		zlog_warn(Util::m_zlog, "查询主机地址失败");
		return addr;
	}

	zlog_info(Util::m_zlog, "返回主机的地址成功");
	return Address(address);
}

Address Socket::Getpeername()
{
	zlog_info(Util::m_zlog, "返回被连接主机的地址");
	int error;
	Address addr;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return addr;
	}

	sockaddr_in address;
	socklen_t addrLen = sizeof(address);

	sockaddr* socketAddress = reinterpret_cast<sockaddr*> (&address);

	error = ::getpeername(m_socket, socketAddress, &addrLen);
	if (error == GETPEERNAME_ERROR)
	{
		zlog_warn(Util::m_zlog, "查询被连接主机地址失败");
		return addr;
	}

	zlog_info(Util::m_zlog, "返回被连接主机的地址成功");
	return Address(address);
}

int Socket::Getsockopt(Option& option)
{
	zlog_debug(Util::m_zlog, "返回socket参数");
	int error;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return SOCKET_ERROR;
	}

	socklen_t optSize = option.Size();

	error = ::getsockopt(m_socket, option.Level(), option.Id(),
			option.Pointer(), &optSize);
	if (error == GETSOCKOPT_ERROR)
	{
		zlog_warn(Util::m_zlog, "读取参数失败");
		return GETSOCKOPT_ERROR;
	}

	zlog_debug(Util::m_zlog, "返回socket参数成功");
	return optSize;
}

bool Socket::Setsockopt(const Option& option)
{
	zlog_debug(Util::m_zlog, "设置socket参数");
	int error;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return false;
	}

	error = ::setsockopt(m_socket, option.Level(), option.Id(),
			option.Pointer(), option.Size());
	if (error == SETSOCKOPT_ERROR)
	{
		zlog_warn(Util::m_zlog, "读取参数失败");
		return false;
	}

	zlog_debug(Util::m_zlog, "设置socket参数成功");
	return true;
}

bool Socket::Shutdown(ShutdownMode how)
{
	zlog_info(Util::m_zlog, "关闭读/写功能");
	int error;

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return false;
	}

	error = ::shutdown(m_socket, how);
	if (error == SHUTDOWN_ERROR)
	{
		zlog_warn(Util::m_zlog, "执行shutdown失败");
		return false;
	}

	zlog_info(Util::m_zlog, "关闭读/写功能成功");
	return true;
}

bool Socket::Wait(bool& read, bool& write, bool& exception, int seconds,
		int useconds)
{
	//zlog_info(Util::m_zlog, "判断是否可读/可写/异常功能");

	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog, "socket没有打开");
		return false;
	}

	struct timeval time;
	time.tv_sec = seconds;
	time.tv_usec = useconds;

	fd_set* readfds = NULL;
	fd_set* writefds = NULL;
	fd_set* exceptfds = NULL;

	if (read)
	{
		readfds = new fd_set;

		FD_ZERO(readfds);
		FD_SET(m_socket, readfds);
	}

	if (write)
	{
		writefds = new fd_set;

		FD_ZERO(writefds);
		FD_SET(m_socket, writefds);
	}

	if (exception)
	{
		exceptfds = new fd_set;

		FD_ZERO(exceptfds);
		FD_SET(m_socket, exceptfds);
	}

	int ret = select(m_socket + 1, readfds, writefds, exceptfds, &time);
	if (read)
	{
		read = FD_ISSET(m_socket, readfds);
	}

	if (write)
	{
		write = FD_ISSET(m_socket, writefds);
	}

	if (exception)
	{
		exception = FD_ISSET(m_socket, exceptfds);
	}

	delete readfds;
	delete writefds;
	delete exceptfds;

	if (ret < 0)
	{
		zlog_warn(Util::m_zlog, "执行select失败");
		return false;
	}

	//zlog_info(Util::m_zlog, "判断是否可读/可写/异常功能成功");
	return (ret != 0);
}

//查询操作
int Socket::TxRxMsg(String& dataRead, String& dataWrite, int len)
{
	int ret = 0;
	int timeout = 400;//ms

	dataRead.clear();
	//判断网口是否有效
	if (!IsOpened())
	{
		zlog_warn(Util::m_zlog,"网口没有打开");
		return ErrorInfo::ERR_OPENED;
	}

	//写入网口
	Lock();//给互斥变量加锁

	//清除网口缓冲区数据
	tcflush(m_socket, TCIOFLUSH);
	ret = SendFlush(dataWrite);
	if (ret < 0)
	{
		Unlock();
		this->Close();
		zlog_warn(Util::m_zlog,"向网口写入数据失败");
		return ErrorInfo::ERR_WRITE;
	}

	//读取网口数据
	ret = Recv(dataRead, len, timeout / 1000, (timeout % 1000)*1000);
//	ret = Recv(dataRead, len, 5, 0);

	if (ret < 0)
	{
		Unlock();
		this->Close();
		zlog_warn(Util::m_zlog,"读取数据失败");
		return ErrorInfo::ERR_READ;
	}

#if 1
	if ((ret < len) && (ret == 0))
	{
		Unlock();
		zlog_debug(Util::m_zlog,"读取数据数量不足: %d < %d",ret,len);
		return ErrorInfo::ERR_INV_RESP;
	}
#endif
	Unlock();
	zlog_debug(Util::m_zlog,"读写操作完成");

	return ErrorInfo::ERR_OK;
}

