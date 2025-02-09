/*
 * ChromaSource.h
 *
 *  创建日期: 2016-6-28
 *  作          者: 刘海涛
 */

#ifndef CHROMASOURCE_H_
#define CHROMASOURCE_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"

class TcpClient;

class ChromaSource
{
public:
	ChromaSource();
	virtual ~ChromaSource();

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取chroma交流电源数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置chroma交流电源命令
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  Chroma61860Read
	 *  功能: 读取Chroma61860数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma61860Read(Device::SlaveDev* dev);

	/*
	 *  Chroma61860Preset
	 *  功能: 设置Chroma61860数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma61860Preset(Device::SlaveDev* dev);

	/*
	 *  Chroma61800Read
	 *  功能: 读取chroma61800系列交流电源
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma61800Read(Device::SlaveDev* dev);

	/*
	 *  Chroma61800Preset
	 *  功能: 设置chroma61800系列交流电源
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma61800Preset(Device::SlaveDev* dev);

	/*
	 *  PresetPack
	 *  功能: 下发控制数据
	 *  输入：
	 *  str		  :[in]string 控制数据包
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PresetPack(string str);

	/*
	 *  ReadPack
	 *  功能: 读取数据
	 *  输入：
	 *  str			:[in]string 下发数据包
	 *  packData    :[in]string& 读取到的数据
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ReadPack(string str, string& packData);

	/*
	 *  SelPhase
	 *  功能: 选择相位
	 *  输入：
	 *  phase			:[in]int 相位（1:A相、2：B相、3：C相）
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SelPhase(int phase);

	/*
	 *  SetCouple
	 *  功能: 设置使用单独命令还是所有命令
	 *  输入：
	 *  couple			:[in]int 1：所有命令/0：单独命令
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCouple(int couple);

	/*
	 *  SetRst
	 *  功能: 设置电网模拟器初始状态
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetRst();

	/*
	 *  GetVolt
	 *  功能: 获取三相电压
	 *  输入：
	 *  volt[3]			:[in]float ABC三相电压
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVolt(float volt[3]);

	/*
	 *  GetCurr
	 *  功能: 获取三相电流
	 *  输入：
	 *  curr[3]			:[in]float ABC三相电流
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCurr(float curr[3]);

	/*
	 *  GetPow
	 *  功能: 获取三相有功功率
	 *  输入：
	 *  power[3]		:[in]float ABC三相有功功率
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPow(float power[3]);

	/*
	 *  GetSPow
	 *  功能: 获取三相视在功率
	 *  输入：
	 *  sPower[3]		:[in]float ABC三相视在功率
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSPow(float sPower[3]);

	/*
	 *  GetQPow
	 *  功能: 获取三相无功功率
	 *  输入：
	 *  qPower[3]		:[in]float ABC三相无功功率
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetQPow(float qPower[3]);

	/*
	 *  Getpf
	 *  功能: 获取三相功率因数
	 *  输入：
	 *  pf[3]			:[in]float ABC三相功率因数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Getpf(float pf[3]);

	/*
	 *  GetHz
	 *  功能: 获取三相频率
	 *  输入：
	 *  hz[3]			:[in]float ABC三相频率
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetHz(float hz[3]);

	/*
	 *  GetFault
	 *  功能: 获取故障码
	 *  输入：
	 *  fault			:[in]int 故障码
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetFault(int fault);

	//变量
public:
protected:
private:
	TcpClient* m_tcp;

	int m_flag;
};

#endif /* CHROMASOURCE_H_ */
