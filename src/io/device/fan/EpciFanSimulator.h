/*
 * EpciFanSimulator.h
 *
 *  Created on: 2014-2-23
 *      Author: liuhaitao
 */

#ifndef EPCIFANSIMULATOR_H_
#define EPCIFANSIMULATOR_H_
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

class EpciFanSimulator
{
public:
	EpciFanSimulator();
	virtual ~EpciFanSimulator();

	//设置串口
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
	 *  Wes6930Read
	 *  功能: 读取WES-6930数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Wes6930Read(Device::SlaveDev* dev);

	/*
	 *  Wes6930Preset
	 *  功能: 设置WES-6930命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Wes6930Preset(Device::SlaveDev* dev);

	/*
	 *  Wind10SRead
	 *  功能: 读取Wind-10S数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Wind10SRead(Device::SlaveDev* dev);

	/*
	 *  Wind10SPreset
	 *  功能: 设置Wind-10S命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Wind10SPreset(Device::SlaveDev* dev);

	/*
	 *  PresetStart
	 *  功能: 设置启停命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetStart(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetFanType
	 *  功能: 设置风机类型
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetFanType(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetLVRTC（Low voltage ride through capability）
	 *  功能: 设置低电压穿越命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetLVRTC(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetFaultReset
	 *  功能: 设置故障复位
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetFaultReset(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetReactivePower
	 *  功能: 设置无功功率
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetReactivePower(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetCutInSpeed
	 *  功能: 设置切入风速命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetCutInSpeed(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetCutOutSpeed
	 *  功能: 设置切出风速命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetCutOutSpeed(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetRatedWindSpeed
	 *  功能: 设置额定风速命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetRatedWindSpeed(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetEROB
	 *  功能: 设置叶片半径命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetEROB(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetAirDensity
	 *  功能: 设置空气密度命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetAirDensity(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetRPC
	 *  功能: 设置风能利用率命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetRPC(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetPOutMode
	 *  功能: 设置有功功率输出方式命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetPOutMode(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetWindSpeed
	 *  功能: 设置风速命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetWindSpeed(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetGeneralCurveType
	 *  功能: 设置通用曲线类型命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetGeneralCurveType(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetGeneralCurveCycle
	 *  功能: 设置通用曲线周期命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetGeneralCurveCycle(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetGeneralAmplitude
	 *  功能: 设置通用曲线幅值命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetGeneralAmplitude(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetGeneralCurveConCycle
	 *  功能: 设置通用曲线控制周期命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetGeneralCurveConCycle(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetHisCurveAddr
	 *  功能: 设置历史曲线地址命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetHisCurveAddr(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetPowerCurOfEx
	 *  功能: 设置功率曲线执行方式命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetPowerCurOfEx(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetCurStartTime
	 *  功能: 设置曲线开始执行时间命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetCurStartTime(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  SetStartCur
	 *  功能: 设置开始曲线设置命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetStartCur(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  SetStopCur
	 *  功能: 设置结束曲线设置命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetStopCur(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetCurStartExTime
	 *  功能: 设置曲线开始执行时间设置命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetCurStartExTime(int addr,int regStart,ModbusTcpClient client);

	/*
	 *  PresetCurStopExTime
	 *  功能: 设置曲线结束执行时间设置命令
	 *  输入：
	 *  addr	:int 设备地址
	 *  regStart:int 共享内存地址
	 *  client		：ModbusTcpClient Modbus client实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetCurStopExTime(int addr,int regStart,ModbusTcpClient client);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* EPCIFANSIMULATOR_H_ */
