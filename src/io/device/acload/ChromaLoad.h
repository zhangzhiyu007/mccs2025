/*
 * ChromaLoad.h
 *
 *  创建日期: 2016-6-20
 *  作          者: 刘海涛
 */

#ifndef CHROMALOAD_H_
#define CHROMALOAD_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

class ChromaLoad
{
public:
	ChromaLoad();
	virtual ~ChromaLoad();

	//设置串口
	void SetCom(Com* com);
	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取chroma交流负载数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置chroma交流负载命令
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:
	/*
	 *  Chroma63804Read
	 *  功能: 读取chroma63804数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma63804Read(Device::SlaveDev* dev);

	/*
	 *  Chroma63204Preset
	 *  功能: 设置chroma63804数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma63804Preset(Device::SlaveDev* dev);

	/*
	 *  Chroma63800Read
	 *  功能: 读取chroma63800系列交流负载
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma63800Read(Device::SlaveDev* dev);

	/*
	 *  Chroma63800Preset
	 *  功能: 设置chroma63800系列交流负载
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma63800Preset(Device::SlaveDev* dev);

    //chroma通讯协议
    //实时量
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
    int GetVol(float& voltage);

	/*
	 *  GetPower
	 *  功能: 获取实时输出有功功率值
	 *  输入：
	 *  power		:[out]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPower(float& power);

	/*
	 *  GetQPower
	 *  功能: 获取实时输出无功功率值
	 *  输入：
	 *  qPower		:[out]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetQPower(float& qPower);

	/*
	 *  GetSPower
	 *  功能: 获取实时输出视在功率值
	 *  输入：
	 *  sPower		:[out]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetSPower(float& sPower);

	/*
	 *  GetFreq
	 *  功能: 获取实时输出频率值
	 *  输入：
	 *  freq		:[out]float& 频率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetFreq(float& freq);

	/*
	 *  GetPF
	 *  功能: 获取实时输出功率因数值
	 *  输入：
	 *  pf			:[out]float& 功率因数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPF(float& pf);

	/*
	 *  GetFault
	 *  功能: 获取实时故障码
	 *  输入：
	 *  fault		:[out]int& 故障码
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetFault(int& fault);

    //设置值
	/*
	 *  SetStart
	 *  功能: 设置启动停止
	 *  输入：
	 *  runSta		:[in]int 启动停止 0:停止/1:启动
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetStart(int runSta);

	/*
	 *  GetStart
	 *  功能: 获取设置启动停止
	 *  输入：
	 *  runSta		:[out]int& 启动停止
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetStart(int& runSta);

	/*
	 *  SetOutMode
	 *  功能: 设置输出工作模式
	 *  输入：
	 *  mode		:[in]int 工作模式 1：CC/2：CP/3：CR/4：RLC/5：RLC定功率/6：RLC浪涌电流
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetOutMode(int mode);

	/*
	 *  GetOutMode
	 *  功能: 获取设置输出工作模式
	 *  输入：
	 *  mode		:[out]int& 工作模式 1：CC/2：CP/3：CR/4：RLC/5：RLC定功率/6：RLC浪涌电流
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetOutMode(int& mode);

	/*
	 *  SetScirCuit
	 *  功能: 设置短路模拟
	 *  输入：
	 *  value		:[in]int 短路模拟模式 0：关闭短路模拟/1：开启短路模拟（CC、CP、CR下可以执行）
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetScirCuit(int value);

	/*
	 *  GetScirCuit
	 *  功能: 获取设置短路模拟
	 *  输入：
	 *  value		:[in]int& 短路模拟模式 0：关闭短路模拟/1：开启短路模拟（CC、CP、CR下可以执行）
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetScirCuit(int& value);

	/*
	 *  ClearFault
	 *  功能: 清除故障
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ClearFault();

    //CC
	/*
	 *  SetCCCur
	 *  功能: 设置CC模式电流值
	 *  输入：
	 *  current		:[in]float 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCCur(float current);

	/*
	 *  SetIpMax
	 *  功能: 设置IpMax
	 *  输入：
	 *  ipMax		:[in]float ipMax值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetIpMax(float ipMax);

	/*
	 *  SetIrmsMax
	 *  功能: 设置IrmsMax
	 *  输入：
	 *  IrmsMax		:[in]float IrmsMax值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetIrmsMax(float IrmsMax);

	/*
	 *  GetCCCur
	 *  功能: 获取设置CC模式电流值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCCur(float& current);

	/*
	 *  GetIpMax
	 *  功能: 获取设置IpMax
	 *  输入：
	 *  ipMax		:[out]float& ipMax值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetIpMax(float& ipMax);

	/*
	 *  GetIrmsMax
	 *  功能: 获取设置IrmsMax
	 *  输入：
	 *  IrmsMax		:[out]float& IrmsMax值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetIrmsMax(float& IrmsMax);

    //CP
	/*
	 *  SetCPPow
	 *  功能: 设置CP模式功率值
	 *  输入：
	 *  power		:[in]float 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCPPow(float power);

	/*
	 *  GetCPPow
	 *  功能: 获取设置CP模式功率值
	 *  输入：
	 *  power		:[in]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPPow(float& power);

    //CR
	/*
	 *  SetCRRes
	 *  功能: 设置CR模式电阻值
	 *  输入：
	 *  res			:[in]float 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRRes(float res);

	/*
	 *  GetCRRes
	 *  功能: 获取设置CR模式电阻值
	 *  输入：
	 *  res			:[in]float& 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRRes(float& res);

    //RLC
	/*
	 *  SetRLCC
	 *  功能: 设置RLC模式电容值
	 *  输入：
	 *  cap			:[in]float 电容值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetRLCC(float cap);

	/*
	 *  GetRLCC
	 *  功能: 获取设置RLC模式电容值
	 *  输入：
	 *  cap			:[in]float& 电容值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetRLCC(float& cap);

	/*
	 *  SetRLCLs
	 *  功能: 设置RLC模式Ls值
	 *  输入：
	 *  ls			:[in]float Ls值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetRLCLs(float ls);

	/*
	 *  GetRLCLs
	 *  功能: 获取设置RLC模式Ls值
	 *  输入：
	 *  ls			:[in]float& Ls值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetRLCLs(float& ls);

	/*
	 *  SetRLCRl
	 *  功能: 设置RLC模式Rl值
	 *  输入：
	 *  rl			:[in]float Rl值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetRLCRl(float rl);

	/*
	 *  GetRLCRl
	 *  功能: 获取设置RLC模式Rl值
	 *  输入：
	 *  rl			:[in]float& Rl值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetRLCRl(float& rl);

	/*
	 *  SetRLCRs
	 *  功能: 设置RLC模式Rs值
	 *  输入：
	 *  rs			:[in]float Rs值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetRLCRs(float rs);

	/*
	 *  GetRLCRs
	 *  功能: 获取设置RLC模式Rs值
	 *  输入：
	 *  rs			:[in]float& Rs值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetRLCRs(float& rs);

    //RLCCP
	/*
	 *  SetRLCCPPow
	 *  功能: 设置RLC定功率模式功率值
	 *  输入：
	 *  power		:[in]float 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetRLCCPPow(float power);

	/*
	 *  GetRLCCPPow
	 *  功能: 获取设置RLC定功率模式功率值
	 *  输入：
	 *  power		:[in]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetRLCCPPow(float& power);

	/*
	 *  SetRLCCPPf
	 *  功能: 设置RLC定功率模式功率因数
	 *  输入：
	 *  pf			:[in]float 功率因数值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetRLCCPPf(float pf);

	/*
	 *  GetRLCCPPf
	 *  功能: 获取设置RLC定功率模式功率因数
	 *  输入：
	 *  pf			:[in]float& 功率因数值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetRLCCPPf(float& pf);

    //INR(RLC浪涌电流)
	/*
	 *  SetINRC
	 *  功能: 设置INR(RLC浪涌电流)模式电容值
	 *  输入：
	 *  cap			:[in]float 电容值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetINRC(float cap);

	/*
	 *  GetINRC
	 *  功能: 获取设置INR(RLC浪涌电流)模式电容值
	 *  输入：
	 *  cap			:[in]float& 电容值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetINRC(float& cap);

	/*
	 *  SetINRLs
	 *  功能: 设置INR(RLC浪涌电流)模式Ls值
	 *  输入：
	 *  ls			:[in]float Ls值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetINRLs(float ls);

	/*
	 *  GetINRLs
	 *  功能: 获取设置INR(RLC浪涌电流)模式Ls值
	 *  输入：
	 *  ls			:[in]float& Ls值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetINRLs(float& ls);

	/*
	 *  SetINRRl
	 *  功能: 设置INR(RLC浪涌电流)模式Rl值
	 *  输入：
	 *  rl			:[in]float Rl值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetINRRl(float rl);

	/*
	 *  GetINRRl
	 *  功能: 获取设置INR(RLC浪涌电流)模式Rl值
	 *  输入：
	 *  rl			:[in]float& Rl值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetINRRl(float& rl);

	/*
	 *  SetINRRs
	 *  功能: 设置INR(RLC浪涌电流)模式Rs值
	 *  输入：
	 *  rs			:[in]float Rs值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetINRRs(float rs);

	/*
	 *  GetINRRs
	 *  功能: 获取设置INR(RLC浪涌电流)模式Rs值
	 *  输入：
	 *  rs			:[in]float& Rs值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetINRRs(float& rs);

	/*
	 *  SetINRPhas
	 *  功能: 设置INR(RLC浪涌电流)模式启动相位值
	 *  输入：
	 *  phas		:[in]float 启动相位值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetINRPhas(float phas);

	/*
	 *  GetINRPhas
	 *  功能: 获取INR(RLC浪涌电流)模式启动相位值
	 *  输入：
	 *  phas		:[in]float& 启动相位值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetINRPhas(float& phas);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* CHROMALOAD_H_ */
