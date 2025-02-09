/*
 * CharingStation.h
 *
 *  创建日期: 2016-4-15
 *  作          者: 刘海涛
 */

#ifndef CHARINGSTATION_H_
#define CHARINGSTATION_H_

#include "SchtiTansChargStation.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

class CharingStation
{
public:
	CharingStation();
	virtual ~CharingStation();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取充电桩数据
	 *  输入：
	 *  dev    		:Device::SlaveDev* 从设备指针
	 *  recvData	:String 读取数据
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev, String recvData);

	/*
	 *  Preset
	 *  功能: 设置充电桩命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  SchtiTansChargRead
	 *  功能: 读取四川泰坦豪特充电桩数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  recvData	:String 读取数据
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SchtiTansChargRead(Device::SlaveDev* dev, String recvData);

	/*
	 *  SchtiTansChargPreset
	 *  功能: 设置四川泰坦豪特充电桩命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SchtiTansChargPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
	SchtiTansChargStation m_charg;
};

#endif /* CHARINGSTATION_H_ */
