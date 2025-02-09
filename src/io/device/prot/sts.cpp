/**
 * @file sts.cpp
 * @Synopsis
 * @author Mingyu Wu
 * @version 1.0.0
 * @date 2022-05-27
 */

#include "./sts.h"

STS::STS() { m_tcpClient = NULL; }

STS::~STS() {}

void STS::SetTcp(TcpClient *tcpClient) { this->m_tcpClient = tcpClient; }

int STS::Read(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    int regStart = dev->regStart;
    int addr     = atoi(dev->slaveAddr.c_str());

    UshortArray values;
    values.clear();

    bool comm    = false;
    int i_data   = 0;
    float f_data = 0.0f;
    bool status  = 0;
    int length   = 0;
    int start    = 0;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "tcp 未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    values.clear();
    start  = 0x1000;
    length = 3;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;

        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 1, f_data, false); // 交流A相电压

        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 2, f_data, false); // 交流B相电压

        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 3, f_data, false); // 交流C相电压

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start  = 0x1006;
    length = 5;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;

        f_data = (signed short)values[0] * 0.01;
        MemDb::SetRealData(regStart + 4, f_data, false); // 交流有功功率

        f_data = (signed short)values[1] * 0.01;
        MemDb::SetRealData(regStart + 5, f_data, false); // 交流无功功率

        f_data = (signed short)values[2] * 0.01;
        MemDb::SetRealData(regStart + 6, f_data, false); // 交流视在功率

        i_data = values[3] * 0.001;
        MemDb::SetRealData(regStart + 7, i_data, false); // 交流频率

        i_data = values[4] * 0.001;
        MemDb::SetRealData(regStart + 8, i_data, false); // 功率因数

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start  = 0x1015;
    length = 2;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        i_data = values[0];
        MemDb::SetRealData(regStart + 9, i_data, false); // STS故障字

        i_data = values[1];
        MemDb::SetRealData(regStart + 10, i_data, false); // 设备运行状态字

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start  = 0x102F;
    length = 6;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {

        f_data = (signed short)values[0] * 0.1;
        MemDb::SetRealData(regStart + 11, f_data, false); // A相SCR温度

        f_data = (signed short)values[1] * 0.1;
        MemDb::SetRealData(regStart + 12, f_data, false); // B相SCR温度

        f_data = (signed short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 13, f_data, false); // C相SCR温度

        f_data = (signed short)values[3] * 0.1;
        MemDb::SetRealData(regStart + 14, f_data, false); // STS进风口温度

        f_data = (signed short)values[4] * 0.1;
        MemDb::SetRealData(regStart + 15, f_data, false); // STS出风口温度

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start  = 0x1075;
    length = 9;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {

        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 16, f_data, false); // 负载A相电压

        f_data = (signed short)values[1] * 0.1;
        MemDb::SetRealData(regStart + 17, f_data, false); // 负载B相电压

        f_data = (signed short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 18, f_data, false); // 负载C相电压

        f_data = (signed short)values[3] * 0.01;
        MemDb::SetRealData(regStart + 19, f_data, false); // 负载A相电流

        f_data = (signed short)values[4] * 0.01;
        MemDb::SetRealData(regStart + 20, f_data, false); // 负载B相电流

        f_data = (signed short)values[5] * 0.01;
        MemDb::SetRealData(regStart + 21, f_data, false); // 负载C相电流

        f_data = (signed short)values[6] * 0.01;
        MemDb::SetRealData(regStart + 22, f_data, false); // 电网A相电流

        f_data = (signed short)values[7] * 0.01;
        MemDb::SetRealData(regStart + 23, f_data, false); // 电网B相电流

        f_data = (signed short)values[8] * 0.01;
        MemDb::SetRealData(regStart + 24, f_data, false); // 电网C相电流

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }
    values.clear();
    start  = 0x1442;
    length = 1;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        i_data = values[0];
        switch (i_data) {
        case 0:
            MemDb::SetRealData(regStart + 30, i_data, false);
            break;
        case 0xFF00:
            MemDb::SetRealData(regStart + 30, 1, false);
            break;
        case 0x00FF:
            MemDb::SetRealData(regStart + 30, 2, false);
            break;
        default:
            MemDb::SetRealData(regStart + 30, 99, false);
            break;
        }

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}

int STS::Preset(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    int regStart = dev->regStart;
    int addr     = atoi(dev->slaveAddr.c_str());

    bool comm  = false;
    int start  = 0;
    int length = 0;
    int data   = 0;

    RealData realData;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    if (MemDb::GetRealData(realData, regStart + 31, true)) {
        zlog_warn(Util::m_zlog, "STS 下发并离网切换命令");
        data = realData.pv.data.iValue;
        switch (data) {
        case 0:
            ret = rtu.PresetSingleRegister(addr, 0x1442, 0x0);
            break;
        case 1:
            ret = rtu.PresetSingleRegister(addr, 0x1442, 0xFF00);
            break;
        case 2:
            ret = rtu.PresetSingleRegister(addr, 0x1442, 0x00FF);
            break;
        default:
            zlog_warn(Util::m_zlog, "下发并离网指令错误！ 下发指令: %d", data);
            break;
        }
    }

    if (MemDb::GetRealData(realData, regStart + 32, true)) {
        data = realData.pv.data.iValue;
        switch (data) {
        case 0:
            ret = rtu.PresetSingleRegister(addr, 0x1445, 0x00FF);
            break;
        case 1:
            ret = rtu.PresetSingleRegister(addr, 0x1445, 0xFF00);
            break;
        default:
            break;
        }
    }

    //  油机旁路模式使能
    if (MemDb::GetRealData(realData, regStart + 33, true)) {
        data = realData.pv.data.iValue;

        switch (data) {
        case 0:
            ret = rtu.PresetSingleRegister(addr, 0x1449, 0x00FF);
        case 1:
            ret = rtu.PresetSingleRegister(addr, 0x1449, 0xFF00);
        }
    }

    return ret;
}
