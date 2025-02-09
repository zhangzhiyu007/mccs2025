/**
 *    文 件       :			insulationMonitor.cpp
 *    功 能       :			绝缘监测仪-非标准通讯协议
 *    作 者       :			Mingyu Wu
 *    version     :			1.0.0
 *    日 期       :			2021-04-06
 */

#include "insulationMonitor.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util//Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include <cstdlib>

InsulationMonitor::InsulationMonitor() {
    m_isTcp     = false;
    m_tcpClient = NULL;
}

InsulationMonitor::~InsulationMonitor() {}

void InsulationMonitor::SetTcp(TcpClient *tcpClient) {
    m_tcpClient = tcpClient;
}

void InsulationMonitor::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

int InsulationMonitor::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    switch (dev->devModel) {
    case DevModel::Model_ACREL_AIM_T500:
        ret = this->AIM_T500Read(dev);
        break;
    case DevModel::Model_ACREL_AIM_T350:
        ret = this->AIM_T350Read(dev);
        break;
    default:
        zlog_warn(Util::m_zlog, "%s wrong model", dev->name.c_str());
        ret = ErrorInfo::ERR_READ;
        break;
    }

    return ret;
}

int InsulationMonitor::AIM_T500Read(Device::SlaveDev *dev) {
    bool comm    = false;
    int ret      = ErrorInfo::ERR_OK;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0;
    int length   = 0;
    int i_data   = 0;

    UshortArray values;
    values.clear();

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(m_tcpClient);

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    values.clear();

    start  = 0;
    length = 17;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        i_data = values[9];
        MemDb::SetRealData(regStart + 1, i_data, false);
        i_data = values[13];
        MemDb::SetRealData(regStart + 2, i_data, false);
        i_data = values[16];
        MemDb::SetRealData(regStart + 3, i_data, false);

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取绝缘监测仪失败%s", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false); // 通讯状态

    return ret;
}

int InsulationMonitor::AIM_T350Read(Device::SlaveDev *dev) {

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

    start  = 0x00;
    length = 20;

    values.clear();
    // 读取模块信息
    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        i_data = values[0x4] & 0xFF;
        MemDb::SetRealData(regStart + 1, i_data); // 报警标志位
        i_data = values[0x9];
        MemDb::SetRealData(regStart + 2, i_data); // 绝缘电阻值
        i_data = values[0xD];
        MemDb::SetRealData(regStart + 3, i_data); // 泄漏电容值
        i_data = values[0xD] >> 8;
        MemDb::SetRealData(regStart + 4, i_data); // 断线标志

        i_data = values[0x10] & 0xFF;
        MemDb::SetRealData(regStart + 5, i_data); // 事件1报警
        i_data = values[0x11];
        MemDb::SetRealData(regStart + 6, i_data); // 绝缘电阻值

        i_data = values[0x12] >> 8;
        MemDb::SetRealData(regStart + 7, i_data); //  年
        i_data = values[0x12] & 0xFF;
        MemDb::SetRealData(regStart + 8, i_data); // 月

        i_data = values[0x13] >> 8;
        MemDb::SetRealData(regStart + 9, i_data); // 日
        i_data = values[0x13] & 0xFF;
        MemDb::SetRealData(regStart + 10, i_data); // 时

        i_data = values[0x14] >> 8;
        MemDb::SetRealData(regStart + 11, i_data); // 分
        i_data = values[0x14] & 0xFF;
        MemDb::SetRealData(regStart + 12, i_data); // 秒

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}
