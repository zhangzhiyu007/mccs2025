#include "Acrel.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include <math.h>

Acrel::Acrel() { m_tcpClient = NULL; }

Acrel::~Acrel() {}

void Acrel::SetTcp(TcpClient *tcpClient) { this->m_tcpClient = tcpClient; }

int Acrel::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ErrorInfo::ERR_NULL;
        zlog_warn(Util::m_zlog, "从设备为NULL");
    }

    switch (dev->devModel) {
    case DevModel::Model_ACREL_AEM96:
        ret = this->AEM96Read(dev);
        break;
    case DevModel::Model_ACREL_APM:
        ret = this->APMRead(dev);
        break;
    case DevModel::Model_ACREL_DJSF1352:
        ret = this->DJSF1352Read(dev);
        break;
    default:
        break;
    }

    return ret;
}

int Acrel::AEM96Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    bool comm = false;
    // zlog_warn(Util::m_zlog, "AEM96 %d", dev->regStart);

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    //判断串口是否打开
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(m_tcpClient);

    UshortArray values;
    values.clear();
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 0;
    int i_data   = 0;
    float f_data = 0.0f;

    start  = 5;
    length = 1;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    // 获取CT变比
    if (ret == ErrorInfo::ERR_OK) {
        comm    = true;
        CTRatio = values[0];
    }

    start  = 0x50;
    length = 41;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);

    if (ret == ErrorInfo::ERR_OK) {
        comm = true;

        // UA 电压
        f_data = (float)(values[0] * 0.1);
        MemDb::SetRealData(regStart + 1, f_data, false);
        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 2, f_data, false);
        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 3, f_data, false);
        f_data = values[3] * 0.1;
        MemDb::SetRealData(regStart + 4, f_data, false);
        f_data = values[4] * 0.1;
        MemDb::SetRealData(regStart + 5, f_data, false);
        f_data = values[5] * 0.1;
        MemDb::SetRealData(regStart + 6, f_data, false);

        // 电流
        f_data = values[6] * 0.001 * CTRatio;
        MemDb::SetRealData(regStart + 7, f_data, false);
        f_data = values[7] * 0.001 * CTRatio;
        MemDb::SetRealData(regStart + 8, f_data, false);
        f_data = values[8] * 0.001 * CTRatio;
        MemDb::SetRealData(regStart + 9, f_data, false);
        f_data = values[9] * 0.001 * CTRatio;
        MemDb::SetRealData(regStart + 10, f_data, false);

        // 功率
        f_data = values[10] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 11, f_data, false);
        f_data = values[11] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 12, f_data, false);
        f_data = values[12] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 13, f_data, false);
        f_data = values[13] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 14, f_data, false);
        f_data = values[14] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 15, f_data, false);
        f_data = values[15] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 16, f_data, false);
        f_data = values[16] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 17, f_data, false);
        f_data = values[17] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 18, f_data, false);
        f_data = values[18] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 19, f_data, false);
        f_data = values[19] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 20, f_data, false);
        f_data = values[20] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 21, f_data, false);
        f_data = values[21] * 0.0001 * CTRatio;
        MemDb::SetRealData(regStart + 22, f_data, false);

        // PFA PFB  PFC PF
        f_data = values[22];
        MemDb::SetRealData(regStart + 23, f_data, false);
        f_data = values[23];
        MemDb::SetRealData(regStart + 24, f_data, false);
        f_data = values[24];
        MemDb::SetRealData(regStart + 25, f_data, false);
        f_data = values[25];
        MemDb::SetRealData(regStart + 26, f_data, false);

        // 功率方向
        i_data = values[26];
        MemDb::SetRealData(regStart + 27, (i_data >> 0) & 0b1, false);
        MemDb::SetRealData(regStart + 28, (i_data >> 1) & 0b1, false);
        MemDb::SetRealData(regStart + 29, (i_data >> 2) & 0b1, false);
        MemDb::SetRealData(regStart + 30, (i_data >> 3) & 0b1, false);
        MemDb::SetRealData(regStart + 31, (i_data >> 4) & 0b1, false);
        MemDb::SetRealData(regStart + 32, (i_data >> 5) & 0b1, false);
        MemDb::SetRealData(regStart + 33, (i_data >> 6) & 0b1, false);
        MemDb::SetRealData(regStart + 34, (i_data >> 7) & 0b1, false);

        // 频率
        f_data = values[27] * 0.01;
        MemDb::SetRealData(regStart + 35, f_data, false);

        // 006BH
        f_data = values[28] * 0.0001;
        MemDb::SetRealData(regStart + 36, f_data, false);
        f_data = values[29] * 0.0001;
        MemDb::SetRealData(regStart + 37, f_data, false);
        f_data = values[30] * 0.0001;
        MemDb::SetRealData(regStart + 38, f_data, false);
        f_data = values[31] * 0.0001;
        MemDb::SetRealData(regStart + 39, f_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    start  = 0x7c;
    length = 10;
    values.clear();

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ret == ErrorInfo::ERR_OK) {
        comm = true;

        //   当前组合有功总电能
        i_data = ((values[0] << 16) + values[1]) * CTRatio * 0.01;
        MemDb::SetRealData(regStart + 40, i_data, false);

        i_data = ((values[2] << 16) + values[3]) * CTRatio * 0.01;
        MemDb::SetRealData(regStart + 41, i_data, false);

        i_data = ((values[4] << 16) + values[5]) * CTRatio * 0.01;
        MemDb::SetRealData(regStart + 42, i_data, false);

        i_data = ((values[6] << 16) + values[7]) * CTRatio * 0.01;
        MemDb::SetRealData(regStart + 43, i_data, false);

        i_data = ((values[8] << 16) + values[9]) * CTRatio * 0.01;
        MemDb::SetRealData(regStart + 44, i_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    start  = 0x1A2;
    length = 5;

    values.clear();
    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ret == ErrorInfo::ERR_OK) {
        comm = true;
        // 电压不平衡度
        f_data = values[0] * 0.01;
        MemDb::SetRealData(regStart + 45, f_data, false);
        // 电流不平衡度
        f_data = values[1] * 0.01;
        MemDb::SetRealData(regStart + 46, f_data, false);

        // ABC 相位角
        f_data = (signed short)values[2] * 0.01;
        MemDb::SetRealData(regStart + 47, f_data, false);
        f_data = (signed short)values[3] * 0.01;
        MemDb::SetRealData(regStart + 48, f_data, false);
        f_data = (signed short)values[4] * 0.01;
        MemDb::SetRealData(regStart + 49, f_data, false);

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}

int Acrel::APMRead(Device::SlaveDev *dev) {

    int ret = ErrorInfo::ERR_OK;

    bool comm = false;
    // zlog_warn(Util::m_zlog, "AEM96 %d", dev->regStart);

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    //判断串口是否打开
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(m_tcpClient);

    UshortArray values;
    values.clear();
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 0;
    int i_data   = 0;
    float f_data = 0.0f;

    start  = 243;
    length = 39;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // ABC  AB BC  CA 电压
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 1, f_data, false);
        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 2, f_data, false);
        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 3, f_data, false);
        f_data = values[3] * 0.1;
        MemDb::SetRealData(regStart + 4, f_data, false);
        f_data = values[4] * 0.1;
        MemDb::SetRealData(regStart + 5, f_data, false);
        f_data = values[5] * 0.1;
        MemDb::SetRealData(regStart + 6, f_data, false);

        // ABC 相电流
        f_data = values[6] * 0.001;
        MemDb::SetRealData(regStart + 7, f_data, false);
        f_data = values[7] * 0.001;
        MemDb::SetRealData(regStart + 8, f_data, false);
        f_data = values[8] * 0.001;
        MemDb::SetRealData(regStart + 9, f_data, false);

        // 频率
        f_data = values[9] * 0.01;
        MemDb::SetRealData(regStart + 10, f_data, false);

        // A相有功功率
        f_data = ((values[10] << 16) + values[11]) * 0.01;
        MemDb::SetRealData(regStart + 11, f_data, false);
        f_data = ((values[12] << 16) + values[13]) * 0.01;
        MemDb::SetRealData(regStart + 12, f_data, false);
        f_data = ((values[14] << 16) + values[15]) * 0.01;
        MemDb::SetRealData(regStart + 13, f_data, false);

        // 总有功功率
        f_data = ((values[16] << 16) + values[17]) * 0.01;
        MemDb::SetRealData(regStart + 14, f_data, false);

        // A相无功功率
        f_data = (values[18] << 16) + values[19];
        MemDb::SetRealData(regStart + 15, f_data, false);
        f_data = (values[20] << 16) + values[21];
        MemDb::SetRealData(regStart + 16, f_data, false);
        f_data = (values[22] << 16) + values[23];
        MemDb::SetRealData(regStart + 17, f_data, false);

        // 总无功功率
        f_data = (values[24] << 16) + values[25];
        MemDb::SetRealData(regStart + 18, f_data, false);

        // ABC  视在功率
        f_data = (values[27] << 16) + values[26];
        MemDb::SetRealData(regStart + 19, f_data, false);
        f_data = (values[28] << 16) + values[29];
        MemDb::SetRealData(regStart + 20, f_data, false);
        f_data = (values[30] << 16) + values[31];
        MemDb::SetRealData(regStart + 21, f_data, false);

        // 总视在功率
        f_data = (values[32] << 16) + values[33];
        MemDb::SetRealData(regStart + 22, f_data, false);

        // A相功率因数
        i_data = values[34];
        MemDb::SetRealData(regStart + 23, i_data, false);
        i_data = values[35];
        MemDb::SetRealData(regStart + 24, i_data, false);
        i_data = values[36];
        MemDb::SetRealData(regStart + 25, i_data, false);
        i_data = values[37];
        MemDb::SetRealData(regStart + 26, i_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    start  = 300;
    length = 8;

    values.clear();
    ret = rtu.ReadOutputRegisters(values, addr, start, length);

    if (ret == ErrorInfo::ERR_OK) {
        // 正向有功电能EPI
        f_data = (values[1] + (values[0] << 16)) * 0.001;
        MemDb::SetRealData(regStart + 27, i_data, false);
        // EPE
        f_data = (values[3] + (values[2] << 16)) * 0.001;
        MemDb::SetRealData(regStart + 28, i_data, false);
        // EQL
        f_data = (values[5] + (values[4] << 16)) * 0.001;
        MemDb::SetRealData(regStart + 29, i_data, false);
        // EQC
        f_data = (values[7] + (values[6] << 16)) * 0.001;
        MemDb::SetRealData(regStart + 29, i_data, false);
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    start  = 1144;
    length = 3;

    values.clear();
    ret = rtu.ReadOutputRegisters(values, addr, start, length);

    if (ret == ErrorInfo::ERR_OK) {
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 31, i_data, false);
        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 32, i_data, false);
        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 33, i_data, false);

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    start  = 1174;
    length = 3;

    values.clear();
    ret = rtu.ReadOutputRegisters(values, addr, start, length);

    if (ret == ErrorInfo::ERR_OK) {
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 31, i_data, false);
        f_data = values[1] * 0.1;
        MemDb::SetRealData(regStart + 32, i_data, false);
        f_data = values[2] * 0.1;
        MemDb::SetRealData(regStart + 33, i_data, false);

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}

