#include "./JingShiDCDC.h"
#include "../../../db/MemDb.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

JingshiDCDC::JingshiDCDC() { this->m_tcpClient = NULL; }

JingshiDCDC::~JingshiDCDC() {}

void JingshiDCDC::SetTcp(TcpClient *tcpClient) {
    this->m_tcpClient = tcpClient;
}

int JingshiDCDC::Read(Device::SlaveDev *dev) {

    int ret = ErrorInfo::ERR_OK;

    int start    = 0;
    int regStart = dev->regStart;

    bool comm = false;

    UshortArray values;
    values.clear();

    int addr = atoi(dev->slaveAddr.c_str());

    int i_data   = 0;
    float f_data = 0.0f;

    int length = 0;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    if (!m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "tcp 未打开");
        return ErrorInfo::ERR_OPENED;
    }

    // 初始化完成，开始读取数据
    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    start  = 0xC9;
    length = 54;

    values.clear();
    // 读取模块信息
    ret = rtu.ReadInputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 1, f_data, false); // 高压侧电压

        f_data = (signed short)values[1] * 0.1;
        MemDb::SetRealData(regStart + 2, f_data, false); // 高压侧电流

        f_data = (signed short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 3, f_data, false); // 高压侧功率

        f_data = (signed short)values[3] * 0.1;
        MemDb::SetRealData(regStart + 4, f_data, false); // 模块IGBT1温度

        f_data = (signed short)values[4] * 0.1;
        MemDb::SetRealData(regStart + 5, f_data, false); // 模块IGBT2温度

        f_data = (signed short)values[5] * 0.1;
        MemDb::SetRealData(regStart + 6, f_data, false); // 模块IGBT3温度

        f_data = (signed short)values[6] * 0.1;
        MemDb::SetRealData(regStart + 7, f_data, false); // 模块IGBT4温度

        f_data = (signed short)values[7] * 0.1;
        MemDb::SetRealData(regStart + 8, f_data, false); // 模块IGBT5温度

        f_data = (signed short)values[8] * 0.1;
        MemDb::SetRealData(regStart + 9, f_data, false); // 模块IGBT6温度

        f_data = (signed short)values[9] * 0.1;
        MemDb::SetRealData(regStart + 10, f_data, false); // 模块进风口温度

        f_data = (signed short)values[10] * 0.1;
        MemDb::SetRealData(regStart + 11, f_data, false); // 模块出风口温度

        f_data = values[11];
        MemDb::SetRealData(regStart + 12, f_data, false); // 预留1
        f_data = values[12];
        MemDb::SetRealData(regStart + 13, f_data, false); // 预留2
        f_data = values[13];
        MemDb::SetRealData(regStart + 14, f_data, false); // 预留3
        f_data = values[14];
        MemDb::SetRealData(regStart + 15, f_data, false); // 预留4

        f_data = values[15] * 0.1;
        MemDb::SetRealData(regStart + 16, f_data, false); // 低压侧接入1 电压

        f_data = (signed short)values[16] * 0.1;
        MemDb::SetRealData(regStart + 17, f_data, false); // 低压侧接入1 电流

        f_data = (signed short)values[17] * 0.1;
        MemDb::SetRealData(regStart + 18, f_data, false); // 低压侧接入1 功率

        f_data = values[18] * 0.1;
        MemDb::SetRealData(regStart + 19, f_data, false); // 预留

        f_data = values[19] * 0.1;
        MemDb::SetRealData(regStart + 20, f_data, false); // 低压侧接入2电压

        f_data = (signed short)values[20] * 0.1;
        MemDb::SetRealData(regStart + 21, f_data, false); // 低压侧接入2电流

        f_data = (signed short)values[21] * 0.1;
        MemDb::SetRealData(regStart + 22, f_data, false); // 低压侧接入2功率

        f_data = values[22];
        MemDb::SetRealData(regStart + 23, f_data, false); // 预留

        f_data = values[23] * 0.1;
        MemDb::SetRealData(regStart + 24, f_data, false); // 低压侧接入3电压

        f_data = (signed short)values[24] * 0.1;
        MemDb::SetRealData(regStart + 25, f_data, false); // 低压侧接入3电流

        f_data = (signed short)values[25] * 0.1;
        MemDb::SetRealData(regStart + 26, f_data, false); // 低压侧接入3功率

        f_data = values[26];
        MemDb::SetRealData(regStart + 27, f_data, false); // 预留

        f_data = values[27] * 0.1;
        MemDb::SetRealData(regStart + 28, f_data, false); // 低压侧接入4电压

        f_data = (signed short)values[28] * 0.1;
        MemDb::SetRealData(regStart + 29, f_data, false); // 低压侧接入4电流

        f_data = (signed short)values[29] * 0.1;
        MemDb::SetRealData(regStart + 30, f_data, false); // 低压侧接入4功率

        f_data = values[30];
        MemDb::SetRealData(regStart + 31, f_data, false); // 预留

        f_data = values[31] * 0.1;
        MemDb::SetRealData(regStart + 32, f_data, false); // 低压侧接入5电压

        f_data = (signed short)values[32] * 0.1;
        MemDb::SetRealData(regStart + 33, f_data, false); // 低压侧接入5电流

        f_data = (signed short)values[33] * 0.1;
        MemDb::SetRealData(regStart + 34, f_data, false); // 低压侧接入5功率

        f_data = values[34];
        MemDb::SetRealData(regStart + 35, f_data, false); // 预留

        f_data = values[35] * 0.1;
        MemDb::SetRealData(regStart + 36, f_data, false); // 低压侧接入6电压

        f_data = (signed short)values[36] * 0.1;
        MemDb::SetRealData(regStart + 37, f_data, false); // 低压侧接入6电流

        f_data = (signed short)values[37] * 0.1;
        MemDb::SetRealData(regStart + 38, f_data, false); // 低压侧接入6功率

        f_data = values[38];
        MemDb::SetRealData(regStart + 39, f_data, false); // 预留

        i_data = (values[39] >> 2) & 0b11;
        MemDb::SetRealData(regStart + 40, i_data,
                           false); // 设备状态字1 1:停机，2:软启， 3:运行

        i_data = (values[40] >> 2) & 0b111;
        MemDb::SetRealData(regStart + 41, i_data,
                           false); // 设备状态字2  0:停机，1:电池非充非放，
                                   // 2:电池充电，3:电池放电，4：光伏发电

        i_data = values[39] & 0b1;
        MemDb::SetRealData(regStart + 42, i_data,
                           false); // 设备状态字3 0:无告警，1:有告警

        i_data = (values[39] >> 1) & 0b1;
        MemDb::SetRealData(regStart + 43, i_data,
                           false); // 设备状态字4 0:无故障，1:有故障

        i_data = values[43];
        MemDb::SetRealData(regStart + 44, i_data, false); // 并机故障故障字

        i_data = values[44];
        MemDb::SetRealData(regStart + 45, i_data, false); // 系统故障字

        i_data = values[45];
        MemDb::SetRealData(regStart + 46, i_data, false); // 直流接入1故障字

        i_data = values[46];
        MemDb::SetRealData(regStart + 47, i_data, false); // 直流接入2故障字

        i_data = values[47];
        MemDb::SetRealData(regStart + 48, i_data, false); // 直流接入3故障字

        i_data = values[48];
        MemDb::SetRealData(regStart + 49, i_data, false); // 直流接入4故障字

        i_data = values[49];
        MemDb::SetRealData(regStart + 50, i_data, false); // 直流接入5故障字

        i_data = values[50];
        MemDb::SetRealData(regStart + 51, i_data, false); // 直流接入6故障字

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start = 0x12D;
    // TODO
    length = 51;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;

        i_data = (values[0] & 0b11) >> 1;
        MemDb::SetRealData(regStart + 52, i_data, false); // 支路1启动 停机命令

        i_data = ((values[0] >> 2) & 0b11) >> 1;
        MemDb::SetRealData(regStart + 53, i_data, false); // 支路2启动 停机命令

        i_data = ((values[0] >> 4) & 0b11) >> 1;
        MemDb::SetRealData(regStart + 54, i_data, false); // 支路3启动 停机命令

        i_data = ((values[0] >> 6) & 0b11) >> 1;
        MemDb::SetRealData(regStart + 55, i_data, false); // 支路4启动 停机命令

        i_data = ((values[0] >> 8) & 0b11) >> 1;
        MemDb::SetRealData(regStart + 56, i_data, false); // 支路5启动 停机命令

        i_data = ((values[0] >> 10) & 0b11) >> 1;
        MemDb::SetRealData(regStart + 57, i_data, false); // 支路6启动 停机命令

        i_data = values[1];
        MemDb::SetRealData(regStart + 58, i_data, false); // 工作模式

        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 59, f_data, false); // 高压侧电压

        f_data = (signed short)values[3] * 0.1;
        MemDb::SetRealData(regStart + 60, f_data, false); // 恒流模式电流1

        f_data = (signed short)values[4] * 0.1;
        MemDb::SetRealData(regStart + 61, f_data, false); // 恒流模式电流2

        f_data = (signed short)values[5] * 0.1;
        MemDb::SetRealData(regStart + 62, f_data, false); // 恒流模式电流3

        f_data = (signed short)values[6] * 0.1;
        MemDb::SetRealData(regStart + 63, f_data, false); // 恒流模式电流4

        f_data = (signed short)values[7] * 0.1;
        MemDb::SetRealData(regStart + 64, f_data, false); // 恒流模式电流5

        f_data = (signed short)values[8] * 0.1;
        MemDb::SetRealData(regStart + 65, f_data, false); // 恒流模式电流6

        f_data = values[9] * 0.1;
        MemDb::SetRealData(regStart + 66, f_data, false); // 低压侧电压1

        f_data = values[10] * 0.1;
        MemDb::SetRealData(regStart + 67, f_data, false); // 低压侧电压2

        f_data = values[11] * 0.1;
        MemDb::SetRealData(regStart + 68, f_data, false); // 低压侧电压3

        f_data = values[12] * 0.1;
        MemDb::SetRealData(regStart + 69, f_data, false); // 低压侧电压4

        f_data = values[13] * 0.1;
        MemDb::SetRealData(regStart + 70, f_data, false); // 低压侧电压5

        f_data = values[14] * 0.1;
        MemDb::SetRealData(regStart + 71, f_data, false); // 低压侧电压6

        f_data = (signed short)values[15] * 0.1;
        MemDb::SetRealData(regStart + 72, f_data, false); // 恒功率模式功率1

        f_data = (signed short)values[16] * 0.1;
        MemDb::SetRealData(regStart + 73, f_data, false); // 恒功率模式功率2

        f_data = (signed short)values[17] * 0.1;
        MemDb::SetRealData(regStart + 74, f_data, false); // 恒功率模式功率3

        f_data = (signed short)values[18] * 0.1;
        MemDb::SetRealData(regStart + 75, f_data, false); // 恒功率模式功率4

        f_data = (signed short)values[19] * 0.1;
        MemDb::SetRealData(regStart + 76, f_data, false); // 恒功率模式功率5

        f_data = (signed short)values[20] * 0.1;
        MemDb::SetRealData(regStart + 77, f_data, false); // 恒功率模式功率6

        f_data = values[21];
        MemDb::SetRealData(regStart + 78, f_data, false); // 低压侧接线方式

        f_data = values[22];
        MemDb::SetRealData(regStart + 79, f_data, false); // 低压侧接入类型

        f_data = values[23] * 10;
        MemDb::SetRealData(regStart + 80, f_data, false); // 电流变化率

        f_data = values[24] * 0.1;
        MemDb::SetRealData(regStart + 81, f_data, false); // 模块功率限制

        i_data = values[25];
        MemDb::SetRealData(regStart + 82, i_data, false); // 设备类型

        f_data = values[26];
        MemDb::SetRealData(regStart + 83, f_data, false); // 预留

        f_data = values[27];
        MemDb::SetRealData(regStart + 84, f_data, false); // 预留

        f_data = values[28];
        MemDb::SetRealData(regStart + 85, f_data, false); // 预留

        f_data = values[29] * 0.1;
        MemDb::SetRealData(regStart + 86, f_data, false); // 母线电压最大限值

        f_data = values[30] * 0.1;
        MemDb::SetRealData(regStart + 87, f_data, false); // 母线电压最小限值

        f_data = values[31] * 0.1;
        MemDb::SetRealData(regStart + 88, f_data, false); // 低压侧最大电压1

        f_data = values[32] * 0.1;
        MemDb::SetRealData(regStart + 89, f_data, false); // 低压侧最小电压1

        f_data = values[35] * 0.1;
        MemDb::SetRealData(regStart + 90, f_data,
                           false); // 低压侧恒压模式电流限值1
        f_data = values[36] * 0.1;
        MemDb::SetRealData(regStart + 91, f_data, false); // 低压侧最大电压2

        f_data = values[37] * 0.1;
        MemDb::SetRealData(regStart + 92, f_data, false); // 低压侧最小电压2

        f_data = values[38] * 0.1;
        MemDb::SetRealData(regStart + 93, f_data, false); // 低压侧最大放电电流2

        f_data = values[39] * 0.1;
        MemDb::SetRealData(regStart + 94, f_data, false); // 低压侧最大充电电流2

        f_data = values[40] * 0.1;
        MemDb::SetRealData(regStart + 95, f_data,
                           false); // 低压侧恒压模式电流限制2

        f_data = values[41] * 0.1;
        MemDb::SetRealData(regStart + 96, f_data, false); // 低压侧最大电压3

        f_data = values[42] * 0.1;
        MemDb::SetRealData(regStart + 97, f_data, false); // 低压侧最小电压3

        f_data = values[43] * 0.1;
        MemDb::SetRealData(regStart + 98, f_data, false); // 低压侧最大放电电流3

        f_data = values[44] * 0.1;
        MemDb::SetRealData(regStart + 99, f_data,
                           false); // 低压侧最大充电电流电流3

        f_data = values[45] * 0.1;
        MemDb::SetRealData(regStart + 100, f_data,
                           false); // 低压侧恒压模式电流限制3

        f_data = values[46] * 0.1;
        MemDb::SetRealData(regStart + 101, f_data, false); // 低压侧最大电压4

        f_data = values[47] * 0.1;
        MemDb::SetRealData(regStart + 102, f_data, false); // 低压侧最小电压4

        f_data = values[48] * 0.1;
        MemDb::SetRealData(regStart + 103, f_data,
                           false); // 低压侧最大放电电流4

        f_data = values[49] * 0.1;
        MemDb::SetRealData(regStart + 104, f_data,
                           false); // 低压侧最大充电电流4

        f_data = values[50] * 0.1;
        MemDb::SetRealData(regStart + 105, f_data,
                           false); // 低压侧恒压模式电流限制4

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start  = 0x160;
    length = 10;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 106, f_data, false); // 低压侧最大电压5
        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 107, f_data, false); // 低压侧最小电压5
        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 108, f_data,
                           false); // 低压侧最大放电电流5
        f_data = values[3] * 0.1;
        MemDb::SetRealData(regStart + 109, f_data,
                           false); // 低压侧最大充电电流5
        f_data = values[4] * 0.1;
        MemDb::SetRealData(regStart + 110, f_data,
                           false); // 低压侧恒压模式电流限值5
        f_data = values[5] * 0.1;
        MemDb::SetRealData(regStart + 111, f_data, false); // 低压侧最大电压6
        f_data = values[6] * 0.1;
        MemDb::SetRealData(regStart + 112, f_data, false); // 低压侧最小电压6
        f_data = values[7] * 0.1;
        MemDb::SetRealData(regStart + 113, f_data,
                           false); // 低压侧最大放电电流6
        f_data = values[8] * 0.1;
        MemDb::SetRealData(regStart + 114, f_data,
                           false); // 低压侧最大充电电流6
        f_data = values[9] * 0.1;
        MemDb::SetRealData(regStart + 115, f_data,
                           false); // 低压侧恒压模式电流限制6

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}

