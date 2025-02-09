/*
 * PowerDetection.h
 *
 *  Created on: 2014-3-13
 *      Author: liuhaitao
 */

#ifndef POWERDETECTION_H_
#define POWERDETECTION_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class TcpClient;
class Com;

class PowerDetection
{
public:
	PowerDetection();
	virtual ~PowerDetection();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	//设置串口指针
	void SetCom(Com* com);

	/*
	 *  Read
	 *  功能: 读取读取电能质量监测仪数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);
protected:
private:
	/*
	 *  APMD730Read
	 *  功能: 读取安科瑞电能质量监测仪数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int APMD730Read(Device::SlaveDev* dev);

	/*
	 *  APM810Read
	 *  功能: 读取安科瑞电能质量监测仪数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int APM810Read(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	TcpClient* m_tcpClient;//以太网
	Com* m_com;//串口指针
	bool m_isTcp;
};

#endif /* POWERDETECTION_H_ */
