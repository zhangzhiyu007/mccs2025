/*
 * EcmHFIPcs.h
 *
 *  Created on: 2019-4-24
 *      Author: zl
 */

#ifndef ECMHFIPCS_H_
#define ECMHFIPCS_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class EcmHFIPcs {
public:
	EcmHFIPcs();
	virtual ~EcmHFIPcs();

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
	 *  EcmPcsRead
	 *  功能: 读取艾科迈PCS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int EcmHFIPcsRead(Device::SlaveDev* dev);

	/*
	 *  EcmPcsPreset
	 *  功能: 设置艾科迈PCS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int EcmHFIPcsPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	bool m_isTcp;
	TcpClient* m_tcpClient;//以太网，用于串口服务器
	int m_state;
	int m_mode;
};

#endif /* ECMPCS_H_ */