int JingshiDCDC::Preset(Device::SlaveDev *dev) {

    int ret      = ErrorInfo::ERR_OK;
    int regStart = dev->regStart;
    int i_data   = 0;

    RealData realData;

    int addr = atoi(dev->slaveAddr.c_str());

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

    if (MemDb::GetRealData(realData, regStart + 201, true)) {
        zlog_warn(Util::m_zlog, "1号支路启停");
        i_data = (realData.pv.data.iValue << 1) & 0b10;
        ret    = rtu.PresetSingleRegister(addr, 0x12D, i_data);
    }

    if (MemDb::GetRealData(realData, regStart + 202, true)) {
        i_data = (realData.pv.data.iValue << 1) & 0b10;
        ret    = rtu.PresetSingleRegister(addr, 0x12D, i_data << 2);
    }

    if (MemDb::GetRealData(realData, regStart + 203, true)) {
        i_data = (realData.pv.data.iValue << 1) & 0b10;
        rtu.PresetSingleRegister(addr, 0x12D, i_data << 4);
    }

    if (MemDb::GetRealData(realData, regStart + 204, true)) {
        i_data = (realData.pv.data.iValue << 1) & 0b10;
        rtu.PresetSingleRegister(addr, 0x12D, i_data << 6);
    }

    if (MemDb::GetRealData(realData, regStart + 205, true)) {
        i_data = (realData.pv.data.iValue << 1) & 0b10;
        rtu.PresetSingleRegister(addr, 0x12D, i_data << 8);
    }

    if (MemDb::GetRealData(realData, regStart + 206, true)) {
        i_data = (realData.pv.data.iValue << 1) & 0b10;
        rtu.PresetSingleRegister(addr, 0x12D, i_data << 10);
    }

    // 工作模式
    if (MemDb::GetRealData(realData, regStart + 207, true)) {
        i_data = realData.pv.data.iValue;
        rtu.PresetSingleRegister(addr, 0x12E, i_data);
    }

    // 高压侧电压
    if (MemDb::GetRealData(realData, regStart + 208, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x12F, i_data);
    }

    // 恒流模式电流1
    if (MemDb::GetRealData(realData, regStart + 209, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x130, i_data);
    }

    // 恒流模式电流2
    if (MemDb::GetRealData(realData, regStart + 210, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x131, i_data);
    }

    // 恒流模式电流3
    if (MemDb::GetRealData(realData, regStart + 211, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x132, i_data);
    }

    // 恒流模式电流4
    if (MemDb::GetRealData(realData, regStart + 212, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x133, i_data);
    }

    // 恒流模式电流5
    if (MemDb::GetRealData(realData, regStart + 213, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x134, i_data);
    }

    // 恒流模式电流6
    if (MemDb::GetRealData(realData, regStart + 214, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x135, i_data);
    }

    // 低压侧电压1
    if (MemDb::GetRealData(realData, regStart + 215, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x136, i_data);
    }

    // 低压侧电压2
    if (MemDb::GetRealData(realData, regStart + 216, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x137, i_data);
    }

    // 低压侧电压3
    if (MemDb::GetRealData(realData, regStart + 217, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x138, i_data);
    }

    // 低压侧电压4
    if (MemDb::GetRealData(realData, regStart + 218, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x139, i_data);
    }

    // 低压侧电压5
    if (MemDb::GetRealData(realData, regStart + 219, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x13A, i_data);
    }

    // 低压侧电压6
    if (MemDb::GetRealData(realData, regStart + 220, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x13B, i_data);
    }

    // 恒功率模式1
    if (MemDb::GetRealData(realData, regStart + 221, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x13C, i_data);
    }

    // 恒功率模式2
    if (MemDb::GetRealData(realData, regStart + 222, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x13D, i_data);
    }

    // 恒功率模式3
    if (MemDb::GetRealData(realData, regStart + 223, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x13E, i_data);
    }

    // 恒功率模式4
    if (MemDb::GetRealData(realData, regStart + 224, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x13F, i_data);
    }

    // 恒功率模式5
    if (MemDb::GetRealData(realData, regStart + 225, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x140, i_data);
    }

    // 恒功率模式6
    if (MemDb::GetRealData(realData, regStart + 226, true)) {
        i_data = realData.pv.data.iValue * 10;
        rtu.PresetSingleRegister(addr, 0x141, i_data);
    }

    // 故障清除
    if (MemDb::GetRealData(realData, regStart + 227, true)) {
        i_data = realData.pv.data.iValue;
        ret    = rtu.PresetSingleRegister(addr, 0x171, i_data);
    }

    return ret;
}