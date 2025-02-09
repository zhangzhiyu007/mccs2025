/**
 * @file RLCLoad.h
 * @Synopsis 30kW RLC负荷, 秦皇岛
 * @author Mingyu Wu
 * @version 1.0
 * @date 2023-12-20
 */

#ifndef _RCL_LOAD_30_H_
#define _RCL_LOAD_30_H_
#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

class RLCLoad {
  public:
    RLCLoad();
    virtual ~RLCLoad();
    void SetTcp(TcpClient *tcpClient);

    int Read(Device::SlaveDev *dev);
    int RLC_a_Read(Device::SlaveDev *dev);
    int RLC_b_Read(Device::SlaveDev *dev);
    int RLC_c_Read(Device::SlaveDev *dev);

    int Preset(Device::SlaveDev *dev);
    int RLC_a_Preset(Device::SlaveDev *dev);
    int RLC_b_Preset(Device::SlaveDev *dev);
    int RLC_c_Preset(Device::SlaveDev *dev);

    int GetMeterInfo(Device::SlaveDev *dev);

    /**
     * @Synopsis 获取功率下发命令
     *
     * @Param power 功率单位W
     * @Param cmd 下发指令
     */
    int GetActivePowerCmd(int power);

    int GetReactivePowerCmd(int power);

  private:
    void Wait();

  private:
    TcpClient *m_tcpClient;

    int m_cur_range[3] = {0};
    int m_cur_ratio[3] = {0};

    int m_volt_range[3] = {0};
    int m_volt_ratio[3] = {0};

    float m_cur_index[3]   = {0.0};
    float m_volt_index[3]  = {0.0};
    float m_power_index[3] = {0.0};
};
#endif // !_RCL_LOAD_30_H_
