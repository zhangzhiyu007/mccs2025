/**
 * @file GoodWe.h
 * @Synopsis GOODWE SDT 8kW  光伏逆变器
 * @author Mingyu Wu
 * @version 1.0
 * @date 2023-12-15
 */

#ifndef GOODWE_H_
#define GOODWE_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class Com;

class GoodWe {
  public:
    GoodWe();
    virtual ~GoodWe();

    /*
     * Read
     * 功能：读取PVS数据
     * 输入：
     * dev			:[in] Device::SlaveDev* 从设备指针
     * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev);

    /*
     * Preset
     * 功能：设置PVS数据
     * 输入：
     * dev			:[in] Device::SlaveDev* 从设备指针
     * 输出：返回错误码ErrorInfo，成功返回ErrorInfo::ERR_OK
     */
    int Preset(Device::SlaveDev *dev);

    void SetTcp(TcpClient *tcpClient);
    void SetIsTcp(bool isTcp);
    void SetCom(Com *com);

  protected:
  private:
    int Gw8kSDTRead(Device::SlaveDev *dev);
    int Gw8kSDTPreset(Device::SlaveDev *dev);

    // 变量
  public:
  protected:
  private:
    // 串口指针
    TcpClient *m_tcpClient;
    Com *m_com;
    bool m_isTcp;
};

#endif /* GOODWE_H_ */
