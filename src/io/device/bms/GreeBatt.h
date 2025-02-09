/*
 * GreeBatt.h
 *
 *  创建日期: 2019-7-31
 *  作          者: 刘海涛
 */

#ifndef GREEBATT_H_
#define GREEBATT_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class GreeBatt
{
public:
	GreeBatt();
	virtual ~GreeBatt();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取格力储能数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置格力储能数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  GreeBattRead
	 *  功能: 读取格林储能数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GreeBattRead(Device::SlaveDev* dev);

	/*
	 *  GreeBattPreset
	 *  功能: 设置格林储能数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GreeBattPreset(Device::SlaveDev* dev);

protected:
private:
    Com* m_com;
	bool m_isTcp;
	TcpClient* m_tcpClient;//以太网，用于串口服务器
};

#endif /* GREEBATT_H_ */
