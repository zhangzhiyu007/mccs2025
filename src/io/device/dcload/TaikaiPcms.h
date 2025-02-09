/*
 * TaikaiPcms.h
 *
 *  创建日期: 2019-7-31
 *  作          者: 刘海涛
 */

#ifndef TAIKAIPCMS_H_
#define TAIKAIPCMS_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;
class TaikaiPcms
{
public:
	TaikaiPcms();
	virtual ~TaikaiPcms();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取泰开相变储热数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置泰开相变储热数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  TaiKaiPcmsRead
	 *  功能: 读取泰开相变储热数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int TaiKaiPcmsRead(Device::SlaveDev* dev);

	/*
	 *  TaiKaiPcmsPreset
	 *  功能: 设置泰开相变储热数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int TaiKaiPcmsPreset(Device::SlaveDev* dev);

protected:
private:
    Com* m_com;
	bool m_isTcp;
	TcpClient* m_tcpClient;//以太网，用于串口服务器
};

#endif /* TAIKAIPCMS_H_ */
