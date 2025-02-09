#include "TaikaiBMS.h"

TaikaiBMS::TaikaiBMS() { this->m_tcpClient = NULL; }

TaikaiBMS::~TaikaiBMS() {}

void TaikaiBMS::SetTcp(TcpClient *tcp_Client) {
    this->m_tcpClient = tcp_Client;
}

int TaikaiBMS::Read(Device::SlaveDev *dev) {
    int ret   = ErrorInfo::ERR_OK;
    bool comm = false;

    int regStart = dev->regStart;
    int addr     = atoi(dev->slaveAddr.c_str());

    UshortArray values;
    values.clear();

    int i_data   = 0;
    float f_data = 0.0f;

    int start  = 0;
    int length = 0;

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

    start  = 0x100;
    length = 11;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;

        i_data = (values[0] >> 3) & 0b111;
        MemDb::SetRealData(regStart + 1, i_data, false); // BMS 状态字

        i_data = values[1];
        MemDb::SetRealData(regStart + 2, i_data, false); // 预留

        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 3, f_data, false); // 电压

        f_data = values[3] * 0.1 - 2000;
        MemDb::SetRealData(regStart + 4, f_data, false); // 电流

        i_data = values[4];
        MemDb::SetRealData(regStart + 5, i_data, false); // SOC

        i_data = values[5];
        MemDb::SetRealData(regStart + 6, i_data, false); // SOH

        f_data = values[6] * 0.1;
        MemDb::SetRealData(regStart + 7, f_data, false); // 充电限值电流

        f_data = values[7] * 0.1;
        MemDb::SetRealData(regStart + 8, f_data, false); // 放电限值电流

        f_data = values[8] * 0.1;
        MemDb::SetRealData(regStart + 9, f_data, false); // 充电限值电压

        f_data = values[9] * 0.1;
        MemDb::SetRealData(regStart + 10, f_data, false); // 放电限值电压

        f_data = values[10] * 0.1;
        MemDb::SetRealData(regStart + 11, f_data, false); // 充电可用电量

        f_data = values[11] * 0.1;
        MemDb::SetRealData(regStart + 12, f_data, false); // 放电可用电量

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);
}
