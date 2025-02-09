/*
 * ChromaSimultor.h
 *
 *  创建日期: 2016-6-17
 *  作          者: 刘海涛
 */

#ifndef CHROMASIMULTOR_H_
#define CHROMASIMULTOR_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

class ChromaSimultor
{
public:
	ChromaSimultor();
	virtual ~ChromaSimultor();

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

protected:
private:

	/*
	 *  Chroma62020Read
	 *  功能: 读取chroma62020H-150S数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma62020Read(Device::SlaveDev* dev);

	/*
	 *  Chroma62020Preset
	 *  功能: 设置chroma62020H-150S数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma62020Preset(Device::SlaveDev* dev);

	/*
	 *  Chroma62150Read
	 *  功能: 读取chroma62150H-1000S数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma62150Read(Device::SlaveDev* dev);

	/*
	 *  Chroma62150Preset
	 *  功能: 设置chroma62150H-1000S数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma62150Preset(Device::SlaveDev* dev);

	/*
	 *  Chroma62000Read
	 *  功能: 读取chroma62000H系列光伏模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma62000Read(Device::SlaveDev* dev);

	/*
	 *  Chroma62000Preset
	 *  功能: 设置chroma62000H系列光伏模拟器数据
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma62000Preset(Device::SlaveDev* dev);

    //chroma通讯协议
    //实时量
    /*
	 *  GetCur
	 *  功能: 获取实时输出电流值
	 *  输入：
	 *  current		:float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCur(float& current);

	/*
	 *  GetVol
	 *  功能: 获取实时输出电压值
	 *  输入：
	 *  voltage		:float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVol(float& voltage);

	/*
	 *  GetState
	 *  功能: 获取状态
	 *  输入：
	 *  fault		:int& 故障码、
	 *  state		:int& 工作状态
	 *  mode		:int& 工作模式
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetState(int& fault, int& state, int& mode);

	/*
	 *  GetPow
	 *  功能: 获取实时输出功率值
	 *  输入：
	 *  power		:float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPow(float& power);

    //设置量
	/*
	 *  SetStart
	 *  功能: 设置启动停止
	 *  输入：
	 *  runSta		:int 启动停止 0:停止/1:启动
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetStart(int runSta);

	/*
	 *  GetStart
	 *  功能: 获取设置启动停止
	 *  输入：
	 *  runSta		:int& 启动停止 0:停止/1:启动
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetStart(int& runSta);

	/*
	 *  SetOutMode
	 *  功能: 设置输出工作模式
	 *  输入：
	 *  mode		:int 工作模式 ）1:CVCC/2:SAS模式/3：EN50530/4：SANDIA
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetOutMode(int mode);

	/*
	 *  GetOutMode
	 *  功能: 获取设置输出工作模式
	 *  输入：
	 *  mode		:int& 工作模式
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetOutMode(int& mode);

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
	 *  SetVoc
	 *  功能: 设置开路电压值
	 *  输入：
	 *  voc		:float 开路电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetVoc(float voc);

	/*
	 *  GetVoc
	 *  功能: 获取设置开路电压值
	 *  输入：
	 *  voc		:float& 设置开路电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVoc(float& voc);

	/*
	 *  SetIsc
	 *  功能: 设置短路电流值
	 *  输入：
	 *  isc		:float 短路电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetIsc(float isc);

	/*
	 *  GetIsc
	 *  功能: 获取设置短路电流值
	 *  输入：
	 *  isc		:float& 设置短路电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetIsc(float& isc);

	/*
	 *  SetPmpV
	 *  功能: 设置最大功率点电压值
	 *  输入：
	 *  pmpV		:float 最大功率点电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetPmpV(float pmpV);

	/*
	 *  GetPmpV
	 *  功能: 获取设置最大功率点电压值
	 *  输入：
	 *  pmpV		:float& 设置最大功率点电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPmpV(float& pmpV);

	/*
	 *  SetPmpI
	 *  功能: 设置最大功率点电流值
	 *  输入：
	 *  pmpI		:float 最大功率点电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetPmpI(float pmpI);

	/*
	 *  GetPmpI
	 *  功能: 获取设置最大功率点电流值
	 *  输入：
	 *  pmpI		:float& 设置最大功率点电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPmpI(float& pmpI);

	/*
	 *  SetTrig
	 *  功能: 当SAS模式正在执行是，更新SASMode输出波形
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetTrig();

	/*
	 *  SetPmpP
	 *  功能: 设置最大功率点功率值
	 *  输入：
	 *  pmpP		:float 最大功率点功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetPmpP(float pmpP);

	/*
	 *  GetPmpP
	 *  功能: 获取设置最大功率点功率值
	 *  输入：
	 *  pmpP		:float& 设置最大功率点功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPmpP(float& pmpP);

	/*
	 *  SetTech
	 *  功能: 设置EN50530模式填充因子
	 *  输入：
	 *  mode		:int 填充电子
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetTech(int mode);

	/*
	 *  GetTech
	 *  功能: 获取设置EN50530模式填充因子值
	 *  输入：
	 *  mode		:int& 填充因子
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetTech(int& mode);

	/*
	 *  SetIrr
	 *  功能: 设置EN50530模式光照幅度
	 *  输入：
	 *  irr		:float 光照幅度
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetIrr(float irr);

	/*
	 *  GetIrr
	 *  功能: 获取设置EN50530模式光照幅度值
	 *  输入：
	 *  irr		:float& 光照幅度
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetIrr(float& irr);

	/*
	 *  SetIrrRef
	 *  功能: 设置SANDIA模式光照幅度参考值
	 *  输入：
	 *  irrRef		:float 光照幅度参考值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetIrrRef(float irrRef);

	/*
	 *  GetIrrRef
	 *  功能: 获取SANDIA模式设置光照幅度参考值
	 *  输入：
	 *  irrRef		:float& 光照幅度参考值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetIrrRef(float& irrRef);

	/*
	 *  SetTmpRef
	 *  功能: 设置SANDIA模式温度参考值
	 *  输入：
	 *  tmpRef		:float 温度参考值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetTmpRef(float tmpRef);

	/*
	 *  GetTmpRef
	 *  功能: 获取设置SANDIA模式温度参考值
	 *  输入：
	 *  tmpRef		:float& 温度参考值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetTmpRef(float& tmpRef);

	/*
	 *  SetBeta
	 *  功能: 设置SANDIA模式电压温度系数
	 *  输入：
	 *  beta		:float 电压温度系数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetBeta(float beta);

	/*
	 *  GetBeta
	 *  功能: 获取设置SANDIA模式电压温度系数
	 *  输入：
	 *  beta		:float& 电压温度系数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetBeta(float& beta);

	/*
	 *  SetFF
	 *  功能: 设置SANDIA模式填充因数
	 *  输入：
	 *  ff		:float 填充因数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetFF(float ff);

	/*
	 *  GetFF
	 *  功能: 获取设置SANDIA模式填充因数
	 *  输入：
	 *  ff		:float& 填充因数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetFF(float& ff);

	/*
	 *  SetSandiaIrr
	 *  功能: 设置SANDIA模式光照幅度
	 *  输入：
	 *  irr		:float 光照幅度
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetSandiaIrr(float irr);

	/*
	 *  GetSandiaIrr
	 *  功能: 获取设置SANDIA模式光照幅度
	 *  输入：
	 *  irr		:float& 光照幅度
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSandiaIrr(float& irr);

	/*
	 *  SetSandiaTmp
	 *  功能: 设置SANDIA模式温度
	 *  输入：
	 *  temp		:float 温度
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetSandiaTmp(float temp);

	/*
	 *  GetSandiaTmp
	 *  功能: 获取设置SANDIA模式温度
	 *  输入：
	 *  temp		:float& 温度
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSandiaTmp(float& temp);

	/*
	 *  SetSandiaPmpP
	 *  功能: 设置SANDIA模式最大功率点功率
	 *  输入：
	 *  pmpP		:float 最大功率点功率
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetSandiaPmpP(float pmpP);

	/*
	 *  GetSandiaPmpP
	 *  功能: 获取设置SANDIA模式最大功率点功率
	 *  输入：
	 *  pmpP		:float& 最大功率点功率
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSandiaPmpP(float& pmpP);

	/*
	 *  SetSandiaPmpV
	 *  功能: 设置SANDIA模式最大功率点电压
	 *  输入：
	 *  pmpV		:float 最大功率点电压
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetSandiaPmpV(float pmpV);

	/*
	 *  GetSandiaPmpV
	 *  功能: 获取设置SANDIA模式最大功率点电压
	 *  输入：
	 *  pmpV		:float& 最大功率点电压
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSandiaPmpV(float& pmpV);
	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* CHROMASIMULTOR_H_ */
