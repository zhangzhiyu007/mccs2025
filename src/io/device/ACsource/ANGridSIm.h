/*
 * ANGridSIm.h
 *
 *  Created on: 2022-12-18
 *      Author: 16843
 */

#ifndef ANGRIDSIM_H_
#define ANGRIDSIM_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../../util/Check.h"

class Com;
class TcpClient;

class ANGridSIm
{
public:
	ANGridSIm();
	virtual ~ANGridSIm();

	//设置串口
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取光伏电池模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置光伏电池模拟器命令
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

    /*
     * nPreset
     * 功能：设置艾诺参数
     * 输入：
     * addr		:int 从设备地址
     * type		:int 命令类型
     * data		:int 命令字
     * length	:short 长度
     * values	:values 设置
     * 输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int AngridPreset(int addr,int type,int data,short length,UshortArray values);

    /*
     * AnRead
     * 功能：读取艾诺实时数据
     * 输入：
     * addr		:int 从设备地址
     * type		:int 命令类型
     * data		:int 命令字
     * length	:short 长度
     * values	:UshortArray& 读取的数据
     * 输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int AngridRead(int addr, int type,int data,short length,UshortArray& values,int bit9);
	int AngridRead(int addr, int type,int data,short length,UshortArray& values);
	int AngridRead(int addr, int type,int data,short length,UshortArray& values,int bit9,int bit10);

protected:
private:
	/*
	 *  AcltAn51Read
	 *  功能: 读取ACLT-AN51数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int JDHNAngridRead(Device::SlaveDev* dev);

	/*
	 *  AcltAn51Preset
	 *  功能: 设置ACLT-AN51数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int JDHNAngridPreset(Device::SlaveDev* dev);

	/*
	 *  AcltAn51010Read
	 *  功能: 读取ACLT-AN51数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltAn51010Read(Device::SlaveDev* dev);

	/*
	 *  AcltAn51010Preset
	 *  功能: 设置ACLT-AN51数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltAn51010Preset(Device::SlaveDev* dev);

	/*
	 *  AcltAnRead
	 *  功能: 读取光伏电池模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltAnRead(Device::SlaveDev* dev);

	/*
	 *  AcltAnPreset
	 *  功能: 设置光伏电池模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int AcltAnPreset(Device::SlaveDev* dev);

    int ANRead(Device::SlaveDev* dev);
    int ANPreset(Device::SlaveDev* dev);


	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcpClient;//以太网，用于串口服务器
	int m_retries;
	int m_delayTime;
};

#endif /* ANGRIDSIM_H_ */
