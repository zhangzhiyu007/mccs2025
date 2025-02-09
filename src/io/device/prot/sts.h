/**
 * @date 2022-05-27
 * @version 1.0.0
 * @author Mingyu Wu
 * @Synopsis
 * @file sts.h
 */

#ifndef _STS_H_
#define _STS_H_
#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class STS {
  public:
    STS();
    virtual ~STS();

    void SetTcp(TcpClient *tcpClient);
    int Read(Device::SlaveDev *dev);

    int Preset(Device::SlaveDev *dev);

  private:
    TcpClient *m_tcpClient;
};

#endif // !_STS_H_
