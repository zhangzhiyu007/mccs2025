/*
 * QunLingDcLoad.h
 *
 *  Created on: 2013-10-11
 *      Author: liuhaitao
 */

#ifndef QUNLINGDCLOAD_H_
#define QUNLINGDCLOAD_H_
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../util/Check.h"

class Com;

class QunLingDcLoad
{
public:
	QunLingDcLoad();
	virtual ~QunLingDcLoad();
	//设置串口
	void SetCom(Com* com);

	//设置返回查询失败的重复查询次数
	void SetRetries(int retries);
	int GetRetries();

	//设置延时时间
	void SetDelayTime(int delayTime);

	/*
	 *  Read
	 *  功能: 读取直流负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置直流负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

	/*
	 *  PackData
	 *  功能: 封装数据包
	 *  输入：
	 *  function		:int 功能码
	 *  length 			:int 数据区长度
	 *  values			:UshortArray 数据区数据
	 *  query			:String& 返回数据
	 *  输出：无
	 */
    void PackData(int function,int length,UshortArray values,String& query);

protected:
private:
	/*
	 *  Read
	 *  功能: 读取BDCT-5000数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Bdct5000Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置BDCT-5000命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Bdct5000Preset(Device::SlaveDev* dev);
	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	//查询失败冲查询次数
	int m_retries;
	//查询出错后，延时该时间，在进行下次查询，ms
	int m_delayTime;
};

#endif /* QUNLINGDCLOAD_H_ */
