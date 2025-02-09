/**
 * @file RLCLoad.cpp
 * @Synopsis 30kW RLC负荷, 秦皇岛
 * @author Mingyu Wu
 * @version 1.0
 * @date 2023-12-20
 */

#include "./RLCLoad.h"

RLCLoad::RLCLoad() { this->m_tcpClient = NULL; }
RLCLoad::~RLCLoad() {}

void RLCLoad::SetTcp(TcpClient *tcpClient) { this->m_tcpClient = tcpClient; }

int RLCLoad::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    if (NULL == dev) {
        return ret;
    }
    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    if (!m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "tcp 未打开");
        return ErrorInfo::ERR_OPENED;
    }
    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::RLC_A:
        ret = this->RLC_a_Read(dev);
        break;
    case DevModel::RLC_B:
        ret = this->RLC_b_Read(dev);
        break;
    case DevModel::RLC_C:
        ret = this->RLC_c_Read(dev);
        break;
    default:
        break;
    };

    return ret;
}

int RLCLoad::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "tcp 未打开");
        return ErrorInfo::ERR_OPENED;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::RLC_A:
        ret = this->RLC_a_Preset(dev);
        break;
    case DevModel::RLC_B:
        ret = this->RLC_b_Preset(dev);
        break;
    case DevModel::RLC_C:
        ret = this->RLC_c_Preset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int RLCLoad::RLC_a_Read(Device::SlaveDev *dev) {
    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
        return ErrorInfo::ERR_NULL;
    }

    // zlog_warn(Util::m_zlog, "< %s >", dev->name.c_str());
    int ret          = ErrorInfo::ERR_OK;
    int regStart     = dev->regStart;
    int addr         = atoi(dev->slaveAddr.c_str());
    int comm         = true;
    int start        = 0;
    int length       = 0;
    float f_data     = 0.0f;
    int i_data       = 0;
    float sumA       = 0.0;
    float sumB       = 0.0;
    float sumC       = 0.0;
    const char *name = dev->name.c_str();

    if (!this->m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "网口未打开");
    }
    this->GetMeterInfo(dev);

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    UshortArray values;
    ShortArray svalues;
    svalues.clear();

    values.clear();

    start  = 0x15;
    length = 20;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data = values[0] * m_volt_index[0];            MemDb::SetRealData(regStart + 1, f_data); // A相阻性电压
        f_data = values[1] * m_volt_index[0];            MemDb::SetRealData(regStart + 2, f_data); // A相感性电压
        f_data = values[2] * m_volt_index[0];            MemDb::SetRealData(regStart + 3, f_data); // A相容性电压

        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", name);
    }

    start  = 0x1d;
    length = 3;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data = values[0] * m_cur_index[0];           MemDb::SetRealData(regStart + 4, f_data); //  A相阻性电流
        f_data = values[1] * m_cur_index[0];           MemDb::SetRealData(regStart + 5, f_data); //  A相感性电流
        f_data = values[2] * m_cur_index[0];           MemDb::SetRealData(regStart + 6, f_data); //  A相荣性电流

        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", name);
    }

    start  = 0x21;
    length = 17;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data =(short) values[3] * m_power_index[0]*3;       MemDb::SetRealData(regStart + 7, f_data); // A相阻性有功
        sumA = f_data;
      //  zlog_warn(Util::m_zlog, "SUMA------------------------=%d", sumA);
        f_data = (short)values[7] * m_power_index[0]*3;       MemDb::SetRealData(regStart + 8, f_data); // A相感性无功
        f_data = values[11] * m_power_index[0]*3;       MemDb::SetRealData(regStart + 9, f_data); // A相容性无功
        f_data =((short) values[15]) *0.0001;      MemDb::SetRealData(regStart + 10, f_data); // A 相视在功率
        f_data = values[16] *  0.01;                    MemDb::SetRealData(regStart + 11, f_data); // A 相视在功率

        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, " %s 读取数据失败", name);
    }

    // B相数据

    start  = 0x32;
    length = 3;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data = values[0] * m_volt_index[0];       MemDb::SetRealData(regStart + 12, f_data); // B相阻性电压
        f_data = values[1] * m_volt_index[0];       MemDb::SetRealData(regStart + 13, f_data); // B相感性电压
        f_data = values[2] * m_volt_index[0];       MemDb::SetRealData(regStart + 14, f_data); // B相容性电压

        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, " %s 读取数据失败", name);
    }

    start  = 0x3a;
    length = 3;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data = values[0] * m_cur_index[1];            MemDb::SetRealData(regStart + 15, f_data); // B相阻性电流
        f_data = values[1] * m_cur_index[1];            MemDb::SetRealData(regStart + 16, f_data); // B相感性电流
        f_data = values[2] * m_cur_index[1];            MemDb::SetRealData(regStart + 17, f_data); // B相容性电流
        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", name);
    }

    start  = 0x3E;
    length = 17;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data = (short)values[3] * m_power_index[1]*3;          MemDb::SetRealData(regStart + 18, f_data); // 阻性有功
        sumB = f_data;
        f_data = (short)values[7] * m_power_index[1]*3;          MemDb::SetRealData(regStart + 19, f_data); // 感性无功
        f_data = values[11] * m_power_index[1]*3;          MemDb::SetRealData(regStart + 20, f_data); // 容性无功
        f_data =((short) values[15]) * 0.0001;         MemDb::SetRealData(regStart + 21, f_data); // 视在功率
        f_data = values[16] *   0.01;            MemDb::SetRealData(regStart + 22, f_data); // 视在功率
        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", name);
    }

    // C相数据
    start  = 0x4f;
    length = 3;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data = values[0] * m_volt_index[2];           MemDb::SetRealData(regStart + 23, f_data);   // C相设备电阻电压
        f_data = values[1] * m_volt_index[2];           MemDb::SetRealData(regStart + 24, f_data);   // C相设备电感电压
        f_data = values[2] * m_volt_index[2];           MemDb::SetRealData(regStart + 25, f_data);   // C相设备电容电压

        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 数据数据失败", name);
    }

    start  = 0x57;
    length = 3;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data = values[0] * m_cur_index[2];        MemDb::SetRealData(regStart + 26, f_data); // C相阻性电流
        f_data = values[1] * m_cur_index[2];        MemDb::SetRealData(regStart + 27, f_data); // C相感性电流
        f_data = values[2] * m_cur_index[2];        MemDb::SetRealData(regStart + 28, f_data); // C相容性电流

        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, " %s 读取数据失败", name);
    }

    start  = 0x5b;
    length = 17;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        f_data = (short)values[3] * m_power_index[2]*3;      MemDb::SetRealData(regStart + 29, f_data); // C相阻性有功
        sumC = f_data;
        f_data = (short)values[7] * m_power_index[2]*3;      MemDb::SetRealData(regStart + 30, f_data); // C相感性无功
        f_data = values[11] * m_power_index[2]*3;      MemDb::SetRealData(regStart + 31, f_data); // C相容性无功
        f_data =((short) values[15]) * 0.0001;     MemDb::SetRealData(regStart + 32, f_data); // C相视在功率
        f_data = values[16] * 0.01;      MemDb::SetRealData(regStart + 33, f_data); // C相视在功率
        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", name);
    }

    start  = 0x13f;
    length = 1;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm &= true;
        // clang-format off
        i_data = values[0] & 0b1;         MemDb::SetRealData(regStart + 34, i_data);  //加载卸载状态
        if (i_data == 1)
        {
            MemDb::SetRealData(regStart + 60, 1);
            MemDb::SetRealData(regStart + 61, 0);
        }else if (i_data == 0)
        {
            MemDb::SetRealData(regStart + 61, 1);
            MemDb::SetRealData(regStart + 60, 0);
        }
        
     
        // i_data = values[0] >> 4 & 0b1;      MemDb::SetRealData(regStart + 32, i_data); // A相风机
        // i_data = values[0] >> 5 & 0b1;      MemDb::SetRealData(regStart + 33, i_data); // B相风机
        // i_data = values[0] >> 6 & 0b1;      MemDb::SetRealData(regStart + 34, i_data); // C相风机
        i_data = values[0] >> 7 & 0b1;      MemDb::SetRealData(regStart + 35, i_data); // A相过温
        i_data = values[0] >> 8 & 0b1;      MemDb::SetRealData(regStart + 36, i_data); // B相过温
        i_data = values[0] >> 9 & 0b1;      MemDb::SetRealData(regStart + 37, i_data); // C相过温
        i_data = values[0] >> 10 & 0b1;      MemDb::SetRealData(regStart + 38, i_data); // 过压报警
        i_data = values[0] >> 11 & 0b1;      MemDb::SetRealData(regStart + 39, i_data); // 急停报警
        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", name);
    }

    const int power_level[5]     = {100, 200, 300, 400, 1000};
    const int res_power_level[7] = {100, 200, 300, 400, 1000, 2000, 3000};

    int phase_a_res[7] = {0};
    int phase_a_ind[5] = {0};
    int phase_a_cap[5] = {0};

    int phase_b_res[7] = {0};
    int phase_b_ind[5] = {0};
    int phase_b_cap[5] = {0};

    int phase_c_res[7] = {0};
    int phase_c_ind[5] = {0};
    int phase_c_cap[5] = {0};

    int phase_a_r = 0, phase_a_l = 0, phase_a_c = 0;
    int phase_b_r = 0, phase_b_l = 0, phase_b_c = 0;
    int phase_c_r = 0, phase_c_l = 0, phase_c_c = 0;

    start  = 310;
    length = 9;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        // clang-format off
        comm &= true;
        phase_a_res[0] = (values[0] >> 0) & 0b1;
        phase_a_res[1] = (values[0] >> 1) & 0b1;
        phase_a_res[2] = (values[0] >> 2) & 0b1;
        phase_a_res[3] = (values[0] >> 3) & 0b1;
        phase_a_res[4] = (values[0] >> 4) & 0b1;
        phase_a_res[5] = (values[0] >> 5) & 0b1;
        phase_a_res[6] = (values[0] >> 6) & 0b1;

        phase_a_ind[0] = (values[1] >> 0) & 0b1;
        phase_a_ind[1] = (values[1] >> 1) & 0b1;
        phase_a_ind[2] = (values[1] >> 2) & 0b1;
        phase_a_ind[3] = (values[1] >> 3) & 0b1;
        phase_a_ind[4] = (values[1] >> 4) & 0b1;

        phase_a_cap[0] = (values[2] >> 0) & 0b1;
        phase_a_cap[1] = (values[2] >> 1) & 0b1;
        phase_a_cap[2] = (values[2] >> 2) & 0b1;
        phase_a_cap[3] = (values[2] >> 3) & 0b1;
        phase_a_cap[4] = (values[2] >> 4) & 0b1;

        phase_b_res[0] = (values[3] >> 0) & 0b1;
        phase_b_res[1] = (values[3] >> 1) & 0b1;
        phase_b_res[2] = (values[3] >> 2) & 0b1;
        phase_b_res[3] = (values[3] >> 3) & 0b1;
        phase_b_res[4] = (values[3] >> 4) & 0b1;
        phase_b_res[5] = (values[3] >> 5) & 0b1;
        phase_b_res[6] = (values[3] >> 6) & 0b1;

        phase_b_ind[0] = (values[4] >> 0) & 0b1;
        phase_b_ind[1] = (values[4] >> 1) & 0b1;
        phase_b_ind[2] = (values[4] >> 2) & 0b1;
        phase_b_ind[3] = (values[4] >> 3) & 0b1;
        phase_b_ind[4] = (values[4] >> 4) & 0b1;

        phase_b_cap[0] = (values[5] >> 0) & 0b1;
        phase_b_cap[1] = (values[5] >> 1) & 0b1;
        phase_b_cap[2] = (values[5] >> 2) & 0b1;
        phase_b_cap[3] = (values[5] >> 3) & 0b1;
        phase_b_cap[4] = (values[5] >> 4) & 0b1;

        phase_c_res[0] = (values[6] >> 0) & 0b1;
        phase_c_res[1] = (values[6] >> 1) & 0b1;
        phase_c_res[2] = (values[6] >> 2) & 0b1;
        phase_c_res[3] = (values[6] >> 3) & 0b1;
        phase_c_res[4] = (values[6] >> 4) & 0b1;
        phase_c_res[5] = (values[6] >> 5) & 0b1;
        phase_c_res[6] = (values[6] >> 6) & 0b1;

        phase_c_ind[0] = (values[7] >> 0) & 0b1;
        phase_c_ind[1] = (values[7] >> 1) & 0b1;
        phase_c_ind[2] = (values[7] >> 2) & 0b1;
        phase_c_ind[3] = (values[7] >> 3) & 0b1;
        phase_c_ind[4] = (values[7] >> 4) & 0b1;

        phase_c_cap[0] = (values[8] >> 0) & 0b1;
        phase_c_cap[1] = (values[8] >> 1) & 0b1;
        phase_c_cap[2] = (values[8] >> 2) & 0b1;
        phase_c_cap[3] = (values[8] >> 3) & 0b1;
        phase_c_cap[4] = (values[8] >> 4) & 0b1;

        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", name);
    }

    for (int i = 0; i < 7; i++) {
        if (i < 5) {
            phase_a_l += phase_a_ind[i] * power_level[i];
            phase_a_c += phase_a_cap[i] * power_level[i];

            phase_b_l += phase_b_ind[i] * power_level[i];
            phase_b_c += phase_b_cap[i] * power_level[i];

            phase_c_l += phase_c_ind[i] * power_level[i];
            phase_c_c += phase_c_cap[i] * power_level[i];
        }
        phase_a_r += phase_a_res[i] * res_power_level[i];

        phase_b_r += phase_b_res[i] * res_power_level[i];

        phase_c_r += phase_c_res[i] * res_power_level[i];
    }
    MemDb::SetRealData(regStart + 40, phase_a_r); // A相阻性功率档位
    MemDb::SetRealData(regStart + 41, phase_a_l); // A相电感功率档位
    MemDb::SetRealData(regStart + 42, phase_a_c); // A相电容功率档位
    MemDb::SetRealData(regStart + 43, phase_b_r); // B相阻性功率档位
    MemDb::SetRealData(regStart + 44, phase_b_l); // B相电感功率档位
    MemDb::SetRealData(regStart + 45, phase_b_c); // B相电容功率档位
    MemDb::SetRealData(regStart + 46, phase_c_r); // C相阻性功率档位
    MemDb::SetRealData(regStart + 47, phase_c_l); // C相电感功率档位
    MemDb::SetRealData(regStart + 48, phase_c_c); // C相电容功率档位

    // 预制档位
    start     = 410;
    length    = 9;
    phase_a_r = 0, phase_a_l = 0, phase_a_c = 0;
    phase_b_r = 0, phase_b_l = 0, phase_b_c = 0;
    phase_c_r = 0, phase_c_l = 0, phase_c_c = 0;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        // clang-format off
        comm &= true;
        phase_a_res[0] = (values[0] >> 0) & 0b1;
        phase_a_res[1] = (values[0] >> 1) & 0b1;
        phase_a_res[2] = (values[0] >> 2) & 0b1;
        phase_a_res[3] = (values[0] >> 3) & 0b1;
        phase_a_res[4] = (values[0] >> 4) & 0b1;
        phase_a_res[5] = (values[0] >> 5) & 0b1;
        phase_a_res[6] = (values[0] >> 6) & 0b1;

        phase_a_ind[0] = (values[1] >> 0) & 0b1;
        phase_a_ind[1] = (values[1] >> 1) & 0b1;
        phase_a_ind[2] = (values[1] >> 2) & 0b1;
        phase_a_ind[3] = (values[1] >> 3) & 0b1;
        phase_a_ind[4] = (values[1] >> 4) & 0b1;

        phase_a_cap[0] = (values[2] >> 0) & 0b1;
        phase_a_cap[1] = (values[2] >> 1) & 0b1;
        phase_a_cap[2] = (values[2] >> 2) & 0b1;
        phase_a_cap[3] = (values[2] >> 3) & 0b1;
        phase_a_cap[4] = (values[2] >> 4) & 0b1;

        phase_b_res[0] = (values[3] >> 0) & 0b1;
        phase_b_res[1] = (values[3] >> 1) & 0b1;
        phase_b_res[2] = (values[3] >> 2) & 0b1;
        phase_b_res[3] = (values[3] >> 3) & 0b1;
        phase_b_res[4] = (values[3] >> 4) & 0b1;

        phase_b_ind[0] = (values[4] >> 0) & 0b1;
        phase_b_ind[1] = (values[4] >> 1) & 0b1;
        phase_b_ind[2] = (values[4] >> 2) & 0b1;
        phase_b_ind[3] = (values[4] >> 3) & 0b1;
        phase_b_ind[4] = (values[4] >> 4) & 0b1;

        phase_b_cap[0] = (values[5] >> 0) & 0b1;
        phase_b_cap[1] = (values[5] >> 1) & 0b1;
        phase_b_cap[2] = (values[5] >> 2) & 0b1;
        phase_b_cap[3] = (values[5] >> 3) & 0b1;
        phase_b_cap[4] = (values[5] >> 4) & 0b1;

        phase_c_res[0] = (values[6] >> 0) & 0b1;
        phase_c_res[1] = (values[6] >> 1) & 0b1;
        phase_c_res[2] = (values[6] >> 2) & 0b1;
        phase_c_res[3] = (values[6] >> 3) & 0b1;
        phase_c_res[4] = (values[6] >> 4) & 0b1;

        phase_c_ind[0] = (values[7] >> 0) & 0b1;
        phase_c_ind[1] = (values[7] >> 1) & 0b1;
        phase_c_ind[2] = (values[7] >> 2) & 0b1;
        phase_c_ind[3] = (values[7] >> 3) & 0b1;
        phase_c_ind[4] = (values[7] >> 4) & 0b1;

        phase_c_cap[0] = (values[8] >> 0) & 0b1;
        phase_c_cap[1] = (values[8] >> 1) & 0b1;
        phase_c_cap[2] = (values[8] >> 2) & 0b1;
        phase_c_cap[3] = (values[8] >> 3) & 0b1;
        phase_c_cap[4] = (values[8] >> 4) & 0b1;

        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", name);
    }

    for (int i = 0; i < 7; i++) {
        if (i < 5) {
            phase_a_l += phase_a_ind[i] * power_level[i];
            phase_a_c += phase_a_cap[i] * power_level[i];

            phase_b_l += phase_b_ind[i] * power_level[i];
            phase_b_c += phase_b_cap[i] * power_level[i];

            phase_c_l += phase_c_ind[i] * power_level[i];
            phase_c_c += phase_c_cap[i] * power_level[i];
        }
        phase_a_r += phase_a_res[i] * res_power_level[i];

        phase_b_r += phase_b_res[i] * res_power_level[i];

        phase_c_r += phase_c_res[i] * res_power_level[i];
    }
    sumA = sumA + sumB + sumC;

    MemDb::SetRealData(regStart + 49, sumA);      // 阻性总功率
    MemDb::SetRealData(regStart + 50, phase_a_r); // A相阻性功率档位 反馈
    MemDb::SetRealData(regStart + 51, phase_a_l); // A相电感功率档位
    MemDb::SetRealData(regStart + 52, phase_a_c); // A相电容功率档位
    MemDb::SetRealData(regStart + 53, phase_b_r); // B相阻性功率档位
    MemDb::SetRealData(regStart + 54, phase_b_l); // B相电感功率档位
    MemDb::SetRealData(regStart + 55, phase_b_c); // B相电容功率档位
    MemDb::SetRealData(regStart + 56, phase_c_r); // C相阻性功率档位
    MemDb::SetRealData(regStart + 57, phase_c_l); // C相电感功率档位
    MemDb::SetRealData(regStart + 58, phase_c_c); // C相电容功率档位

    MemDb::SetRealData(regStart + 0, comm);
    return ret;
}

