/**
 * @file JingshiPcs.cpp
 * @Synopsis
 * @author Mingyu Wu
 * @version
 * @date 2022-05-30
 */
#include "./JingshiPcs.h"

JingshiPcs::JingshiPcs() { this->m_tcpClient = NULL; }

JingshiPcs::~JingshiPcs() {}

void JingshiPcs::SetTcp(TcpClient *TcpClient) { this->m_tcpClient = TcpClient; }

int JingshiPcs::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    int regStart = dev->regStart;
    int addr     = atoi(dev->slaveAddr.c_str());

    UshortArray values;
    values.clear();

    bool comm    = false;
    int i_data   = 0;
    float f_data = 0.0f;

    int length = 0;
    int start  = 0;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "tcp 未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusTcpClient client;
    client.SetTcpClient(this->m_tcpClient);

    values.clear();
    start  = 0x0;
    length = 31;

    ret = client.ReadOutputRegisters(values, addr, start, length);
    if (ret == ErrorInfo::ERR_OK) {
        comm = true;

        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 1, f_data, false); // A相电压

        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 2, f_data, false); // B相电压

        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 3, f_data, false); // C相电压

        f_data = values[3] * 0.1;
        MemDb::SetRealData(regStart + 4, f_data, false); // A相电流

        f_data = values[4] * 0.1;
        MemDb::SetRealData(regStart + 5, f_data, false); // B相电流

        f_data = values[5] * 0.1;
        MemDb::SetRealData(regStart + 6, f_data, false); // C相电流

        f_data = (signed short)values[6] * 0.1;
        MemDb::SetRealData(regStart + 7, f_data, false); // 有功功率

        f_data = (signed short)values[7] * 0.1;
        MemDb::SetRealData(regStart + 8, f_data, false); // 无功功率

        f_data = (signed short)values[8] * 0.1;
        MemDb::SetRealData(regStart + 9, f_data, false); // 视在功率

        f_data = values[9] * 0.01;
        MemDb::SetRealData(regStart + 10, f_data, false); // 频率

        i_data = (signed short)values[10] * 0.1;
        MemDb::SetRealData(regStart + 11, i_data, false); // 功率因数

        f_data = values[11] * 0.1;
        MemDb::SetRealData(regStart + 12, f_data, false); // 直流端口电压

        f_data = (signed short)values[12] * 0.1;
        MemDb::SetRealData(regStart + 13, f_data, false); // 直流电流

        f_data = (signed short)values[13] * 0.1;
        MemDb::SetRealData(regStart + 14, f_data, false); // 直流功率

        f_data = (signed short)values[14] * 0.1;
        MemDb::SetRealData(regStart + 15, f_data, false); // IGBT1温度

        f_data = (signed short)values[15] * 0.1;
        MemDb::SetRealData(regStart + 16, f_data, false); // IGBT2温度

        f_data = (signed short)values[16] * 0.1;
        MemDb::SetRealData(regStart + 17, f_data, false); // IGBT3温度

        f_data = (signed short)values[17] * 0.1;
        MemDb::SetRealData(regStart + 18, f_data, false); // IGBT4温度

        f_data = (signed short)values[18] * 0.1;
        MemDb::SetRealData(regStart + 19, f_data, false); // IGBT5温度

        f_data = (signed short)values[19] * 0.1;
        MemDb::SetRealData(regStart + 20, f_data, false); // IGBT6温度

        f_data = (signed short)values[20] * 0.1;
        MemDb::SetRealData(regStart + 21, f_data, false); // 进风口温度

        f_data = (signed short)values[21] * 0.1;
        MemDb::SetRealData(regStart + 22, f_data, false); // 出风口温度

        f_data = (signed short)values[22] * 0.1;
        MemDb::SetRealData(regStart + 23, f_data, false); // A相有功功率　

        f_data = (signed short)values[23] * 0.1;
        MemDb::SetRealData(regStart + 24, f_data, false); // B相有功功率

        f_data = (signed short)values[24] * 0.1;
        MemDb::SetRealData(regStart + 25, f_data, false); // C相有功功率

        f_data = values[25] * 0.1;
        MemDb::SetRealData(regStart + 26, f_data, false); // 直流母线电压

        i_data = (values[26] >> 1) & 0b111;
        MemDb::SetRealData(regStart + 27, i_data, false); // 设备运行状态字 1:停机，2:自检：3并网运行，4:离网运行

        f_data = (values[26] >> 4) & 0b11;
        MemDb::SetRealData(regStart + 28, f_data, false); // 系统综合故障字1 0:待机，1充电，2：放电

        f_data = values[28];
        MemDb::SetRealData(regStart + 29, f_data, false); // 系统环境故障字2

        f_data = values[29];
        MemDb::SetRealData(regStart + 30, f_data, false); // 系统硬件故障字3

        f_data = values[30];
        MemDb::SetRealData(regStart + 31, f_data, false); // 系统温度故障字4

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start  = 0x65;
    length = 28;

    ret = client.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;

        i_data = values[0] >> 8;
        MemDb::SetRealData(regStart + 32, i_data, false); // 启动/停机

        i_data = values[1];
        MemDb::SetRealData(regStart + 33, i_data, false); // 工作模式设置

        f_data = (signed short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 34, f_data, false); // 有功设定

        f_data = (signed short)values[3] * 0.1;
        MemDb::SetRealData(regStart + 35, f_data, false); // 无功设定

        i_data = (signed short)values[4];
        MemDb::SetRealData(regStart + 36, i_data, false); // 功率因数设定

        f_data = (signed short)values[5] * 0.1;
        MemDb::SetRealData(regStart + 37, f_data, false); // 直流电流设置

        f_data = values[6] * 0.1;
        MemDb::SetRealData(regStart + 38, f_data, false); // 直流电压设置

        f_data = (signed short)values[7] * 0.1;
        MemDb::SetRealData(regStart + 39, f_data, false); // 直流功率设置

        f_data = (signed short)values[8] * 0.1;
        MemDb::SetRealData(regStart + 40, f_data, false); // A相有功功率

        f_data = (signed short)values[9] * 0.1;
        MemDb::SetRealData(regStart + 41, f_data, false); // B相有功功率

        f_data = (signed short)values[10] * 0.1;
        MemDb::SetRealData(regStart + 42, f_data, false); // C相有功功率

        i_data = values[11];
        MemDb::SetRealData(regStart + 43, i_data, false); // 电网代码

        f_data = values[12] * 0.1;
        MemDb::SetRealData(regStart + 44, f_data, false); // 交流电压设置

        f_data = (signed short)values[13] * 0.1;
        MemDb::SetRealData(regStart + 45, f_data, false); // 交流电流设置

        f_data = values[14] * 0.1;
        MemDb::SetRealData(regStart + 46, f_data, false); // 交流频率设置

        i_data = values[15];
        MemDb::SetRealData(regStart + 47, i_data, false); // 有功变化率

        i_data = values[16];
        MemDb::SetRealData(regStart + 48, i_data, false); // 无功设置方式

        i_data = values[17];
        MemDb::SetRealData(regStart + 49, i_data, false); // 无功变化率

        i_data = values[18];
        MemDb::SetRealData(regStart + 50, i_data, false); // 三相不平衡输出

        i_data = values[19];
        MemDb::SetRealData(regStart + 51, i_data, false); // 设备类型

        i_data = values[20];
        MemDb::SetRealData(regStart + 52, i_data, false); // 预留

        i_data = values[21];
        MemDb::SetRealData(regStart + 53, i_data, false); // 预留

        i_data = values[22];
        MemDb::SetRealData(regStart + 54, i_data, false); // 预留

        i_data = values[23];
        MemDb::SetRealData(regStart + 55, i_data, false); // 预留

        i_data = values[24];
        MemDb::SetRealData(regStart + 56, i_data, false); // 直流电压最大限值

        i_data = values[25];
        MemDb::SetRealData(regStart + 57, i_data, false); // 直流电压最小限值

        i_data = values[26];
        MemDb::SetRealData(regStart + 58, i_data, false); // 电池最大充电电流

        i_data = values[27];
        MemDb::SetRealData(regStart + 59, i_data, false); // 电池最大放电电流

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start  = 0x88;
    length = 1;
    ret    = client.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;

        i_data = values[0];
        switch (i_data) {
        case 0xEE:
            MemDb::SetRealData(regStart + 60, 0, false);
            break;
        case 0xEE00:
            MemDb::SetRealData(regStart + 60, 1, false);
            break;
        default:
            MemDb::SetRealData(regStart + 60, 99, false);
            break;
        }

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    values.clear();
    start  = 0x244;
    length = 10;

    ret = client.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        i_data = values[0];
        MemDb::SetRealData(regStart + 62, i_data, false); // 电池状态

        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 63, f_data, false); // 电池电压

        f_data = (signed short)values[2];
        MemDb::SetRealData(regStart + 64, i_data, false); // 电池电流

        i_data = values[3];
        MemDb::SetRealData(regStart + 65, i_data, false); // 电池SOC

        f_data = values[4] * 0.1;
        MemDb::SetRealData(regStart + 66, i_data, false); // 最大允许充电电流

        f_data = values[5] * 0.1;
        MemDb::SetRealData(regStart + 67, i_data, false); // 最大允许放电电流

        f_data = values[6] * 0.001;
        MemDb::SetRealData(regStart + 68, i_data, false); // 最高单体电压

        f_data = values[7] * 0.001;
        MemDb::SetRealData(regStart + 69, i_data, false); // 最低单体电压

        i_data = values[8];
        MemDb::SetRealData(regStart + 70, i_data, false); // 预留

        i_data = values[9];
        MemDb::SetRealData(regStart + 71, i_data, false); // 预留

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);

    return ret;
}

