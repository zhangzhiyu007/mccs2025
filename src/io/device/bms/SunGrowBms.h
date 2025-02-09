/*
 * SunGrowBms.h
 *
 *  Created on: 2013-10-15
 *      Author: liuhaitao
 */

#ifndef SUNGROWBMS_H_
#define SUNGROWBMS_H_
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;

class SunGrowBms
{
public:
	SunGrowBms();
	virtual ~SunGrowBms();

	//设置串口
	void SetCom(Com* com);

	/*
	 *  Read
	 *  功能: 读取BMS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置BMS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  Read
	 *  功能: 读取BMS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SunGrow30KWRead(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置BMS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SunGrow30KWPreset(Device::SlaveDev* dev);
	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
};

#endif /* SUNGROWBMS_H_ */
