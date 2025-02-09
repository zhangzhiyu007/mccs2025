#ifndef AAC3810_LOAD_H
#define AAC3810_LOAD_H

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"

class TcpClient;

class AAC3810
{
public:
	AAC3810();
	virtual ~AAC3810();

	void SetTcpClient(TcpClient *client);

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
    int LoadRead(Device::SlaveDev* dev);

	/*
	 *  VilvaPreset
	 *  功能: 设置威尔华负载命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int LoadPreset(Device::SlaveDev* dev);

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
	TcpClient* m_tcpClient;
	int m_value;
	int PhaseAResisPower;
	int PhaseAIndPower;
	int PhaseACapaPower;

	int PhaseBResisPower;
	int PhaseBIndPower;
	int PhaseBCapaPower;

	int PhaseCResisPower;
	int PhaseCIndPower;
	int PhaseCCapaPower;
};

#endif /* VILVALOAD_H_ */
