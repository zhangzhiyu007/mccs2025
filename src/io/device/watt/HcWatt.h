/*
 * HcWatt.h
 *
 *  Created on: 2019-4-23
 *      Author: 刘海涛
 */

#ifndef HCWATT_H_
#define HCWATT_H_

#include "../../../db/Device.h"

class Com;
class TcpClient;

class HcWatt {
public:
	HcWatt();
	virtual ~HcWatt();
	//设置是否采用以太网通讯
	void SetIsTcp(bool isTcp);

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取惠测电表数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);


	int Preset(Device::SlaveDev* dev);



protected:
private:

	/*
	 *  Hc63CRead
	 *  功能: 读取惠测三相多功能电表数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Hc63CRead(Device::SlaveDev* dev);

	/*
	 *  Hc221Read
	 *  功能: 读取惠测模拟量测量模块数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Hc221Read(Device::SlaveDev* dev);

	/*
	 *  HC33BDRead
	 *  功能: 读取惠测直流电子电能模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HC33BDRead(Device::SlaveDev* dev);

	int HC201Read(Device::SlaveDev* dev);

	int HC202Preset(Device::SlaveDev* dev);
	int HC202Read(Device::SlaveDev* dev);

	int HC209Read(Device::SlaveDev* dev);

	int HC209Preset(Device::SlaveDev* dev);

	int HC33BRead(Device::SlaveDev* dev);

	int HC33Preset(Device::SlaveDev* dev);


protected:
private:
    Com* m_com;
	bool m_isTcp;
	TcpClient* m_tcpClient;//以太网，用于串口服务器
	int U0_33B;
	int I0_33B;
	int Ir_33B;
	int Ur_33B;



};

#endif /* HCWATT_H_ */
