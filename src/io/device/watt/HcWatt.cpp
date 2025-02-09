/*
 * HcWatt.cpp
 *
 *  Created on: 2019-4-23
 *      Author: 刘海涛
 */

#include "HcWatt.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

HcWatt::HcWatt() {
    // TODO Auto-generated constructor stub
    m_isTcp     = false;
    m_com       = NULL;
    m_tcpClient = NULL;
    U0_33B      = 0;
    I0_33B      = 0;
    Ir_33B      = 0;
    Ur_33B      = 0;
}

HcWatt::~HcWatt() {
    // TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void HcWatt::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

void HcWatt::SetCom(Com *com) { m_com = com; }

//设置TcpClient指针
void HcWatt::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int HcWatt::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::Model_HC_63C: // hc-63c
        ret = this->Hc63CRead(dev);
        break;
    case DevModel::Model_HC_221: // hc-221
        ret = this->Hc221Read(dev);
        break;
    case DevModel::Model_HC_33BD: //惠测电子HC-33BD
        ret = this->HC33BDRead(dev);
        break;
    case DevModel::Model_HC_201:
        ret = this->HC201Read(dev);
        break;
    case DevModel::Model_HC_202:
        ret = this->HC202Read(dev);
        break;
    case DevModel::Model_HC_209:
        ret = this->HC209Read(dev);
        break;
    case DevModel::Model_HC_33B:
        ret = this->HC33BRead(dev);
        break;
    default:
        break;
    };

    return ret;
}

