/*
 * DiModle.h
 *
 *  Created on: 2014-9-16
 *      Author: liuhaitao
 */

#ifndef DIDOMODLE_H_
#define DIDOMODLE_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class Com;
class TcpClient;

class DiDoModle
{
public:
	DiDoModle();
	virtual ~DiDoModle();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取DI模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置DO模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

private:
	/*
	 *  HC209Read
	 *  功能: 读取开关模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int HC209Read(Device::SlaveDev* dev);

	/*
	 *  HC201Read
	 *  功能: 读取开关模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int HC201Read(Device::SlaveDev* dev);

	/*
	 *  HC202Preset
	 *  功能: 设置DO模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HC202Preset(Device::SlaveDev* dev);

	/*
	 *  ConfCabinet1Preset
	 *  功能: 设置组态柜1数据017
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ConfCabinet1Preset(Device::SlaveDev* dev);

	/*
	 *  ConfCabinet2Preset
	 *  功能: 设置组态柜2数据018
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ConfCabinet2Preset(Device::SlaveDev* dev);

	/*
	 *  ConfCabinet3Preset
	 *  功能: 设置组态柜3数据019
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ConfCabinet3Preset(Device::SlaveDev* dev);

	/*
	 *  ConfCabinet4Preset
	 *  功能: 设置组态柜4数据020
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ConfCabinet4Preset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	bool m_isTcp;
	TcpClient* m_tcpClient;//以太网，用于串口服务器

};

#endif /* DIDOMODLE_H_ */
