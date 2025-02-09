/**
 * *    文 件       :		insulationMonitor.h
 * *    功 能       :	 	Acrel  	AIM-T500 绝缘监测仪
 * *    作 者       :		Mingyu Wu
 * *    version     :		1.0.0
 * *    日 期       :		2021-04-06
 * */

#ifndef _INSULATION_MONITOR_H
#define _INSULATION_MONITOR_H
#include "../../../db/Device.h"
#include "../../../util/Check.h"

class TcpClient;

class InsulationMonitor {
  public:
    InsulationMonitor();
    virtual ~InsulationMonitor();
    void SetTcp(TcpClient *tcpClient);
    void SetIsTcp(bool isTcp);

    int Read(Device::SlaveDev *dev);
    int AIM_T500Read(Device::SlaveDev *dev);
    int AIM_T350Read(Device::SlaveDev *dev);

  private:
    TcpClient *m_tcpClient;
    bool m_isTcp;
};

#endif
