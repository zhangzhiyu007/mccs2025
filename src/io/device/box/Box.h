/*
 * Box.h
 *
 *  Created on: 2013-10-16
 *      Author: liuhaitao
 */

#ifndef BOX_H_
#define BOX_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class Box
{
public:
	Box();
	virtual ~Box();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取汇流箱数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  KbtBoxRead
	 *  功能: 读取汇流箱数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int KbtBoxRead(Device::SlaveDev* dev);

	/*
	 *  AcrelBoxRead
	 *  功能: 读取安科瑞汇流箱数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcrelBoxRead(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
};

#endif /* BOX_H_ */
