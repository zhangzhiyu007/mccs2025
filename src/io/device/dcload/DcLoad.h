/*
 * DcLoad.h
 *
 *  Created on: 2013-10-11
 *      Author: liuhaitao
 */

#ifndef DCLOAD_H_
#define DCLOAD_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class DcLoad
{
public:
	DcLoad();
	virtual ~DcLoad();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取直流负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置直流负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  QunlingDcLoadRead
	 *  功能: 读取直流负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int QunlingDcLoadRead(Device::SlaveDev* dev);

	/*
	 *  QunlingDcLoadPreset
	 *  功能: 设置直流负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int QunlingDcLoadPreset(Device::SlaveDev* dev);

	/*
	 *  ChromaDcLoadRead
	 *  功能: 读取Chroma直流负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ChromaDcLoadRead(Device::SlaveDev* dev);

	/*
	 *  ChromaDcLoadPreset
	 *  功能: 设置Chroma直流负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ChromaDcLoadPreset(Device::SlaveDev* dev);

	/*
	 *  GreeKongtiaoRead
	 *  功能: 读取格力空调数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GreeKongtiaoRead(Device::SlaveDev* dev);

	/*
	 *  TaikaiPcmsRead
	 *  功能: 读取泰开相变储热数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int TaikaiPcmsRead(Device::SlaveDev* dev);

	/*
	 *  TaikaiPcmsPreset
	 *  功能: 设置泰开相变储热命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int TaikaiPcmsPreset(Device::SlaveDev* dev);
	
	int EADCLoadRead(Device::SlaveDev* dev);

	int EADCLoadPreset(Device::SlaveDev* dev);


	int ZJNCLoadRead(Device::SlaveDev* dev);

	int ZJNCLoadPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
	bool m_isTcp;
};

#endif /* DCLOAD_H_ */
