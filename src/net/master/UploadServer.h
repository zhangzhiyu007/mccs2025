/*
 * UploadServer.h
 *
 *   创建日期: 2013-4-1
 *   作         者: mzh
 */

#ifndef UPLOADSERVER_H_
#define UPLOADSERVER_H_
#include "../../util/Util.h"
#include "../../etc/socket/Thread.h"
#include "../../etc/socket/TcpClient.h"
#include "../Iec103.h"

class MasterManager;
class UdpNode;
/*名称:UploadServer
 *描述:站内通讯规约接收从站UDP主动上传数据.
 *用法:
 *创建人:mzh
 *创建日期:2013-4-1
 */
class UploadServer :public Thread
{
	//方法
public:
	UploadServer();
	virtual ~UploadServer();

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
	 *  RecvInfo
	 *  功能:接收UDP主站广播信息包，收到广播信息后立即发送确认帧，
	 *  确认帧按照原来帧复制发送，sn改变，其余不变。
	 *  输入：
	 *  udp    :[in]const UdpNode* UDP指针，不存在着指针为NULL
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
    int RecvInfo(UdpNode* udp);

    //变量
public:
protected:
private:
	const int m_timeout;//接收数据超时时间
	MasterManager* m_masterManager;//站内通讯规约兼容103协议主站管理类

	UdpNode* m_udp;//UDP指针
	int m_networkA;//A网段通讯状况
	int m_networkB;//B网段通讯状况
};

#endif /* UPLOADSERVER_H_ */