int Acrel::DJSF1352Read(Device::SlaveDev *dev) {

    int ret = ErrorInfo::ERR_OK;

    int start    = 0;
    int regStart = dev->regStart;
    int addr     = atoi(dev->slaveAddr.c_str());

    bool comm = false;

    UshortArray values;
    values.clear();

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

    start  = 0x0;
    length = 16;

    values.clear();
    // 读取模块信息

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;

        this->VoltValue    = (signed short)values[0];
        this->VoltDpt      = values[1];
        this->CurrentValue = (signed short)values[2];
        this->CurrentDpt   = values[3];
        this->PowerValue   = (signed short)values[8];
        this->PowerDpt     = values[9];

        f_data = this->VoltValue * pow(10, this->VoltDpt - 3);
        MemDb::SetRealData(regStart + 1, f_data, false); // 电压

        f_data = this->CurrentValue * pow(10, this->CurrentDpt - 3);
        MemDb::SetRealData(regStart + 2, f_data, false); // 电流

        f_data = this->PowerValue * pow(10, this->PowerDpt - 3) * 0.001;
        MemDb::SetRealData(regStart + 4, f_data, false); // 功率

        f_data = values[5] * 0.1;
        MemDb::SetRealData(regStart + 3, f_data, false); // 温度

        i_data = values[4];
        MemDb::SetRealData(regStart + 5, i_data, false); // 断线检测

        i_data = (values[12] << 16) + values[13];
        MemDb::SetRealData(regStart + 6, i_data, false); // 总正向有功电能

        i_data = (values[14] << 16) + values[15];
        MemDb::SetRealData(regStart + 7, i_data, false); // 总反向有功电能

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}
