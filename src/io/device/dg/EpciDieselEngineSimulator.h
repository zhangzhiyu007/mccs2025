/*
 * EpciDieselEngineSimulator.h
 *
 *  Created on: 2014-2-24
 *      Author: liuhaitao
 */

#ifndef EPCIDIESELENGINESIMULATOR_H_
#define EPCIDIESELENGINESIMULATOR_H_
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

class EpciDieselEngineSimulator
{
public:
	EpciDieselEngineSimulator();
	virtual ~EpciDieselEngineSimulator();

	//设置串口
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
	 *  Dgs4030Read
	 *  功能: 读取DGS-4030数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Dgs4030Read(Device::SlaveDev* dev);

	/*
	 *  Dgs4030Preset
	 *  功能: 设置DGS-4030命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Dgs4030Preset(Device::SlaveDev* dev);

	/*
	 *  SetInt
	 *  功能: 设置只有整形的命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  start	:int 设备寄存器地址
	 *  client	:ModbusTcpClient modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetInt(int addr,int regStart,int start,ModbusTcpClient client);

	/*
	 *  SetFloat10
	 *  功能: 设置只有float的命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  start	:int 设备寄存器地址
	 *  client	:ModbusTcpClient modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetFloat10(int addr,int regStart,int start,ModbusTcpClient client);

	/*
	 *  SetFloatRange10
	 *  功能: 设置只有float并且设置值有范围的命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  start	:int 设备寄存器地址
	 *  client	:ModbusTcpClient modbus client实例
	 *  minValue:int 范围小值
	 *  maxValue:int 范围大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetFloatRange10(int addr,int regStart,int start,ModbusTcpClient client,int minValue,int maxValue);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* EPCIDIESELENGINESIMULATOR_H_ */
