/*
 * ApcACSource.h
 *
 *  Created on: 2014-1-8
 *      Author: liuhaitao
 */

#ifndef APCACSOURCE_H_
#define APCACSOURCE_H_
#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"

class Com;

class ApcACSource
{
public:
	ApcACSource();
	virtual ~ApcACSource();

	//设置串口
	void SetCom(Com* com);

	/*
	 *  Read
	 *  功能: 读取交流电源数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置交流电源命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  AcstN33030Read
	 *  功能: 读取ACST-N-33030数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcstN33030Read(Device::SlaveDev* dev);

	/*
	 *  AcstN33030Preset
	 *  功能: 设置ACLT-AN99030命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcstN33030Preset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	//查询失败，重复查询次数
	int m_retries;
	int m_delayTime;
};

#endif /* APCACSOURCE_H_ */
