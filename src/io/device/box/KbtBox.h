/*
 * KbtBox.h
 *
 *  Created on: 2013-10-16
 *      Author: liuhaitao
 */

#ifndef KBTBOX_H_
#define KBTBOX_H_
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;

class KbtBox
{
public:
	KbtBox();
	virtual ~KbtBox();

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

protected:
private:

	/*
	 *  Read
	 *  功能: 读取KBT-PVX8数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int KbtPvx8Read(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
};

#endif /* KBTBOX_H_ */
