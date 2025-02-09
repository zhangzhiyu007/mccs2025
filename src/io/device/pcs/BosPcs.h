/*
 * BasPcs.h
 *
 *  Created on: 2015-5-20
 *      Author: liuhaitao
 */

#ifndef BosPcs_H_
#define BosPcs_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"

class TcpClient;

class BosPcs
{
public:
	BosPcs();
	virtual ~BosPcs();

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取PCS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置PCS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  Bos10Read
	 *  功能: 读取博奥斯10kWPCS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Bos10Read(Device::SlaveDev* dev);

	/*
	 *  Bos10Preset
	 *  功能: 设置博奥斯10kWPCS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Bos10Preset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	TcpClient* m_tcp;
};

#endif /* BosPcs_H_ */
