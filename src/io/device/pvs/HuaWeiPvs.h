/*
 * HuaWeiPvs.h
 *
 *  Created on: 2014-12-25
 *      Author: liuhaitao
 */

#ifndef HUAWEIPVS_H_
#define HUAWEIPVS_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;

class HuaWeiPvs
{
public:
	HuaWeiPvs();
	virtual ~HuaWeiPvs();

	//设置串口
	void SetCom(Com* com);

	/*
	 * Read
	 * 功能：读取PVS数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Read(Device::SlaveDev* dev);

	/*
	 * Preset
	 * 功能：设置PVS数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 * Sun200010KtlRead
	 * 功能：读取PVS数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Sun200010KtlRead(Device::SlaveDev* dev);

	/*
	 * Sun200010KtlPreset
	 * 功能：设置PVS数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Sun200010KtlPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	int m_limitPower;
	int m_powerFlag;

	//计算时间差
	StatTimer m_controlStart;
};

#endif /* HUAWEIPVS_H_ */
