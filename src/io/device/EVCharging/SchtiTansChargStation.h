/*
 * SchtiTansChargStation.h
 *
 *  创建日期: 2016-4-15
 *  作          者: 刘海涛
 */

#ifndef SCHTITANSCHARGSTATION_H_
#define SCHTITANSCHARGSTATION_H_

#include "../../../util/Util.h"
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

class SchtiTansChargStation
{
public:
	SchtiTansChargStation();
	virtual ~SchtiTansChargStation();

	//设置串口
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取充电桩数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
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
	 *  TczJRead
	 *  功能: 读取四川泰坦豪特TCZ-J系列充电桩数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  recvData	:String 读取数据
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int TczJRead(Device::SlaveDev* dev, String recvData);

	/*
	 *  TczJPreset
	 *  功能: 设置四川泰坦豪特TCZ-J系列充电桩命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int TczJPreset(Device::SlaveDev* dev);

	/*
	 *  UnpackData
	 *  功能: 解析收到的数据
	 *  输入：
	 *  recvData	:String 读取数据
	 *  id			:int& 数据帧ID
	 *  data		:String& 数据
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int UnpackData(String recvData, int& id, String& data);

	/*
	 *  SendData
	 *  功能: 发送数据
	 *  输入：
	 *  sendData	:String 发送数据
	 *  id			:int 数据帧ID
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SendData(int id, String sendData);
	//变量
public:
protected:
private:
	Com* m_com;
	TcpClient* m_tcp;

	//计算时间差
	StatTimer m_startTimer;
};

#endif /* SCHTITANSCHARGSTATION_H_ */
