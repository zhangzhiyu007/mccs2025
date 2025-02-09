/*
 * DCDC.h
 *
 *  Created on: 2013-10-14
 *      Author: liuhaitao
 */

#ifndef DCDC_H_
#define DCDC_H_
#include "../../../db/Device.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../can/Can.h"
#include "../../com/Com.h"

class Can;
class Com;

class DCDC {
  public:
    DCDC();
    virtual ~DCDC();

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
     *  功能: 读取DC/DC数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  frame   :Can::FrameInfo& CAN信息帧
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev, Can::FrameInfo frame);

    /*
     *  Read
     *  功能: 读取DC/DC数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev);

    /*
     *  Preset
     *  功能: 设置DC/DC命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Preset(Device::SlaveDev *dev);

  protected:
  private:
    int STGKRead(Device::SlaveDev *dev);

    /*
     *  STGKPreset
     *  功能: 设置盛通高科DCDC命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int STGKPreset(Device::SlaveDev *dev);
    //变量
    //
    //
    int ECMRead(Device::SlaveDev *dev);

    int ECMPreset(Device::SlaveDev *dev);
    
    int JingshiRead(Device::SlaveDev* dev);

    int JingshiPreset(Device::SlaveDev* dev);



  public:
  protected:
  private:
    Can *m_can;
    Com *m_com;
    bool m_isTcp;
    TcpClient *m_tcpClient; //以太网，用于串口服务器
};

#endif /* DCDC_H_ */
