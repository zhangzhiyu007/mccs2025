/*
 * PvAcLoad.h
 *
 *  Created on: 2013-10-21
 *      Author: liuhaitao
 */

#ifndef PVACLOAD_H_
#define PVACLOAD_H_
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

class PvAcLoad
{
public:
	PvAcLoad();
	virtual ~PvAcLoad();

	//设置串口
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取交流负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置交流负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  PvRLCRead
	 *  功能: 读取菊水皇家RLC负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PvRLCRead(Device::SlaveDev* dev);

	/*
	 *  PvRLCPreset
	 *  功能: 设置菊水皇家RLC负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PvRLCPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* PVACLOAD_H_ */