int HcWatt::Hc63CRead(Device::SlaveDev *dev) {
    int ret   = ErrorInfo::ERR_OK;
    bool comm = false;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    //判断串口是否打开
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(m_isTcp);
    rtu.SetTcpClient(m_tcpClient);

    UshortArray values;
    values.clear();
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 0;
    int data     = 0;
    float tmp    = 0.0;
    int U0       = 260;       //电压量程
    int Ur       = 1;         //电压变比
    int I0       = 5;         //电流量程
    int Ir       = dev->rate; //电流变比

    start  = 0x0000;
    length = 29;
    //读取电能模块实时数据
    ret = rtu.ReadInputRegisters(values, addr, start, length);
    if (ret == ErrorInfo::ERR_OK) {
        comm = true;
        tmp  = ((short)values[0]) * 0.0001 * U0 * Ur;
        MemDb::SetRealData(regStart + 3, tmp, false); // A相电压
        tmp = ((short)values[1]) * 0.0001 * U0 * Ur;
        MemDb::SetRealData(regStart + 4, tmp, false); // B相电压
        tmp = ((short)values[2]) * 0.0001 * U0 * Ur;
        MemDb::SetRealData(regStart + 5, tmp, false); // C相电压

        tmp = ((short)values[8]) * 0.0001 * I0 * Ir;
        MemDb::SetRealData(regStart + 6, tmp, false); // A相电流
        tmp = ((short)values[9]) * 0.0001 * I0 * Ir;
        MemDb::SetRealData(regStart + 7, tmp, false); // B相电流
        tmp = ((short)values[10]) * 0.0001 * I0 * Ir;
        MemDb::SetRealData(regStart + 8, tmp, false); // C相电流

        tmp = ((short)values[12]) * 0.0001 * U0 * Ur * I0 * Ir;
        MemDb::SetRealData(regStart + 9, tmp, false); // A相有功功率
        tmp = ((short)values[13]) * 0.0001 * U0 * Ur * I0 * Ir;
        MemDb::SetRealData(regStart + 10, tmp, false); // B相有功功率
        tmp = ((short)values[14]) * 0.0001 * U0 * Ur * I0 * Ir;
        MemDb::SetRealData(regStart + 11, tmp, false); // C相有功功率
        tmp = ((short)values[15]) * 0.0001 * U0 * Ur * I0 * Ir * 3;
        MemDb::SetRealData(regStart + 12, tmp, false); //总有功功率

        tmp = ((short)values[24]) * 0.0001;
        MemDb::SetRealData(regStart + 13, tmp, false); // A相功率因数
        tmp = ((short)values[25]) * 0.0001;
        MemDb::SetRealData(regStart + 14, tmp, false); // B相功率因数
        tmp = ((short)values[26]) * 0.0001;
        MemDb::SetRealData(regStart + 15, tmp, false); // C相功率因数
        tmp = ((short)values[27]) * 0.0001;
        MemDb::SetRealData(regStart + 16, tmp, false); //总功率因数
        tmp = ((short)values[28]) * 0.01;
        MemDb::SetRealData(regStart + 2, tmp, false); //频率

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}

int HcWatt::Hc221Read(Device::SlaveDev *dev) {
    int ret   = ErrorInfo::ERR_OK;
    bool comm = false;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    //判断串口是否打开
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(m_isTcp);
    rtu.SetTcpClient(m_tcpClient);

    UshortArray values;
    values.clear();
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 0;
    int data     = 0;
    float tmp    = 0.0;
    start        = 0x0000;
    length       = 29;
    //读取模块实时数据


    start  = 0;
    length = 6;
    // 获取设备量程信息
    // zlog_warn(Util::m_zlog, "addr: %d, start: %d, length: %d", addr, start,
    // length);
   

    ret = rtu.ReadOutputRegisters(values, addr, start, length);

    // zlog_warn(Util::m_zlog, "0: %d, 1: %d,", values[0], values[1]);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 电压量程信息
        U0_33B =  values[2];
        MemDb::SetRealData(regStart + 5, U0_33B, false);
        I0_33B =  values[3];
        MemDb::SetRealData(regStart + 6, I0_33B, false);
        msleep(200);
    } else {
        zlog_warn(Util::m_zlog, "读取%s实时数据失败 :%d", dev->name.c_str(),
                  ret);
        comm = false;
        // return ErrorInfo::ERR_FAILED;
    }




    ret = rtu.ReadInputRegisters(values, addr, start, length);

    if (ret == ErrorInfo::ERR_OK) {
        comm = true;

        tmp = ((short)values[0]) * 0.0001 * U0_33B;
        // tmp = tmp - 7;
        // zlog_warn(Util::m_zlog, "vol: %f", tmp);
        // if (tmp < 0) {
        //     tmp = 0;
        // }
        MemDb::SetRealData(regStart + 1, tmp, false); //电压


        tmp = ((short)values[1]) * 0.0001 * I0_33B;
        MemDb::SetRealData(regStart + 2, tmp, false); //电流
        // zlog_warn(Util::m_zlog, "cur: %f", tmp);

        tmp = ((short)values[2]) * 0.0001 * U0_33B * I0_33B;//功率
        MemDb::SetRealData(regStart + 3, tmp, false);
        
    } else {
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
        comm = false;
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}

int HcWatt::HC33BDRead(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    bool comm    = false;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 5;
    int U0       = 0; //电压量程
    int I0       = 0; //第一路电流量程
    float tmp    = 0.0;
    int i_data   = 0;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    //判断串口是否打开
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(m_isTcp);
    rtu.SetTcpClient(m_tcpClient);

    UshortArray values;
    values.clear();
   

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        U0   = (short)values[2];
        I0   = (short)values[3];

        i_data = values[0];
        zlog_warn(Util::m_zlog, "device model: %x", i_data);

    } else {
        zlog_warn(Util::m_zlog, "读取设备量程信息失败");
    }

    start  = 0x0000;
    length = 3;
    ret    = rtu.ReadInputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        //		zlog_warn(Util::m_zlog,"读取直流模块");
        comm = true;
        MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
        tmp = ((short)values[0]) * 0.0001 * U0;
        MemDb::SetRealData(regStart + 1, tmp, false); //电压
        tmp = ((short)values[1]) * 0.0001 * I0;
        MemDb::SetRealData(regStart + 2, tmp, false); //电流
        tmp = ((short)values[2]) * 0.0001 * U0 * I0;
        MemDb::SetRealData(regStart + 3, tmp, false); //功率
    } else {
        zlog_warn(Util::m_zlog, "读取%s状态失败", dev->name.c_str());
    }

    start = 0;
    ret   = rtu.ReadInputStatus(values, addr, start, 2);
    if (ErrorInfo::ERR_OK == ret) {

        comm = true;
        bool bit;
        bit = values[0];
        MemDb::SetRealData(regStart + 4, bit, false); //第一路开关状态量

        bit = values[1];
        MemDb::SetRealData(regStart + 5, bit, false); //第二路开光状态量
    }

    return ret;
}

