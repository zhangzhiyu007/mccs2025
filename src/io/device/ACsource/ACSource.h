/*
 * ACSource.h
 *
 *  Created on: 2013-10-11
 *      Author: liuhaitao
 */

#ifndef ACSOURCE_H_
#define ACSOURCE_H_
#include "../../../db/Device.h"
#include "ChromaSource.h"

class Com;
class TcpClient;

/*名称：ACSource
 *描述：用于交流源控制与数据采集
 *用法：
 *创建人：lht
 *创建日期：2013-10-11
 */
class ACSource {
  public:
    ACSource();
    virtual ~ACSource();

    // 保存串口指针
    void SetCom(Com *com);

    // 设置TcpClient指针
    void SetTcp(TcpClient *tcpClient);

    /*
     *  Read
     *  功能: 读取交流电源数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev);

    /*
     *  Preset
     *  功能: 设置交流电源命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Preset(Device::SlaveDev *dev);

  protected:
  private:
    /*
     *  ApcACSourceRead
     *  功能: 读取艾普斯交流电源数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ApcACSourceRead(Device::SlaveDev *dev);

    /*
     *  ApcACSourcePreset
     *  功能: 设置艾普斯交流电源命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ApcACSourcePreset(Device::SlaveDev *dev);

    /*
     *  ChromaSourceRead
     *  功能: 读取chroma交流电源数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ChromaSourceRead(Device::SlaveDev *dev);

    /*
     *  ChromaSourcePreset
     *  功能: 设置chroma交流电源命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int ChromaSourcePreset(Device::SlaveDev *dev);

    int ANGridSImRead(Device::SlaveDev *dev);

	int ANGridSImPreset(Device::SlaveDev *dev);
    // 变量
  public:
  protected:
  private:
    Com *m_com;
    TcpClient *m_tcpClient; // 以太网
    ChromaSource m_chromaSource;
};

#endif /* ACSOURCE_H_ */
