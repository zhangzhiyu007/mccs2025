#ifndef N35200_H_
#define N35200_H_

#include "../../../db/Device.h"
#include "../../../etc/socket/TcpClient.h"

class ModbusRtuMaster;
class Com;
class TcpClient;
class N35200 {
public:
    N35200();
    virtual ~N35200();

    // 设置是否采用TCP通信
    void SetIsTcp(bool isTcp);

    // 设置TCP客户端
    void SetTcpClient(TcpClient* tcpClient);

    // 从设备读取状态和参数
    int Read(Device::SlaveDev* dev);

    // 向设备发送预设参数
    int Preset(Device::SlaveDev* dev);

    void SetCom(Com *com);

private:
    bool m_isTcp;                  // 标识是否通过TCP/IP进行通信
    TcpClient* m_tcpClient;        // TCP客户端对象，用于数据通信
    Com *m_com;
};

#endif /* N35200_H_ */
