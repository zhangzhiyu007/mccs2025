/*
 * ChromaDcLoad.h
 *
 *  创建日期: 2016-6-20
 *  作          者: 刘海涛
 */

#ifndef CHROMADCLOAD_H_
#define CHROMADCLOAD_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"

class Com;
class TcpClient;

class ChromaDcLoad
{
public:
	ChromaDcLoad();
	virtual ~ChromaDcLoad();

	//设置串口
	void SetCom(Com* com);
	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  Read
	 *  功能: 读取chroma直流负载数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

	/*
	 *  Preset
	 *  功能: 设置chroma直流负载命令
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Preset(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  Chroma63204Read
	 *  功能: 读取chroma63204A-150-400数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma63204Read(Device::SlaveDev* dev);

	/*
	 *  Chroma63204Preset
	 *  功能: 设置chroma63204A-150-400数据
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma63204Preset(Device::SlaveDev* dev);

	/*
	 *  Chroma63200Read
	 *  功能: 读取chroma63200A系列直流负载
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma63200Read(Device::SlaveDev* dev);

	/*
	 *  Chroma63200Preset
	 *  功能: 设置chroma63200A系列直流负载
	 *  输入：
	 *  dev    :[in]Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Chroma63200Preset(Device::SlaveDev* dev);

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
	 *  功能: 获取实时输出功率值
	 *  输入：
	 *  power		:[out]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetPower(float& power);

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
	 *  SetRemAndLoc
	 *  功能: 设置本地远程
	 *  输入：
	 *  vaule		:[in]int 本地远程1:远程/2:本地
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetRemAndLoc(int value);

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
	 *  mode		:[in]int 工作模式 1：CC/2：CV/3：CR/4：CP/5：CCD /6：CRD
	 *  modeGear	:[in]int 工作模式档位（1：低/2：中/3：高（所有模式必须下发））
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetOutMode(int mode, int modeGear);

	/*
	 *  GetOutMode
	 *  功能: 获取设置输出工作模式
	 *  输入：
	 *  mode		:[out]int& 工作模式 1：CC/2：CV/3：CR/4：CP/5：CCD /6：CRD
	 *  modeGear	:[out]int& 工作模式档位（1：低/2：中/3：高（所有模式必须下发））
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetOutMode(int& mode, int& modeGear);

	/*
	 *  ClearFault
	 *  功能: 清除故障
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int ClearFault();
    //CC模式
	/*
	 *  SetCCL1Cur
	 *  功能: 设置CC模式L1电流值
	 *  输入：
	 *  current		:[in]float 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCL1Cur(float current);

	/*
	 *  GetCCL1Cur
	 *  功能: 获取设置CC模式L1电流值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCL1Cur(float& current);

	/*
	 *  GetCCL1CurMax
	 *  功能: 获取设置CC模式L1电流值最大值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCL1CurMax(float& current);

	/*
	 *  GetCCL1CurMin
	 *  功能: 获取设置CC模式L1电流值最小值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCL1CurMin(float& current);

	/*
	 *  SetCCL2Cur
	 *  功能: 设置CC模式L2电流值
	 *  输入：
	 *  current		:[in]float 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCL2Cur(float current);

	/*
	 *  GetCCL2Cur
	 *  功能: 获取设置CC模式L2电流值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCL2Cur(float& current);

	/*
	 *  GetCCL2CurMax
	 *  功能: 获取设置CC模式L2电流值最大值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCL2CurMax(float& current);

	/*
	 *  GetCCL2CurMin
	 *  功能: 获取设置CC模式L2电流值最小值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCL2CurMin(float& current);

	/*
	 *  SetCCCurRisSlope
	 *  功能: 设置CC模式电流上升斜率
	 *  输入：
	 *  rise		:[in]float 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCCurRisSlope(float rise);

	/*
	 *  GetCCCurRisSlope
	 *  功能: 获取设置CC模式电流上升斜率
	 *  输入：
	 *  rise		:[out]float& 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCCurRisSlope(float& rise);

	/*
	 *  GetCCCurRisSlopeMax
	 *  功能: 获取设置CC模式电流上升斜率最大值
	 *  输入：
	 *  rise		:[out]float& 上升斜率值最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCCurRisSlopeMax(float& rise);

	/*
	 *  GetCCCurRisSlopeMin
	 *  功能: 获取设置CC模式电流上升斜率最小值
	 *  输入：
	 *  rise		:[out]float& 上升斜率值最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCCurRisSlopeMin(float& rise);

	/*
	 *  SetCCCurFallSlope
	 *  功能: 设置CC模式电流下降斜率
	 *  输入：
	 *  fall		:[in]float 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCCurFallSlope(float fall);

	/*
	 *  GetCCCurFallSlope
	 *  功能: 获取设置CC模式电流下降斜率
	 *  输入：
	 *  fall		:[out]float& 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCCurFallSlope(float& fall);

	/*
	 *  GetCCCurFallSlopeMax
	 *  功能: 获取设置CC模式电流下降斜率最大值
	 *  输入：
	 *  fall		:[out]float& 下降斜率值最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCCurFallSlopeMax(float& fall);

	/*
	 *  GetCCCurFallSlopeMin
	 *  功能: 获取设置CC模式电流下降斜率最小值
	 *  输入：
	 *  fall		:[out]float& 下降斜率值最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCCurFallSlopeMin(float& fall);

	/*
	 *  SetCCVolGear
	 *  功能: 设置CC模式电压量测档位
	 *  输入：
	 *  gear		:[in]int 电压量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCVolGear(int gear);

	/*
	 *  GetCCVolGear
	 *  功能: 获取设置CC模式电压量测档位
	 *  输入：
	 *  gear		:[out]int& 电压量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCVolGear(int& gear);

    //CCD模式
	/*
	 *  SetCCDL1Cur
	 *  功能: 设置CCD模式L1电流值
	 *  输入：
	 *  current		:[in]float 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCDL1Cur(float current);

	/*
	 *  GetCCDL1Cur
	 *  功能: 获取设置CCD模式L1电流值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDL1Cur(float& current);

	/*
	 *  GetCCDL1CurMax
	 *  功能: 获取设置CCD模式L1电流值最大值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDL1CurMax(float& current);

	/*
	 *  GetCCDL1CurMin
	 *  功能: 获取设置CCD模式L1电流值最小值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDL1CurMin(float& current);

	/*
	 *  SetCCDL2Cur
	 *  功能: 设置CCD模式L2电流值
	 *  输入：
	 *  current		:[in]float 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCDL2Cur(float current);

	/*
	 *  GetCCDL2Cur
	 *  功能: 获取设置CCD模式L2电流值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDL2Cur(float& current);

	/*
	 *  GetCCDL2CurMax
	 *  功能: 获取设置CCD模式L2电流值最大值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDL2CurMax(float& current);

	/*
	 *  GetCCDL2CurMin
	 *  功能: 获取设置CCD模式L2电流值最小值
	 *  输入：
	 *  current		:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDL2CurMin(float& current);

	/*
	 *  SetCCDT1
	 *  功能: 设置CCD模式T1
	 *  输入：
	 *  time		:[in]float 时间
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCDT1(float time);

	/*
	 *  GetCCDT1
	 *  功能: 获取设置CCD模式T1
	 *  输入：
	 *  time		:[out]float& 时间
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDT1(float& time);

	/*
	 *  GetCCDT1Max
	 *  功能: 获取设置CCD模式T1最大值
	 *  输入：
	 *  time		:[out]float& 时间值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDT1Max(float& time);

	/*
	 *  GetCCDT1Min
	 *  功能: 获取设置CCD模式T1最小值
	 *  输入：
	 *  time		:[out]float& 时间值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDT1Min(float& time);

	/*
	 *  SetCCDT2
	 *  功能: 设置CCD模式T2
	 *  输入：
	 *  time		:[in]float 时间
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCDT2(float time);

	/*
	 *  GetCCDT2
	 *  功能: 获取设置CCD模式T2
	 *  输入：
	 *  time		:[out]float& 时间
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDT2(float& time);

	/*
	 *  GetCCDT2Max
	 *  功能: 获取设置CCD模式T2最大值
	 *  输入：
	 *  time		:[out]float& 时间值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDT2Max(float& time);

	/*
	 *  GetCCDT2Min
	 *  功能: 获取设置CCD模式T2最小值
	 *  输入：
	 *  time		:[out]float& 时间值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDT2Min(float& time);

	/*
	 *  SetCCDRepeat
	 *  功能: 设置CCD模式重复次数
	 *  输入：
	 *  num		:[in]int 重复次数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCDRepeat(int num);

	/*
	 *  GetCCDRepeat
	 *  功能: 获取设置CCD模式重复次数
	 *  输入：
	 *  num		:[out]int& 重复次数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDRepeat(int& num);

	/*
	 *  GetCCDRepeatMax
	 *  功能: 获取设置CCD模式重复次数最大值
	 *  输入：
	 *  num		:[out]int& 重复次数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDRepeatMax(int& num);

	/*
	 *  GetCCDRepeatMin
	 *  功能: 获取设置CCD模式重复次数最小值
	 *  输入：
	 *  num		:[out]int& 重复次数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDRepeatMin(int& num);

	/*
	 *  SetCCDCurRisSlope
	 *  功能: 设置CCD模式电流上升斜率
	 *  输入：
	 *  rise		:[in]float 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCDCurRisSlope(float rise);

	/*
	 *  GetCCDCurRisSlope
	 *  功能: 获取设置CCD模式电流上升斜率
	 *  输入：
	 *  rise		:[out]float& 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDCurRisSlope(float& rise);

	/*
	 *  GetCCDCurRisSlopeMax
	 *  功能: 获取设置CCD模式上升斜率最大值
	 *  输入：
	 *  rise		:[out]float& 上升斜率最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDCurRisSlopeMax(float& rise);

	/*
	 *  GetCCDCurRisSlopeMin
	 *  功能: 获取设置CCD模式上升斜率最小值
	 *  输入：
	 *  rise		:[out]flaot& 上升斜率最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDCurRisSlopeMin(float& rise);

	/*
	 *  SetCCDCurFallSlope
	 *  功能: 设置CCD模式电流下降斜率
	 *  输入：
	 *  fall		:[in]float 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCDCurFallSlope(float fall);

	/*
	 *  GetCCDCurFallSlope
	 *  功能: 获取设置CCD模式电流下降斜率
	 *  输入：
	 *  fall		:[out]float& 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDCurFallSlope(float& fall);

	/*
	 *  GetCCDCurFallSlopeMax
	 *  功能: 获取设置CCD模式下降斜率最大值
	 *  输入：
	 *  fall		:[out]float& 下降斜率最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDCurFallSlopeMax(float& fall);

	/*
	 *  GetCCDCurFallSlopeMin
	 *  功能: 获取设置CCD模式下降斜率最小值
	 *  输入：
	 *  fall		:[out]flaot& 下降斜率最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDCurFallSlopeMin(float& fall);

	/*
	 *  SetCCDVolGear
	 *  功能: 设置CCD模式电压量测档位
	 *  输入：
	 *  gear		:[in]int 电压量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCCDVolGear(int gear);

	/*
	 *  GetCCDVolGear
	 *  功能: 获取设置CCD模式电压量测档位
	 *  输入：
	 *  gear		:[out]int& 电压量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCCDVolGear(int& gear);

    //CR模式
	/*
	 *  SetCRL1R
	 *  功能: 设置CR模式L1电阻值
	 *  输入：
	 *  resistance 	:[in]float 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRL1R(float resistance);

	/*
	 *  GetCRL1R
	 *  功能: 获取设置CR模式L1电阻值
	 *  输入：
	 *  resistance	:[out]float& 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRL1R(float& resistance);

	/*
	 *  GetCRL1RMax
	 *  功能: 获取设置CR模式L1电阻最大值
	 *  输入：
	 *  resistance	:[out]float& L1电阻最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRL1RMax(float& resistance);

	/*
	 *  GetCRL1RMin
	 *  功能: 获取设置CR模式L1电阻最小值
	 *  输入：
	 *  resistance	:[out]flaot& L1电阻最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRL1RMin(float& resistance);

	/*
	 *  SetCRL2R
	 *  功能: 设置CR模式L2电阻值
	 *  输入：
	 *  resistance 	:[in]float 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRL2R(float resistance);

	/*
	 *  GetCRL2R
	 *  功能: 获取设置CR模式L2电阻值
	 *  输入：
	 *  resistance	:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRL2R(float& resistance);

	/*
	 *  GetCRL2RMax
	 *  功能: 获取设置CR模式L2电阻最大值
	 *  输入：
	 *  resistance	:[out]float& L2电阻最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRL2RMax(float& resistance);

	/*
	 *  GetCRL2RMin
	 *  功能: 获取设置CR模式L2电阻最小值
	 *  输入：
	 *  resistance	:[out]flaot& L2电阻最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRL2RMin(float& resistance);

	/*
	 *  SetCRCurRisSlope
	 *  功能: 设置CR模式电流上升斜率
	 *  输入：
	 *  rise		:[in]float 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRCurRisSlope(float rise);

	/*
	 *  GetCRCurRisSlope
	 *  功能: 获取设置CR模式电流上升斜率
	 *  输入：
	 *  rise		:[out]float& 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRCurRisSlope(float& rise);

	/*
	 *  GetCRCurRisSlopeMax
	 *  功能: 获取设置CR模式上升斜率最大值
	 *  输入：
	 *  rise		:[out]float& 上升斜率最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRCurRisSlopeMax(float& rise);

	/*
	 *  GetCRCurRisSlopeMin
	 *  功能: 获取设置CR模式上升斜率最小值
	 *  输入：
	 *  rise		:[out]flaot& 上升斜率最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRCurRisSlopeMin(float& rise);

	/*
	 *  SetCRCurFallSlope
	 *  功能: 设置CR模式电流下降斜率
	 *  输入：
	 *  fall		:[in]float 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRCurFallSlope(float fall);

	/*
	 *  GetCRCurFallSlope
	 *  功能: 获取设置CR模式电流下降斜率
	 *  输入：
	 *  fall		:[out]float& 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRCurFallSlope(float& fall);

	/*
	 *  GetCRCurFallSlopeMax
	 *  功能: 获取设置CR模式下降斜率最大值
	 *  输入：
	 *  fall		:[out]float& 下降斜率最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRCurFallSlopeMax(float& fall);

	/*
	 *  GetCRCurFallSlopeMin
	 *  功能: 获取设置CR模式下降斜率最小值
	 *  输入：
	 *  fall		:[out]flaot& 下降斜率最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRCurFallSlopeMin(float& fall);

	/*
	 *  SetCRCurGear
	 *  功能: 设置CR模式电流量测档位
	 *  输入：
	 *  gear		:[in]int 电流量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRCurGear(int gear);

	/*
	 *  GetCRCurGear
	 *  功能: 获取设置CR模式电流量测档位
	 *  输入：
	 *  gear		:[out]int& 电流量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRCurGear(int& gear);

    //CRD模式
	/*
	 *  SetCRDL1R
	 *  功能: 设置CRD模式L1电阻值
	 *  输入：
	 *  resistance 	:[in]float 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRDL1R(float resistance);

	/*
	 *  GetCRDL1R
	 *  功能: 获取设置CRD模式L1电阻值
	 *  输入：
	 *  resistance	:[out]float& 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDL1R(float& resistance);

	/*
	 *  GetCRDL1RMax
	 *  功能: 获取设置CRD模式L1电阻最大值
	 *  输入：
	 *  resistance	:[out]float& 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDL1RMax(float& resistance);

	/*
	 *  GetCRDL1RMin
	 *  功能: 获取设置CRD模式L1电阻最小值
	 *  输入：
	 *  resistance	:[out]float& 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDL1RMin(float& resistance);

	/*
	 *  SetCRDL2R
	 *  功能: 设置CRD模式L2电阻值
	 *  输入：
	 *  resistance 	:[in]float 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRDL2R(float resistance);

	/*
	 *  GetCRDL2R
	 *  功能: 获取设置CRD模式L2电阻值
	 *  输入：
	 *  resistance	:[out]float& 电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDL2R(float& resistance);

	/*
	 *  GetCRDL2R
	 *  功能: 获取设置CRD模式L2电阻最大值
	 *  输入：
	 *  resistance	:[out]float& 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDL2RMax(float& resistance);

	/*
	 *  GetCRDL2RMin
	 *  功能: 获取设置CRD模式L2电阻最小值
	 *  输入：
	 *  resistance	:[out]float& 电阻值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDL2RMin(float& resistance);

	/*
	 *  SetCRDT1
	 *  功能: 设置CRD模式T1
	 *  输入：
	 *  time		:[in]float 时间
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRDT1(float time);

	/*
	 *  GetCRDT1
	 *  功能: 获取设置CRD模式T1
	 *  输入：
	 *  time		:[out]float& 时间
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDT1(float& time);

	/*
	 *  GetCRDT1Max
	 *  功能: 获取设置CRD模式T1最大值
	 *  输入：
	 *  time		:[out]float& 时间值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDT1Max(float& time);

	/*
	 *  GetCRDT1Min
	 *  功能: 获取设置CRD模式T1最小值
	 *  输入：
	 *  time		:[out]float& 时间值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDT1Min(float& time);

	/*
	 *  SetCRDT2
	 *  功能: 设置CRD模式T2
	 *  输入：
	 *  time		:[in]float 时间
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRDT2(float time);

	/*
	 *  GetCRDT2
	 *  功能: 获取设置CRD模式T2
	 *  输入：
	 *  time		:[out]float& 时间
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDT2(float& time);

	/*
	 *  GetCRDT2Max
	 *  功能: 获取设置CRD模式T2最大值
	 *  输入：
	 *  time		:[out]float& 时间值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDT2Max(float& time);

	/*
	 *  GetCRDT2Min
	 *  功能: 获取设置CRD模式T2最小值
	 *  输入：
	 *  time		:[out]float& 时间值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDT2Min(float& time);

	/*
	 *  SetCRDRepeat
	 *  功能: 设置CRD模式重复次数
	 *  输入：
	 *  num		:[in]int 重复次数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRDRepeat(int num);

	/*
	 *  GetCRDRepeat
	 *  功能: 获取设置CRD模式重复次数
	 *  输入：
	 *  num		:[out]int& 重复次数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDRepeat(int& num);

	/*
	 *  GetCRDRepeatMax
	 *  功能: 获取设置CRD模式重复次数最大值
	 *  输入：
	 *  num		:[out]int& 重复次数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDRepeatMax(int& num);

	/*
	 *  GetCRDRepeatMin
	 *  功能: 获取设置CRD模式重复次数最小值
	 *  输入：
	 *  num		:[out]int& 重复次数
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDRepeatMin(int& num);

	/*
	 *  SetCRDCurRisSlope
	 *  功能: 设置CRD模式电流上升斜率
	 *  输入：
	 *  rise		:[in]float 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRDCurRisSlope(float rise);

	/*
	 *  GetCRDCurRisSlope
	 *  功能: 获取设置CRD模式电流上升斜率
	 *  输入：
	 *  rise		:[out]float& 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDCurRisSlope(float& rise);

	/*
	 *  GetCRDCurRisSlopeMax
	 *  功能: 获取设置CRD模式上升斜率最大值
	 *  输入：
	 *  rise		:[out]float& 上升斜率最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDCurRisSlopeMax(float& rise);

	/*
	 *  GetCRDCurRisSlopeMin
	 *  功能: 获取设置CRD模式上升斜率最小值
	 *  输入：
	 *  rise		:[out]flaot& 上升斜率最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDCurRisSlopeMin(float& rise);

	/*
	 *  SetCRDCurFallSlope
	 *  功能: 设置CRD模式电流下降斜率
	 *  输入：
	 *  fall		:[in]float 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRDCurFallSlope(float fall);

	/*
	 *  GetCRDCurFallSlope
	 *  功能: 获取设置CRD模式电流下降斜率
	 *  输入：
	 *  fall		:[out]float& 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDCurFallSlope(float& fall);

	/*
	 *  GetCRDCurFallSlopeMax
	 *  功能: 获取设置CRD模式下降斜率最大值
	 *  输入：
	 *  fall		:[out]float& 下降斜率最大值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDCurFallSlopeMax(float& fall);

	/*
	 *  GetCRDCurFallSlopeMin
	 *  功能: 获取设置CRD模式下降斜率最小值
	 *  输入：
	 *  fall		:[out]flaot& 下降斜率最小值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDCurFallSlopeMin(float& fall);

	/*
	 *  SetCRDCurGear
	 *  功能: 设置CRD模式电流量测档位
	 *  输入：
	 *  gear		:[in]int 电流量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCRDCurGear(int gear);

	/*
	 *  GetCRDCurGear
	 *  功能: 获取设置CRD模式电流量测档位
	 *  输入：
	 *  gear		:[out]int& 电流量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCRDCurGear(int& gear);

    //CV模式
	/*
	 *  SetCVL1Vol
	 *  功能: 设置CV模式L1电压值
	 *  输入：
	 *  vol		 	:[in]float 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCVL1Vol(float vol);

	/*
	 *  GetCVL1Vol
	 *  功能: 获取设置CV模式L1电压值
	 *  输入：
	 *  vol		 	:[in]float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVL1Vol(float& vol);

	/*
	 *  GetCVL1VolMax
	 *  功能: 获取设置CV模式L1电压最大值
	 *  输入：
	 *  vol		 	:[in]float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVL1VolMax(float& vol);

	/*
	 *  GetCVL1VolMin
	 *  功能: 获取设置CV模式L1电压最小值
	 *  输入：
	 *  vol		 	:[in]float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVL1VolMin(float& vol);

	/*
	 *  SetCVL2Vol
	 *  功能: 设置CV模式L2电压值
	 *  输入：
	 *  vol		 	:[in]float 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCVL2Vol(float vol);

	/*
	 *  GetCVL2Vol
	 *  功能: 获取设置CV模式L2电压值
	 *  输入：
	 *  vol		 	:[in]float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVL2Vol(float& vol);

	/*
	 *  GetCVL2VolMax
	 *  功能: 获取设置CV模式L2电压最大值
	 *  输入：
	 *  vol		 	:[in]float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVL2VolMax(float& vol);

	/*
	 *  GetCVL2VolMin
	 *  功能: 获取设置CV模式L2电压最小值
	 *  输入：
	 *  vol		 	:[in]float& 电压值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVL2VolMin(float& vol);

	/*
	 *  SetCVCurLimit
	 *  功能: 设置CV模式限电流
	 *  输入：
	 *  limitCur	:[in]float 限电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCVCurLimit(float limitCur);

	/*
	 *  GetCVCurLimit
	 *  功能: 获取设置CV模式限电流
	 *  输入：
	 *  limitCur	:[in]float& 限电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVCurLimit(float& limitCur);

	/*
	 *  GetCVCurLimitMax
	 *  功能: 获取设置CV模式限电流最大值
	 *  输入：
	 *  limitCur	:[in]float& 限电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVCurLimitMax(float& limitCur);

	/*
	 *  GetCVCurLimitMin
	 *  功能: 获取设置CV模式限电流最小值
	 *  输入：
	 *  limitCur	:[in]float& 限电流值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVCurLimitMin(float& limitCur);

	/*
	 *  SetCVVolReact
	 *  功能: 设置CV模式电压反应速度
	 *  输入：
	 *  react		:[in]int 电压反应速度(0：SLOW/1：NORMAL/2：FAST)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCVVolReact(int react);

	/*
	 *  GetCVVolReact
	 *  功能: 获取设置CV模式电压反应速度
	 *  输入：
	 *  react		:[out]int& 电压反应速度(0：SLOW/1：NORMAL/2：FAST)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVVolReact(int& react);

	/*
	 *  SetCVCurGear
	 *  功能: 设置CV模式电流量测档位
	 *  输入：
	 *  gear		:[in]int 电流量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCVCurGear(int gear);

	/*
	 *  GetCVCurGear
	 *  功能: 获取设置CV模式电流量测档位
	 *  输入：
	 *  gear		:[out]int& 电流量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCVCurGear(int& gear);

    //CP模式
	/*
	 *  SetCPL1Pow
	 *  功能: 设置CP模式L1功率值
	 *  输入：
	 *  power		:[in]float 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCPL1Pow(float power);

	/*
	 *  GetCPL1Pow
	 *  功能: 获取设置CP模式L1功率值
	 *  输入：
	 *  power		:[in]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPL1Pow(float& power);

	/*
	 *  GetCPL1PowMax
	 *  功能: 获取设置CP模式L1功率值最大值
	 *  输入：
	 *  power		:[in]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPL1PowMax(float& power);

	/*
	 *  GetCPL1PowMin
	 *  功能: 获取设置CP模式L1功率值最小值
	 *  输入：
	 *  power		:[in]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPL1PowMin(float& power);

	/*
	 *  SetCPL2Pow
	 *  功能: 设置CP模式L2功率值
	 *  输入：
	 *  power		:[in]float 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCPL2Pow(float power);

	/*
	 *  GetCPL2Pow
	 *  功能: 获取设置CP模式L2功率值
	 *  输入：
	 *  power		:[in]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPL2Pow(float& power);

	/*
	 *  GetCPL2PowMax
	 *  功能: 获取设置CP模式L2功率值最大值
	 *  输入：
	 *  power		:[in]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPL2PowMax(float& power);

	/*
	 *  GetCPL2PowMin
	 *  功能: 获取设置CP模式L2功率值最小值
	 *  输入：
	 *  power		:[in]float& 功率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPL2PowMin(float& power);

	/*
	 *  SetCPCurRisSlope
	 *  功能: 设置CP模式电流上升斜率
	 *  输入：
	 *  rise		:[in]float 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCPCurRisSlope(float rise);

	/*
	 *  GetCPCurRisSlope
	 *  功能: 获取设置CP模式电流上升斜率
	 *  输入：
	 *  rise		:[out]float& 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPCurRisSlope(float& rise);

	/*
	 *  GetCPCurRisSlopeMax
	 *  功能: 获取设置CP模式电流上升斜率最大值
	 *  输入：
	 *  rise		:[out]float& 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPCurRisSlopeMax(float& rise);

	/*
	 *  GetCPCurRisSlopeMin
	 *  功能: 获取设置CP模式电流上升斜率最小值
	 *  输入：
	 *  rise		:[out]float& 上升斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPCurRisSlopeMin(float& rise);

	/*
	 *  SetCPCurFallSlope
	 *  功能: 设置CP模式电流下降斜率
	 *  输入：
	 *  fall		:[in]float 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCPCurFallSlope(float fall);

	/*
	 *  GetCPCurFallSlope
	 *  功能: 获取设置CP模式电流下降斜率
	 *  输入：
	 *  fall		:[out]float& 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPCurFallSlope(float& fall);

	/*
	 *  GetCPCurFallSlopeMax
	 *  功能: 获取设置CP模式电流下降斜率最大值
	 *  输入：
	 *  fall		:[out]float& 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPCurFallSlopeMax(float& fall);

	/*
	 *  GetCPCurFallSlopeMin
	 *  功能: 获取设置CP模式电流下降斜率最小值
	 *  输入：
	 *  fall		:[out]float& 下降斜率值
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPCurFallSlopeMin(float& fall);

	/*
	 *  SetCPVolGear
	 *  功能: 设置CP模式电压量测档位
	 *  输入：
	 *  gear		:[in]int 电压量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int SetCPVolGear(int gear);

	/*
	 *  GetCPVolGear
	 *  功能: 获取设置CP模式电压量测档位
	 *  输入：
	 *  gear		:[out]int& 电压量测档位值(1：High/2：Middle/3：Low)
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int GetCPVolGear(int& gear);

	//变量
public:
protected:
private:
	//串口指针
	Com* m_com;
	TcpClient* m_tcp;
};

#endif /* CHROMADCLOAD_H_ */
