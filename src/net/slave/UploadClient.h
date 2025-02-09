/*
 * UploadClient.h
 *
 *   创建日期: 2013-4-5
 *   作         者: mzh
 */

#ifndef UPLOADCLIENT_H_
#define UPLOADCLIENT_H_
#include "../../util/Util.h"
#include "../../etc/socket/Thread.h"
#include "../../etc/socket/UdpNode.h"
#include "../Iec103.h"

class SlaveManager;
/*名称:UploadClient
 *描述:站内通讯规约从站UDP主动上传数据.
 *用法:
 *创建人:mzh
 *创建日期:2013-4-5
 */
class UploadClient: public Thread
{
	//方法
public:
	UploadClient();
	virtual ~UploadClient();

	/*
	 *  SetMasterManager
	 *  功能:保存主站管理类指针
	 *  输入：
	 *  manager    :[in]MasterManager* 主站管理类指针
	 *  输出：返回Device*指针，
	 */
	void SetMasterManager(SlaveManager* manager);
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
	 *  SendRealData
	 *  功能:发送实时数据
	 *  输入：
	 *  data   :[in]PtrArray& 保存实时数据指针RealData*
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int SendRealData(PtrArray& data);

	/*
	 *  SendRealData
	 *  功能:发送实时数据,采用非连续数据下发
	 *  输入：
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int SendRealData();

	/*
	 *  PackHead
	 *  功能:封装包头
	 *  输入：
	 *  head     :[out]Iec103::PacketHead& 封装包头
	 *  输出：
	 */
    void PackHead(Iec103::PacketHead& head);

	/*
	 *  GetRealData
	 *  功能:返回需要控制数据,每次不超过128个数据,
	 *  用完后调用ReleaseControlData释放内存
	 *  输入：
	 *  start   :[in]const int& 起始地址
	 *  len     :[in]const int& 寄存器长度
	 *  输出：bool true/false
	 */
    bool GetRealData(PtrArray& data,const int& start,const int& len);

	/*
	 *  ReleaseRealData
	 *  功能:将GetControlData得到的数据内存释放
	 *  输入：array  :[in]PtrArray& GetControlData得到的数组
	 *  输出：无
	 */
    void ReleaseRealData(PtrArray& array);

	/*
	 *  GetSlaveDevOwn
	 *  功能:读取属于自己的从设备
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int GetSlaveDevOwn();

    //变量
public:
protected:
private:
	//发送数据帧，数据编号
	unsigned short m_dataNumber;
	const unsigned int m_sendTime;//发送周期ms

	SlaveManager* m_slaveManager;//站内通讯规约兼容103协议主站管理类
	bool m_isMaster;//是否是集中控制器或者组合控制器

	UdpNode* m_udp;//UDP指针
	int m_networkA;//A网段通讯状况
	int m_networkB;//B网段通讯状况

	//读取属于自己的从设备
	PtrArray m_slaveDevOwn;
};

#endif /* UPLOADCLIENT_H_ */