int JingshiPcs::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    int regStart = dev->regStart;
    int addr     = atoi(dev->slaveAddr.c_str());

    RealData realData;

    bool comm  = false;
    int length = 0;
    int start  = 0;
    int i_data = 0;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusTcpClient client;
    client.SetTcpClient(this->m_tcpClient);

    // 起停命令
    if (MemDb::GetRealData(realData, regStart + 101, true)) {
        zlog_warn(Util::m_zlog, "下发PCS启停命令");

        i_data = realData.pv.data.iValue;
        switch (i_data) {
        case 0:
            ret = client.PresetSingleRegister(addr, 0x65, 0xFF);
            break;
        case 1:
            ret = client.PresetSingleRegister(addr, 0x65, 0xFF00);
            break;
        default:
            break;
        }
    }

    // 工作模式设置
    if (MemDb::GetRealData(realData, regStart + 102, true)) {
        ret = client.PresetSingleRegister(addr, 0x66, realData.pv.data.iValue);
    }

    // 有功设定
    if (MemDb::GetRealData(realData, regStart + 103, true)) {
        ret = client.PresetSingleRegister(addr, 0x67,
                                          realData.pv.data.iValue * 10);
    }

    // 无功设定
    if (MemDb::GetRealData(realData, regStart + 104, true)) {
        ret = client.PresetSingleRegister(addr, 0x68,
                                          realData.pv.data.iValue * 10);
    }

    // 功率因数设定
    if (MemDb::GetRealData(realData, regStart + 105, true)) {
        ret = client.PresetSingleRegister(addr, 0x69, realData.pv.data.iValue);
    }

    // 直流电流设置
    if (MemDb::GetRealData(realData, regStart + 106, true)) {
        ret = client.PresetSingleRegister(addr, 0x6a,
                                          realData.pv.data.iValue * 10);
    }

    // 直流电压设置
    if (MemDb::GetRealData(realData, regStart + 107, true)) {
        ret = client.PresetSingleRegister(addr, 0x6b,
                                          realData.pv.data.iValue * 10);
    }

    // 直流功率设置
    if (MemDb::GetRealData(realData, regStart + 108, true)) {
        ret = client.PresetSingleRegister(addr, 0x6c,
                                          realData.pv.data.iValue * 10);
    }

    // A相有功功率
    if (MemDb::GetRealData(realData, regStart + 109, true)) {
        ret = client.PresetSingleRegister(addr, 0x6d,
                                          realData.pv.data.iValue * 10);
    }

    // B相有功功率
    if (MemDb::GetRealData(realData, regStart + 110, true)) {
        ret = client.PresetSingleRegister(addr, 0x6e,
                                          realData.pv.data.iValue * 10);
    }

    // C相有功功率
    if (MemDb::GetRealData(realData, regStart + 111, true)) {
        ret = client.PresetSingleRegister(addr, 0x6f,
                                          realData.pv.data.iValue * 10);
    }

    // 交流电压设置
    if (MemDb::GetRealData(realData, regStart + 112, true)) {
        ret = client.PresetSingleRegister(addr, 0x71,
                                          realData.pv.data.iValue * 10);
    }

    // 交流电流设置
    if (MemDb::GetRealData(realData, regStart + 113, true)) {
        ret = client.PresetSingleRegister(addr, 0x72,
                                          realData.pv.data.iValue * 10);
    }

    // 交流频率设置
    if (MemDb::GetRealData(realData, regStart + 114, true)) {
        ret = client.PresetSingleRegister(addr, 0x73, realData.pv.data.iValue);
    }

    // 无功设置方式
    if (MemDb::GetRealData(realData, regStart + 115, true)) {
        ret = client.PresetSingleRegister(addr, 0x75, realData.pv.data.iValue);
    }

    // 故障清除
    if (MemDb::GetRealData(realData, regStart + 116, true)) {
        i_data = realData.pv.data.iValue;
        switch (i_data) {
        case 0:
            ret = client.PresetSingleRegister(addr, 0x88, 0xEE);
            break;
        case 1:
            ret = client.PresetSingleRegister(addr, 0x88, 0xEE00);
            break;
        default:
            zlog_warn(Util::m_zlog, "%s 下发指令错误", dev->name.c_str());
            break;
        }
    }

    return ret;
}
