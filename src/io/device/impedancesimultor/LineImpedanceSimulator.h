#ifndef LINE_IMPEDANCE_SIMULATOR_H
#define LINE_IMPEDANCE_SIMULATOR_H

#include "../../../db/Device.h"
#include "../../../etc/socket/TcpClient.h"

class LineImpedanceSimulator {
public:
    LineImpedanceSimulator();
    virtual ~LineImpedanceSimulator();

    void SetTcpClient(TcpClient* tcpClient);

    int Read(Device::SlaveDev* dev);
    int Preset(Device::SlaveDev* dev);

private:
    TcpClient* m_tcpClient;
    bool m_isTcp;
};

#endif // LINE_IMPEDANCE_SIMULATOR_H
