/*
 * UdpNode.h
 *
 *   创建日期: 2013-3-27
 *   作         者: mzh
 */

#ifndef UDPNODE_H_
#define UDPNODE_H_
#include "Socket.h"

/*名称:UdpNode
 *描述:实现UDP通讯,包括客户端/服务器端,广播形式和点对点
 *用法:
   1 UDP服务器端程序
    1)打开socket绑定本机端口
 	string ip("192.168.1.249");
	bool isServer = true;
	bool isBroadcast = true;
	int port = 10000;
	Address addrSend(ip, port);
	ret = client.Open(isServer, isBroadcast, addrSend);
   2)等待接收数据
	num = client.Recvfrom(addrRecv, dataRecv);//阻塞式
	3)发送数据
	client.Sendto(addrRecv,dataSend);
  2 UDP客户端程序
   1)打开socket,设置广播式/点对点式
   	string ip("192.168.1.249");
	bool isServer = false;
	bool isBroadcast = true;
	int port = 10000;
	Address addrSend(ip, port);
	ret = client.Open(isServer, isBroadcast, addrSend);
   2)等待接收数据
	num = client.Recvfrom(addrRecv, dataRecv,1,0);//非阻塞式
   3)发送数据
	client.Sendto(addrRecv,dataSend);
 *创建人:mzh
 *创建日期:2013-3-27
 */
class UdpNode : public Socket
{
public:

public:
	UdpNode();
	virtual ~UdpNode();

	/*
	 * 名称: Open
	 * 功能: 打开UDP
	 * 输入: 无
	 * 输出: true打开成功/false打开失败
	 */
    bool Open();
	/*
	 * 名称: Open
	 * 功能: 打开UDP
	 * 输入:
	 * isServer     :[in]const bool true服务器/false客户端
	 * isBroadcast  :[in]const bool true广播模式/false点对点
	 * port         :[in]const int 设备地址,绑定的本地IP端口,0表示任意端口
	 * 输出: true打开成功/false打开失败
	 */
    bool Open(const bool isServer,const bool isBroadcast,const int port);

	/*
	 * 名称: Recvfrom
	 * 功能: 阻塞式,通过UDP读取数据到其他设备数据
	 * 输入:
	 * addr    :[in]const Address& 网络上其他设备地址
	 * data    :[in]const String& 接受数据
	 * len     :[in]int		读取长度
	 * flags   :[in]int socket 参数标志
	 * 输出: int 接收数据个数,<0表示连接断开,=0表示没有数据
	 */
	int Recvfrom(Address& addr, String& data, int len, int flags = 0);

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
	 * 输出: int 发送数据个数,<0表示连接断开,=0表示超时时间内没有数据
	 */
	int Recvfrom(Address& addr, String& data, int len, int seconds, int useconds,
			int flags = 0);

	/*
	 * 名称: SetServer
	 * 功能: 设置客户端/服务器
	 * 输入:
	 * server     :[in]bool true服务器/false客户端
	 * 输出: 无
	 */
    void SetServer(bool server);

	/*
	 * 名称: IsServer
	 * 功能: 返回客户端/服务器
	 * 输入:
	 * 输出: bool true服务器/false客户端
	 */
    bool IsServer();

	/*
	 * 名称: SetBroadcast
	 * 功能: 设置广播模式/点对点
	 * 输入:
	 * broadcast     :[in]bool true广播模式/false点对点
	 * 输出: 无
	 */
    void SetBroadcast(bool broadcast);
	/*
	 * 名称: IsBroadcast
	 * 功能: 返回广播模式/点对点
	 * 输入: 无
	 * 输出: bool true广播模式/false点对点
	 */
    bool IsBroadcast();

	/*
	 * 名称: SetRemoteAddr
	 * 功能: 设置对方地址地址
	 * 输入:
	 * addr         :[in]const Address& 设备对方地址地址
	 * 输出: 无
	 */
    void SetRemoteAddr(const Address& addr);
	/*
	 * 名称: GetRemoteAddr
	 * 功能: 返回对方地址地址,最后一次读取/发送地址
	 * 输入:
	 * 输出: Address 设备对方地址
	 */
    Address GetRemoteAddr();

	/*
	 * 名称: SetHostAddr
	 * 功能: 设置本机地址
	 * 输入:
	 * addr         :[in]const Address& 设备本机地址
	 * 输出: 无
	 */
    void SetHostAddr(const Address& addr);
	/*
	 * 名称: GetHostAddr
	 * 功能: 返回设备本机地址
	 * 输入:
	 * 输出: Address 设备本机地址
	 */
    Address GetHostAddr();

protected:

private:
	bool m_isServer;//服务器/客户端
	bool m_isBroadcast;//广播模式/点对点
	Address m_remoteAddr;//连接设备地址,会根据接收到数据更新
	Address m_hostAddr;//本机地址

	int m_port;//绑定的本地端口,0表示有系统决定,用于客户端

};

#endif /* UDPNODE_H_ */
