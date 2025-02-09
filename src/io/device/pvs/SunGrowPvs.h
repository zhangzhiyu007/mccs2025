/*
 * SunGrowPvs.h
 *
 *  Created on: 2013-10-16
 *      Author: liuhaitao
 */

#ifndef SUNGROWPVS_H_
#define SUNGROWPVS_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;

class SunGrowPvs
{
public:
	SunGrowPvs();
	virtual ~SunGrowPvs();

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
	 * Sg30KtlRead
	 * 功能：读取SG-30KTL数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Sg30KtlRead(Device::SlaveDev* dev);

	/*
	 * Sg30KtlPreset
	 * 功能：设置SG-30KTL数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Sg30KtlPreset(Device::SlaveDev* dev);

	/*
	 * Sg20KtlRead
	 * 功能：读取SG-20KTL数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Sg20KtlRead(Device::SlaveDev* dev);

	/*
	 * Sg20KtlPreset
	 * 功能：设置SG-20KTL数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Sg20KtlPreset(Device::SlaveDev* dev);

	/*
	 * SgKtlRead
	 * 功能：读取PVS数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int SgKtlRead(Device::SlaveDev* dev);

	/*
	 * SgKtlPreset
	 * 功能：设置PVS数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int SgKtlPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
};

#endif /* SUNGROWPVS_H_ */
