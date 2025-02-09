/*
 * SignalModule.h
 *
 *  Created on: 2014-4-19
 *      Author: liuhaitao
 */

#ifndef SIGNALMODULE_H_
#define SIGNALMODULE_H_
#include "../../../db/Device.h"

class Com;
class TcpClient;

class SignalModule
{
public:
	SignalModule();
	virtual ~SignalModule();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取信号采集模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置电能采集模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  HC33BRead
	 *  功能: 读取惠测电子电能模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HC33BRead(Device::SlaveDev* dev);

	/*
	 *  HC33BDRead
	 *  功能: 读取惠测直流电子电能模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HC33BDRead(Device::SlaveDev* dev);

	/*
	 *  SetMemDb
	 *  功能: 把读取惠测电子电能模块数据设置到共享内存中
	 *  输入：
	 *  addr 	:int 设备地址
	 *  start	:int 设备共享内存起始地址
	 *  values	:UshortArray 读到的设备数据
	 *  comm	:bool 通讯状态
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetMemDb(int addr,UshortArray values,int start,bool comm);

	/*
	 *  HC33BPreset
	 *  功能: 设置惠测电子电能模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HC33BPreset(Device::SlaveDev* dev);

	/*
	 *  HC33BDPreset
	 *  功能: 设置惠测电子直流电能模块数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HC33BDPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
    Com* m_com;
    TcpClient* m_tcpClient;//以太网
};

#endif /* SIGNALMODULE_H_ */
