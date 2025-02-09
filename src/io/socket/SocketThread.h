/*
 * SocketThread.h
 *
 *  Created on: 2013-4-10
 *      Author: liuhaitao
 */

#ifndef SOCKETTHREAD_H_
#define SOCKETTHREAD_H_

#include "../../etc/socket/Thread.h"
#include "../../util/Util.h"
#include "../../db/Device.h"
#include "../../etc/socket/TcpClient.h"
#include "../../etc/socket/Socket.h"

class SocketThreads;

class SocketAddress
{
public:
	string ip;//IP地址
	int    port;//端口
};

class SocketThread: public Thread
{
public:
	SocketThread();
	virtual ~SocketThread();

	/*
	 * SetSocketThreads
	 * 功能：保存Socket线程池指针
	 * 输入：
	 * socketThreads	:[in] SokcetThread* IO管理类指针
	 * 输出：无
	 */
	void SetSocketThreads(SocketThreads* socketThreads);

	/*
	 * SocketAddress
	 * 功能：保存以太网设备IP和端口信息
	 * 输入：
	 * socketAddress	:[in] SocketAddress* SocketAddress指针
	 * 输出：无
	 */
	void SetSocketTypeDev(SocketAddress* socketAddress);

	/*
	 *  SetTcpClent
	 *  功能:保存socket设备指针
	 *  输入：
	 *  client    	  :[in]TcpClient* socket指针
	 *  输出：无
	 */
	void SetTcpClent(TcpClient* client);
protected:
private:
	/*
	 * Run
	 * 功能：线程执行函数
	 * 该线程内运行Socket口数据采集程序
	 * 输入：无
	 * 输出：无
	 */
	virtual void Run();

	/*
	 * FindSocketSlaveDevs
	 * 功能：查找指定IP和端口以太网从设备
	 *
	 * 输入：无
	 * 输出：无
	 */
	void FindSocketSlaveDevs();

	//变量
public:
protected:
private:

//	Device::SlaveDev* m_socketSlaveDev;//从设备管理指针

	PtrArray m_socketSlaveDevs;//从设备管理指针数组
	SocketAddress* m_socketAddress;//以太网通讯设备IP和端口

	SocketThreads* m_socketThreads;//线程父类指针

	TcpClient* m_tcpClient;
};

#endif /* SOCKETTHREAD_H_ */


























































