int RLCLoad::RLC_b_Read(Device::SlaveDev *dev) {
    int ret  = ErrorInfo::ERR_OK;
    int addr = atoi(dev->slaveAddr.c_str());

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
        return ErrorInfo::ERR_NULL;
    }

    if (!this->m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "网口未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    UshortArray values;
    ShortArray svalues;
    values.clear();
    svalues.clear();

    // 下发读状态命令格式 （功能码03）
    // 地址（1字节）+功能码（1字节）+ 起始寄存器地址（2字节） +
    // 寄存器数量（2字节） +校验码（2字节）
    //     01             03                 00 64                   00 62 XX XX
    // 本机默认6
    // 数据返回格式
    // 地址(1字节)+功能码(1字节)+数据长度(字节数)+数据1+数据2+数据3+数据4+数据5+数据6+数据7+数据8+数据9+数据10
    //     01            03             1A          XX     XX    XX     XX XX XX
    //     XX    XX    XX     XX
    // +数据11+数据12+数据13+数据14+数据15+数据16+数据17+数据18+数据19+数据20+数据21+数据22+数据23+数据24
    //         XX      XX     XX      XX     XX      XX     XX      XX      XX
    //         XX     XX      XX      XX      XX
    //     +数据25+数据26+数据27+数据28+数据29+数据30+数据31+数据32+数据33+数据34+数据35+数据36+数据37+数据38
    //     XX     XX      XX       XX      XX     XX     XX      XX     XX XX XX
    //     XX     XX     XX
    // +数据39+数据40+数据41+数据42+数据43+数据44+数据45+数据46+数据47+数据48+数据49+数据50+数据51+数据52
    // XX     XX      XX       XX      XX     XX     XX      XX     XX      XX
    // XX     XX     XX     XX
    // +数据53+数据54+数据55+数据56+数据57+数据58+数据59+数据60+数据61+数据62+数据63+数据64+数据65+数据66
    // XX     XX      XX       XX      XX     XX     XX      XX     XX      XX
    // XX     XX     XX     XX
    // +数据67+数据68+数据69+数据70+数据71+数据72+数据73+数据74+数据75+数据76+数据77+数据78+数据79+数据80
    // XX     XX      XX       XX      XX     XX     XX      XX     XX      XX
    // XX     XX     XX     XX
    // +数据81+数据82+数据83+数据84+数据85+数据86+数据87+数据88+数据89+数据90+数据91+数据92+数据93+数据94
    // XX     XX      XX       XX      XX     XX     XX      XX     XX      XX
    // XX     XX     XX     XX +数据95+数据96+数据97+数据98+校验码（2字节） XX
    // XX      XX       XX      XX XX 数据返回数据说明：
    //         寄存器	数据及状态
    // 数据1	工作状态	15	备用	备用
    //         14	备用	备用
    //         13	备用	备用
    //         12	备用	备用
    //         11	备用	备用
    //         10	备用	备用
    //         9	备用	备用
    //         8	备用	备用
    // 数据2	工作状态	7	 1：过温报警  	0：未过温
    //         6	1：过压报警状态	0：未过压状态
    //         5	1：急停报警状态	0：未急停状态
    //         4	  备用	备用
    //         3	1：=加载状态	0：卸载状态
    //         2	备用	备用
    //         1	备用	备用
    //         0	备用	备用

    // 数据3+数据4+数据5+数据6
    // 转换10进制（双精度整数）=A相R档位加载功率（单位：W）
    // 数据7+数据8+数据9+数据10
    // 转换10进制（双精度整数）=A相L档位加载功率（单位：Var）
    // 数据11+数据12+数据13+数据14
    // 转换10进制（双精度整数）=A相C档位加载功率（单位：Var）
    // 数据15+数据16+数据17+数据18
    // 转换10进制（双精度整数）=B相R档位加载功率（单位：W）
    // 数据19+数据20+数据21+数据22
    // 转换10进制（双精度整数）=B相L档位加载功率（单位：Var）
    // 数据23+数据24+数据25+数据26
    // 转换10进制（双精度整数）=B相C档位加载功率（单位：Var）
    // 数据27+数据28+数据29+数据30
    // 转换10进制（双精度整数）=C相R档位加载功率（单位：W）
    // 数据31+数据32+数据33+数据34
    // 转换10进制（双精度整数）=C相L档位加载功率（单位：Var）
    // 数据35+数据36+数据37+数据38
    // 转换10进制（双精度整数）=C相C档位加载功率（单位：Var） 电量采集值
    // 注：U0(电压量程)=260，Ur(电压变比)=1，
    // I0(电流量程)=20，Ir(电流变比)=1
    //     寄存器地址	定义	数据说明	数据类型与计算 说明
    // 数据39+数据40	0077H(119)	Ua	A相相电压
    // 无符号数，0～10000对应0～满量程 实际值=DATA/10000*U0*Ur，单位V
    // 数据41+数据42	0078H(120)	Ub	B相相电压
    // 数据43+数据44	0079H(121)	Uc	C相相电压
    // 数据45+数据46	007AH(122)	Ulnavg	平均相电压
    // 数据47+数据48	007BH(123)	Uab	AB线电压	无符号数
    // 实际值=DATA/10000*U0*Ur
    // 单位V
    // 数据49+数据50	007CH(124)	Ubc	BC线电压
    // 数据51+数据52	007DH(125)	Uca	CA线电压
    // 数据53+数据54	007EH(126)	Ullavg	平均线电压
    // 数据55+数据56	007FH(127)	Ia	A相电流	无符号数
    // 实际值=DATA/10000*I0*Ir
    // 单位A
    // 数据57+数据58	0080H(128)	Ib	B相电流
    // 数据59+数据60	0081H(129)	Ic	C相电流
    // 数据61+数据62	0082H(130)	Iavg	平均电流
    // 数据63+数据64	0083H(131)	Pa	A相有功功率	补码格式有符号数
    // 实际值= DATA/10000*U0*Ur*I0*Ir
    // 单位W
    // 数据65+数据66	0084H(132)	Pb	B相有功功率
    // 数据67+数据68	0085H(133)	Pc	C相有功功率
    // 数据69+数据70	0086H(134)	Psum	总有功功率	补码格式有符号数
    // ;单位W 实际值=DATA/10000*3*U0*Ur*I0*Ir 数据71+数据72	0087H(135)
    // Qa	A相无功功率	补码格式有符号数 实际值= DATA/10000*U0*Ur*I0*Ir
    // 单位Var
    // 数据73+数据74	0088H(136)	Qb	B相无功功率
    // 数据75+数据76	0089H(137)	Qc	C相无功功率
    // 数据77+数据78	008AH(138)	Qsum	总无功功率
    // 补码格式有符号数;  单位Var 实际值=DATA/10000*3*U0*Ur*I0*Ir 数据79+数据80
    // 008BH(139)	Sa	A相视在功率	无符号数 实际值=
    // DATA/10000*U0*Ur*I0*Ir 单位VA 数据81+数据82	008CH(140)	Sb
    // B相视在功率 数据83+数据84	008DH(141)	Sc	C相视在功率
    // 数据85+数据86	008EH(142)	Ssum	总视在功率	无符号数; 单位VA
    // 实际值=DATA/10000*3*U0*Ur*I0*Ir
    // 数据87+数据88	008FH(143)	PFa	A相功率因数	补码格式有符号数
    // 实际值=DATA/10000
    // 数据89+数据90	0090H(144)	PFb	B相功率因数
    // 数据91+数据92	0091H(145)	PFc	C相功率因数
    // 数据93+数据94	0092H(146)	PF	总功率因数	补码格式有符号数
    // 实际值=DATA/10000
    // 数据95+数据96	0093H(147)	FR	频率	无符号数
    // 实际值=DATA/100，单位Hz；
    // 数据97+数据98	0094H(148)	Temp	模块内部温度
    // 补码格式有符号数；实际值=DATA/10，单位℃；注：温度值只作参考

    // 读取数据
    int start  = 0x64;
    int length = 0x62;
    ret        = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        // clang-format off
        int regStart = dev->regStart;

        int i_data = 0;
        float f_data = 0;
        bool comm = true;
        comm &= true;
        i_data = values[1] >> 7 & 0b1;      MemDb::SetRealData(regStart + 0, i_data); // 过温报警
        i_data = values[1] >> 6 & 0b1;      MemDb::SetRealData(regStart + 1, i_data); // 过压报警
        i_data = values[1] >> 5 & 0b1;      MemDb::SetRealData(regStart + 2, i_data); // 急停报警
        i_data = values[1] >> 3 & 0b1;      MemDb::SetRealData(regStart + 3, i_data); // 加载状态

        // 数据3+数据4+数据5+数据6 转换10进制（双精度整数）=A相R档位加载功率（单位：W）
        i_data = values[2] + (values[3] << 8)+ (values[4] << 16) + (values[5] << 24); 
        MemDb::SetRealData(regStart + 4, i_data);
        // 数据7+数据8+数据9+数据10 转换10进制（双精度整数）=A相L档位加载功率（单位：Var）
        i_data = values[6] + (values[7] << 8)+ (values[8] << 16) + (values[9] << 24);
        MemDb::SetRealData(regStart + 5, i_data);
        // 数据11+数据12+数据13+数据14 转换10进制（双精度整数）=A相C档位加载功率（单位：Var）
        i_data = values[10] + (values[11] << 8)+ (values[12] << 16) + (values[13] << 24);
        MemDb::SetRealData(regStart + 6, i_data);
        // 数据15+数据16+数据17+数据18 转换10进制（双精度整数）=B相R档位加载功率（单位：W）
        i_data = values[14] + (values[15] << 8)+ (values[16] << 16) + (values[17] << 24);
        MemDb::SetRealData(regStart + 7, i_data);
        // 数据19+数据20+数据21+数据22 转换10进制（双精度整数）=B相L档位加载功率（单位：Var）
        i_data = values[18] + (values[19] << 8)+ (values[20] << 16) + (values[21] << 24);
        MemDb::SetRealData(regStart + 8, i_data);
        // 数据23+数据24+数据25+数据26 转换10进制（双精度整数）=B相C档位加载功率（单位：Var）
        i_data = values[22] + (values[23] << 8)+ (values[24] << 16) + (values[25] << 24);
        MemDb::SetRealData(regStart + 9, i_data);
        // 数据27+数据28+数据29+数据30 转换10进制（双精度整数）=C相R档位加载功率（单位：W）
        i_data = values[26] + (values[27] << 8)+ (values[28] << 16) + (values[29] << 24);
        MemDb::SetRealData(regStart + 10, i_data);
        // 数据31+数据32+数据33+数据34 转换10进制（双精度整数）=C相L档位加载功率（单位：Var）
        i_data = values[30] + (values[31] << 8)+ (values[32] << 16) + (values[33] << 24);
        MemDb::SetRealData(regStart + 11, i_data);
        // 数据35+数据36+数据37+数据38 转换10进制（双精度整数）=C相C档位加载功率（单位：Var）
        i_data = values[34] + (values[35] << 8)+ (values[36] << 16) + (values[37] << 24);
        MemDb::SetRealData(regStart + 12, i_data);

        // 电量采集值
           // 注：U0(电压量程)=260，Ur(电压变比)=1，
        // I0(电流量程)=20，Ir(电流变比)=1 
        //     寄存器地址	定义	数据说明	数据类型与计算 说明
        // 数据39+数据40	0077H(119)	Ua	A相相电压	无符号数，0～10000对应0～满量程
        // 实际值=DATA/10000*U0*Ur，单位V
        // 数据41+数据42	0078H(120)	Ub	B相相电压	
        // 数据43+数据44	0079H(121)	Uc	C相相电压	
        // 数据45+数据46	007AH(122)	Ulnavg	平均相电压	
        // 数据47+数据48	007BH(123)	Uab	AB线电压	无符号数
        // 实际值=DATA/10000*U0*Ur
        // 单位V
        // 数据49+数据50	007CH(124)	Ubc	BC线电压	
        // 数据51+数据52	007DH(125)	Uca	CA线电压	
        // 数据53+数据54	007EH(126)	Ullavg	平均线电压	
        // 数据55+数据56	007FH(127)	Ia	A相电流	无符号数
        // 实际值=DATA/10000*I0*Ir
        // 单位A
        // 数据57+数据58	0080H(128)	Ib	B相电流	
        // 数据59+数据60	0081H(129)	Ic	C相电流	
        // 数据61+数据62	0082H(130)	Iavg	平均电流	
        // 数据63+数据64	0083H(131)	Pa	A相有功功率	补码格式有符号数
        // 实际值= DATA/10000*U0*Ur*I0*Ir
        // 单位W
        // 数据65+数据66	0084H(132)	Pb	B相有功功率	
        // 数据67+数据68	0085H(133)	Pc	C相有功功率	
        // 数据69+数据70	0086H(134)	Psum	总有功功率	补码格式有符号数 ;单位W
        // 实际值=DATA/10000*3*U0*Ur*I0*Ir
        // 数据71+数据72	0087H(135)	Qa	A相无功功率	补码格式有符号数
        // 实际值= DATA/10000*U0*Ur*I0*Ir
        // 单位Var
        // 数据73+数据74	0088H(136)	Qb	B相无功功率	
        // 数据75+数据76	0089H(137)	Qc	C相无功功率	
        // 数据77+数据78	008AH(138)	Qsum	总无功功率	补码格式有符号数;  单位Var
        // 实际值=DATA/10000*3*U0*Ur*I0*Ir
        // 数据79+数据80	008BH(139)	Sa	A相视在功率	无符号数
        // 实际值= DATA/10000*U0*Ur*I0*Ir
        // 单位VA
        // 数据81+数据82	008CH(140)	Sb	B相视在功率	
        // 数据83+数据84	008DH(141)	Sc	C相视在功率	
        // 数据85+数据86	008EH(142)	Ssum	总视在功率	无符号数;   单位VA
        // 实际值=DATA/10000*3*U0*Ur*I0*Ir
        // 数据87+数据88	008FH(143)	PFa	A相功率因数	补码格式有符号数
        // 实际值=DATA/10000
        // 数据89+数据90	0090H(144)	PFb	B相功率因数	
        // 数据91+数据92	0091H(145)	PFc	C相功率因数	
        // 数据93+数据94	0092H(146)	PF	总功率因数	补码格式有符号数
        // 实际值=DATA/10000
        // 数据95+数据96	0093H(147)	FR	频率	无符号数
        // 实际值=DATA/100，单位Hz；
        // 数据97+数据98	0094H(148)	Temp	模块内部温度	补码格式有符号数；实际值=DATA/10，单位℃；注：温度值只作参考

        // A相电压
        i_data = values[38] + (values[39] << 8);
        f_data = i_data / 10000.0 * 260.0;
        MemDb::SetRealData(regStart + 13, f_data);
        // B相电压
        i_data = values[40] + (values[41] << 8);
        f_data = i_data / 10000.0 * 260.0;
        MemDb::SetRealData(regStart + 14, f_data);
        // C相电压
        i_data = values[42] + (values[43] << 8);
        f_data = i_data / 10000.0 * 260.0;
        MemDb::SetRealData(regStart + 15, f_data);
        // 平均相电压
        i_data = values[44] + (values[45] << 8);
        f_data = i_data / 10000.0 * 260.0;
        MemDb::SetRealData(regStart + 16, f_data);
        // AB线电压
        i_data = values[46] + (values[47] << 8);
        f_data = i_data / 10000.0 * 260.0;
        MemDb::SetRealData(regStart + 17, f_data);
        // BC线电压
        i_data = values[48] + (values[49] << 8);
        f_data = i_data / 10000.0 * 260.0;
        MemDb::SetRealData(regStart + 18, f_data);
        // CA线电压
        i_data = values[50] + (values[51] << 8);
        f_data = i_data / 10000.0 * 260.0;
        MemDb::SetRealData(regStart + 19, f_data);
        // 平均线电压
        i_data = values[52] + (values[53] << 8);
        f_data = i_data / 10000.0 * 260.0;
        MemDb::SetRealData(regStart + 20, f_data);
        // A相电流
        i_data = values[54] + (values[55] << 8);
        f_data = i_data / 10000.0 * 20.0;
        MemDb::SetRealData(regStart + 21, f_data);
        // B相电流
        i_data = values[56] + (values[57] << 8);
        f_data = i_data / 10000.0 * 20.0;
        MemDb::SetRealData(regStart + 22, f_data);
        // C相电流
        i_data = values[58] + (values[59] << 8);
        f_data = i_data / 10000.0 * 20.0;
        MemDb::SetRealData(regStart + 23, f_data);
        // 平均电流
        i_data = values[60] + (values[61] << 8);
        f_data = i_data / 10000.0 * 20.0;
        MemDb::SetRealData(regStart + 24, f_data);
        // A相有功功率
        i_data = values[62] + (values[63] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 25, f_data);
        // B相有功功率
        i_data = values[64] + (values[65] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 26, f_data);
        // C相有功功率
        i_data = values[66] + (values[67] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 27, f_data);
        // 总有功功率
        i_data = values[68] + (values[69] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0 * 3;
        MemDb::SetRealData(regStart + 28, f_data);
        // A相无功功率
        i_data = values[70] + (values[71] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 29, f_data);
        // B相无功功率
        i_data = values[72] + (values[73] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 30, f_data);
        // C相无功功率
        i_data = values[74] + (values[75] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 31, f_data);
        // 总无功功率
        i_data = values[76] + (values[77] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0 * 3;
        MemDb::SetRealData(regStart + 32, f_data);
        // A相视在功率
        i_data = values[78] + (values[79] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 33, f_data);
        // B相视在功率
        i_data = values[80] + (values[81] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 34, f_data);
        // C相视在功率
        i_data = values[82] + (values[83] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0;
        MemDb::SetRealData(regStart + 35, f_data);
        // 总视在功率
        i_data = values[84] + (values[85] << 8);
        f_data = i_data / 10000.0 * 260.0 * 20.0 * 3;
        MemDb::SetRealData(regStart + 36, f_data);
        // A相功率因数
        i_data = values[86] + (values[87] << 8);
        f_data = i_data / 10000.0;
        MemDb::SetRealData(regStart + 37, f_data);
        // B相功率因数
        i_data = values[88] + (values[89] << 8);
        f_data = i_data / 10000.0;
        MemDb::SetRealData(regStart + 38, f_data);
        // C相功率因数
        i_data = values[90] + (values[91] << 8);
        f_data = i_data / 10000.0;
        MemDb::SetRealData(regStart + 39, f_data);
        // 总功率因数
        i_data = values[92] + (values[93] << 8);
        f_data = i_data / 10000.0;
        MemDb::SetRealData(regStart + 40, f_data);
        // 频率
        i_data = values[94] + (values[95] << 8);
        f_data = i_data / 100.0;
        MemDb::SetRealData(regStart + 41, f_data);
        // 模块内部温度
        i_data = values[96] + (values[97] << 8);
        f_data = i_data / 10.0; 
        MemDb::SetRealData(regStart + 42, f_data);
        // clang-format on

        // 保存数据
        MemDb::SetRealData(regStart, 1);
        zlog_info(Util::m_zlog, "< %s > 读取数据成功", dev->name.c_str());
    } else {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
    }

    return ret;
}

int RLCLoad::RLC_c_Read(Device::SlaveDev *dev) {
    int ret  = ErrorInfo::ERR_OK;
    int addr = atoi(dev->slaveAddr.c_str());

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
        return ErrorInfo::ERR_NULL;
    }

    if (!this->m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "网口未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    UshortArray values;
    ShortArray svalues;
    values.clear();
    svalues.clear();

    //  下发读状态命令格式 （功能码03）
    //  地址（1字节）+功能码（1字节）+ 起始寄存器地址（2字节） +
    //  寄存器数量（2字节） +校验码（2字节）
    //        01             03                 00 64                   00 1A XX
    //        XX
    // 本机默认6
    // 数据返回格式
    // 地址(1字节)+功能码(1字节)+数据长度(字节数)+数据1+数据2+数据3+数据4+数据5+数据6+数据7+数据8+数据9+数据10
    //     01            03             1A          XX     XX    XX     XX XX XX
    //     XX    XX    XX     XX
    // +数据11+数据12+数据13+数据14+数据15+数据16+数据17+数据18+数据19+数据20+数据21+数据22+数据23+数据24
    //      XX      XX     XX      XX     XX      XX     XX      XX      XX XX
    //      XX      XX      XX      XX
    //     +数据25+数据26+校验码（2字节）
    //      XX      XX        XX XX
    // 数据返回数据说明：
    //  		寄存器	数据及状态
    // 数据1	工作状态	15	备用	备用
    // 		14	备用	备用
    // 		13	备用	备用
    // 		12	备用	备用
    // 		11	备用	备用
    // 		10	备用	备用
    // 		9	备用	备用
    // 		8	备用	备用
    // 数据2	工作状态	7	 1：过温报警  	0：未过温
    // 		6	1：过压报警状态	0：未过压状态
    // 		5	1：急停报警状态	0：未急停状态
    // 		4	  备用	备用
    // 		3	1：=加载状态	0：卸载状态
    // 		2	备用	备用
    // 		1	备用	备用
    // 		0	备用	备用

    // 数据3 + 数据4 + 数据5 + 数据6
    // 转换10进制（双精度整数）=档位加载功率（单位：W） 数据7+数据8+数据9+数据10
    // 备用 电量采集值 注：U0(电压量程)=260，Ur(电压变比)=1，
    // I0(电流量程)=5或3(1000W负载模块为5，500W负载模块为3)，Ir(电流变比)=1
    // 	寄存器地址	定义	数据说明	数据类型与计算 说明
    // 数据11+数据12	0069H(105)	U	电压
    // 无符号数，0～10000对应0～满量程 实际值=DATA/10000*U0*Ur：单位V
    // 数据13+数据14	006AH(106)	I	电流
    // 无符号数，0～10000对应0～满量程 实际值=DATA/10000*I0*Ir：单位A
    // 数据15+数据16	006BH(107)	P	有功功率	补码格式有符号数
    // 实际值=DATA/10000*U0*Ur*I0*Ir：单位W
    // 数据17+数据18	006CH(108)	Q	无功功率	补码格式有符号数
    // 实际值=DATA/10000*U0*Ur*I0*Ir：单位Var
    // 数据19+数据20	006DH(109)	S	视在功率	无符号数
    // 实际值=DATA/10000*U0*Ur*I0*Ir：单位VA
    // 数据21+数据22	006EH(110)	PF	功率因数	补码格式有符号数
    // 实际值=DATA/10000
    // 数据23+数据24	006FH(111)	FR	频率	无符号数
    // 实际值=DATA/100：单位Hz
    // 数据25+数据26	0070H(112)	Temp	模块内部温度	补码格式有符号数
    // 实际值=DATA/10，单位℃：注：温度值只作参考

    // 读取数据
    int start    = 0x64;
    int length   = 9;
    int regStart = dev->regStart;
    int regEnd   = regStart + 26;
    int i_data   = 0;
    float f_data = 0;
    bool comm    = true;
    int U0 = 260, Ur = 1, I0 = 0, Ir = 1;
    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        // clang-format off
        
        // 工作状态
        // 数据1	工作状态	15	备用	备用
        // 		14	备用	备用
        // 		13	备用	备用
        // 		12	备用	备用
        // 		11	备用	备用
        // 		10	备用	备用
        // 		9	备用	备用
        // 		8	备用	备用
        // 数据2	工作状态
    	//      7	1：过温报警  	0：未过温
        // 		6	1：过压报警状态	0：未过压状态
        // 		5	1：急停报警状态	0：未急停状态
        // 		4	  备用	备用
        // 		3	1：=加载状态	0：卸载状态
        // 		2	备用	备用
        // 		1	备用	备用
        // 		0	备用	备用
        i_data = values[0] >> 7 & 0b1;      MemDb::SetRealData(regStart + 1, i_data); // 过温报警
        i_data = values[0] >> 6 & 0b1;      MemDb::SetRealData(regStart + 2, i_data); // 过压报警
        i_data = values[0] >> 5 & 0b1;      MemDb::SetRealData(regStart + 3, i_data); // 急停报警
        i_data = values[0] >> 3 & 0b1;           MemDb::SetRealData(regStart + 4, i_data); // 加载状态
        
        // 功率 数据3 + 数据4 + 数据5 + 数据6
        // 转换10进制（双精度整数）=档位加载功率（单位：W）

        f_data = (values[1]<<16)+values[2];
        MemDb::SetRealData(regStart + 5, f_data);

        // 保存数据
		MemDb::SetRealData(regStart + 0, comm);
        zlog_info(Util::m_zlog, "< %s > 读取数据成功", dev->name.c_str());
    } 
    else {
        MemDb::SetRealData(regStart + 0, comm);
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
    }

    start  = 105;
    length = 8;
    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {

        if (dev->commNo==500)
        {
            I0=3;
        }else if (dev->commNo==1000)
        {
            I0=5;
        }
        f_data=values[0]*0.0001*U0*Ur;//电压
        MemDb::SetRealData(regStart + 6, f_data);
        f_data=values[1]*0.0001*I0*Ir;//电流
        MemDb::SetRealData(regStart + 7, f_data);
        f_data=values[2]*0.0001*U0*Ur*I0*Ir;//有功功率
        MemDb::SetRealData(regStart + 8, f_data);
        f_data=values[3]*0.0001*U0*Ur*I0*Ir;//无功功率
        MemDb::SetRealData(regStart + 9, f_data);
        f_data=values[4]*0.0001*U0*Ur*I0*Ir;//视在功率
        MemDb::SetRealData(regStart + 10, f_data);
        f_data=values[5]*0.0001;//功率因数
        MemDb::SetRealData(regStart + 11, f_data);
        f_data=values[6]*0.01;//频率
        MemDb::SetRealData(regStart + 12, f_data);
        f_data=values[7]*0.1;//模块内部温度
        MemDb::SetRealData(regStart + 13, f_data);
        // 保存数据
		MemDb::SetRealData(regStart + 0, comm);
        zlog_info(Util::m_zlog, "< %s > 读取数据成功", dev->name.c_str());
    }
    else {
        MemDb::SetRealData(regStart + 0, comm);
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
    }
    // printf( "< %s > \n", dev->name.c_str());
    // for (size_t i = 0; i < 8; i++)
    // {
    //     printf("values[%d]:%d\n",i,values[i]);
    //     printf("values[%d]:%d\n",i,values[i]);
    //     printf("------------------\n");
    // }
    
    return ret;
}

int RLCLoad::GetMeterInfo(Device::SlaveDev *dev) {
    int ret  = ErrorInfo::ERR_OK;
    int addr = atoi(dev->slaveAddr.c_str());

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
        return ErrorInfo::ERR_NULL;
    }

    if (!this->m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "网口未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    UshortArray values;
    values.clear();

    int start  = 2;
    int length = 5;

    // 获取A相电表信息
    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        m_volt_range[0]  = values[0];
        m_cur_range[0]   = values[1];
        m_volt_ratio[0]  = values[3];
        m_cur_ratio[0]   = values[4];
        m_volt_index[0]  = m_volt_range[0] * m_volt_ratio[0] * 0.0001;
        m_cur_index[0]   = m_cur_ratio[0] * m_cur_range[0] * 0.0001;
        m_power_index[0] = m_volt_range[0] * m_cur_range[0] * 0.0001 *
                           m_volt_ratio[0] * m_cur_ratio[0];
    } else {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
    }

    start  = 9;
    length = 5;
    // 获取B相电表信息
    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        m_volt_range[1]  = values[0];
        m_cur_range[1]   = values[1];
        m_volt_ratio[1]  = values[3];
        m_cur_ratio[1]   = values[4];
        m_volt_index[1]  = m_volt_range[1] * m_volt_ratio[1] * 0.0001;
        m_cur_index[1]   = m_cur_ratio[1] * m_cur_range[1] * 0.0001;
        m_power_index[1] = m_volt_range[1] * m_cur_range[1] * 0.0001 *
                           m_volt_ratio[1] * m_cur_ratio[1];
    } else {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
    }

    // 获取C相电表信息
    start  = 0x10;
    length = 5;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        m_volt_range[2] = values[0];
        m_cur_range[2]  = values[1];

        m_volt_ratio[2] = values[3];
        m_cur_ratio[2]  = values[4];

        m_volt_index[2]  = m_volt_range[2] * m_volt_ratio[2] * 0.0001;
        m_cur_index[2]   = m_cur_ratio[2] * m_cur_range[2] * 0.0001;
        m_power_index[2] = m_volt_range[2] * m_cur_range[2] * 0.0001 *
                           m_volt_ratio[2] * m_cur_ratio[2];
    } else {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
    }

    return ret;
}

