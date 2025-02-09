/*
 * ModbusTcpClient.h
 *
 *   创建日期: 2013-4-11
 *   作         者: mzh
 */

#ifndef MODBUSTCPCLIENT_H_
#define MODBUSTCPCLIENT_H_
#include "../../../util/Util.h"

class TcpClient;

/*名称:
 *描述:
 *用法:
 *创建人:mzh
 *创建日期:2013-4-11
 */
class ModbusTcpClient
{
	//方法
public:
	ModbusTcpClient();
	virtual ~ModbusTcpClient();

	//设置/返回网口指针
	void SetTcpClient(TcpClient* tcpClient);
	TcpClient* GetTcpClient();

	//设置返回查询失败的重复查询次数
	void SetRetries(int retries);
	int GetRetries();

	//设置返回查询延时
	void SetDelay(int delay);
	int GetDelay();

	/*
	 * 名称: ReadOutputStatus
	 * 功能: 1号命令，读输出状态量（开关量）
	 * 输入:
	 * values      :[out]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 30005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadOutputStatus(UshortArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ReadInputStatus
	 * 功能: 2号命令，读输入状态量（开关量）
	 * 输入:
	 * values      :[out]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 30005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadInputStatus(UshortArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ReadOutputRegisters
	 * 功能: 3号命令,读取输出寄存器值
	 * 输入:
	 * values      :[out]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 40005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadOutputRegisters(UshortArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ReadRs485OutputRegisters
	 * 功能: 3号命令,读取串口服务器输出寄存器值
	 * 输入:
	 * values      :[out]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 40005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadRs485OutputRegisters(UshortArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ReadRLCFloatRegisters
	 * 功能: 3号命令,通过串口服务器读取RLC输出寄存器值
	 * 输入:
	 * values      :[out]FloatArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 40005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadRLCFloatRegisters(FloatArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ReadInputRegisters
	 * 功能: 4号命令,读取输入寄存器值
	 * 输入:
	 * values      :[out]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 30005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadInputRegisters(UshortArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ReadRS485InputRegisters
	 * 功能: 4号命令,读取输入寄存器值
	 * 输入:
	 * values      :[out]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 30005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadRS485InputRegisters(UshortArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ForceSingleCoil
	 * 功能: 5号命令,设置单个开关量
	 * 输入:
	 * addr        :[in]int   modbus设备地址
	 * regAddr     :[in]int   寄存器地址,地址从0开始
	 * value       :[in]bool  写入开关量值
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ForceSingleCoil(int addr, int regAddr, bool value);

	/*
	 * 名称: ForceRS485SingleCoil
	 * 功能: 5号命令,通过串口服务器设置单个开关量
	 * 输入:
	 * addr        :[in]int   modbus设备地址
	 * regAddr     :[in]int   寄存器地址,地址从0开始
	 * value       :[in]bool  写入开关量值
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ForceRS485SingleCoil(int addr, int regAddr, bool value);

	/*
	 * 名称: PresetSingleRegister
	 * 功能: 6号命令,设置单个寄存器
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * regAddr     :[in]int  寄存器地址,地址从0开始
	 * value       :[in]int  写入寄存器的值
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetSingleRegister(int addr, int regAddr, unsigned short value);

	/*
	 * 名称: PresetRS485SingleRegister
	 * 功能: 6号命令,通过串口服务器设置单个寄存器
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * regAddr     :[in]int  寄存器地址,地址从0开始
	 * value       :[in]int  写入寄存器的值
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetRS485SingleRegister(int addr, int regAddr, unsigned short value);

	/*
	 * 名称: ForceMultipleCoils
	 * 功能: 15号命令,设置多个开关量
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始
	 * length      :[in]int  读开关量数目
	 * values      :[in]UshortArray&  如果没有错误情况下读取的数据，UShort类型,0/1
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ForceMultipleCoils(int addr, int start, int length,UshortArray& values);

	/*
	 * 名称: PresetMultipleRegisters
	 * 功能: 16号命令,设置多个寄存器
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始
	 * length      :[in]int  读寄存器数目
	 * values      :[in]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetMultipleRegisters(int addr, int start, int length,UshortArray& values);

	/*
	 * 名称: PresetRS485MultipleRegisters
	 * 功能: 16号命令,通过串口服务器设置多个寄存器
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始
	 * length      :[in]int  读寄存器数目
	 * values      :[in]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetRS485MultipleRegisters(int addr, int start, int length,UshortArray& values);

	/*
	 * 名称: PresetRLCFloatRegisters
	 * 功能: 16号命令,设置RLC浮点型寄存器
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始
	 * length      :[in]int  读寄存器数目
	 * values      :[in]FloatArray&  如果没有错误情况下读取的数据，float类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetRLCFloatRegisters(int addr, int start, int length, FloatArray& values);

	/*
	 * 名称：ModBool
	 * 功能：C++布尔型to modbus开关量类型
	 * 输入：
	 * coil			:[in]bool bool型开关量
	 * 输出：
	 * modbus型开关量
	 */
	int ModBool(int coil);

	//PresetMultipleFloatRegisters--2024-9-5   EcmPcs 北京移动
	int PresetMultipleFloatRegisters(int addr, int start, int length,FloatArray& values);




protected:
private:

	//变量
public:
protected:
private:
	TcpClient* m_tcpClient;

	//查询失败，重复查询次数
	int m_retries;

	int m_delay;//延时时间，ms
};

#endif /* MODBUSTCPCLIENT_H_ */
