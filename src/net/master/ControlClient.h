/*
 * ControlClient.h
 *
 *   创建日期: 2013-4-1
 *   作         者: mzh
 */

#ifndef CONTROLCLIENT_H_
#define CONTROLCLIENT_H_
#include "../../util/Util.h"
#include "../../etc/socket/Thread.h"
#include "../../etc/socket/TcpClient.h"
#include "../Iec103.h"

class TcpClient;
class MasterManager;

/*名称:ControlClient
 *描述:站内通讯规约TCP控制线程
 *用法:实现站内通讯规约下发TCP控制指令
 *创建人:mzh
 *创建日期:2013-4-1
 */
class ControlClient: public Thread
{
	//方法
public:
	ControlClient();
	virtual ~ControlClient();

	/*
	 *  SetMasterManager
	 *  功能:保存主站管理类指针
	 *  输入：
	 *  manager    :[in]MasterManager* 主站管理类指针
	 *  输出：返回Device*指针，
	 */
	void SetMasterManager(MasterManager* manager);

	/*
	 *  SetSlaveConfig
	 *  功能:保存从站管理类指针
	 *  输入：
	 *  slaveConfig    :[in]SlaveConfig* 主站管理类指针
	 *  输出：返回设备指针，
	 */
	void SetSlaveConfig(SlaveConfig* slaveConfig);
protected:
private:
	/*
	 *  Run
	 *  功能:线程执行函数
	 *  输入：无
	 *  输出：无
	 */
    virtual void Run();

    //判断A/B网段TCP是否打开
    bool IsTcpOpened();

	/*
	 *  OpenedTcp
	 *  功能:打开TCP控制端口
	 *  输入：无
	 *  输出：true成功/false失败
	 */
    bool OpenedTcp();

    bool OpenedTcp(TcpClient** tcp,Address& addr);

	/*
	 *  CloseTcp
	 *  功能:关闭TCP控制端口
	 *  输入：无
	 *  输出：无
	 */
    void CloseTcp();

	/*
	 *  SendCtrlData
	 *  功能:发送TCP控制信息
	 *  输入：
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int SendCtrlData();

	/*
	 *  SendTimeSync
	 *  功能:发送TCP时间同步信息
	 *  输入：
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int SendTimeSync();

	/*
	 *  RecvInfo
	 *  功能:等接收TCP信息包，
	 *  输入：
	 *  tcp    :[in]const TcpClient* TCP指针，不存在着指针为NULL
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
    int RecvInfo(TcpClient* tcp);

	/*
	 *  GetControlData
	 *  功能:返回需要控制数据,每次不超过128个数据,用完后调用ReleaseControlData释放内存
	 *  输入：
	 *  array    :[out]PtrArray& 保存RealData类型指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
    int GetControlData(PtrArray& array);

	/*
	 *  ReleaseControlData
	 *  功能:将GetControlData得到的数据内存释放
	 *  输入：array  :[in]PtrArray& GetControlData得到的数组
	 *  输出：无
	 */
    void ReleaseControlData(PtrArray& array);

	/*
	 *  PackHead
	 *  功能:封装包头
	 *  输入：
	 *  head     :[out]Iec103::PacketHead& 封装包头
	 *  输出：
	 */
    void PackHead(Iec103::PacketHead& head);

    //变量
public:
protected:
private:
    MasterManager* m_masterManager;//站内通讯规约兼容103协议主站管理类
	SlaveConfig* m_slaveConfig;//被连接的从设备

	//TCP客户端
	TcpClient* m_tcpA;//A网段TCP客户端
	TcpClient* m_tcpB;//B网段TCP客户端

	//当前读取数据长度
	int m_regLength;//用于分包下发数据时,保存已经下发数据长度
	const int m_failNumber;//下发数据失败最大次数
	const int m_timeSync;//时间同步周期
	const int m_timeout;//超时时间,默认500ms

	//主站不数据编号
	unsigned short m_dataNumber;

	//断开连接后重新连接时间
	static const int m_networkLink = 6;//断开连接后，1分钟重连1次
	int m_networkALink;//A网重新连接
	int m_networkBLink;//B网重新连接

};

#endif /* CONTROLCLIENT_H_ */