int RLCLoad::RLC_a_Preset(Device::SlaveDev *dev) {
    int ret              = ErrorInfo::ERR_OK;
    int regStart         = dev->regStart;
    int addr             = atoi(dev->slaveAddr.c_str());
    int data             = 0;
    unsigned short cmd   = 0;
    const int prefix     = 0b11111;
    const int res_prefix = 0b1111111;
    RealData realData;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "设备空");
        return ErrorInfo::ERR_NULL;
    }

    if (!this->m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "网口未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    // A相阻性功率设置
    if (MemDb::GetRealData(realData, regStart + 71, true)) {
        data = realData.pv.data.iValue;
        // // float fdata=realData.pv.data.dbValue *1000;
        // if (data > 1200) {
        //     data = 1200;
        // }
        MemDb::SetRealData(regStart + 82, data, true);
        MemDb::SetRealData(regStart + 74, data, true);
        MemDb::SetRealData(regStart + 77, data, true);

        // cmd = this->GetActivePowerCmd(realData.pv.data.dbValue*1000);
        // zlog_warn(Util::m_zlog, "<%s> 下发A相阻性功率 %d, %d",
        //           dev->name.c_str(), realData.pv.data.iValue, cmd);
        // // 加载选择的档位
        // rtu.PresetSingleRegister(addr, 110, cmd & res_prefix);
        // // 卸载选择的档位
        // rtu.PresetSingleRegister(addr, 210, (~cmd) & res_prefix);
    }

    // A相感性功率设置
    if (MemDb::GetRealData(realData, regStart + 72, true)) {
        data = realData.pv.data.iValue;
        // float fdata=realData.pv.data.dbValue *1000;
        if (data > 0) {
            MemDb::SetRealData(regStart + 83, data, true);
            MemDb::SetRealData(regStart + 75, data, true);
            MemDb::SetRealData(regStart + 78, data, true);
            MemDb::SetRealData(regStart + 73, 0, true);
            MemDb::SetRealData(regStart + 76, 0, true);
            MemDb::SetRealData(regStart + 79, 0, true);

        } else {
            // 取绝对值
            // fdata = -fdata;
            MemDb::SetRealData(regStart + 73, -data, true);
            MemDb::SetRealData(regStart + 76, -data, true);
            MemDb::SetRealData(regStart + 79, -data, true);
            MemDb::SetRealData(regStart + 83, 0, true);
            MemDb::SetRealData(regStart + 75, 0, true);
            MemDb::SetRealData(regStart + 78, 0, true);
        }
        // MemDb::SetRealData(regStart + 75, realData.pv.data.iValue*1000,
        // true); MemDb::SetRealData(regStart + 78,
        // realData.pv.data.iValue*1000, true); cmd =
        // this->GetReactivePowerCmd(fdata);
        // // 加载选择的档位
        // rtu.PresetSingleRegister(addr, 111, cmd & prefix);
        // // 卸载选择的档位
        // rtu.PresetSingleRegister(addr, 211, (~cmd) & prefix);
    }

    // A相阻性功率设置
    if (MemDb::GetRealData(realData, regStart + 82, true)) {
        data = realData.pv.data.iValue;
        // if (data > 7000) {
        //     data = 7000;
        // }
        cmd = this->GetActivePowerCmd(data);

        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 110, cmd & res_prefix);
        // 卸载选择的档位
        rtu.PresetSingleRegister(addr, 210, (~cmd) & res_prefix);
    }

    // A相感性功率设置
    if (MemDb::GetRealData(realData, regStart + 83, true)) {
        data = realData.pv.data.iValue;
        // if (data > 2000) {
        //     data = 2000;
        // }
        cmd = this->GetReactivePowerCmd(data);

        zlog_warn(Util::m_zlog, "<%s> 下发感性功率 %d, %d", dev->name.c_str(),
                  realData.pv.data.iValue, cmd);

        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 111, cmd & prefix);
        // 卸载选择的档位
        rtu.PresetSingleRegister(addr, 211, (~cmd) & prefix);
    }

    // A相容性功率设置
    if (MemDb::GetRealData(realData, regStart + 73, true)) {
        data = realData.pv.data.iValue;
        // if (data > 2000) {
        //     data = 2000;
        // }
        cmd = this->GetReactivePowerCmd(data);

        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 112, cmd & prefix);
        // 卸载选择的档位
        rtu.PresetSingleRegister(addr, 212, (~cmd) & prefix);
    }

    // B相阻性功率设置
    if (MemDb::GetRealData(realData, regStart + 74, true)) {
        data = realData.pv.data.iValue;
        if (data > 7000) {
            data = 7000;
        }
        cmd = this->GetActivePowerCmd(data);

        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 113, cmd & res_prefix);
        this->Wait();
        // 卸载选择的档位
        rtu.PresetSingleRegister(addr, 213, (~cmd) & res_prefix);
    }

    // B相感性功率设置
    if (MemDb::GetRealData(realData, regStart + 75, true)) {
        data = realData.pv.data.iValue;
        if (data > 2000) {
            data = 2000;
        }
        cmd = this->GetReactivePowerCmd(data);

        zlog_warn(Util::m_zlog, "<%s> 下发感性功率 %d, %d", dev->name.c_str(),
                  realData.pv.data.iValue, cmd);

        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 114, cmd & prefix);
        // 卸载选择的档位
        rtu.PresetSingleRegister(addr, 214, (~cmd) & prefix);
    }

    // B相容性功率设置
    if (MemDb::GetRealData(realData, regStart + 76, true)) {
        data = realData.pv.data.iValue;
        if (data > 2000) {
            data = 2000;
        }
        cmd = this->GetReactivePowerCmd(data);

        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 115, cmd & prefix);
        // 卸载选择的档位
        rtu.PresetSingleRegister(addr, 215, (~cmd) & prefix);
    }

    // C相阻性功率设置
    if (MemDb::GetRealData(realData, regStart + 77, true)) {
        data = realData.pv.data.iValue;
        if (data > 7000) {
            data = 7000;
        }
        cmd = this->GetActivePowerCmd(data);

        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 116, cmd & res_prefix);
        this->Wait();
        // 卸载选择的档位
        rtu.PresetSingleRegister(addr, 216, (~cmd) & res_prefix);
    }

    // C相感性功率设置
    if (MemDb::GetRealData(realData, regStart + 78, true)) {
        data = realData.pv.data.iValue;
        if (data > 2000) {
            data = 2000;
        }
        cmd = this->GetReactivePowerCmd(data);
        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 117, cmd & prefix);
        this->Wait();
        // 卸载选择的档位z
        rtu.PresetSingleRegister(addr, 217, (~cmd) & prefix);
    }

    // C相容性功率设置
    if (MemDb::GetRealData(realData, regStart + 79, true)) {
        data = realData.pv.data.iValue;
        if (data > 2000) {
            data = 2000;
        }
        cmd = this->GetReactivePowerCmd(data);

        // 加载选择的档位
        rtu.PresetSingleRegister(addr, 118, cmd & prefix);
        // 卸载选择的档位
        rtu.PresetSingleRegister(addr, 218, (~cmd) & prefix);
    }

    // 加载卸载
    if (MemDb::GetRealData(realData, regStart + 80, true)) {
        data = realData.pv.data.iValue;
        if (data == 1) {
            // 加载
            rtu.PresetSingleRegister(addr, 220, 0);
            rtu.PresetSingleRegister(addr, 120, 1);
            zlog_warn(Util::m_zlog, "< %s > 加载", dev->name.c_str());
        } else {
            // 卸载

            MemDb::SetRealData(regStart + 82, 0);
            MemDb::SetRealData(regStart + 74, 0);
            MemDb::SetRealData(regStart + 77, 0);
            MemDb::SetRealData(regStart + 83, 0);
            MemDb::SetRealData(regStart + 75, 0);
            MemDb::SetRealData(regStart + 78, 0);
            MemDb::SetRealData(regStart + 73, 0);
            MemDb::SetRealData(regStart + 76, 0);
            MemDb::SetRealData(regStart + 79, 0);

            rtu.PresetSingleRegister(addr, 120, 0);
            rtu.PresetSingleRegister(addr, 220, 1);
            zlog_warn(Util::m_zlog, "< %s > 卸载", dev->name.c_str());
        }
    }

    return ret;
}

