/*
 * Socket.h
 *
 *   创建日期: 2013-3-27
 *   作         者: mzh
 */

#ifndef SOCKET_H_
#define SOCKET_H_
#include "../../util/Util.h"
#include "Buffer.h"
#include "Option.h"

//flags标志
//    MSG_DONTROUTE：不要使用网关来发送封包，只发送到直接联网的主机。这个标志主要用于诊断或者路由程序。
//    MSG_DONTWAIT：操作不会被阻塞。
//    MSG_EOR：终止一个记录。
//    MSG_MORE：调用者有更多的数据需要发送。
//    MSG_NOSIGNAL：当另一端终止连接时，请求在基于流的错误套接字上不要发送SIGPIPE信号。
//    MSG_OOB：发送out-of-band数据(需要优先处理的数据)，同时现行协议必须支持此种操作。

class Socket
{
public:
	typedef int Descriptor;

	//socket错误码
	enum ErrorCode
	{
		NOT_OPENED_CODE = 0x0001, //没有打开
		OPENED_CODE = 0x0002
	//已经打开
	};

	//支持协议类型
	enum Protocol
	{
		TCP = SOCK_STREAM, //TCP
		UDP = SOCK_DGRAM   //UDP
	};

	//send(), sendto(), recv() and recvfrom()发送标志
	//可以用|
	enum Flags
	{
		OOB = MSG_OOB, //带外数据
		DONTROUTE = MSG_DONTROUTE, //不通过路由器而是直接发送信息帧
		DONTWAIT = MSG_DONTWAIT, //使用非阻塞模式
		NOSIGNAL = MSG_NOSIGNAL, //禁止send()函数向系统发送异常消息
		CONFIRM = MSG_CONFIRM, //SOCK_DGRAM和SOCK_RAW类型。通知数据链路层发生了转发，且得到了通信另一段的回应
		PEEK = MSG_PEEK, //查看当前数据不清除缓冲区
		WAITALL = MSG_WAITALL, //等待直到所有数据收到
		TRUNC = MSG_TRUNC
	//返回实际包大小
	};

	//关闭连接
	enum ShutdownMode
	{
		READ = SHUT_RD, //关闭读功能
		WRITE = SHUT_WR, //关闭写功能
		READ_WRITE = SHUT_RDWR
	//关闭读写功能
	};

public:
	// 创建socket
	Socket();
	//用于已经创建的socket，accept()返回已经打开的socket
	Socket(Descriptor sfd);
	//按照UDP/TCP创建socket
	explicit Socket(Protocol protocol);

	virtual ~Socket();

	/*
	 * 名称: Open
	 * 功能: 打开socket
	 * 输入:protocol    :[in]Protocol UDP/TCP类型
	 * 输出: true成功/false失败
	 */
	bool Open(Protocol protocol);

	/*
	 * 名称: close
	 * 功能: 关闭socket
	 * 输入: 无
	 * 输出: 无
	 */
	void Close();

	/*
	 * 名称: 操作符
	 * 功能: 返回socket句柄
	 * 输入: 无
	 * 输出: 无
	 */
	operator Descriptor()
	{
		return m_socket;
	}

	/*
	 * 名称: IsOpened
	 * 功能: 返回socket是否打开
	 * 输入: 无
	 * 输出: true打开/false关闭
	 */
	bool IsOpened()
	{
		return m_socket > 0;
	}

	/*
	 * 名称: Bind
	 * 功能: 绑定socket地址
	 * 输入: addr    :[in]const sockaddr_in& IP地址
	 * 输出: true成功/false失败
	 */
	bool Bind(const sockaddr_in& addr);
	/*
	 * 名称: Bind
	 * 功能: 绑定socket地址
	 * 输入: addr    :[in]const Address& IP地址
	 * 输出: true成功/false失败
	 */
	bool Bind(const Address& addr);

