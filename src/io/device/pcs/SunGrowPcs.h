/*
 * SunGrowPcs.h
 *
 *  Created on: 2013-10-15
 *      Author: liuhaitao
 */

#ifndef SUNGROWPCS_H_
#define SUNGROWPCS_H_
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;

class SunGrowPcs
{
public:
	SunGrowPcs();
	virtual ~SunGrowPcs();

	//设置串口
	void SetCom(Com* com);

	/*
	 *  Read
	 *  功能: 读取PCS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置PCS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  Read
	 *  功能: 读取SC50数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Sc50Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置SC50命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Sc50Preset(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置SC50工作模式
	 *  输入：
	 *	start	 :int MemDb起始地址
	 *	regStart :int 内存地址
	 *	data     :工作模式所在位置
	 *	addr	 :设备地址
	 *	value	 :工作模式
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetState(int start,int regStart,int data,int addr,int value);
	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
};

#endif /* SUNGROWPCS_H_ */
