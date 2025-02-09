/*
 * ZheDaBatt.h
 *
 *  创建日期: 2017-12-11
 *  作          者: 刘海涛
 */

#ifndef ZHEDABATT_H_
#define ZHEDABATT_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

class Com;
class TcpClient;

class ZheDaBatt
{
public:
	ZheDaBatt();
	virtual ~ZheDaBatt();

	//设置串口指针
	void setCom(Com* com);
	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取电池数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置电池命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  BattRead
	 *  功能: 读取浙大电池数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int BattRead(Device::SlaveDev* dev);

	/*
	 *  BattPreset
	 *  功能: 设置浙大电池命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int BattPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* ZHEDABATT_H_ */
