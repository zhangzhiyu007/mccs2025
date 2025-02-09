/*
 * ApcDCSource.h
 *
 *  Created on: 2014-1-11
 *      Author: liuhaitao
 */

#ifndef APCDCSOURCE_H_
#define APCDCSOURCE_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class Com;

class ApcDCSource
{
public:
	ApcDCSource();
	virtual ~ApcDCSource();

	//设置串口
	void SetCom(Com* com);

	/*
	 *  Read
	 *  功能: 读取直流稳定电源数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置直流稳定电源命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  Dcst30Read
	 *  功能: 读取DCST-30数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Dcst30Read(Device::SlaveDev* dev);

	/*
	 *  Dcst30Preset
	 *  功能: 设置DCST-30数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Dcst30Preset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	int m_retries;
	int m_delayTime;
};

#endif /* APCDCSOURCE_H_ */
