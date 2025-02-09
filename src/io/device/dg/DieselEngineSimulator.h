/*
 * DieselEngineSimulator.h
 *
 *  Created on: 2014-2-24
 *      Author: liuhaitao
 */

#ifndef DIESELENGINESIMULATOR_H_
#define DIESELENGINESIMULATOR_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class DieselEngineSimulator
{
public:
	DieselEngineSimulator();
	virtual ~DieselEngineSimulator();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取柴油模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置柴油模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  EpciDieselRead
	 *  功能: 读取中电建投柴油模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int EpciDieselRead(Device::SlaveDev* dev);

	/*
	 *  EpciDieselPreset
	 *  功能: 设置中电建投柴油模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int EpciDieselPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
};

#endif /* DIESELENGINESIMULATOR_H_ */
