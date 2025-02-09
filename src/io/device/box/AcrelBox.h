/*
 * AcrelBox.h
 *
 *  创建日期: 2016-3-25
 *  作          者: 刘海涛
 */

#ifndef ACRELBOX_H_
#define ACRELBOX_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class AcrelBox
{
public:
	AcrelBox();
	virtual ~AcrelBox();

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
	 *  AcrelM4FTRead
	 *  功能: 读取安科瑞M4FTRA_KV_T2汇流箱数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcrelM4FTRead(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
};

#endif /* ACRELBOX_H_ */
