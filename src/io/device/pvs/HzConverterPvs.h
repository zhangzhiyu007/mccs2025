/*
 * HzConverterPvs.h
 *
 *  创建日期: 2016-6-7
 *  作          者: 刘海涛
 */

#ifndef HZCONVERTERPVS_H_
#define HZCONVERTERPVS_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

class HzConverterPvs
{
public:
	HzConverterPvs();
	virtual ~HzConverterPvs();

	//设置串口
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

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
	 * Spc030KtlRead
	 * 功能：读取SPC030KTL数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Spc030KtlRead(Device::SlaveDev* dev);

	/*
	 * Spc030KtlPreset
	 * 功能：设置SPC030KTL数据
	 * 输入：
	 * dev			:[in] Device::SlaveDev* 从设备指针
	 * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
	 */
	int Spc030KtlPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* HZCONVERTERPVS_H_ */
