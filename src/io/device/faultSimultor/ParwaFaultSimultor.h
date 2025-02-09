/*
 * ParwaFaultSimultor.h
 *
 *  创建日期: 2017-12-10
 *  作          者: 刘海涛
 */

#ifndef PARWAFAULTSIMULTOR_H_
#define PARWAFAULTSIMULTOR_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class Com;
class TcpClient;

class ParwaFaultSimultor
{
public:
	ParwaFaultSimultor();
	virtual ~ParwaFaultSimultor();

	//设置串口
	void SetCom(Com* com);
	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取故障模拟器数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置故障模拟器命令
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  FaultSimRead
	 *  功能: 读取故障模拟器数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int FaultSimRead(Device::SlaveDev* dev);

	/*
	 *  FaultSimPreset
	 *  功能: 设置故障模拟器数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int FaultSimPreset(Device::SlaveDev* dev);

	/*
	 *  PrawaRs485FaultSimRead
	 *  功能: 读取菊水皇家故障模拟器数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PrawaRs485FaultSimRead(Device::SlaveDev* dev);

	/*
	 *  PrawaRs485FaultSimPreset
	 *  功能: 设置菊水皇家故障模拟器数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PrawaRs485FaultSimPreset(Device::SlaveDev* dev);

	/*
	 *  PrawaTcpFaultSimRead
	 *  功能: 读取菊水皇家故障模拟器数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PrawaTcpFaultSimRead(Device::SlaveDev* dev);

	/*
	 *  PrawaTcpFaultSimPreset
	 *  功能: 设置菊水皇家故障模拟器数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PrawaTcpFaultSimPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* PARWAFAULTSIMULTOR_H_ */
