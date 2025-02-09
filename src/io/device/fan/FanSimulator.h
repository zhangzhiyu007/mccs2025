/*
 * FanSimulator.h
 *
 *  Created on: 2014-2-23
 *      Author: liuhaitao
 */

#ifndef FANSIMULATOR_H_
#define FANSIMULATOR_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class FanSimulator
{
public:
	FanSimulator();
	virtual ~FanSimulator();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取风机模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置风机模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  EpciFanRead
	 *  功能: 读取中电建投风机模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int EpciFanRead(Device::SlaveDev* dev);

	/*
	 *  EpciFanPreset
	 *  功能: 设置中电建投风机模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int EpciFanPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
};

#endif /* FANSIMULATOR_H_ */
