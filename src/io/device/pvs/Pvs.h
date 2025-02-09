/*
 * Pvs.h
 *
 *   创建日期: 2013-6-24
 *   作         者: mzh
 */

#ifndef PVS_H_
#define PVS_H_

#include "../../../util/Util.h"
#include "../../../db/Device.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"
#include "HuaWeiPvs.h"

/*名称:Pvs
 *描述:光伏逆变器设备数据采集及控制
 *用法:
 *创建人:mzh
 *创建日期:2013-6-24
 */
class Pvs
{
public:
	Pvs();
	virtual ~Pvs();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取g光伏逆变器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置光伏逆变器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);
	void SetIsTcp(bool IsTcp);
protected:
private:
	/*
	 *  SungrowRead
	 *  功能: 读取阳光逆变器数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SungrowRead(Device::SlaveDev* dev);

	/*
	 *  SungrowPreset
	 *  功能: 设置阳光逆变器命令
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SungrowPreset(Device::SlaveDev* dev);

	/*
	 *  goodWeRead
	 *  功能: 读取固德威逆变器数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int goodWeRead(Device::SlaveDev* dev);

	/*
	 *  goodWePreset
	 *  功能: 设置固德威逆变器命令
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int goodWePreset(Device::SlaveDev* dev);

	/*
	 *  HuaWeiRead
	 *  功能: 读取华为逆变器数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HuaWeiRead(Device::SlaveDev* dev);

	/*
	 *  HuaWeiPreset
	 *  功能: 设置华为逆变器命令
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HuaWeiPreset(Device::SlaveDev* dev);

	/*
	 *  APsystemsRead
	 *  功能: 读取APsystems逆变器数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int APsystemsRead(Device::SlaveDev* dev);

	/*
	 *  APsystemsPreset
	 *  功能: 设置APsystems逆变器命令
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int APsystemsPreset(Device::SlaveDev* dev);

	/*
	 *  HzConverterRead
	 *  功能: 读取禾迈逆变器数据
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HzConverterRead(Device::SlaveDev* dev);

	/*
	 *  HzConverterPreset
	 *  功能: 设置禾迈逆变器命令
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int HzConverterPreset(Device::SlaveDev* dev);

protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网
	HuaWeiPvs m_pvs;
	bool m_isTcp;
};

#endif /* PVS_H_ */