int HcWatt::HC201Read(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    bool comm    = false;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 16;
    bool status  = false;

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
    start  = 0;
    length = 16;

    ret = rtu.ReadInputStatus(values, addr, start, length);
    if (ret == ErrorInfo::ERR_OK) {
        comm = true;

        status = values[0] & 0b1;
        MemDb::SetRealData(regStart + 1, status, false);
        status = values[1] & 0b1;
        MemDb::SetRealData(regStart + 2, status, false);
        status = values[2] & 0b1;
        MemDb::SetRealData(regStart + 3, status, false);
        status = values[3] & 0b1;
        MemDb::SetRealData(regStart + 4, status, false);
        status = values[4] & 0b1;
        MemDb::SetRealData(regStart + 5, status, false);
        status = values[5] & 0b1;
        MemDb::SetRealData(regStart + 6, status, false);
        status = values[6] & 0b1;
        MemDb::SetRealData(regStart + 7, status, false);
        status = values[7] & 0b1;
        MemDb::SetRealData(regStart + 8, status, false);
        status = values[8] & 0b1;
        MemDb::SetRealData(regStart + 9, status, false);
        status = values[9] & 0b1;
        MemDb::SetRealData(regStart + 10, status, false);
        status = values[10] & 0b1;
        MemDb::SetRealData(regStart + 11, status, false);
        status = values[11] & 0b1;
        MemDb::SetRealData(regStart + 12, status, false);
        status = values[12] & 0b1;
        MemDb::SetRealData(regStart + 13, status, false);
        status = values[13] & 0b1;
        MemDb::SetRealData(regStart + 14, status, false);
        status = values[14] & 0b1;
        MemDb::SetRealData(regStart + 15, status, false);
        status = values[15] & 0b1;
        MemDb::SetRealData(regStart + 16, status, false);
    } else {
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
        comm = false;
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}

int HcWatt::HC202Preset(Device::SlaveDev *dev) {

    int ret      = ErrorInfo::ERR_OK;
    bool comm    = false;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 16;
    bool status  = false;

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

    RealData realData;
    // 继电器初始化，将所有继电器断开
    for (int i = 0; i < 16; i++) {
        ret = rtu.ForceSingleCoil(addr, start + i, false);
        msleep(150);
        if (ret != ErrorInfo::ERR_OK) {
            zlog_warn(Util::m_zlog, "%s HC202 初始化失败，禁止使用电操！",
                      dev->name.c_str());
            return ErrorInfo::ERR_FAILED;
        }
    }

    for (int i = 0; i < 16; i++) {
        if (MemDb::GetRealData(realData, regStart + 17 + i, true)) {
            zlog_warn(Util::m_zlog, "%s 开始下发指令", dev->name.c_str());
            ret = rtu.ForceSingleCoil(addr, start + 0 + i,
                                      realData.pv.data.iValue);
            msleep(200);
            ret = rtu.ForceSingleCoil(addr, start + 0 + i, false);
        }
    }
    return ret;
}

int HcWatt::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    switch (dev->devModel) {
    case DevModel::Model_HC_202:
        ret = this->HC202Preset(dev);
        break;
    case DevModel::Model_HC_209:
        ret = this->HC209Preset(dev);
        break;
    case DevModel::Model_HC_33B:
        ret = this->HC33Preset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int HcWatt::HC202Read(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    bool comm    = false;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 16;
    bool status  = false;

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
    start  = 0;
    length = 16;

    ret = rtu.ReadOutputStatus(values, addr, start, length);
    if (ret == ErrorInfo::ERR_OK) {
        comm = true;

        status = values[0] & 0b1;
        MemDb::SetRealData(regStart + 1, status, false);
        status = values[1] & 0b1;
        MemDb::SetRealData(regStart + 2, status, false);
        status = values[2] & 0b1;
        MemDb::SetRealData(regStart + 3, status, false);
        status = values[3] & 0b1;
        MemDb::SetRealData(regStart + 4, status, false);
        status = values[4] & 0b1;
        MemDb::SetRealData(regStart + 5, status, false);
        status = values[5] & 0b1;
        MemDb::SetRealData(regStart + 6, status, false);
        status = values[6] & 0b1;
        MemDb::SetRealData(regStart + 7, status, false);
        status = values[7] & 0b1;
        MemDb::SetRealData(regStart + 8, status, false);
        status = values[8] & 0b1;
        MemDb::SetRealData(regStart + 9, status, false);
        status = values[9] & 0b1;
        MemDb::SetRealData(regStart + 10, status, false);
        status = values[10] & 0b1;
        MemDb::SetRealData(regStart + 11, status, false);
        status = values[11] & 0b1;
        MemDb::SetRealData(regStart + 12, status, false);
        status = values[12] & 0b1;
        MemDb::SetRealData(regStart + 13, status, false);
        status = values[13] & 0b1;
        MemDb::SetRealData(regStart + 14, status, false);
        status = values[14] & 0b1;
        MemDb::SetRealData(regStart + 15, status, false);
        status = values[15] & 0b1;
        MemDb::SetRealData(regStart + 16, status, false);
    } else {
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
        comm = false;
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}

int HcWatt::HC209Read(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    bool comm    = false;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 8;
    bool status  = false;

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
    start  = 0;
    length = 8;

    ret = rtu.ReadInputStatus(values, addr, start, length);
    if (ret == ErrorInfo::ERR_OK) {
        comm = true;

        status = values[0] & 0b1;
        MemDb::SetRealData(regStart + 1, status, false);
        status = values[1] & 0b1;
        MemDb::SetRealData(regStart + 2, status, false);
        status = values[2] & 0b1;
        MemDb::SetRealData(regStart + 3, status, false);
        status = values[3] & 0b1;
        MemDb::SetRealData(regStart + 4, status, false);
        status = values[4] & 0b1;
        MemDb::SetRealData(regStart + 5, status, false);
        status = values[5] & 0b1;
        MemDb::SetRealData(regStart + 6, status, false);
        status = values[6] & 0b1;
        MemDb::SetRealData(regStart + 7, status, false);
        status = values[7] & 0b1;
        MemDb::SetRealData(regStart + 8, status, false);
    } else {
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
        comm = false;
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}
int HcWatt::HC209Preset(Device::SlaveDev *dev) {

    int ret      = ErrorInfo::ERR_OK;
    bool comm    = false;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 8;
    bool status  = false;

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

    RealData realData;
    // 继电器初始化，将所有继电器断开
    for (int i = 0; i < 8; i++) {
        ret = rtu.ForceSingleCoil(addr, start + i, false);
        msleep(150);
        if (ret != ErrorInfo::ERR_OK) {
            zlog_warn(Util::m_zlog, "%s HC209 初始化失败，禁止使用电操！",
                      dev->name.c_str());
            return ErrorInfo::ERR_FAILED;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (MemDb::GetRealData(realData, regStart + 11 + i, true)) {
            zlog_warn(Util::m_zlog, "%s 开始下发指令", dev->name.c_str());
            ret = rtu.ForceSingleCoil(addr, start + 0 + i,
                                      realData.pv.data.iValue);
            msleep(200);
            ret = rtu.ForceSingleCoil(addr, start + 0 + i, false);
        }
    }
    return ret;
}

int HcWatt::HC33BRead(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    bool comm    = false;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 0;
    bool status  = false;
    int i_data   = 0;
    float f_data = 0.0f;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    //判断串口是否打开
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }
    UshortArray values;
    values.clear();

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(m_tcpClient);

    start  = 0;
    length = 6;
    // 获取设备量程信息
    // zlog_warn(Util::m_zlog, "addr: %d, start: %d, length: %d", addr, start,
    // length);
   

    ret = rtu.ReadOutputRegisters(values, addr, start, length);

    // zlog_warn(Util::m_zlog, "0: %d, 1: %d,", values[0], values[1]);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 电压量程信息
        U0_33B =  values[2];
        MemDb::SetRealData(regStart + 35, U0_33B, false);
        I0_33B =  values[3];
        MemDb::SetRealData(regStart + 36, I0_33B, false);
        msleep(200);
    } else {
        zlog_warn(Util::m_zlog, "读取%s实时数据失败 :%d", dev->name.c_str(),
                  ret);
        comm = false;
        // return ErrorInfo::ERR_FAILED;
    }
    
    // 0010H	Ur	电压变比; 1～2000  默认值1;配置时，U0*Ur应不超过200 000；
    // 0011H	Ir	电流变比; 1～2000  默认值1; 配置时，I0*Ir应不超过20 000；
    start  = 0x0010;
    length = 2;
    // 获取设备量程信息
    // zlog_warn(Util::m_zlog, "addr: %d, start: %d, length: %d", addr, start,
    // length);

    ret = rtu.ReadOutputRegisters(values, addr, start, length);

    // zlog_warn(Util::m_zlog, "0: %d, 1: %d,", values[0], values[1]);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 电压量程信息

        Ur_33B =  values[0];
        MemDb::SetRealData(regStart + 37, Ur_33B, false);
        Ir_33B =  values[1];
        MemDb::SetRealData(regStart + 38, Ir_33B, false);


        msleep(200);
    } else {
        zlog_warn(Util::m_zlog, "读取%s实时数据失败 :%d", dev->name.c_str(),
                  ret);
        comm = false;
        // return ErrorInfo::ERR_FAILED;
    }

    start  = 0x0;
    length = 2;
    values.clear();
    //读取开关量输出
   

    ret = rtu.ReadOutputStatus(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        status = values[0];
        MemDb::SetRealData(regStart + 1, status, false);
        status = values[1];
        MemDb::SetRealData(regStart + 2, status, false);
    } else {
        zlog_warn(Util::m_zlog, "读取%s  %s实时数据失败", dev->name.c_str(),dev->slaveAddr.c_str());
        comm = false;
    }

    values.clear();
    // 读取开关量输入定义
    start  = 0;
    length = 2;
   

    ret    = rtu.ReadInputStatus(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        status = values[0];
        if (status==0)
        {
            MemDb::SetRealData(regStart + 3, 1, false);
        }else{
            MemDb::SetRealData(regStart + 3, 0, false);
        }
        status = values[1];
        MemDb::SetRealData(regStart + 4, status, false);
    } else {
        zlog_warn(Util::m_zlog, "读取%s  %s实时数据失败", dev->name.c_str(),dev->slaveAddr.c_str());
        comm = false;
    }

    values.clear();
    start  = 0;
    length = 30;

    // 读取电表数据
   
    ret = rtu.ReadInputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // 电压
        // zlog_warn(Util::m_zlog, "U0: %d, I0: %d,  Ir: %d, Ur: %d", U0_33B,
        // I0_33B, Ur_33B, Ir_33B);
        MemDb::GetRealData(U0_33B, regStart + 35);
        MemDb::GetRealData(I0_33B, regStart + 36);
        MemDb::GetRealData(Ur_33B, regStart + 37);
        MemDb::GetRealData(Ir_33B, regStart + 38);
        // printf("-------------------------------\n");
        // printf("U0:%d, I0:%d, Ur:%d, Ir:%d\n", U0_33B, I0_33B, Ur_33B, Ir_33B);
        // printf("U0:%d, I0:%d, Ur:%d, Ir:%d\n", U0_33B, I0_33B, Ur_33B, Ir_33B);
        // printf("U0:%d, I0:%d, Ur:%d, Ir:%d\n", U0_33B, I0_33B, Ur_33B, Ir_33B);
        // printf("-------------------------------\n");


        f_data = values[0] * U0_33B * Ur_33B * 0.0001;
        MemDb::SetRealData(regStart + 5, f_data, false);
        f_data = values[1] * U0_33B * Ur_33B * 0.0001;
        MemDb::SetRealData(regStart + 6, f_data, false);
        f_data = values[2] * U0_33B * Ur_33B * 0.0001;
        MemDb::SetRealData(regStart + 7, f_data, false);
        f_data = values[3] * U0_33B * Ur_33B * 0.0001;
        MemDb::SetRealData(regStart + 8, f_data, false);
        f_data = values[4] * U0_33B * Ur_33B * 0.0001;
        MemDb::SetRealData(regStart + 9, f_data, false);
        f_data = values[5] * U0_33B * Ur_33B * 0.0001;
        MemDb::SetRealData(regStart + 10, f_data, false);
        f_data = values[6] * U0_33B * Ur_33B * 0.0001;
        MemDb::SetRealData(regStart + 11, f_data, false);
        f_data = values[7] * U0_33B * Ur_33B * 0.0001;
        MemDb::SetRealData(regStart + 12, f_data, false);

        //电流
        f_data = values[8] * I0_33B * Ir_33B * 0.0001;
        MemDb::SetRealData(regStart + 13, f_data, false);
        f_data = values[9] * I0_33B * Ir_33B * 0.0001;
        MemDb::SetRealData(regStart + 14, f_data, false);
        f_data = values[10] * I0_33B * Ir_33B * 0.0001;
        MemDb::SetRealData(regStart + 15, f_data, false);
        f_data = values[11] * I0_33B * Ir_33B * 0.0001;
        MemDb::SetRealData(regStart + 16, f_data, false);

        // 有功功率
        f_data = (signed short)values[12] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        // printf("有功功率%f u0:%d ur:%d i0:%d ir:%d\n", values[12], U0_33B, Ur_33B, I0_33B, Ir_33B);
        // printf("有功功率%f u0:%d ur:%d i0:%d ir:%d\n", values[12], U0_33B, Ur_33B, I0_33B, Ir_33B);
        // printf("有功功率%f u0:%d ur:%d i0:%d ir:%d\n", values[12], U0_33B, Ur_33B, I0_33B, Ir_33B);

        MemDb::SetRealData(regStart + 17, f_data, false);
        f_data = (signed short)values[13] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 18, f_data, false);
        f_data = (signed short)values[14] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 19, f_data, false);
        f_data = (signed short)values[15] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B*3;
        MemDb::SetRealData(regStart + 20, f_data, false);

        // 无功功率
        f_data = (signed short)values[16] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 21, f_data, false);
        // printf("无功功率：%f u0:%d ur:%d i0:%d ir:%d\n", values[16], U0_33B, Ur_33B, I0_33B, Ir_33B);
        // printf("无功功率：%f u0:%d ur:%d i0:%d ir:%d\n", values[16], U0_33B, Ur_33B, I0_33B, Ir_33B);
        // printf("无功功率：%f u0:%d ur:%d i0:%d ir:%d\n", values[16], U0_33B, Ur_33B, I0_33B, Ir_33B);

        f_data = (signed short)values[17] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 22, f_data, false);
        f_data = (signed short)values[18] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 23, f_data, false);
        f_data = (signed short)values[19] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B*3;
        MemDb::SetRealData(regStart + 24, f_data, false);

        // 视在功率
        f_data = (unsigned short)values[20] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 25, f_data, false);
        f_data = (unsigned short)values[21] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 26, f_data, false);
        f_data = (unsigned short)values[22] * I0_33B * Ir_33B * 0.0001* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 27, f_data, false);
        f_data = (unsigned short)values[23] * I0_33B * Ir_33B * 0.0001*3* U0_33B * Ur_33B;
        MemDb::SetRealData(regStart + 28, f_data, false);

        // 功率因数
        f_data = ((signed short)values[24]) * 0.0001;
        MemDb::SetRealData(regStart + 29, f_data, false);
        f_data = ((signed short)values[25]) * 0.0001;
        MemDb::SetRealData(regStart + 30, f_data, false);
        f_data = ((signed short)values[26]) * 0.0001;
        MemDb::SetRealData(regStart + 31, f_data, false);
        f_data = ((signed short)values[27]) * 0.0001;
        MemDb::SetRealData(regStart + 32, f_data, false);

        // 频率
        f_data = values[28] * 0.01;
        MemDb::SetRealData(regStart + 33, f_data, false);
        // printf("读取成功\n");

    } else {
        zlog_warn(Util::m_zlog, "读取%s  %s实时数据失败", dev->name.c_str(),dev->slaveAddr.c_str());
        comm = false;
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}

int HcWatt::HC33Preset(Device::SlaveDev *dev) {

    int ret      = ErrorInfo::ERR_OK;
    bool comm    = false;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0x0000;
    int length   = 2;
    bool status  = false;
    int i_data   = 0;
    float f_data = 0.0f;

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
    
    RealData realData;
    // 继电器初始化，将所有继电器断开
    for (int i = 0; i < 2; i++) {
        ret = rtu.ForceSingleCoil(addr, start + i, false);
        msleep(150);
        if (ret != ErrorInfo::ERR_OK) {
            zlog_warn(Util::m_zlog, "%s HC209 初始化失败，禁止使用电操！",
                      dev->name.c_str());
            return ErrorInfo::ERR_FAILED;
        }

    }

    for (int i = 0; i < 2; i++) {
        if (MemDb::GetRealData(realData, regStart + 41 + i, true)) {
            zlog_warn(Util::m_zlog, "%s 开始下发指令", dev->name.c_str());
            ret = rtu.ForceSingleCoil(addr, start + 0 + i,
                                      realData.pv.data.iValue);
            msleep(200);
            ret = rtu.ForceSingleCoil(addr, start + 0 + i, false);
        }
    }
    return ret;
}
