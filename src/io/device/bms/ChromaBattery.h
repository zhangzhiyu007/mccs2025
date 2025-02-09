/*
 * ChromaBattery.h
 *
 *  创建日期: 2016-12-16
 *  作          者: zl
 */

#ifndef CHROMABATTERY_H_
#define CHROMABATTERY_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

typedef struct BattPara
{
	float volaMax;//电压上限(V)
	float volaMin;//电压下限(V)
	float rateI;//额定电流(A)
	float ratePower;//额定功率(W)
	float slewRateI;//最大电流转换速率(A/ms)
	float esrMax;//ESR上限
	float esrMin;//ESR下限
	float bvhMax;//BVH上限
	float bvhMin;//BVH下限
	float bvlMax;//BVL上限
	float bvlMin;//BVL下限
	float vhMax;//VH上限
	float vhMin;//VH下限
	float vlMax;//VL上限
	float vlMin;//VL下限
	float vohMax;//VOH上限
	float vohMin;//VOH下限
	float volMax;//VOL上限
	float volMin;//VOL下限
}BattPara;

class ChromaBattery
{
public:
	ChromaBattery();
	virtual ~ChromaBattery();

	//设置串口
	void SetCom(Com* com);
	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取chroma电池模拟器数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置chroma电池模拟器命令
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  Chroma17040Read
	 *  功能: 读取chroma17040数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma17040Read(Device::SlaveDev* dev);

	/*
	 *  Chroma17040Preset
	 *  功能: 设置chroma17040数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma17040Preset(Device::SlaveDev* dev);

	/*
	 *  Chroma17040Read
	 *  功能: 读取chroma17040数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma62180DRead(Device::SlaveDev* dev);

	/*
	 *  Chroma17040Preset
	 *  功能: 设置chroma17040数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma62180DPreset(Device::SlaveDev* dev);






    //实时值
	/*
	 *  GetBattSpecAll
	 *  功能: 获取电池模拟器设备规格
	 *  输入：
	 *  battPara	:[out]BattPara& 电池模拟器参数值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetBattSpecAll(BattPara& battPara);

    /*
	 *  GetOperat
	 *  功能: 获取实时操作状态
	 *  输入：
	 *  oper		:[out]int& 操作状态
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetOperat(int& oper);

    /*
	 *  GetUserAlarm
	 *  功能: 获取用户告警状态
	 *  输入：
	 *  alarm		:[out]int& 告警状态
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetUserAlarm(int& alarm);

    /*
	 *  GetStat
	 *  功能: 获取保护状态
	 *  输入：
	 *  stat		:[out]int& 保护状态
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetStat(int& stat);


    int GetModel(int& model);



    /*
	 *  GetCur
	 *  功能: 获取实时输出电流值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCur(float& current);

	/*
	 *  GetVol
	 *  功能: 获取实时输出电压值
	 *  输入：
	 *  voltage		:[out]float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVoltage(float& voltage);

	/*
	 *  GetPower
	 *  功能: 获取实时输出有功功率值
	 *  输入：
	 *  power		:[out]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPower(float& power);

	/*
	 *  GetDcir
	 *  功能: 获取实时内阻值
	 *  输入：
	 *  dcir		:[out]float& 内阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetDcir(float& dcir);

	/*
	 *  GetAh
	 *  功能: 获取实时可放电容量
	 *  输入：
	 *  ah			:[out]float& 可放电容量
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetAh(float& ah);

	/*
	 *  GetTemp
	 *  功能: 获取实时温度
	 *  输入：
	 *  temp		:[out]FloatArray& 温度
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetTemp(FloatArray& temp);

    //设置值
	/*
	 *  SetOutMode
	 *  功能: 设置输出工作模式
	 *  输入：
	 *  mode		:[in]int 工作模式 0：Manual Test；1：Battery Simulator
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetOutMode(int mode);

	/*
	 *  GetOutMode
	 *  功能: 获取设置输出工作模式
	 *  输入：
	 *  mode		:[out]int& 工作模式 0：Manual Test；1：Battery Simulator
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetOutMode(int& mode);

	/*
	 *  SetStart
	 *  功能: 设置启动停止
	 *  输入：
	 *  runSta		:[in]int 启动停止 0：停止;1:暂停;2:继续
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetStart(int runSta);

	/*
	 *  GetStart
	 *  功能: 获取设置启动停止
	 *  输入：
	 *  runSta		:[out]int& 启动停止0：停止;1:暂停;2:继续
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetStart(int& runSta);

	/*
	 *  SetInit
	 *  功能: 设置初始模拟器设置值
	 *  输入：
	 *  initValue	:[in]int 初始模式设置值0:Initial CAP;1:Initial VOL
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetInit(int initValue);
    int SetInit62180(int initValue);
	/*
	 *  GetInit
	 *  功能: 获取设置初始模式设置值
	 *  输入：
	 *  initValue	:[out]int& 初始模式设置值0:Initial CAP;1:Initial VOL
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetInit(int& initValue);
    int GetInit62180(int& initValue);
	/*
	 *  SetPara
	 *  功能: 设置模拟器参数设置值
	 *  输入：
	 *  para		:[in]int 模拟器参数设置值0:Ah;1:SOC
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetPara(int para);

	/*
	 *  GetPara
	 *  功能: 获取设置模拟器参数设置值
	 *  输入：
	 *  para		:[out]int& 模拟器参数设置值0:Ah;1:SOC
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPara(int& para);

	/*
	 *  SetInitCap
	 *  功能: 设置模拟器初始CAP值
	 *  输入：
	 *  initCap		:[in]float 模拟器初始CAP值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetInitCap(float initCap);

	/*
	 *  GetInitCap
	 *  功能: 获取设置模拟器初始CAP值
	 *  输入：
	 *  initCap		:[out]float& 模拟器初始CAP值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetInitCap(float& initCap);

	/*
	 *  SetInitVolt
	 *  功能: 设置模拟器初始VOLT值
	 *  输入：
	 *  initVolt	:[in]float 模拟器初始VOLT值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */

