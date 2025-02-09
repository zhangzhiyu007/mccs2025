/*
 * ApfSvg.h
 *
 *  创建日期: 2016-6-20
 *  作          者: 刘海涛
 */

#ifndef APFSVG_H_
#define APFSVG_H_

#include "../../../db/Device.h"

class Com;
class TcpClient;

class ApfSvg
{
public:
	ApfSvg();
	virtual ~ApfSvg();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取APF/SVG数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置APF/SVG命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  SinexcelRead
	 *  功能: 读取深圳盛弘APF/SVG数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SinexcelRead(Device::SlaveDev* dev);

	/*
	 *  SinexcelPreset
	 *  功能: 设置深圳盛弘APF/SVG命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SinexcelPreset(Device::SlaveDev* dev);


	//变量
public:
protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
};

#endif /* APFSVG_H_ */
