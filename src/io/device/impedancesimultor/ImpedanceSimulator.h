/*
 * ImpedanceSimulator.h
 *
 *  Created on: 2013-10-12
 *      Author: liuhaitao
 */

#ifndef IMPEDANCESIMULATOR_H_
#define IMPEDANCESIMULATOR_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class ImpedanceSimulator
{
public:
	ImpedanceSimulator();
	virtual ~ImpedanceSimulator();

	//保存串口指针
	void SetCom(Com* com);

	void SetIsTcp(bool IsTcp);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取阻抗模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置阻抗模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

	int DCXGRead(Device::SlaveDev* dev);
	int DCXGPreset(Device::SlaveDev* dev);
	
	int LineImSimRead(Device::SlaveDev* dev);
	int LineImSimPreset(Device::SlaveDev* dev);
protected:
private:
	/*
	 *  ParwaImSimRead
	 *  功能: 读取阻抗模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ParwaImSimRead(Device::SlaveDev* dev);

	/*
	 *  ParwaImSimPreset
	 *  功能: 设置阻抗模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ParwaImSimPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
	bool m_isTcp;
};

#endif /* IMPEDANCESIMULATOR_H_ */
