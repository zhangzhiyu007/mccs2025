/*
 * ChargerController.h
 *
 * Created on: 2024-04-15
 * Author: zzy
 */

#ifndef CHARGERCONTROLLER_H_
#define CHARGERCONTROLLER_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../util/Util.h"
#include "../../../etc/socket/TcpClient.h"




class ChargerController
{
public:
    ChargerController();
    virtual ~ChargerController();

    // 设置串口通信对象
    void SetTcp(TcpClient *tcpClient);

    /*
     *  Read
     *  功能: 读取充电桩状态和参数
     *  输入：
     *  dev    : Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev* dev);

    /*
     *  Write
     *  功能: 写入充电桩控制参数
     *  输入：
     *  dev    : Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Write(Device::SlaveDev* dev);

        // void SetIsTcp(bool isTcp);

protected:
    // 执行Modbus读取
    int ModbusRead(Device::SlaveDev* dev);
    // 执行Modbus设置
    int ModbusPreset(Device::SlaveDev* dev);
private:
    // 串口指针
    TcpClient *m_tcpClient;
    // bool m_isTcp;
};

#endif /* CHARGERCONTROLLER_H_ */
