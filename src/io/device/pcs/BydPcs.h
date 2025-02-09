/*
 * BydPcs.h
 *
 *   创建日期: 2013-10-8
 *   作          者: 马中华
 */

#ifndef BYDPCS_H_
#define BYDPCS_H_
#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

class Com;

/*名称:BYD PCS
 *描述:
 *用法:
 *创建人:马中华
 *创建日期:2013-10-8
 */
class BydPcs
{
public:
	BydPcs();
	virtual ~BydPcs();

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
	 *  Read
	 *  功能: 读取ACLT-BEG数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltBegRead(Device::SlaveDev* dev);

	/*
	 *  AcltBegReadFeedBack
	 *  功能: 读取ACLT-BEG反馈数据
	 *  输入：
	 *  regStart :int MemDb起始地址
	 *  start	 :int Master起始地址
	 *  rtu		 :ModbusRtuMaster 实例
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltBegReadFeedBack(int regStart,int start,ModbusRtuMaster rtu);

	/*
	 *  Preset
	 *  功能: 设置ACLT-BEG命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltBegPreset(Device::SlaveDev* dev);

    /*
     * AcltBegPreSetValueV
     * 功能：设置ACLT-BEG电压参数命令
     * 输入：
     * regStart	:int MemDb地址
     * start	:int Master地址
     * rtu		 :ModbusRtuMaster 实例
     * 输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int AcltBegPreSetValueV(int regStart,int start,ModbusRtuMaster rtu);

    /*
     * AcltBegPreSetValueP
     * 功能：设置ACLT-BEG功率参数命令
     * 输入：
     * regStart	:int MemDb地址
     * start	:int Master地址
     * rtu		 :ModbusRtuMaster 实例
     * 输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int AcltBegPreSetValueP(int regStart,int start,ModbusRtuMaster rtu);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
};

#endif /* BYDPCS_H_ */
