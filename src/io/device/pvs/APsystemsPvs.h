/*
 * APsystemsPvs.h
 *
 *  创建日期: 2016-8-12
 *  作          者: 刘海涛
 */

#ifndef APSYSTEMSPVS_H_
#define APSYSTEMSPVS_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../util/Util.h"

class Com;
class TcpClient;

class APsystemsPvs
{
public:
	APsystemsPvs();
	virtual ~APsystemsPvs();

	//设置串口
	void SetCom(Com* com);
	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);
	/*
	 *  Read
	 *  功能: 读取PVS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置PVS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  APsystemsPvsRead
	 *  功能: 读取APsystemsPVS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int APsystemsPvsRead(Device::SlaveDev* dev);

	/*
	 *  APsystemsPreset
	 *  功能: 设置APsystemsPVS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int APsystemsPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* APSYSTEMSPVS_H_ */
