/*
 * PVSimulator.h
 *
 *  Created on: 2013-10-10
 *      Author: liuhaitao
 */

#ifndef PVSIMULATOR_H_
#define PVSIMULATOR_H_

#include "../../../db/Device.h"

class Com;
class TcpClient;

/*名称：PVSimulator
 *描述：用于光伏电池模拟器控制与数据采集
 *用法：
 *创建人：lht
 *创建日期：2013-10-10
 */
class PVSimulator
{
public:
	PVSimulator();
	virtual ~PVSimulator();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取光伏电池模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置光伏电池模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

	void SetIsTcp(bool IsTcp);

protected:
private:

	int N35200Read(Device::SlaveDev* dev);
	int N35200Preset(Device::SlaveDev* dev);

	/*
	 *  JSHJPVSimulatorRead
	 *  功能: 读取菊水皇家光伏电池模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int JSHJPVSimulatorRead(Device::SlaveDev* dev);

	/*
	 *  JSHJPVSimulatorPreset
	 *  功能: 设置菊水皇家光伏电池模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int JSHJPVSimulatorPreset(Device::SlaveDev* dev);

	/*
	 *  ChromaSimulatorRead
	 *  功能: 读取chroma光伏电池模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ChromaSimulatorRead(Device::SlaveDev* dev);

	/*
	 *  ChromaSimulatorPreset
	 *  功能: 设置chroma光伏电池模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ChromaSimulatorPreset(Device::SlaveDev* dev);

	int ANSimulatorRead(Device::SlaveDev* dev);
	int ANSimulatorPreset(Device::SlaveDev* dev);
	
	int ANvhSimultorRead(Device::SlaveDev* dev);
	int ANvhSimultorPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	Com* m_com;
	TcpClient* m_tcpClient;
	bool m_isTcp;
};

#endif /* PVSIMULATOR_H_ */
