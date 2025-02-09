/*
 * Wattmeter.h
 *
 *   创建日期: 2013-6-24
 *   作         者: mzh
 */

#ifndef WATTMETER_H_
#define WATTMETER_H_

#include "../../../util/Util.h"
#include "../../../db/Device.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

/*名称:
 *描述:
 *用法:
 *创建人:mzh
 *创建日期:2013-6-24
 */
class Wattmeter
{
public:
	Wattmeter();
	virtual ~Wattmeter();

	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取电表数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  AcxionRead
	 *  功能: 读取爱可信电表数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcxionRead(Device::SlaveDev* dev);

	/*
	 *  hcRead
	 *  功能: 读取惠测电表数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int hcRead(Device::SlaveDev* dev);

    int TaikaiRead(Device::SlaveDev* dev);

//	/*
//	 *  Dlt645_2007Read
//	 *  功能: 读取dlt645-2007标准数据
//	 *  dev    :Device::SlaveDev* 从设备指针
//	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
//	 */
//    int Dlt645_2007Read(Device::SlaveDev* dev);

	/*
	 *  Dlt645_1997Read
	 *  功能: 读取dlt645-1997标准数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Dlt645_1997Read(Device::SlaveDev* dev);

	int AcrelRead(Device::SlaveDev *dev);

protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;
	bool m_isTcp;
	int  m_leadBytesResp;     //应答数据前导字节
};

#endif /* WATTMETER_H_ */
