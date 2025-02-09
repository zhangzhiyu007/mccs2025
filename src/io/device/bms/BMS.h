/*
 * BMS.h
 *
 *  Created on: 2013-10-14
 *      Author: liuhaitao
 */

#ifndef BMS_H_
#define BMS_H_
#include "../../../db/Device.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../can/Can.h"
#include "../../com/Com.h"
#include "ChromaBattery.h"

class Can;
class Com;
class TcpClient;

class BMS {
  public:
    BMS();
    virtual ~BMS();

    //保存串口指针
    void SetCan(Can *can);

    //设置是否采用以太网通讯
    void SetIsTcp(bool isTcp);

    //保存串口指针
    void SetCom(Com *com);
    //设置TcpClient指针
    void SetTcp(TcpClient *tcpClient);

    /*
     *  Read
     *  功能: 读取BMS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  frame   :Can::FrameInfo& CAN信息帧
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev, Can::FrameInfo &frame);

    /*
     *  Read
     *  功能: 读取BMS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev);

    /*
     *  Preset
     *  功能: 设置BMS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Preset(Device::SlaveDev *dev);

  protected:
  private:
    /*
     *  Read
     *  功能: 读取峰谷源BMS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  frame   :Can::FrameInfo& CAN信息帧
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int FgyBMSRead(Device::SlaveDev *dev, Can::FrameInfo &frame);

    /*
     *  Preset
     *  功能: 设置峰谷源BMS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int FgyBMSPreset(Device::SlaveDev *dev);

    /*
     *  SunGrowBmsRead
     *  功能: 读取合肥阳光BMS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int SunGrowBmsRead(Device::SlaveDev *dev);

    /*
     *  SunGrowBmsPreset
     *  功能: 设置合肥阳光BMS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int SunGrowBmsPreset(Device::SlaveDev *dev);

    /*
     *  ChromaBattRead
     *  功能: 读取Chroma电池模拟器数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ChromaBattRead(Device::SlaveDev *dev);

    /*
     *  ChromaBattPreset
     *  功能: 设置Chroma电池模拟器命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ChromaBattPreset(Device::SlaveDev *dev);

    /*
     *  GreeBattRead
     *  功能: 读取格力电池数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int GreeBattRead(Device::SlaveDev *dev);

    /*
     *  GreeBattPreset
     *  功能: 设置格力电池命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int GreeBattPreset(Device::SlaveDev *dev);

    /*
     *  ZhedaBattRead
     *  功能: 读取浙大电池数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ZhedaBattRead(Device::SlaveDev *dev);

    // ==========================================================================
    ///    功 能       : 		BenAnBattRead  读取水系钠离子BMS
    /////    Param       : 		dev
    /////    Returns     :
    ////
    ///==========================================================================
    int BenAnBattRead(Device::SlaveDev *dev);

    // ==========================================================================
    ///    功 能       : 		VRBRead 读取全钒液流电池
    /////    Param       : 		dev
    /////    Returns     :
    ////
    ///==========================================================================
    int VRBRead(Device::SlaveDev *dev);

    int FlyWheelRead(Device::SlaveDev *dev);

    int FlyWheelPreset(Device::SlaveDev *dev);

    // 全钒液流读取
    int VanaRead(Device::SlaveDev *dev);

    // 全钒液流控制
    int VanaPreset(Device::SlaveDev *dev);

    int SolidLiRead(Device::SlaveDev *dev);

    int SolidLiPreset(Device::SlaveDev *dev);

    int TaikaiBMSRead(Device::SlaveDev *dev);

  public:
  protected:
  private:
    Can *m_can;
    Com *m_com;
    TcpClient *m_tcp;
    bool m_isTcp;
    ChromaBattery m_batt;
};

#endif /* BMS_H_ */
