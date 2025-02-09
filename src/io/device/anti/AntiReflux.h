/*
 * AntiReflux.h
 *
 *  Created on: 2014-3-7
 *      Author: liuhaitao
 */

#ifndef ANTIREFLUX_H_
#define ANTIREFLUX_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class AntiReflux
{
public:
	AntiReflux();
	virtual ~AntiReflux();

	//设置串口
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取防逆流采集箱数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  EpciRead
	 *  功能: 读取中电建投防逆流采集箱数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int EpciRead(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcpClient;//以太网
};

#endif /* ANTIREFLUX_H_ */
