/**
 * @file JingshiPcs.h
 * @Synopsis
 * @author Mingyu Wu
 * @version
 * @date 2022-05-30
 */

#ifndef _JINGSHI_PCS_H
#define _JINGSHI_PCS_H

#include "../../../db/Device.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../db/MemDb.h"
#include "../../protocol/modbus/ModbusTcpClient.h"

class JingshiPcs {
  public:
    JingshiPcs();
    virtual ~JingshiPcs();

    void SetTcp(TcpClient *tcpClient);

    int Read(Device::SlaveDev *dev);

    int Preset(Device::SlaveDev *dev);

  private:
    TcpClient *m_tcpClient;
};

#endif // !_JINGSHI_PCS_H
