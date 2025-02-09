/*
 * ShWenShunLoad.h
 *
 *  创建日期: 2016-6-16
 *  作          者: 刘海涛
 */

#ifndef SHWENSHUNLOAD_H_
#define SHWENSHUNLOAD_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

class ShWenShunLoad
{
public:
	ShWenShunLoad();
	virtual ~ShWenShunLoad();

	//设置读取串口
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
	 *  ShWenShunIsLandRead
	 *  功能: 读取上海文顺孤岛负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ShWenShunIsLandRead(Device::SlaveDev* dev);

	/*
	 *  ShWenShunIsLandPreset
	 *  功能: 设置上海文顺孤岛负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ShWenShunIsLandPreset(Device::SlaveDev* dev);

	/*
	 *  ShWenShunRLCRead
	 *  功能: 读取上海文顺RLC负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ShWenShunRLCRead(Device::SlaveDev* dev);

	/*
	 *  ShWenShunRLCPreset
	 *  功能: 设置上海文顺RLC负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ShWenShunRLCPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* SHWENSHUNLOAD_H_ */
