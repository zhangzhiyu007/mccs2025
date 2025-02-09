/*
 * UdpManagerServer.h
 *
 *   创建日期: 2013-4-5
 *   作         者: mzh
 */

#ifndef UDPMANAGERSERVER_H_
#define UDPMANAGERSERVER_H_
#include "../../etc/socket/Thread.h"
#include "../../etc/socket/UdpNode.h"
#include "../../util/Util.h"
#include "../Iec103.h"

class SlaveManager;


/*名称:UdpManagerServer
 *描述:用于103协议中，站内设备从站UDP管理端口，该端口默认6603
 * 		应用于从站程序，从展启动后启动监听端口，接收主站信息
 *     一旦接收到,就向主站回复从站配置信息。只向主站发送一次信息,一旦建立TCP服务器
 *     就不再发送从站信息
 *用法:
 *创建人:mzh
 *创建日期:2013-4-5
 */
class UdpManagerServer: public Thread
{
public:
	UdpManagerServer();
	virtual ~UdpManagerServer();

	/*
	 *  SetSlaveManager
	 *  功能:保存从站管理类指针
	 *  输入：
	 *  manager    :[in]SlaveManeger* 主从站管理类指针
	 *  输出：返回Device*指针，
	 */
	void SetSlaveManager(SlaveManager* manager);
protected:
private:
	/*
	 *  Run
	 *  功能:线程执行函数
	 *  输入：无
	 *  输出：无
	 */
    virtual void Run();

    //判断A/B网段UDP是否打开
    bool IsUdpOpened();

	/*
	 *  OpenedUdp
	 *  功能:打开UDP管理端口
	 *  输入：无
	 *  输出：true成功/false失败
	 */
    bool OpenedUdp();

	/*
	 *  CloseUdp
	 *  功能:关闭UDP管理端口
	 *  输入：无
	 *  输出：无
	 */
    void CloseUdp();

	/*
	 *  RecvInfo
	 *  功能:接收UDP主站广播信息包，收到广播信息后查看是否已经存在该主站信息
	 *  如果已经存在并且与该主站之间已经建立TCP控制连接则不返回信息,
	 *  否则返回本机配置信息,使主站可以与本机建立通讯
	 *
	 *  输入：
	 *  udp    :[in]const UdpNode* UDP指针，不存在着指针为NULL
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
    int RecvInfo(UdpNode* udp);

	/*
	 *  PackHead
	 *  功能:封装包头
	 *  输入：
	 *  head           :[out]Iec103::PacketHead& 封装包头
	 *  masterConfig   :[in]MasterConfig* 主站配置指针
	 *  输出：
	 */
    void PackHead(Iec103::PacketHead&head,MasterConfig* masterConfig);

    //变量
public:
protected:
	const int m_timeout;//接收数据超时时间
	unsigned short  m_dataNumber;//数据编号

	SlaveManager* m_slaveManager;//站内通讯规约兼容103协议从站管理类

	UdpNode* m_udp;//UDP指针
	int m_networkA;//A网段通讯状况
	int m_networkB;//B网段通讯状况
};

#endif /* UDPMANAGERSERVER_H_ */