	/*
	 * 名称: Connect
	 * 功能: 连接网络上其他设备IP
	 * 输入: addr    :[in]const sockaddr_in& IP地址
	 * 输出: true成功/false失败
	 */
	bool Connect(const sockaddr_in& addr);

	/*
	 * 名称: Connect
	 * 功能: 连接网络上其他设备IP
	 * 输入: addr    :[in]const Address& IP地址
	 * 输出: true成功/false失败
	 */
	bool Connect(const Address& addr);

	/*
	 * 名称: Send
	 * 功能: 通过TCP发送数据到其他设备
	 * 输入:
	 *
	 * data    :[in]const char* 发送数据
	 * size    :[in]size_t 发送数据data字节数量
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int Send(const char* data, size_t size, int flags = 0);

	/*
	 * 名称: Sendto
	 * 功能: 通过UDP发送数据到其他设备
	 * 输入:
	 * addr    :[in]const sockaddr_in& 网络上其他设备地址
	 * data    :[in]const char* 发送数据
	 * size    :[in]size_t 发送数据data字节数量
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int Sendto(const sockaddr_in& addr, const char* data, size_t size,
			int flags = 0);

	/*
	 * 名称: Recv
	 * 功能: 通过TCP读取网络上其他设备数据
	 * 输入:
	 * data    :[out]const char* 接受数据保存地址
	 * size    :[in]size_t 发送数据data字节数量
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int Recv(char* data, size_t size, int flags = 0);

	/*
	 * 名称: Recvfrom
	 * 功能: 通过UDP读取数据到其他设备数据
	 * 输入:
	 * addr    :[out]const sockaddr_in& 网络上其他设备地址
	 * data    :[out]const char* 发送数据
	 * size    :[in]size_t 发送数据data字节数量
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int Recvfrom(sockaddr_in& addr, char* data, size_t size, int flags = 0);

	/*
	 * 名称: Send
	 * 功能: 通过TCP发送数据到其他设备
	 * 输入:
	 * data    :[in]const String& 发送数据
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int Send(const String& data, int flags = 0);

	/*
	 * 名称: Sendto
	 * 功能: 通过UDP发送数据到其他设备
	 * 输入:
	 * addr    :[in]const Address& 网络上其他设备地址
	 * data    :[in]const String& 发送数据
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int Sendto(const Address& addr, const String& data, int flags = 0);

	/*
	 * 名称: SendFlush
	 * 功能: 通过TCP发送数据到其他设备,立即将数据发送
	 * 输入:
	 * data    :[in]const String& 发送数据
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int SendFlush(const String& data, int flags = 0);

	/*
	 * 名称: Recv
	 * 功能: 阻塞式,通过TCP读取网络上其他设备数据
	 * 输入:
	 * data    :[out]Buffer& 接受数据保存地址
	 * len	   :[int]int     读取长度
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int Recv(String& data, int len, int flags = 0);

	/*
	 * 名称: Recvfrom
	 * 功能: 阻塞式,通过UDP读取数据到其他设备数据
	 * 输入:
	 * addr    :[in]const Address& 网络上其他设备地址
	 * data    :[in]const String& 接受数据
	 * len     :[in]int 	读取长度
	 * flags   :[in]int socket 参数标志
	 * 输出: int 发送数据个数,-1表示连接断开
	 */
	int Recvfrom(Address& addr, String& data, int len, int flags = 0);

	/*
	 * 名称: Recv
	 * 功能: 超时时间内,通过TCP读取网络上其他设备数据
	 * 输入:
	 * data    :[out]String& 接受数据保存地址
	 * len	   :[in]int 读取长度
	 * seconds :[in] int 超时时间 s
	 * useconds:[in] int 超时时间 us微妙
	 * flags   :[in]int socket参数标志
	 * 输出: int 发送数据个数,-1表示连接断开,0表示超时时间内没有数据
	 */
	int Recv(String& data, int len, int seconds, int useconds, int flags = 0);

