/**
 * @file JingShiDCDC.h
 * @Synopsis 2022年泰开实验室程序
 * @author Mingyu Wu
 * @version 1.0.0
 * @date 2022-05-26
 */

#ifndef __JINGSHI_DCDC_H
#define __JINGSHI_DCDC_H

#include "../../../db/Device.h"
#include "../../../etc/socket/TcpClient.h"

class TcpClient;

class JingshiDCDC {
  public:
    JingshiDCDC();
    virtual ~JingshiDCDC();

    void SetTcp(TcpClient *tcpClient);

    int Read(Device::SlaveDev *dev);

    int Preset(Device::SlaveDev *dev);

  private:
    TcpClient *m_tcpClient;
};

#endif // !__JINGSHI_DCDC_H
