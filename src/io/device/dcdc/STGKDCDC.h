/*
 * STGKDCDC.h
 *
 *  Created on: 2015-12-4
 *      Author: liuhaitao
 */

#ifndef STGKDCDC_H_
#define STGKDCDC_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class STGKDCDC
{
public:
	STGKDCDC();
	virtual ~STGKDCDC();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取DCDC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置DCDC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);
protected:
private:
	/*
	 *  STGkPvDcDcRead
	 *  功能: 读取盛通高科光伏DCDC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int STGkPvDcDcRead(Device::SlaveDev* dev);

	/*
	 *  STGkPvDcDcPreset
	 *  功能: 设置盛通高科光伏DCDC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int STGkPvDcDcPreset(Device::SlaveDev* dev);

	/*
	 *  STGkBattDcDcRead
	 *  功能: 读取盛通高科储能DCDC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int STGkBattDcDcRead(Device::SlaveDev* dev);

	/*
	 *  STGkBattDcDcPreset
	 *  功能: 设置盛通高科储能DCDC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int STGkBattDcDcPreset(Device::SlaveDev* dev);
	//变量
public:
protected:
private:
    Com* m_com;
	bool m_isTcp;
	TcpClient* m_tcpClient;//以太网，用于串口服务器
};

#endif /* STGKDCDC_H_ */
