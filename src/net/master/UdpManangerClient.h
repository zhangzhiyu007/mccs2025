/*
 * UdpManangerClient.h
 *
 *   创建日期: 2013-4-2
 *   作         者: mzh
 */

#ifndef UDPMANANGERCLIENT_H_
#define UDPMANANGERCLIENT_H_
#include "../../util/Util.h"
#include "../../etc/socket/Thread.h"
#include "../../etc/socket/TcpClient.h"
#include "../../etc/socket/UdpNode.h"
#include "../Iec103.h"

class MasterManager;
class UdpNode;

/*名称:UdpManangerClient
 *描述:用于103协议中，站内设备UDP管理端口，本地端口6603,从站端口6604
 * 		应用于主站程序，主站启动后启动UDP广播端口，每间隔10s定时发送
 * 		主站类型、主站地址、通讯状态等信息，目的是让从站
 * 		启动后能够找到主站。
 *用法:
 *创建人:mzh
 *创建日期:2013-4-2
 */
class UdpManangerClient: public Thread
{
public:
	UdpManangerClient();
	virtual ~UdpManangerClient();

	/*
	 *  SetMasterManager
	 *  功能:保存主站管理类指针
	 *  输入：
	 *  manager    :[in]MasterManager* 主站管理类指针
	 *  输出：返回Device*指针，
	 */
	void SetMasterManager(MasterManager* manager);

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
	 *  PackHead
	 *  功能:封装包头
	 *  输入：
	 *  head     :[out]Iec103::PacketHead& 封装包头
	 *  输出：
	 */
    void PackHead(Iec103::PacketHead& head);

	/*
	 *  SendInfo
	 *  功能:发送UDP主站广播信息包
	 *  输入：
	 *  dataSend    :[in]const String&  发送数据
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int SendInfo(const String& dataSend);

	/*
	 *  SetDevInfo
	 *  功能:从站接收UDP主站广播信息包，收到广播信息后立即发送确认帧，
	 *  确认帧按照原来帧复制发送，数据编号改变，其余不变。
	 *  输入：
	 *  udp    :[in]const UdpNode* UDP指针，不存在着指针为NULL
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
    int RecvInfo(UdpNode* udp);

protected:
private:

	//发送数据帧，数据编号
	unsigned short m_dataNumber;

	//true收到从站信息正确帧，false表示已经启动或者启动线程时值
	bool m_isRecvRemoteInfo;
	const unsigned int m_sendTime;//发送数据间隔时间s
	const int m_timeout;//接收数据超时时间

	MasterManager* m_masterManager;//站内通讯规约兼容103协议主站管理类

	UdpNode* m_udp;//UDP指针
	int m_networkA;//A网段通讯状况
	int m_networkB;//B网段通讯状况
};

#endif /* UDPMANANGERCLIENT_H_ */
