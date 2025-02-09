/*
 * VilvaLoad.h
 *
 *  创建日期: 2016-5-30
 *  作          者: 刘海涛
 */

#ifndef VILVALOAD_H_
#define VILVALOAD_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;

class VilvaLoad
{
public:
	VilvaLoad();
	virtual ~VilvaLoad();

	//设置读取串口
	void SetCom(Com* com);

	//设置控制串口
	void SetCtrlCom(Com* ctrlCom);

	/*
	 *  Read
	 *  功能: 读取交流负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置交流负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  VilvaRead
	 *  功能: 读取威尔华负载数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int VilvaRead(Device::SlaveDev* dev);

	/*
	 *  VilvaPreset
	 *  功能: 设置威尔华负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int VilvaPreset(Device::SlaveDev* dev);

	/*
	 *  PlcPackData
	 *  功能: 封装威尔华负载命令并下发
	 *  输入：
	 *  value    :int 威尔华负载命令(bit0:A相1kW,bit1:A相2kW,bit2:A相2kW,bit3:A相5kW
									bit4:B相1kW,bit5:B相2kW,bit6:B相2kW,bit7:B相5kW
									bit8:C相1kW,bit9:C相2kW,bit10:C相2kW,bit11:C相5kW
									bit12:电源开关,bit13:负载开关)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PlcPackData(int value);

	//变量
public:
protected:
private:
	Com* m_com;
	Com* m_ctrlCom;
	int m_value;
};

#endif /* VILVALOAD_H_ */
