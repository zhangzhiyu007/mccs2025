/**
 * @file TaikaiBMS.h
 * @Synopsis 泰开厂内调试用BMS
 * @author Mingyu Wu
 * @version
 * @date 2022-05-31
 */

#ifndef _TAIKAI_BMS_H_
#define _TAIKAI_BMS_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class TaikaiBMS {
  public:
    TaikaiBMS();
    virtual ~TaikaiBMS();

    void SetTcp(TcpClient *tcp_Client);

    int Read(Device::SlaveDev *dev);

    int Preset(Device::SlaveDev *dev);

  private:
    TcpClient *m_tcpClient;
};

#endif // !_TAIKAI_BMS_H_
