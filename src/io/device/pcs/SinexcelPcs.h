/*
 * SinexcelPcs.h
 *
 *  创建日期: 2016-7-6
 *  作          者: 刘海涛
 */

#ifndef SINEXCELPCS_H_
#define SINEXCELPCS_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

class Com;
class TcpClient;

class SinexcelPcs
{
public:
	SinexcelPcs();
	virtual ~SinexcelPcs();

	//设置串口指针
	void setCom(Com* com);
	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

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
	 *  PcsRead
	 *  功能: 读取深圳盛弘PCS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PcsRead(Device::SlaveDev* dev);

	/*
	 *  PcsPreset
	 *  功能: 设置深圳盛弘PCS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PcsPreset(Device::SlaveDev* dev);

	/*
	 *  SinexcelPcsRead
	 *  功能: 读取深圳盛弘PCS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SinexcelPcsRead(Device::SlaveDev* dev);

	/*
	 *  SinexcelPcsPreset
	 *  功能: 设置深圳盛弘PCS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SinexcelPcsPreset(Device::SlaveDev* dev);

	/*
	 *  SinexcelTcpPcsRead
	 *  功能: 读取深圳盛弘PCS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SinexcelTcpPcsRead(Device::SlaveDev* dev);

	/*
	 *  SinexcelTcpPcsPreset
	 *  功能: 设置深圳盛弘PCS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SinexcelTcpPcsPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* SINEXCELPCS_H_ */
