/*
 * FgyBMS.h
 *
 *  Created on: 2013-10-14
 *      Author: liuhaitao
 */

#ifndef FGYBMS_H_
#define FGYBMS_H_
#include "../../../db/Device.h"
#include "../../../util/Util.h"
#include "../../can/Can.h"
#include "../../../db/MemDb.h"

class Can;

class FgyBMS
{
public:
	FgyBMS();
	virtual ~FgyBMS();

	//保存串口指针
	void SetCan(Can* can);

	/*
	 *  Read
	 *  功能: 读取BMS数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  frame   :Can::FrameInfo& CAN信息帧
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev,Can::FrameInfo& frame);

	/*
	 *  Preset
	 *  功能: 设置BMS命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

public:
protected:
	/*
	 *  AcltCdzRead
	 *  功能: 读取型号ACLT-LiB数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  frame   :Can::FrameInfo& CAN信息帧
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltLibRead(Device::SlaveDev* dev,Can::FrameInfo& frame);

	/*
	 *  AcltCdzPreset
	 *  功能: 设置型号ACLT-LiB命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltLibPreset(Device::SlaveDev* dev);

	/*
	 *  UnPackData
	 *  功能: 解析单体电压数据
	 *  输入：
	 *  frame		:Can::FrameInfo can帧数据
	 *  values	:UshortArray 解析的数据
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int UnPackData(Can::FrameInfo frame,UshortArray& values);

	//变量
public:
protected:
private:

	Can*  m_can;

};

#endif /* FGYBMS_H_ */
