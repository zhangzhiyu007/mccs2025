/*
 * QingbiaoWatt.cpp
 *
 *  创建日期: 2019-7-31
 *  作          者: 刘海涛
 */

#include "TaikaiWatt.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

TaikaiWatt::TaikaiWatt() {
    // TODO Auto-generated constructor stub
    m_isTcp     = false;
    m_tcpClient = NULL;
}

TaikaiWatt::~TaikaiWatt() {
    // TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void TaikaiWatt::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

//设置TcpClient指针
void TaikaiWatt::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int TaikaiWatt::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ErrorInfo::ERR_NULL;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::Model_TK900M: //  泰开交流低
        ret = this->TK900M_Read(dev);
        break;
    default:
        break;
    };

    return ret;
}

int TaikaiWatt::TK900M_Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    int start    = 0;
    int regStart = dev->regStart;
    int addr     = atoi(dev->slaveAddr.c_str());

    UshortArray values;
    values.clear();

    int i_data   = 0;
    float f_data = 0.0f;
    bool status  = 0;
    bool comm    = false;

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

    start  = 0x0;
    length = 0x20;

    values.clear();
    ret = rtu.ReadOutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;

        f_data = values[0x0] * 0.001;
        MemDb::SetRealData(regStart + 1, f_data); // 相电压Ua

        f_data = values[0x2] * 0.001;
        MemDb::SetRealData(regStart + 2, f_data); // 相电压Ub

        f_data = values[0x4] * 0.001;
        MemDb::SetRealData(regStart + 3, f_data); // 相电压Uc

        f_data = values[0x6] * 0.001;
        MemDb::SetRealData(regStart + 4, f_data); // 线电压Uab

        f_data = values[0x8] * 0.001;
        MemDb::SetRealData(regStart + 5, f_data); // 线电压Ubc

        f_data = values[0xa] * 0.001;
        MemDb::SetRealData(regStart + 6, f_data); // 线电压Uca

        f_data = values[0xc] * 0.001;
        MemDb::SetRealData(regStart + 7, f_data); // 相电流Ia

        f_data = values[0xe] * 0.001;
        MemDb::SetRealData(regStart + 8, f_data); // 相电流Ib

        f_data = values[0x10] * 0.001;
        MemDb::SetRealData(regStart + 9, f_data); //  相电流Ic

        f_data = values[0x12];
        MemDb::SetRealData(regStart + 10, f_data); // A相有功功率Pa

        f_data = values[0x14];
        MemDb::SetRealData(regStart + 11, f_data); // B相有功功率Pb

        f_data = values[0x16];
        MemDb::SetRealData(regStart + 12, f_data); // C相有功功率Pc

        f_data = values[0x18];
        MemDb::SetRealData(regStart + 13, f_data); // 总有功功率Ps

        f_data = values[0x1a];
        MemDb::SetRealData(regStart + 14, f_data); // A相无功功率Qa

        f_data = values[0x1c];
        MemDb::SetRealData(regStart + 15, f_data); // B相无功功率Qb

        f_data = values[0x1e];
        MemDb::SetRealData(regStart + 16, f_data); // C相无功功率Qc
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    start  = 0x20;
    length = 0x20;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        f_data = values[0x0] * 0.1;
        MemDb::SetRealData(regStart + 17, f_data); // 总无功功率Qs

        f_data = values[0x2] * 0.1;
        MemDb::SetRealData(regStart + 18, f_data); // PFa

        f_data = values[0x4] * 0.1;
        MemDb::SetRealData(regStart + 19, f_data); // PFb

        f_data = values[0x6] * 0.1;
        MemDb::SetRealData(regStart + 20, f_data); // PFc

        f_data = values[0x8] * 0.1;
        MemDb::SetRealData(regStart + 21, f_data); // PFs

        f_data = values[0xa] * 0.001;
        MemDb::SetRealData(regStart + 22, f_data); // VAa

        f_data = values[0xc] * 0.001;
        MemDb::SetRealData(regStart + 23, f_data); // VAb

        f_data = values[0xe] * 0.001;
        MemDb::SetRealData(regStart + 24, f_data); // VAc

        f_data = values[0x10] * 0.001;
        MemDb::SetRealData(regStart + 25, f_data); // VAs

        f_data = values[0x12] * 0.001;
        MemDb::SetRealData(regStart + 26, f_data); // 频率

        f_data = values[0x14] * 0.001;
        MemDb::SetRealData(regStart + 27, f_data); // 电压不平衡度

        f_data = values[0x16] * 0.001;
        MemDb::SetRealData(regStart + 28, f_data); // 电流不平衡度

        f_data = values[0x18] * 0.001;
        MemDb::SetRealData(regStart + 29, f_data); // 总有功电度

        f_data = values[0x1a] * 0.001;
        MemDb::SetRealData(regStart + 30, f_data); // 正向有功电度

        f_data = values[0x1c] * 0.001;
        MemDb::SetRealData(regStart + 31, f_data); // 反向有功电度

        f_data = values[0x1e] * 0.001;
        MemDb::SetRealData(regStart + 32, f_data); // 总无功电镀

        f_data = values[0x20] * 0.001;
        MemDb::SetRealData(regStart + 33, f_data); // 正向无功电度

        f_data = values[0x22] * 0.001;
        MemDb::SetRealData(regStart + 34, f_data); // 反向无功电度

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}
