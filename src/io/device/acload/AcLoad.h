/*
 * AcLoad.h
 *
 *  Created on: 2013-10-21
 *      Author: liuhaitao
 */

#ifndef ACLOAD_H_
#define ACLOAD_H_
#include "../../../db/Device.h"
#include "VilvaLoad.h"

class Com;
class TcpClient;

class AcLoad {
  public:
    AcLoad();
    virtual ~AcLoad();
    // 保存串口指针
    void SetCom(Com *com);

    // 保存控制串口指针
    void SetCtrlCom(Com *ctrlCom);

    // 设置TcpClient指针
    void SetTcp(TcpClient *tcpClient);

    /*
     *  Read
     *  功能: 读取交流负载数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev);

    /*
     *  Preset
     *  功能: 设置P交流负载命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Preset(Device::SlaveDev *dev);

  protected:
  private:
    int RlcRead(Device::SlaveDev *dev);

    int RlcPreset(Device::SlaveDev *dev);

    int AcFaultSimulatorRead(Device::SlaveDev *dev);

    int AcFaultSimulatorPreset(Device::SlaveDev *dev);

    /*
     *  PvAcLoadRead
     *  功能: 读取菊水皇家交流负载数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int PvAcLoadRead(Device::SlaveDev *dev);

    /*
     *  PvAcLoadPreset
     *  功能: 设置菊水皇家交流负载命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int PvAcLoadPreset(Device::SlaveDev *dev);

    /*
     *  VilvaLoadRead
     *  功能: 读取威尔华交流负载数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int VilvaLoadRead(Device::SlaveDev *dev);

    /*
     *  VilvaLoadPreset
     *  功能: 设置威尔华交流负载命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int VilvaLoadPreset(Device::SlaveDev *dev);

    /*
     *  ShWenShunLoadRead
     *  功能: 读取上海文顺交流负载数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ShWenShunLoadRead(Device::SlaveDev *dev);

    /*
     *  ShWenShunLoadPreset
     *  功能: 设置上海文顺交流负载命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ShWenShunLoadPreset(Device::SlaveDev *dev);

    /*
     *  ChromaLoadRead
     *  功能: 读取Chroma交流负载数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ChromaLoadRead(Device::SlaveDev *dev);

    /*
     *  ChromaLoadPreset
     *  功能: 设置Chroma交流负载命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ChromaLoadPreset(Device::SlaveDev *dev);

    int AAC3810LoadRead(Device::SlaveDev *dev);

    int AAC3810LoadPreset(Device::SlaveDev *dev);

    int ChargePileControlRead(Device::SlaveDev *dev);

    int ChargePileControlPreset(Device::SlaveDev *dev);

    // 变量
  public:
  protected:
  private:
    Com *m_com;
    Com *m_ctrlCom;
    TcpClient *m_tcpClient; // 以太网
    VilvaLoad m_ac;
};

#endif /* ACLOAD_H_ */
