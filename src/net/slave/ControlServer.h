/*
 * ControlServer.h
 *
 *   创建日期: 2013-4-5
 *   作         者: mzh
 */

#ifndef CONTROLSERVER_H_
#define CONTROLSERVER_H_
#include "../../util/Util.h"
#include "../../etc/socket/Thread.h"
#include "../../etc/socket/TcpServer.h"
#include "../Iec103.h"

class SlaveManager;

/*名称:ControlServer
 *描述:站内通讯规约TCP控制线程
 *用法:实现站内通讯规约接收TCP控制指令
 *创建人:mzh
 *创建日期:2013-4-5
 */
class ControlServer: public TcpService
{
public:
	ControlServer(Socket* sock,void* param);
	virtual ~ControlServer();

protected:
private:
	/*
	 *  Run
	 *  功能:线程执行函数
	 *  输入：无
	 *  输出：无
	 */
	virtual void Run();

	//互斥加锁
	void Lock();
	//互斥解锁
	void Unlock();

	/*
	 *  RecvInfo
	 *  功能: 服务器等待客户端发送数据
	 *  输入：
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
    int RecvInfo();

	/*
	 *  SendInfo
	 *  功能: 服务器返回客户端确认帧数据
	 *  输入：
	 *  confirm   :[in]bool true肯定确认/false否定确认
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
    int SendInfo(bool confirm,Iec103::PacketHead head);

	/*
	 *  PackHead
	 *  功能:封装包头
	 *  输入：
	 *  destHead    :[out]Iec103::PacketHead& 封装帧头
	 *  srcHead     :[in]Iec103::PacketHead& 帧头
	 *  输出：
	 */
    void PackHead(Iec103::PacketHead& destHead,Iec103::PacketHead& srcHead);

	/*
	 *  SaveCtrlData
	 *  功能:保存控制数据
	 *  输入：
	 *  输出：Iec103::PacketHead
	 */
    bool SaveCtrlData(Iec103::PacketData& data);

	/*
	 *  SaveTimesync
	 *  功能:时间同步
	 *  输入：
	 *  输出：Iec103::PacketHead
	 */
    bool SaveTimesync(Iec103::PacketData& data);

public:
protected:
private:
	const int m_timeout;//接收数据超时时间
	SlaveManager* m_slaveManager;//站内通讯规约兼容103协议从站管理类
	MasterConfig* m_masterConfig;//与本线程通讯的主站通讯参数

	bool m_networkA;//当前是A网段/B网段
	pthread_mutex_t m_mutex;
};

typedef TcpServiceFactoryTemplate<ControlServer> ControlServerFactory;

#endif /* CONTROLSERVER_H_ */
