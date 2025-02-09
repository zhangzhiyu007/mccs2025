/**
 * * @file ECMDCDC.h
 * * @Synopsis
 * * @author Mingyu Wu
 * * @version
 * * @date 2022-03-29
 * */

#ifndef EMC_DCDC_H
#define EMC_DCDC_H
#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../protocol/modbus/ModbusTcpClient.h"

class EmcDCDC {
  public:
    EmcDCDC();
    virtual ~EmcDCDC();

    void SetTcp(TcpClient *tcpClient);

    int Read(Device::SlaveDev *dev);

    int Preset(Device::SlaveDev *dev);

  private:
    TcpClient *m_tcpClient;
};

#endif
