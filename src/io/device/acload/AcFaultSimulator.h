#ifndef AC_FAULT_SIMULATOR_H_
#define AC_FAULT_SIMULATOR_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"

class TcpClient;

// 交流故障模拟器类
class AcFaultSimulator
{
public:
    AcFaultSimulator();  // 构造函数
    virtual ~AcFaultSimulator();  // 析构函数

    void SetTcpClient(TcpClient* client);  // 设置TCP客户端

    // 读取设备状态
    int Read(Device::SlaveDev* dev);

    // 设置设备参数
    int Preset(Device::SlaveDev* dev);

protected:
    // 执行Modbus读取
    int ModbusRead(Device::SlaveDev* dev);
    // 执行Modbus设置
    int ModbusPreset(Device::SlaveDev* dev);

private:
    TcpClient* m_tcpClient; // 用于Modbus通信的TCP客户端
};

#endif /* AC_FAULT_SIMULATOR_H_ */
