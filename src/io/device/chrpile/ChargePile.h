/*
 * ChargePile.h
 *
 *   创建日期: 2013-10-9
 *   作          者: 马中华
 */

#ifndef CHARGEPILE_H_
#define CHARGEPILE_H_
#include "../../../db/Device.h"
#include "../../../util/Util.h"
#include "../../can/Can.h"

class Can;
/*名称:ChargePile
 *描述:充电桩，包括直流充电桩/交流充电桩
 *用法:
 *创建人:马中华
 *创建日期:2013-10-9
 */
class TcpClient;

class ChargePile {
  public:
    ChargePile();
    virtual ~ChargePile();

    // 保存串口指针
    void SetCan(Can *can);
    void SetIsTcp(bool IsTcp);

    // 设置TcpClient指针
    void SetTcp(TcpClient *tcpClient);

    /*
     *  Read
     *  功能: 读取充电桩数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  frame   :Can::FrameInfo& CAN信息帧
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev, Can::FrameInfo &frame);

    /*
     *  Preset
     *  功能: 设置充电桩命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Preset(Device::SlaveDev *dev);

  protected:
  private:
    // int ChargePileControlRead(Device::SlaveDev *dev);

    // int ChargePileControlPreset(Device::SlaveDev *dev);
    // 变量
  public:
  protected:
  private:
    TcpClient *m_tcpClient; // 以太网
    Can *m_can;             // CAN
    bool m_isTcp;
};

#endif /* CHARGEPILE_H_ */
