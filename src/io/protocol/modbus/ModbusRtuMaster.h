/*
 * ModbusRtuMaster.h
 *
 *   创建日期: 2013-4-11
 *   作         者: mzh
 */

#ifndef MODBUSRTUMASTER_H_
#define MODBUSRTUMASTER_H_
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

class Com;
class TcpClient;

/*名称:ModbusRtuMaster
 *描述:ModbusRtuz主站协议
 *用法: 实现modbus RTU命令包括:1/2/3/4/5/6/15/16号命令
 *创建人:mzh
 *创建日期:2013-4-11
 */
class ModbusRtuMaster
{
	//方法
public:
	ModbusRtuMaster();
	virtual ~ModbusRtuMaster();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//设置/返回串口指针
	void SetCom(Com* com);
	Com* GetCom();

	//设置/返回网口指针
	void SetTcpClient(TcpClient* tcpClient);
	TcpClient* GetTcpClient();

	//设置返回查询失败的重复查询次数
	void SetRetries(int retries);
	int GetRetries()const;

	//设置延时时间
	void SetDelayTime(int delayTime);

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
	 * 名称: ReadInputStatus
	 * 功能: 2号命令，读RCL输入状态量（开关量）
	 * 输入:
	 * values      :[out]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 30005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadInputStatusRcl(UshortArray& values, int addr, int start,
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
	 * 名称: ReadOutputFloatRegisters
	 * 功能: 3号命令,读取浮点型输出寄存器值
	 * 输入:
	 * values      :[out]FloatArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 40005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadOutputFloatRegisters(FloatArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ReadRLCFloatRegisters
	 * 功能: 3号命令,读取RLC输出寄存器值
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
	 * 名称: ReadOutputRegisters
	 * 功能: 0x4c号命令,读取直流稳定电源寄存器值
	 * 输入:
	 * values      :[out]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 40005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadDCSourceRegisters(UshortArray& values, int addr, int start,
			int length);

	/*
	 * 名称: ReadBipolarRegisters
	 * 功能: 3号命令,读取双极性AC/DC寄存器值
	 * 输入:
	 * values      :[out]String&  如果没有错误情况下读取的数据，UShort类型
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始 如: 40005 -> nDaStart=4
	 * length      :[in]int  读寄存器数目
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadBipolarRegisters(String& values, int addr, int start,
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
	 * 名称: ForceMultipleCoils
	 * 功能: 15号命令,设置多个开关量
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始
	 * length      :[in]int  读寄存器数目
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
	 * values      :[in]FloatArray&  如果没有错误情况下读取的数据，float类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetMultipleRegisters(int addr, int start, int length,UshortArray& values);

	/*
	 * 名称: PresetMultipleFloatRegisters
	 * 功能: 16号命令,设置浮点型寄存器
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始
	 * length      :[in]int  读寄存器数目
	 * values      :[in]FloatArray&  如果没有错误情况下读取的数据，float类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetMultipleFloatRegisters(int addr, int start, int length,FloatArray& values);

	/*
	 * 名称: PresetRLCFloatRegisters
	 * 功能: 16号命令,设置RLC寄存器
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始
	 * length      :[in]int  读寄存器数目
	 * values      :[in]FloatArray&  如果没有错误情况下读取的数据，float类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetRLCFloatRegisters(int addr, int start, int length,FloatArray& values);

	/*
	 * 名称: PresetDCSourceRegisters
	 * 功能: 设置直流稳定电源
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * function	   :[in]int 功能码
	 * length      :[in]int  读寄存器数目
	 * values      :[in]UshortArray&  如果没有错误情况下读取的数据，UShort类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetDCSourceRegisters(int addr, int function,int length, UshortArray& values);

	/*
	 * 名称: PresetBipolarRegisters
	 * 功能: 16号命令,设置双极性AC/DC寄存器
	 * 输入:
	 * addr        :[in]int  modbus设备地址
	 * start       :[in]int  起始地址,地址从0开始
	 * length      :[in]int  读寄存器数目
	 * values      :[in]FloatArray&  如果没有错误情况下读取的数据，float类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PresetBipolarRegisters(int addr, int start, int length,String& values);
protected:
private:

	/*
	 * 名称: TxRxMsg
	 * 功能: 用于在读写数据，先发送查询数据，然后再读取数据，
	 *       不添加任何校验等
	 * 输入:
	 * dataRead      :[out]String& 读取的数据
	 * dataWrite     :[in]String& 写入的数据
	 * len			 :[in] int 期望读取的数据的长度
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int TxRxMsg(String& dataRead, String& dataWrite, int len);

	//变量
public:
protected:
private:
	Com* m_com;
	TcpClient* m_tcpClient;
	bool m_isTcp;

	//查询失败，重复查询次数
	int m_retries;
	int m_delayTime;//查询出错后，延时该时间，再进行下次查询，ms
};

#endif /* MODBUSRTUMASTER_H_ */
