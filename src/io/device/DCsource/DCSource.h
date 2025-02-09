/*
 * DCSource.h
 *
 *  Created on: 2014-1-15
 *      Author: liuhaitao
 */

#ifndef DCSOURCE_H_
#define DCSOURCE_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class DCSource
{
public:
	DCSource();
	virtual ~DCSource();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取直流电源数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置直流电源命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  ApcDcSourceRead
	 *  功能: 读取艾普斯直流电源数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ApcDcSourceRead(Device::SlaveDev* dev);

	/*
	 *  ApcDcSourcePreset
	 *  功能: 设置艾普斯直流电源命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ApcDcSourcePreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
};

#endif /* DCSOURCE_H_ */
