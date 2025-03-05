#include "./ECMDCDC.h"

EmcDCDC::EmcDCDC() { m_tcpClient = NULL; }

EmcDCDC::~EmcDCDC() {}

void EmcDCDC::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int EmcDCDC::Read(Device::SlaveDev *dev) {
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

    // zlog_warn(Util::m_zlog, "飞轮bms读线程");

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

    start  = 4096;
    length = 1;

    values.clear();
    ret = client.ReadOutputRegisters(values, addr, start, length);

    // 开机 停机 故障复位
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        i_data = values[0];
        switch (i_data) {
        // 开机
        case 65279:
            MemDb::SetRealData(regStart + 1, 1, false);
            break;
        case 65535:
            MemDb::SetRealData(regStart + 1, 0, false);
            break;
        case 64511:
            MemDb::SetRealData(regStart + 1, 2, false);
            break;
        default:
            break;
        }

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    start  = 400;
    length = 11;

    values.clear();
    ret = client.ReadInputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 读给定电压
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 2, f_data, false);
        // 低压侧电压
        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 3, f_data, false);
        // 低压侧电流
        f_data = (short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 4, f_data, false);
        // 高压侧电压
        f_data = values[3] * 0.1;
        MemDb::SetRealData(regStart + 5, f_data, false);
        // 高压侧电流
        f_data = (short)values[4] * 0.1;
        MemDb::SetRealData(regStart + 6, f_data, false);
        // 输出功率
        f_data = (short)values[9] *10;
        MemDb::SetRealData(regStart + 7, f_data, false);
        // IGBT温度
        f_data = values[10] * 0.1;
        MemDb::SetRealData(regStart + 8, f_data, false);

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    start  = 100;
    length = 1;

    values.clear();
    ret = client.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 工作模式
        i_data = values[0];
        MemDb::SetRealData(regStart + 9, i_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    start  = 711;
    length = 1;

    values.clear();
    ret = client.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 斩波器工作
        i_data = values[0];
        MemDb::SetRealData(regStart + 10, i_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    start  = 101;
    length = 2;

    values.clear();
    ret = client.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 降压方向最大电流
        i_data = values[0]*0.1;
        MemDb::SetRealData(regStart + 11, i_data, false);

        // 升压方向最大电流
        i_data = values[1]*0.1;
        MemDb::SetRealData(regStart + 12, i_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    start  = 714;
    length = 1;

    values.clear();
    ret = client.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 本地运行最大功率
        f_data = values[0] *100;
        MemDb::SetRealData(regStart + 13, f_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    start  = 4098;
    length = 1;

    values.clear();
    ret = client.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 远程运行最大功率
        f_data = values[0] ;
        MemDb::SetRealData(regStart + 14, f_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    start  = 412;
    length = 1;

    values.clear();
    ret = client.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 故障位
        i_data = (values[0] >> 8) & 0xFF;
        MemDb::SetRealData(regStart + 15, i_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);

    return ret;
}

int EmcDCDC::Preset(Device::SlaveDev *dev) {

    int ret      = ErrorInfo::ERR_OK;
    int regStart = dev->regStart;

    RealData realData;

    int addr = atoi(dev->slaveAddr.c_str());

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusTcpClient client;
    client.SetTcpClient(this->m_tcpClient);

    // 写给定电压
    if (MemDb::GetRealData(realData, regStart + 21, true)) {

        zlog_warn(Util::m_zlog, " 下发给定电压%d", realData.pv.data.iValue);

        ret = client.PresetSingleRegister(addr, 4097,
                                          realData.pv.data.iValue * 10);
    }

    // 运行状态
    if (MemDb::GetRealData(realData, regStart + 22, true)) {
        int runStatus = realData.pv.data.iValue;
        zlog_warn(Util::m_zlog, "下发运行状态指令 %d", realData.pv.data.iValue);

        switch (runStatus) {

        // 停机
        case 0:
            ret = client.PresetSingleRegister(addr, 4096, 65535);
            zlog_warn(Util::m_zlog, "%s 下发停机命令", dev->name.c_str());
            break;

        // 启动
        case 1:
            ret = client.PresetSingleRegister(addr, 4096, 65279);
            zlog_warn(Util::m_zlog, "%s 下发启动命令", dev->name.c_str());
            break;

        // 故障复位
        case 2:
            ret = client.PresetSingleRegister(addr, 4096, 64511);
            zlog_warn(Util::m_zlog, "%s 下发故障复位命令", dev->name.c_str());
            break;

        default:
            zlog_warn(Util::m_zlog, "运行状态未知命令");
            break;
        }
    }

    //  斩波器工作模式
    if (MemDb::GetRealData(realData, regStart + 23, true)) {
        ret = client.PresetSingleRegister(addr, 711, realData.pv.data.iValue);
    }

    // 降压方向最大电流
    if (MemDb::GetRealData(realData, regStart + 24, true)) {
        ret = client.PresetSingleRegister(addr, 101, realData.pv.data.iValue*10);
    }

    // 升压方向最大电流
    if (MemDb::GetRealData(realData, regStart + 25, true)) {
        ret = client.PresetSingleRegister(addr, 102, realData.pv.data.iValue*10);
    }

    // 本地运行最大输出功率
    if (MemDb::GetRealData(realData, regStart + 26, true)) {
        ret = client.PresetSingleRegister(addr, 714, realData.pv.data.iValue );
    }

    // 网络运行最大输出功率
    if (MemDb::GetRealData(realData, regStart + 27, true)) {
        ret = client.PresetSingleRegister(addr, 4098, realData.pv.data.iValue );
    }

    if (ErrorInfo::ERR_OK != ret) {
        zlog_warn(Util::m_zlog, "%s 下发控制指令失败 code:%d", dev->name.c_str(), ret);
        return ErrorInfo::ERR_FAILED;
    }

    return ret;
}
