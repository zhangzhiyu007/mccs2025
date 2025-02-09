/*
 * ParwaImpedanceSimulator.h
 *
 *  创建日期: 2017-12-10
 *  作          者: 刘海涛
 */

#ifndef PARWAIMPEDANCESIMULATOR_H_
#define PARWAIMPEDANCESIMULATOR_H_
#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class Com;
class TcpClient;
class ParwaImpedanceSimulator
{
public:
	ParwaImpedanceSimulator();
	virtual ~ParwaImpedanceSimulator();

	//设置串口
	void SetCom(Com* com);
	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取线缆阻抗数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置线缆阻抗命令
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  ParwaImpSimRead
	 *  功能: 读取线缆阻抗数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ParwaImpSimRead(Device::SlaveDev* dev);

	/*
	 *  ParwaImpSimPreset
	 *  功能: 设置线缆阻抗数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ParwaImpSimPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* PARWAIMPEDANCESIMULATOR_H_ */