	/*
	 * 名称: Recvfrom
	 * 功能: 超时时间内,通过UDP读取数据到其他设备数据
	 * 输入:
	 * addr    :[in]const Address& 网络上其他设备地址
	 * data    :[in]const String& 接受数据
	 * len     :[in]int    读取长度
	 * seconds :[in] int 超时时间 s
	 * useconds:[in] int 超时时间 us微妙
	 * flags   :[in]int socket 参数标志
	 * 输出: int 发送数据个数,-1表示连接断开,0表示超时时间内没有数据
	 */
	int Recvfrom(Address& addr, String& data, int len, int seconds, int useconds,
			int flags = 0);

	/*
	 * 名称: Listen
	 * 功能: 监听客户端连接
	 * 输入:
	 * queuelen    :[in]int 客户端连接最大排队数量
	 * 输出: bool true成功/false失败
	 */
	bool Listen(int queuelen);

	/*
	 * 名称: Accept
	 * 功能: 接受客户端新建连接
	 * 输入:
	 * addr    :[out]sockaddr_in& 客户端地址
	 * 输出:Descriptor 客户端新建连接的socket句柄,失败返回INVALID_HANDLE
	 */
	Descriptor Accept(sockaddr_in& addr);

	/*
	 * 名称: Accept
	 * 功能: 接受客户端新建连接
	 * 输入:
	 * addr    :[out]Address& 客户端地址
	 * 输出:Descriptor 客户端新建连接的socket句柄,失败返回INVALID_HANDLE
	 */
	Descriptor Accept(Address& addr);

	/*
	 * 名称: Getsockname
	 * 功能: 返回本机的地址
	 * 输入:
	 * 输出:Address socket的地址,失败返回空地址
	 */
	Address Getsockname();
	/*
	 * 名称: Getpeername
	 * 功能: 返回被连接设备的地址
	 * 输入:
	 * 输出:Address socket的地址,失败返回空地址
	 */
	Address Getpeername();

	/*
	 * 名称: Getsockopt
	 * 功能: 返回socket参数
	 * 输入:option    :[out]Option& socket参数
	 * 输出:int 返回读取属性的字节数,失败返回-1
	 */
	int Getsockopt(Option& option);

	/*
	 * 名称: Getsockopt
	 * 功能: 设置socket参数
	 * 输入:option    :[out]const Option& socket参数
	 * 输出:bool true成功/false失败
	 */
	bool Setsockopt(const Option& option);

	/*
	 * 名称: Shutdown
	 * 功能: 关闭读/写
	 * 输入: how    :[in]Shutdown 关闭方式:读/写或者读写
	 * 输出:bool true成功/false失败
	 */
	bool Shutdown(ShutdownMode how);

	/*
	 * 名称: Wait
	 * 功能: 判断是否可读\可写\异常
	 * 用法: 如果要查看是否可读,read=true,其余为false即可
	 * 输入:
	 * read         :[in/out]bool& 判断是否可读
	 * write        :[in/out]bool& 判断是否可写
	 * exception    :[in/out]bool& 判断是否异常
	 * seconds      :[in] int 超时时间 s
	 * useconds     :[in] int 超时时间 us微妙
	 * 输出:bool true没有超时/false超时
	 */
	bool Wait(bool& read, bool& write, bool& exception, int seconds = 0,
			int useconds = 0);

	/*
	 * 名称: TxRxMsg
	 * 功能: 用于在网口中读写数据，先发送查询数据，然后再读取数据
	 * 输入:
	 * dataRead      :[out]string& 从网口读取的数据
	 * dataWrite     :[in]string& 向网口写入的数据
	 * len			 :[in] int 期望从网口读取的数据的长度
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int TxRxMsg(String& dataRead, String& dataWrite, int len);

protected:

private:
	//加锁
	inline void Lock();
	//解锁
	inline void Unlock();

private:
	//socket句柄 >=0有效否则无效
	Descriptor m_socket;

	//同步
	pthread_mutex_t m_mutex;//互斥
};

#endif /* SOCKET_H_ */
