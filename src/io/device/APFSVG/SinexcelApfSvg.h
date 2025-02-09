/*
 * SinexcelApfSvg.h
 *
 *  创建日期: 2016-6-20
 *  作          者: 刘海涛
 */

#ifndef SINEXCELAPFSVG_H_
#define SINEXCELAPFSVG_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;

class SinexcelApfSvg
{
public:
	SinexcelApfSvg();
	virtual ~SinexcelApfSvg();

	//设置读取串口
	void SetCom(Com* com);

	/*
	 *  Read
	 *  功能: 读取APF/SVG数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置APF/SVG命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  SinexcelApfSvgRead
	 *  功能: 读取深圳盛弘APF/SVG数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SinexcelApfSvgRead(Device::SlaveDev* dev);

	/*
	 *  SinexcelApfSvgPreset
	 *  功能: 设置深圳盛弘APF/SVG命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SinexcelApfSvgPreset(Device::SlaveDev* dev);

	//变量
public:
protected:
private:
	Com* m_com;
};

#endif /* SINEXCELAPFSVG_H_ */
