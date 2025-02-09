/*
 * SocketCtrlThread.h
 *
 *   创建日期: 2013-8-8
 *   作         者: mzh
 */

#ifndef SOCKETCTRLTHREAD_H_
#define SOCKETCTRLTHREAD_H_

#include "../../etc/socket/Thread.h"
#include "../../util/Util.h"
#include "../../db/Device.h"
#include "../../etc/socket/TcpClient.h"
#include "../../etc/socket/Socket.h"

class SocketThreads;
class SocketAddress;

/*名称:SocketCtrlThread
 *描述:以太网设备控制
 *用法:
 *创建人:mzh
 *创建日期:2013-8-8
 */
class SocketCtrlThread: public Thread
{
public:
	SocketCtrlThread();
	virtual ~SocketCtrlThread();

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

	PtrArray m_socketSlaveDevs;//从设备管理指针数组
	SocketAddress* m_socketAddress;//以太网通讯设备IP和端口

	SocketThreads* m_socketThreads;//线程父类指针

	TcpClient* m_tcpClient;
};

#endif /* SOCKETCTRLTHREAD_H_ */
