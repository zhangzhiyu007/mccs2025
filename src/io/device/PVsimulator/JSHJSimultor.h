/*
 * JSHJSimultor.h
 *
 *  Created on: 2015-12-9
 *      Author: liuhaitao
 */

#ifndef JSHJSIMULTOR_H_
#define JSHJSIMULTOR_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"

class Com;

class JSHJSimultor
{
public:
	//光伏曲线参数
	typedef struct PVivCurve
	{
		int num;//曲线号
		float vmp;//最大功率点电压
		float imp;//最大功率点电流
		float voc;//开路电压
		float isc;//短路电流
		float power;//功率
		float fillFactorV;//电压填充因子（默认0.8）
		float fillFactorI;//电流填充因子（默认0.9）
		float bt;//湿度（默认值-0.25%℃）
		float ta;//温度（默认值25℃）
		float irr;//光照幅度（默认值1000W/㎡）
	}PVivCurve;

	JSHJSimultor();
	virtual ~JSHJSimultor();

	//设置串口
	void SetCom(Com* com);

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

protected:
private:
	/*
	 *  PVS1000Read
	 *  功能: 读取PVS1000数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PVS1000Read(Device::SlaveDev* dev);

	/*
	 *  PVS1000Preset
	 *  功能: 设置PVS1000数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int PVS1000Preset(Device::SlaveDev* dev);


    /*菊水皇家光伏模拟器通讯协议*/
	/*
	 *  SetIVCU
	 *  功能: 生成一条可供调用的IV曲线
	 *  输入：
	 *  type	:int 设置IV曲线类型1、实际曲线/2、理想曲线
	 *  data    :PVivCurve 光伏曲线数据
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetIVCU(int type, PVivCurve data);

	/*
	 *  CallSepCur
	 *  功能: 调用执行指定曲线
	 *  输入：
	 *  num		 :int 曲线编号
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int CallSepCur(int num);

	/*
	 *  SetOutMode
	 *  功能: 设置输出工作模式
	 *  输入：
	 *  mode		:int 工作模式 ）0:正常模式/2:SAS模式
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetOutMode(int mode);

	/*
	 *  SetStart
	 *  功能: 设置启动停止
	 *  输入：
	 *  runSta		:int 启动停止 0:停止/1:启动
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetStart(int runSta);

	/*
	 *  SetCurrent
	 *  功能: 设置输出电流值
	 *  输入：
	 *  current		:float 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCurrent(float current);

	/*
	 *  GetCurrent
	 *  功能: 获取设置输出电流值
	 *  输入：
	 *  current		:float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCurrent(float& current);

	/*
	 *  GetSAS
	 *  功能: 获取设置光伏曲线值
	 *  输入：
	 *  value		:PVivCurve& 光伏曲线参数
	 *  regStart	:int 设备内存起始地址
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSAS(PVivCurve& value, int regStart);

	/*
	 *  SetVoltage
	 *  功能: 设置输出电压值
	 *  输入：
	 *  voltage		:float 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetVoltage(float voltage);

	/*
	 *  GetVoltage
	 *  功能: 获取设置输出电压值
	 *  输入：
	 *  voltage		:float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVoltage(float& voltage);

	/*
	 *  SetPower
	 *  功能: 设置输出功率值
	 *  输入：
	 *  power		:float 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetPower(float power);

	/*
	 *  GetPower
	 *  功能: 获取设置输出功率值
	 *  输入：
	 *  power		:float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPower(float& power);

	/*
	 *  SetResistance
	 *  功能: 设置输出内阻值
	 *  输入：
	 *  resistance	 	:float 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetResistance(float resistance);

	/*
	 *  GetResistance
	 *  功能: 获取设置输出内阻值
	 *  输入：
	 *  resistance	 	:float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetResistance(float& resistance);

	/*
	 *  OutPutMaxDc
	 *  功能: 查询设备可提供输出的能力
	 *  输入：
	 *  voltage		:float& 电压
	 *  current		:float& 电流
	 *  power		:float& 功率
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int OutPutMaxDc(float& voltage, float& current, float& power);

	/*
	 *  GetData
	 *  功能: 获取电压电流
	 *  输入：
	 *  voltage		:float& 电压
	 *  current		:float& 电流
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetData(float& voltage, float& current);

	/*
	 *  GetSysSta
	 *  功能: 获取系统状态
	 *  输入：
	 *  runSta		:int& 工作状态
	 *  mode		:int& 工作模式
	 *  fault		:int& 故障码
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSysSta(int& runSta, int& mode, int& fault);

	/*
	 *  GetDataAccuracy
	 *  功能: 获取数据精度
	 *  输入：
	 *  accuracyV	:int& 电压精度（0~3保留小数点后几位）
	 *  accuracyI	:int& 电流精度（0~3保留小数点后几位）
	 *  accuracyP	:int& 功率精度（0~3保留小数点后几位）
	 *  powerUnit	:int& 功率单位:0:W/1:kW
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSysSta(int& accuracyV, int& accuracyI, int& accuracyP, int& powerUnit);
	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
};

#endif /* JSHJSIMULTOR_H_ */
