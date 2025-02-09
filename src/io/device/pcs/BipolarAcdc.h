/*
 * BipolarAcdc.h
 *
 *  创建日期: 2019-8-1
 *  作          者: 刘海涛
 */

#ifndef BIPOLARACDC_H_
#define BIPOLARACDC_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class BipolarAcdc
{
public:
	BipolarAcdc();
	virtual ~BipolarAcdc();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取双极性AC/DC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置双极性AC/DC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  BipolarAcdcRead
	 *  功能: 读取清华双极性AC/DC数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int BipolarAcdcRead(Device::SlaveDev* dev);

	/*
	 *  BipolarAcdcPreset
	 *  功能: 设置清华双极性AC/DC数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int BipolarAcdcPreset(Device::SlaveDev* dev);

protected:
private:
    Com* m_com;
	bool m_isTcp;
	TcpClient* m_tcpClient;//以太网，用于串口服务器
};

#endif /* BIPOLARACDC_H_ */