   // int SetInitVolt(int initVolt);
    int SetInitVolt(float initVolt);

	/*
	 *  GetInitVolt
	 *  功能: 获取设置模拟器初始VOLT值
	 *  输入：
	 *  initVolt	:[out]float& 模拟器初始VOLT值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetInitVolt(float& initVolt);

	/*
	 *  SetCap
	 *  功能: 设置模拟器CAP值
	 *  输入：
	 *  cap			:[in]float 模拟器CAP值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCap(float cap);

	/*
	 *  GetCap
	 *  功能: 获取设置模拟器CAP值
	 *  输入：
	 *  cap			:[out]float& 模拟器CAP值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCap(float& cap);

	/*
	 *  SetOcp
	 *  功能: 设置模拟器OCP值
	 *  输入：
	 *  ocp			:[in]float 模拟器OCP值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetOcp(float& ocp);

    int SetSourceOcp(float ocp);

    int SetLoadOcp(float ocp);

    int SetOcp(float ocp);

	/*
	 *  GetOcp
	 *  功能: 获取设置模拟器OCP值
	 *  输入：
	 *  ocp			:[out]float& 模拟器OCP值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSourceOcp(float& ocp);

	/*
	 *  GetOcp
	 *  功能: 获取设置模拟器OCP值
	 *  输入：
	 *  ocp			:[out]float& 模拟器OCP值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetLoadOcp(float& ocp);


	/*
	 *  SetEsr
	 *  功能: 设置模拟器ESR值
	 *  输入：
	 *  esr			:[in]float 模拟器ESR值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetEsr(float esr);

	/*
	 *  GetEsr
	 *  功能: 获取设置模拟器ESR值
	 *  输入：
	 *  esr			:[out]float& 模拟器ESR值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetEsr(float& esr);

	/*
	 *  SetBch
	 *  功能: 设置模拟器BCH值
	 *  输入：
	 *  bch			:[in]float 模拟器BCH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetBch(float bch);

	/*
	 *  GetBch
	 *  功能: 获取设置模拟器BCH值
	 *  输入：
	 *  bch			:[out]float& 模拟器BCH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetBch(float& bch);

	/*
	 *  SetBcl
	 *  功能: 设置模拟器BCL值
	 *  输入：
	 *  bcl			:[in]float 模拟器BCL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetBcl(float bcl);

	/*
	 *  GetBcl
	 *  功能: 获取设置模拟器BCL值
	 *  输入：
	 *  bcl			:[out]float& 模拟器BCL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetBcl(float& bcl);

	/*
	 *  SetBvh
	 *  功能: 设置模拟器BVH值
	 *  输入：
	 *  bvh			:[in]float 模拟器BVH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetBvh(float bvh);

	/*
	 *  GetBvh
	 *  功能: 获取设置模拟器BVH值
	 *  输入：
	 *  bvh			:[out]float& 模拟器BVH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetBvh(float& bvh);

	/*
	 *  SetBvl
	 *  功能: 设置模拟器BVL值
	 *  输入：
	 *  bvl			:[in]float 模拟器BVL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetBvl(float bvl);

	/*
	 *  GetBvl
	 *  功能: 获取设置模拟器BVL值
	 *  输入：
	 *  bvl			:[out]float& 模拟器BVL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetBvl(float& bvl);

	/*
	 *  SetVh
	 *  功能: 设置模拟器VH值
	 *  输入：
	 *  vh			:[in]float 模拟器VH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetVh(float vh);

	/*
	 *  GetVh
	 *  功能: 获取设置模拟器VH值
	 *  输入：
	 *  vh			:[out]float& 模拟器VH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVh(float& vh);

	/*
	 *  SetVl
	 *  功能: 设置模拟器VL值
	 *  输入：
	 *  vl			:[in]float 模拟器VL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetVl(float vl);

	/*
	 *  GetVl
	 *  功能: 获取设置模拟器VL值
	 *  输入：
	 *  vl			:[out]float& 模拟器VL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVl(float& vl);

	/*
	 *  SetEffchg
	 *  功能: 设置模拟器EFFCHG值
	 *  输入：
	 *  effchg		:[in]float 模拟器EFFCHG值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetEffchg(float effchg);

	/*
	 *  GetEffchg
	 *  功能: 获取设置模拟器EFFCHG值
	 *  输入：
	 *  effchg		:[out]float& 模拟器EFFCHG值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetEffchg(float& effchg);

	/*
	 *  SetEffdsg
	 *  功能: 设置模拟器EFFDSG值
	 *  输入：
	 *  effdsg		:[in]float 模拟器EFFDSG值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetEffdsg(float effdsg);

	/*
	 *  GetEffdsg
	 *  功能: 获取设置模拟器EFFDSG值
	 *  输入：
	 *  effdsg		:[out]float& 模拟器EFFDSG值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetEffdsg(float& effdsg);

	/*
	 *  SetBoh
	 *  功能: 设置模拟器BOH值
	 *  输入：
	 *  boh			:[in]float 模拟器BOH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetBoh(float boh);

	/*
	 *  GetBoh
	 *  功能: 获取设置模拟器BOH值
	 *  输入：
	 *  boh			:[out]float& 模拟器BOH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetBoh(float& boh);

	/*
	 *  SetBol
	 *  功能: 设置模拟器BOL值
	 *  输入：
	 *  bol			:[in]float 模拟器BOL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetBol(float bol);

	/*
	 *  GetBol
	 *  功能: 获取设置模拟器BOL值
	 *  输入：
	 *  bol			:[out]float& 模拟器BOL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetBol(float& bol);

	/*
	 *  SetVoh
	 *  功能: 设置模拟器VOH值
	 *  输入：
	 *  voh			:[in]float 模拟器VOH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetVoh(float voh);

	/*
	 *  GetVoh
	 *  功能: 获取设置模拟器VOH值
	 *  输入：
	 *  voh			:[out]float& 模拟器VOH值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVoh(float& voh);

	/*
	 *  SetVol
	 *  功能: 设置模拟器VOL值
	 *  输入：
	 *  vol			:[in]float 模拟器VOL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetVol(float vol);

	/*
	 *  GetVol
	 *  功能: 获取设置模拟器VOL值
	 *  输入：
	 *  vol			:[out]float& 模拟器VOL值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetVol(float& vol);

    int SetMode(float battery);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
	BattPara m_battPara;
	int m_flag;
	int count1;
};

#endif /* CHROMABATTERY_H_ */
