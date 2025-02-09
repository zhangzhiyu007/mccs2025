/*
 * FaultSimultor.h
 *
 *  创建日期: 2017-12-10
 *  作          者: 刘海涛
 */

#ifndef FAULTSIMULTOR_H_
#define FAULTSIMULTOR_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class FaultSimultor
{
public:
	FaultSimultor();
	virtual ~FaultSimultor();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取故障模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置故障模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);
protected:
private:
	/*
	 *  ParwaFaultSimRead
	 *  功能: 读取故障模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ParwaFaultSimRead(Device::SlaveDev* dev);

	/*
	 *  ParwaFaultSimPreset
	 *  功能: 设置故障模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ParwaFaultSimPreset(Device::SlaveDev* dev);
	
	int DCXG500Read(Device::SlaveDev* dev);
	int DCXG500Preset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
};

#endif /* FAULTSIMULTOR_H_ */