int RLCLoad::RLC_b_Preset(Device::SlaveDev *dev) {
    int ret              = ErrorInfo::ERR_OK;
    int regStart         = dev->regStart;
    int addr             = atoi(dev->slaveAddr.c_str());
    int data             = 0;
    // unsigned short cmd   = 0;
    // const int prefix     = 0b11111;
    // const int res_prefix = 0b1111111;
    RealData realData;
    FloatArray fvalues;


    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "设备空");
        return ErrorInfo::ERR_NULL;
    }

    if (!this->m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "网口未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    // 下发写状态命令格式1表格说明
    // 功能及寄存器地址	地址（1字节）本机默认1	功能码（1字节）	起始寄存器（2字节）	寄存器数据（2字节）	校验码（2字节）
    // 50.0	备用	05	06	00	32	00	01	E8	41
    // 50.1	备用	05	06	00	32	00	02	A8	40
    // 50.2	备用	05	06	00	32	00	04	28	42
    // 50.3	备用	05	06	00	32	00	08	28	47
    // 50.4	备用	05	06	00	32	00	10	28	4D
    // 50.5	备用	05	06	00	32	00	20	28	59
    // 50.6	加载	05	06	00	32	00	40	28	71
    // 50.7	卸载	05	06	00	32	00	80	28	21
    // 以上下发写状态命令，程序带有互锁，不能同时下发（只能单独），否则为非法
    if (MemDb::GetRealData(realData, regStart + 50, true)) {
        data = realData.pv.data.iValue;
        if (data == 1) {
            // 加载
            rtu.PresetSingleRegister(addr, 0x32, 0x40);
            zlog_warn(Util::m_zlog, "< %s > 加载", dev->name.c_str());
        } else {
            // 卸载
            rtu.PresetSingleRegister(addr, 0x32, 0x80);
            zlog_warn(Util::m_zlog, "< %s > 卸载", dev->name.c_str());
        }
    }

    // 下发写状态命令格式2表格说明
    // 功能及寄存器地址	地址（1字节）本机默认1	功能码（1字节）	起始寄存器（2字节）	寄存器数量（2字节）	字节数	寄存器数据（4字节）
    // 高字节低字节	校验码（2字节）
    // 51 52双字	设置AR功率值（双精度整数）	05	10	00	33	00	02	04	XX	XX	XX	XX	XX	XX
    // 53 54双字	设置AL功率值（双精度整数）	05	10	00	35	00	02	04	XX	XX	XX	XX	XX	XX
    // 55 56双字	设置AC功率值（双精度整数）	05	10	00	37	00	02	04	XX	XX	XX	XX	XX	XX
    // 57 58双字	设置BR功率值（双精度整数）	05	10	00	39	00	02	04	XX	XX	XX	XX	XX	XX
    // 59 60双字	设置BL功率值（双精度整数）	05	10	00	3B	00	02	04	XX	XX	XX	XX	XX	XX
    // 61 62双字	设置BC功率值（双精度整数）	05	10	00	3D	00	02	04	XX	XX	XX	XX	XX	XX
    // 63 64双字	设置CR功率值（双精度整数）	05	10	00	3F	00	02	04	XX	XX	XX	XX	XX	XX
    // 65 66双字	设置CL功率值（双精度整数）	05	10	00	41	00	02	04	XX	XX	XX	XX	XX	XX
    // 67 68双字	设置CC功率值（双精度整数）	05	10	00	43	00	02	04	XX	XX	XX	XX	XX	XX
    // 设置功率值时，按实际设定的“实际数”然后取整得功率值（100W-1700W,大于1700W按1700W算）转换16进制

    if (MemDb::GetRealData(realData, regStart + 51, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x33, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置AR功率值 %d", dev->name.c_str(), data);
    }

    if (MemDb::GetRealData(realData, regStart + 52, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x35, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置AL功率值 %d", dev->name.c_str(), data);
    }

    if (MemDb::GetRealData(realData, regStart + 53, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x37, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置AC功率值 %d", dev->name.c_str(), data);
    }

    if (MemDb::GetRealData(realData, regStart + 54, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x39, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置BR功率值 %d", dev->name.c_str(), data);
    }

    if (MemDb::GetRealData(realData, regStart + 55, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x3B, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置BL功率值 %d", dev->name.c_str(), data);
    }

    if (MemDb::GetRealData(realData, regStart + 56, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x3D, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置BC功率值 %d", dev->name.c_str(), data);
    }

    if (MemDb::GetRealData(realData, regStart + 57, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x3F, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置CR功率值 %d", dev->name.c_str(), data);
    }

    if (MemDb::GetRealData(realData, regStart + 58, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x41, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置CL功率值 %d", dev->name.c_str(), data);
    }

    if (MemDb::GetRealData(realData, regStart + 59, true)) {
        data = realData.pv.data.iValue;
        fvalues.clear();
        fvalues.push_back(data);
        fvalues.push_back(0);
        rtu.PresetMultipleFloatRegisters(addr, 0x43, 2, fvalues);
        zlog_warn(Util::m_zlog, "< %s > 设置CC功率值 %d", dev->name.c_str(), data);
    }

    return ret;
}

int RLCLoad::RLC_c_Preset(Device::SlaveDev *dev) {
    int ret              = ErrorInfo::ERR_OK;
    int regStart         = dev->regStart;
    int addr             = atoi(dev->slaveAddr.c_str());
    int data             = 0;
    // unsigned short cmd   = 0;
    // const int prefix     = 0b11111;
    // const int res_prefix = 0b1111111;
    RealData realData;
    FloatArray fvalues;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "设备空");
        return ErrorInfo::ERR_NULL;
    }

    if (!this->m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "网口未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    // 下发写状态命令格式1表格说明
    // 功能及寄存器地址	地址（1字节）本机默认1	功能码（1字节）
    // 起始寄存器（2字节）	寄存器数据（2字节）	校验码（2字节） 50.0
    // 备用	05	06	00	32	00	01	E8	41 50.1
    // 备用	05	06	00	32	00	02	A8	40 50.2
    // 备用	05	06	00	32	00	04	28	42 50.3
    // 备用	05	06	00	32	00	08	28	47 50.4
    // 备用	05	06	00	32	00	10	28	4D 50.5
    // 备用	05	06	00	32	00	20	28	59 50.6
    // 加载	05	06	00	32	00	40	28	71 50.7
    // 卸载	05	06	00	32	00	80	28	21
    // 以上下发写状态命令，程序带有互锁，不能同时下发（只能单独），否则为非法

    if (MemDb::GetRealData(realData, regStart + 30, true)) {
        data = realData.pv.data.iValue;
        if (data == 1) {
            // 加载
            rtu.PresetSingleRegister(addr, 0x32, 0x40);
            zlog_warn(Util::m_zlog, "< %s > 加载", dev->name.c_str());
        } else {
            // 卸载
            rtu.PresetSingleRegister(addr, 0x32, 0x80);
            zlog_warn(Util::m_zlog, "< %s > 卸载", dev->name.c_str());
        }
    }

    // 下发写状态命令格式2表格说明
    // 功能及寄存器地址	地址（1字节）本机默认1	功能码（1字节）
    // 起始寄存器（2字节）	寄存器数量（2字节）	字节数
    // 寄存器数据（4字节） 高字节>低字节	校验码（2字节） 51 52双字
    // 设置RA功率值（双精度整数）	05	10	00	33	00
    // 02	04	XX	XX	XX	XX	XX	XX

    if (MemDb::GetRealData(realData, regStart + 31, true)) {
        data = realData.pv.data.iValue;
        // fvalues.clear();
        // fvalues.push_back(data>>16);
        // fvalues.push_back(0);
        // rtu.PresetMultipleFloatRegisters(addr, 0x33, 2, fvalues);
        rtu.PresetSingleRegister(addr, 0x33, (data >> 16) & 0xFFFF);
		rtu.PresetSingleRegister(addr, 0x34, data  & 0xFFFF);
        zlog_warn(Util::m_zlog, "< %s > 设置RA功率值 %d", dev->name.c_str(),data);
    }
}

int RLCLoad::GetActivePowerCmd(int set_power) {

    int cmd   = 0;
    int power = set_power;

    // 对每个功率阶段进行检查并设置相应的位
    if (power >= 4) {
        power -= 4;
        cmd |= 1 << 3; // 代表400W及以上，对应A相-L-0.2KW
    }

    if (power >= 2) {
        power -= 2;
        cmd |= 1 << 1; // 代表300W及以上，对应A相-L-0.4KW
    }

    if (power >= 1) {
        power -= 1;
        cmd |= 1 << 0; // 代表200W及以上，对应A相-L-0.3KW
    }

    // if (power >= 200) {
    //     power -= 200;
    //     cmd |= 1 << 1; // 代表100W及以上，对应A相-L-0.2KW
    // }

    // if (power >= 0) {
    //     // power -= 0;
    //     cmd |= 1 << 0; // 代表100W及以上，对应A相-L-0.1KW
    // }

    zlog_warn(Util::m_zlog, "power: %d, cmd:%d", set_power, cmd);
    return cmd;
}

void RLCLoad::Wait() {

    // msleep(100);
}

int RLCLoad::GetReactivePowerCmd(int set_power) {
    int cmd   = 0;
    int power = set_power;

    // 对每个功率阶段进行检查并设置相应的位
    if (power >= 4) {
        power -= 4;
        cmd |= 1 << 3; // 代表400W及以上，对应A相-L-0.2KW
    }

    if (power >= 2) {
        power -= 2;
        cmd |= 1 << 1; // 代表300W及以上，对应A相-L-0.4KW
    }

    if (power >= 1) {
        power -= 1;
        cmd |= 1 << 0; // 代表200W及以上，对应A相-L-0.3KW
    }

    // if (power >= 200) {
    //     power -= 200;
    //     cmd |= 1 << 1; // 代表100W及以上，对应A相-L-0.2KW
    // }

    // if (power >= 0) {
    //     // power -= 0;
    //     cmd |= 1 << 0; // 代表100W及以上，对应A相-L-0.1KW
    // }

    zlog_warn(Util::m_zlog, "power: %d, cmd:%d", set_power, cmd);
    return cmd;
}
