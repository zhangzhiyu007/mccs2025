/*
 * SinexcelPcs.cpp
 *
 *  创建日期: 2016-7-6
 *  作          者: 刘海涛
 */

#include "SinexcelPcs.h"

SinexcelPcs::SinexcelPcs() {
    // TODO Auto-generated constructor stub
    m_com = NULL;
    m_tcp = NULL;
}

SinexcelPcs::~SinexcelPcs() {
    // TODO Auto-generated destructor stub
}

//设置TcpClient指针
void SinexcelPcs::SetTcp(TcpClient *tcpClient) { m_tcp = tcpClient; }

void SinexcelPcs::setCom(Com *com) { m_com = com; }

int SinexcelPcs::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    //判断设备名称
    //判断设备型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::Model_SINEXCEL_PCS: //深圳盛弘PCS
        ret = this->PcsRead(dev);
        break;
    default:
        break;
    };

    return ret;
}

int SinexcelPcs::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    //判断设备名称
    //判断设备型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::Model_SINEXCEL_PCS: //深圳盛弘PCS
        ret = this->PcsPreset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int SinexcelPcs::PcsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    if (dev->commType == 2) {
        ret = this->SinexcelTcpPcsRead(dev);
    } else if (dev->commType == 1) {
        ret = this->SinexcelPcsRead(dev);
    }

    return ret;
}

int SinexcelPcs::PcsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    if (dev->commType == 2) {
        ret = this->SinexcelTcpPcsPreset(dev);
    } else if (dev->commType == 1) {
        ret = this->SinexcelPcsPreset(dev);
    }
    return ret;
}

int SinexcelPcs::SinexcelPcsRead(Device::SlaveDev *dev) {
    int ret   = ErrorInfo::ERR_OK;
    bool comm = false;
    UshortArray values;
    values.clear();
    int addr       = atoi(dev->slaveAddr.c_str());
    int regStart   = dev->regStart;
    int start      = 0;
    int length     = 0;
    float tmp      = 0.0;
    int data       = 0;
    int value      = 0;
    int fault1     = 0;
    int fault2     = 0;
    int fault3     = 0;
    int fault4     = 0;
    int fault5     = 0;
    float gridFreq = 0.0;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    //判断串口是否打开
    if (!m_com->IsOpen()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetCom(m_com);
    //读取电网实时反馈信息
    start  = 54200;
    length = 24;
    ret    = rtu.ReadInputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp  = (short)values[0] * 0.1;
        MemDb::SetRealData(regStart + 49, tmp, false); //电网Uab
        tmp = (short)values[1] * 0.1;
        MemDb::SetRealData(regStart + 50, tmp, false); //电网Ubc
        tmp = (short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 51, tmp, false); //电网Uca
        tmp = ((short)values[3] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 52, tmp, false); //电网A相电流
        tmp = ((short)values[4] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 53, tmp, false); //电网B相电流
        tmp = ((short)values[5] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 54, tmp, false); //电网C相电流
        tmp      = (short)values[6] * 0.01;
        gridFreq = tmp;
        MemDb::SetRealData(regStart + 55, tmp, false); //电网频率
        tmp = ((short)values[9] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 56, tmp, false); //电网A相有功功率
        tmp = ((short)values[10] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 57, tmp, false); //电网B相有功功率
        tmp = ((short)values[11] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 58, tmp, false); //电网C相有功功率
        tmp = ((short)values[21] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 59, tmp, false); //电网总有功功率
        tmp = ((short)values[12] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 60, tmp, false); //电网A相无功功率
        tmp = ((short)values[13] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 61, tmp, false); //电网B相无功功率
        tmp = ((short)values[14] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 62, tmp, false); //电网C相无功功率
        tmp = ((short)values[22] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 63, tmp, false); //电网总无功功率
        tmp = ((short)values[15] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 64, tmp, false); //电网A相视在功率
        tmp = ((short)values[16] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 65, tmp, false); //电网B相视在功率
        tmp = ((short)values[17] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 66, tmp, false); //电网C相视在功率
        tmp = ((short)values[23] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 67, tmp, false); //电网总视在功率
        tmp = ((short)values[18] * 0.01 - 1);
        MemDb::SetRealData(regStart + 68, tmp, false); //电网功率因数
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    //读取负载实时反馈信息
    start  = 55200;
    length = 24;
    ret    = rtu.ReadInputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp  = (short)values[0] * 0.1;
        MemDb::SetRealData(regStart + 73, tmp, false); //负载Uab
        tmp = (short)values[1] * 0.1;
        MemDb::SetRealData(regStart + 74, tmp, false); //负载Ubc
        tmp = (short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 75, tmp, false); //负载Uca
        tmp = ((short)values[3] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 76, tmp, false); //负载A相电流
        tmp = ((short)values[4] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 77, tmp, false); //负载B相电流
        tmp = ((short)values[5] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 78, tmp, false); //负载C相电流
        tmp = (short)values[6] * 0.01;
        MemDb::SetRealData(regStart + 79, tmp, false); //负载频率
        tmp = ((short)values[9] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 80, tmp, false); //负载A相有功功率
        tmp = ((short)values[10] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 81, tmp, false); //负载B相有功功率
        tmp = ((short)values[11] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 82, tmp, false); //负载C相有功功率
        tmp = ((short)values[21] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 83, tmp, false); //负载总有功功率
        tmp = ((short)values[12] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 84, tmp, false); //负载A相无功功率
        tmp = ((short)values[13] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 85, tmp, false); //负载B相无功功率
        tmp = ((short)values[14] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 86, tmp, false); //负载C相无功功率
        tmp = ((short)values[22] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 87, tmp, false); //负载总无功功率
        tmp = ((short)values[15] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 88, tmp, false); //负载A相视在功率
        tmp = ((short)values[16] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 89, tmp, false); //负载B相视在功率
        tmp = ((short)values[17] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 90, tmp, false); //负载C相视在功率
        tmp = ((short)values[23] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 91, tmp, false); //负载总视在功率
        tmp = ((short)values[18] * 0.01 - 1);
        MemDb::SetRealData(regStart + 92, tmp, false); //负载功率因数
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    //读取变流器设置反馈信息
    start  = 53600;
    length = 66;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        data = (short)values[0];
        MemDb::SetRealData(regStart + 4, data, false); //并离网
        data = (short)values[1];
        MemDb::SetRealData(regStart + 2, data, false); //调度方式
        data = (short)values[20];
        MemDb::SetRealData(regStart + 3, data, false); //控制模式
        data = (short)values[27];
        MemDb::SetRealData(regStart + 7, data, false); //电网恢复延时
        tmp = (short)values[21] * 0.01;
        MemDb::SetRealData(regStart + 11, tmp, false); //功率因数
        tmp = (short)values[22] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 12, tmp, false); //有功功率
        tmp = (short)values[23] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 13, tmp, false); //无功功率
        tmp = (short)values[24] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 14, tmp, false); //有功功率变化速率
        tmp = (short)values[25] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 15, tmp, false); //无功功率变化速率
        data = (short)values[26];
        MemDb::SetRealData(regStart + 16, data, false); //功率变化模式
        tmp = (short)values[28] * 0.01;
        MemDb::SetRealData(regStart + 17, tmp, false); //离网电压
        tmp = (short)values[29] * 0.01;
        MemDb::SetRealData(regStart + 18, tmp, false); //离网频率
        data = (short)values[50];
        MemDb::SetRealData(regStart + 19, data, false); //直流控制模式
        tmp = (short)values[51] * 0.1;
        MemDb::SetRealData(regStart + 20, tmp, false); //直流充放电电流
        tmp = (short)values[52] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 21, tmp, false); //直流充放电功率
        tmp = (short)values[53] * 0.1;
        MemDb::SetRealData(regStart + 22, tmp, false); //电池EOD电压
        tmp = (short)values[54] * 0.1;
        MemDb::SetRealData(regStart + 23, tmp, false); //放电起始电压
        tmp = (short)values[55] * 0.1;
        MemDb::SetRealData(regStart + 24, tmp, false); //放电终止电压
        tmp = (short)values[56] * 0.1;
        MemDb::SetRealData(regStart + 25, tmp, false); //预充电电压
        tmp = (short)values[57] * 0.1;
        MemDb::SetRealData(regStart + 26, tmp, false); //预充转快充电压
        tmp = (short)values[58];
        MemDb::SetRealData(regStart + 27, tmp, false); //预充电时间
        tmp = (short)values[59] * 0.1;
        MemDb::SetRealData(regStart + 28, tmp, false); //电池浮充电压
        tmp = (short)values[60] * 0.1;
        MemDb::SetRealData(regStart + 29, tmp, false); //电池均充电压
        tmp = (short)values[61] * 0.1;
        MemDb::SetRealData(regStart + 30, tmp, false); //均充转浮充电流
        tmp = (short)values[62] * 0.1;
        MemDb::SetRealData(regStart + 31, tmp, false); //充电截止电流
        tmp = (short)values[63] * 0.1;
        MemDb::SetRealData(regStart + 32, tmp, false); //最大充电电流
        tmp = (short)values[64] * 0.1;
        MemDb::SetRealData(regStart + 33, tmp, false); //最大放电电流
        tmp = (short)values[65] * 0.1;
        MemDb::SetRealData(regStart + 34, tmp, false); //预充最大电流
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    //读取系统状态
    start  = 53010;
    length = 1;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm  = true;
        data  = (short)values[0];
        value = Util::GetBitValue(data, 0);
        MemDb::SetRealData(regStart + 40, value, false); //运行状态
        value = Util::GetBitValue(data, 2);
        if (value == 1) {
            MemDb::SetRealData(regStart + 42, 0, false); //并网
        }

        value = Util::GetBitValue(data, 3);
        if (value == 1) {
            MemDb::SetRealData(regStart + 42, 1, false); //离网
        }
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    //读取系统状态
    start  = 53000;
    length = 10;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        data = (short)values[0];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault1, i, value);
        }

        data = (short)values[1];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault1, i + 16, value);
        }

        data = (short)values[2];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault2, i, value);
        }

        data = (short)values[3];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault2, i + 16, value);
        }

        data = (short)values[4];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault3, i, value);
        }

        data = (short)values[5];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault3, i + 16, value);
        }

        data = (short)values[6];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault4, i, value);
        }

        data = (short)values[7];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault4, i + 16, value);
        }

        data = (short)values[8];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault5, i, value);
        }

        data = (short)values[9];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault5, i + 16, value);
        }

        if ((fault1 != 0) || (fault2 != 0) || (fault3 != 0) || (fault4 != 0) ||
            (fault5 != 0)) {
            MemDb::SetRealData(regStart + 43, 1, false);      //故障状态
            MemDb::SetRealData(regStart + 44, fault1, false); //故障码1
            MemDb::SetRealData(regStart + 45, fault2, false); //故障码2
            MemDb::SetRealData(regStart + 46, fault3, false); //故障码3
            MemDb::SetRealData(regStart + 47, fault4, false); //故障码4
            MemDb::SetRealData(regStart + 48, fault5, false); //故障码5
        } else {
            MemDb::SetRealData(regStart + 43, 0, false);      //故障状态
            MemDb::SetRealData(regStart + 44, fault1, false); //故障码1
            MemDb::SetRealData(regStart + 45, fault2, false); //故障码2
            MemDb::SetRealData(regStart + 46, fault3, false); //故障码3
            MemDb::SetRealData(regStart + 47, fault4, false); //故障码4
            MemDb::SetRealData(regStart + 48, fault5, false); //故障码5
        }
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}

int SinexcelPcs::SinexcelPcsPreset(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    RealData realData;
    int tmp = 0;

    //判断串口是否打开
    if (!m_com->IsOpen()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetCom(m_com);

    //判断是否有启动停止设置
    if (MemDb::GetRealDataWrited(realData, regStart + 1, true)) {
        if (realData.pv.data.iValue == 1) //启动
        {
            ret = rtu.PresetSingleRegister(addr, 53900, 1);
        } else if (realData.pv.data.iValue == 0) //停止
        {
            ret = rtu.PresetSingleRegister(addr, 53901, 1);
        }
    }

    //判断是否有变流器故障清除
    if (MemDb::GetRealDataWrited(realData, regStart + 5, true)) {
        ret = rtu.PresetSingleRegister(addr, 53903,
                                       realData.pv.data.iValue); //故障清除
    }

    //判断是否有负载故障清除
    if (MemDb::GetRealDataWrited(realData, regStart + 6, true)) {
        ret = rtu.PresetSingleRegister(addr, 54901,
                                       realData.pv.data.iValue); //故障清除
    }

    //判断是否有并离网
    if (MemDb::GetRealDataWrited(realData, regStart + 4, true)) {
        ret = rtu.PresetSingleRegister(addr, 54900,
                                       realData.pv.data.iValue); //并离网
    }

    //判断是否有调度方式
    if (MemDb::GetRealDataWrited(realData, regStart + 2, true)) {
        ret = rtu.PresetSingleRegister(addr, 53601,
                                       realData.pv.data.iValue); //调度方式
    }

    //判断是否有控制模式
    if (MemDb::GetRealDataWrited(realData, regStart + 3, true)) {
        ret = rtu.PresetSingleRegister(addr, 53620,
                                       realData.pv.data.iValue); //控制模式
    }

    //判断是否有电网恢复延时
    if (MemDb::GetRealDataWrited(realData, regStart + 7, true)) {
        ret = rtu.PresetSingleRegister(addr, 53627,
                                       realData.pv.data.iValue); //电网恢复延时
    }

    //判断是否有功率因数
    if (MemDb::GetRealDataWrited(realData, regStart + 11, true)) {
        tmp = realData.pv.data.dbValue * 100;
        ret = rtu.PresetSingleRegister(addr, 53621, tmp); //功率因数
    }

    //判断是否有有功功率
    if (MemDb::GetRealDataWrited(realData, regStart + 12, true)) {
        tmp = realData.pv.data.dbValue * 0.001 * 10;
        ret = rtu.PresetSingleRegister(addr, 53622, tmp); //有功功率
    }

    //判断是否有无功功率
    if (MemDb::GetRealDataWrited(realData, regStart + 13, true)) {
        tmp = realData.pv.data.dbValue * 0.001 * 10;
        ret = rtu.PresetSingleRegister(addr, 53623, tmp); //无功功率
    }

    //判断是否有有功功率变化速率
    if (MemDb::GetRealDataWrited(realData, regStart + 14, true)) {
        tmp = realData.pv.data.dbValue * 0.001 * 10;
        ret = rtu.PresetSingleRegister(addr, 53624, tmp); //有功功率变化速率
    }

    //判断是否有无功变化速率
    if (MemDb::GetRealDataWrited(realData, regStart + 15, true)) {
        tmp = realData.pv.data.dbValue * 0.001 * 10;
        ret = rtu.PresetSingleRegister(addr, 53625, tmp); //无功变化速率
    }

    //判断是否有功率变化模式
    if (MemDb::GetRealDataWrited(realData, regStart + 16, true)) {
        ret = rtu.PresetSingleRegister(addr, 53626,
                                       realData.pv.data.iValue); //电网恢复延时
    }

    //判断是否有离网电压范围
    if (MemDb::GetRealDataWrited(realData, regStart + 17, true)) {
        tmp = realData.pv.data.dbValue * 100;
        ret = rtu.PresetSingleRegister(addr, 53628, tmp); //离网电压范围
    }

    //判断是否有离网电压频率
    if (MemDb::GetRealDataWrited(realData, regStart + 18, true)) {
        tmp = realData.pv.data.dbValue * 100;
        ret = rtu.PresetSingleRegister(addr, 53629, tmp); //离网电压频率
    }

    //判断是否直流控制模式
    if (MemDb::GetRealDataWrited(realData, regStart + 19, true)) {
        ret = rtu.PresetSingleRegister(addr, 53650,
                                       realData.pv.data.iValue); //直流控制模式
    }

    //判断是否有直流充放电电流
    if (MemDb::GetRealDataWrited(realData, regStart + 20, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53651, tmp); //直流充放电电流
    }

    //判断是否有直流充放电功率
    if (MemDb::GetRealDataWrited(realData, regStart + 21, true)) {
        tmp = realData.pv.data.dbValue * 10 * 0.001;
        ret = rtu.PresetSingleRegister(addr, 53652, tmp); //直流充放电功率
    }

    //判断是否有电池EOD电压
    if (MemDb::GetRealDataWrited(realData, regStart + 22, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53653, tmp); //电池EOD电压
    }

    //判断是否有放电起始电压
    if (MemDb::GetRealDataWrited(realData, regStart + 23, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53654, tmp); //放电起始电压
    }

    //判断是否有放电终止电压
    if (MemDb::GetRealDataWrited(realData, regStart + 24, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53655, tmp); //放电终止电压
    }

    //判断是否有预充电电压
    if (MemDb::GetRealDataWrited(realData, regStart + 25, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53656, tmp); //预充电电压
    }

    //判断是否有预充转快充电压
    if (MemDb::GetRealDataWrited(realData, regStart + 26, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53657, tmp); //预充转快充电压
    }

    //判断是否有预充电时间
    if (MemDb::GetRealDataWrited(realData, regStart + 27, true)) {
        tmp = realData.pv.data.dbValue;
        ret = rtu.PresetSingleRegister(addr, 53658, tmp); //预充电时间
    }

    //判断是否有电池浮充电压
    if (MemDb::GetRealDataWrited(realData, regStart + 28, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53659, tmp); //电池浮充电压
    }

    //判断是否有电池均充电压
    if (MemDb::GetRealDataWrited(realData, regStart + 29, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53660, tmp); //电池均充电压
    }

    //判断是否有均充转浮充电流
    if (MemDb::GetRealDataWrited(realData, regStart + 30, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53661, tmp); //均充转浮充电流
    }

    //判断是否有充电截止电流
    if (MemDb::GetRealDataWrited(realData, regStart + 31, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53662, tmp); //充电截止电流
    }

    //判断是否有最大充电电流
    if (MemDb::GetRealDataWrited(realData, regStart + 32, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53663, tmp); //最大充电电流
    }

    //判断是否有最大放电电流
    if (MemDb::GetRealDataWrited(realData, regStart + 33, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53664, tmp); //最大放电电流
    }

    //判断是否有预充最大电流
    if (MemDb::GetRealDataWrited(realData, regStart + 34, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 53665, tmp); //预充最大电流
    }
    return ret;
}

int SinexcelPcs::SinexcelTcpPcsRead(Device::SlaveDev *dev) {
    int ret   = ErrorInfo::ERR_OK;
    bool comm = false;
    UshortArray values;
    values.clear();
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 0;
    int length   = 0;
    float tmp    = 0.0;
    int data     = 0;
    int value    = 0;
    int fault1   = 0;
    int fault2   = 0;
    int fault3   = 0;
    int fault4   = 0;
    int fault5   = 0;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    //判断串口是否打开
    if (!m_tcp->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusTcpClient client;
    client.SetTcpClient(this->m_tcp);
    //读取电网实时反馈信息
    start  = 54200;
    length = 24;
    ret    = client.ReadRs485OutputRegisters(values, addr, start, length);

    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp  = (short)values[0] * 0.1;
        MemDb::SetRealData(regStart + 49, tmp, false); //电网Uab
        tmp = (short)values[1] * 0.1;
        MemDb::SetRealData(regStart + 50, tmp, false); //电网Ubc
        tmp = (short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 51, tmp, false); //电网Uca
        tmp = ((short)values[3] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 52, tmp, false); //电网A相电流
        tmp = ((short)values[4] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 53, tmp, false); //电网B相电流
        tmp = ((short)values[5] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 54, tmp, false); //电网C相电流
        tmp = (short)values[6] * 0.01;
        MemDb::SetRealData(regStart + 55, tmp, false); //电网频率
        tmp = ((short)values[9] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 56, tmp, false); //电网A相有功功率
        tmp = ((short)values[10] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 57, tmp, false); //电网B相有功功率
        tmp = ((short)values[11] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 58, tmp, false); //电网C相有功功率
        tmp = ((short)values[21] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 59, tmp, false); //电网总有功功率
        tmp = ((short)values[12] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 60, tmp, false); //电网A相无功功率
        tmp = ((short)values[13] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 61, tmp, false); //电网B相无功功率
        tmp = ((short)values[14] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 62, tmp, false); //电网C相无功功率
        tmp = ((short)values[22] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 63, tmp, false); //电网总无功功率
        tmp = ((short)values[15] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 64, tmp, false); //电网A相视在功率
        tmp = ((short)values[16] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 65, tmp, false); //电网B相视在功率
        tmp = ((short)values[17] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 66, tmp, false); //电网C相视在功率
        tmp = ((short)values[23] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 67, tmp, false); //电网总视在功率
        tmp = ((short)values[18] * 0.01 - 1);
        MemDb::SetRealData(regStart + 68, tmp, false); //电网功率因数
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    //读取负载实时反馈信息
    start  = 55200;
    length = 24;
    ret    = client.ReadRs485OutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp  = (short)values[0] * 0.1;
        MemDb::SetRealData(regStart + 73, tmp, false); //负载Uab
        tmp = (short)values[1] * 0.1;
        MemDb::SetRealData(regStart + 74, tmp, false); //负载Ubc
        tmp = (short)values[2] * 0.1;
        MemDb::SetRealData(regStart + 75, tmp, false); //负载Uca
        tmp = ((short)values[3] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 76, tmp, false); //负载A相电流
        tmp = ((short)values[4] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 77, tmp, false); //负载B相电流
        tmp = ((short)values[5] * 0.1 - 1500);
        MemDb::SetRealData(regStart + 78, tmp, false); //负载C相电流
        tmp = (short)values[6] * 0.01;
        MemDb::SetRealData(regStart + 79, tmp, false); //负载频率
        tmp = ((short)values[9] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 80, tmp, false); //负载A相有功功率
        tmp = ((short)values[10] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 81, tmp, false); //负载B相有功功率
        tmp = ((short)values[11] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 82, tmp, false); //负载C相有功功率
        tmp = ((short)values[21] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 83, tmp, false); //负载总有功功率
        tmp = ((short)values[12] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 84, tmp, false); //负载A相无功功率
        tmp = ((short)values[13] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 85, tmp, false); //负载B相无功功率
        tmp = ((short)values[14] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 86, tmp, false); //负载C相无功功率
        tmp = ((short)values[22] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 87, tmp, false); //负载总无功功率
        tmp = ((short)values[15] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 88, tmp, false); //负载A相视在功率
        tmp = ((short)values[16] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 89, tmp, false); //负载B相视在功率
        tmp = ((short)values[17] * 0.1 - 400) * 1000;
        MemDb::SetRealData(regStart + 90, tmp, false); //负载C相视在功率
        tmp = ((short)values[23] * 0.1 - 1200) * 1000;
        MemDb::SetRealData(regStart + 91, tmp, false); //负载总视在功率
        tmp = ((short)values[18] * 0.01 - 1);
        MemDb::SetRealData(regStart + 92, tmp, false); //负载功率因数
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    //读取变流器设置反馈信息
    start  = 53600;
    length = 66;
    ret    = client.ReadRs485OutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        data = (short)values[0];
        MemDb::SetRealData(regStart + 4, data, false); //并离网
        data = (short)values[1];
        MemDb::SetRealData(regStart + 2, data, false); //调度方式
        data = (short)values[20];
        MemDb::SetRealData(regStart + 3, data, false); //控制模式
        data = (short)values[27];
        MemDb::SetRealData(regStart + 7, data, false); //电网恢复延时
        tmp = (short)values[21] * 0.01;
        MemDb::SetRealData(regStart + 11, tmp, false); //功率因数
        tmp = (short)values[22] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 12, tmp, false); //有功功率
        tmp = (short)values[23] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 13, tmp, false); //无功功率
        tmp = (short)values[24] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 14, tmp, false); //有功功率变化速率
        tmp = (short)values[25] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 15, tmp, false); //无功功率变化速率
        data = (short)values[26];
        MemDb::SetRealData(regStart + 16, data, false); //功率变化模式
        tmp = (short)values[28] * 0.01;
        MemDb::SetRealData(regStart + 17, tmp, false); //离网电压
        tmp = (short)values[29] * 0.01;
        MemDb::SetRealData(regStart + 18, tmp, false); //离网频率
        data = (short)values[50];
        MemDb::SetRealData(regStart + 19, data, false); //直流控制模式
        tmp = (short)values[51] * 0.1;
        MemDb::SetRealData(regStart + 20, tmp, false); //直流充放电电流
        tmp = (short)values[52] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 21, tmp, false); //直流充放电功率
        tmp = (short)values[53] * 0.1;
        MemDb::SetRealData(regStart + 22, tmp, false); //电池EOD电压
        tmp = (short)values[54] * 0.1;
        MemDb::SetRealData(regStart + 23, tmp, false); //放电起始电压
        tmp = (short)values[55] * 0.1;
        MemDb::SetRealData(regStart + 24, tmp, false); //放电终止电压
        tmp = (short)values[56] * 0.1;
        MemDb::SetRealData(regStart + 25, tmp, false); //预充电电压
        tmp = (short)values[57] * 0.1;
        MemDb::SetRealData(regStart + 26, tmp, false); //预充转快充电压
        tmp = (short)values[58];
        MemDb::SetRealData(regStart + 27, tmp, false); //预充电时间
        tmp = (short)values[59] * 0.1;
        MemDb::SetRealData(regStart + 28, tmp, false); //电池浮充电压
        tmp = (short)values[60] * 0.1;
        MemDb::SetRealData(regStart + 29, tmp, false); //电池均充电压
        tmp = (short)values[61] * 0.1;
        MemDb::SetRealData(regStart + 30, tmp, false); //均充转浮充电流
        tmp = (short)values[62] * 0.1;
        MemDb::SetRealData(regStart + 31, tmp, false); //充电截止电流
        tmp = (short)values[63] * 0.1;
        MemDb::SetRealData(regStart + 32, tmp, false); //最大充电电流
        tmp = (short)values[64] * 0.1;
        MemDb::SetRealData(regStart + 33, tmp, false); //最大放电电流
        tmp = (short)values[65] * 0.1;
        MemDb::SetRealData(regStart + 34, tmp, false); //预充最大电流
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    //读取系统状态
    start  = 53010;
    length = 1;
    ret    = client.ReadRs485OutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm  = true;
        data  = (short)values[0];
        value = Util::GetBitValue(data, 0);
        MemDb::SetRealData(regStart + 40, value, false); //运行状态
        value = Util::GetBitValue(data, 2);
        if (value == 1) {
            MemDb::SetRealData(regStart + 42, 0, false); //并网
        }

        value = Util::GetBitValue(data, 3);
        if (value == 1) {
            MemDb::SetRealData(regStart + 42, 1, false); //离网
        }
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    //读取系统状态
    start  = 53000;
    length = 10;
    ret    = client.ReadRs485OutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        data = (short)values[0];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault1, i, value);
        }

        data = (short)values[1];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault1, i + 16, value);
        }

        data = (short)values[2];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault2, i, value);
        }

        data = (short)values[3];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault2, i + 16, value);
        }

        data = (short)values[4];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault3, i, value);
        }

        data = (short)values[5];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault3, i + 16, value);
        }

        data = (short)values[6];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault4, i, value);
        }

        data = (short)values[7];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault4, i + 16, value);
        }

        data = (short)values[8];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault5, i, value);
        }

        data = (short)values[9];
        for (int i = 0; i < 16; i++) {
            value = Util::GetBitValue(data, i);
            Util::SetBitValue(fault5, i + 16, value);
        }

        if ((fault1 != 0) || (fault2 != 0) || (fault3 != 0) || (fault4 != 0) ||
            (fault5 != 0)) {
            MemDb::SetRealData(regStart + 43, 1, false);      //故障状态
            MemDb::SetRealData(regStart + 44, fault1, false); //故障码1
            MemDb::SetRealData(regStart + 45, fault2, false); //故障码2
            MemDb::SetRealData(regStart + 46, fault3, false); //故障码3
            MemDb::SetRealData(regStart + 47, fault4, false); //故障码4
            MemDb::SetRealData(regStart + 48, fault5, false); //故障码5
        } else {
            MemDb::SetRealData(regStart + 43, 0, false);      //故障状态
            MemDb::SetRealData(regStart + 44, fault1, false); //故障码1
            MemDb::SetRealData(regStart + 45, fault2, false); //故障码2
            MemDb::SetRealData(regStart + 46, fault3, false); //故障码3
            MemDb::SetRealData(regStart + 47, fault4, false); //故障码4
            MemDb::SetRealData(regStart + 48, fault5, false); //故障码5
        }
    } else {
        zlog_warn(Util::m_zlog, "读取%s设备，地址=%d数据失败",
                  dev->name.c_str(), start);
    }

    MemDb::SetRealData(regStart + 0, comm, false); //通讯状态
    return ret;
}

int SinexcelPcs::SinexcelTcpPcsPreset(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    RealData realData;
    int tmp = 0;

    //判断串口是否打开
    if (!m_tcp->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusTcpClient client;
    client.SetTcpClient(this->m_tcp);

    //判断是否有启动停止设置
    if (MemDb::GetRealDataWrited(realData, regStart + 1, true)) {
        if (realData.pv.data.iValue == 1) //启动
        {
            ret = client.PresetRS485SingleRegister(addr, 53900, 1);
        } else if (realData.pv.data.iValue == 0) //停止
        {
            ret = client.PresetRS485SingleRegister(addr, 53901, 1);
        }
    }

    //判断是否有变流器故障清除
    if (MemDb::GetRealDataWrited(realData, regStart + 5, true)) {
        ret = client.PresetRS485SingleRegister(
            addr, 53903, realData.pv.data.iValue); //故障清除
    }

    //判断是否有负载故障清除
    if (MemDb::GetRealDataWrited(realData, regStart + 6, true)) {
        ret = client.PresetRS485SingleRegister(
            addr, 54901, realData.pv.data.iValue); //故障清除
    }

    //判断是否有并离网
    if (MemDb::GetRealDataWrited(realData, regStart + 4, true)) {
        ret = client.PresetRS485SingleRegister(
            addr, 54900, realData.pv.data.iValue); //并离网
    }

    //判断是否有调度方式
    if (MemDb::GetRealDataWrited(realData, regStart + 2, true)) {
        ret = client.PresetRS485SingleRegister(
            addr, 53601, realData.pv.data.iValue); //调度方式
    }

    //判断是否有控制模式
    if (MemDb::GetRealDataWrited(realData, regStart + 3, true)) {
        ret = client.PresetRS485SingleRegister(
            addr, 53620, realData.pv.data.iValue); //控制模式
    }

    //判断是否有电网恢复延时
    if (MemDb::GetRealDataWrited(realData, regStart + 7, true)) {
        ret = client.PresetRS485SingleRegister(
            addr, 53627, realData.pv.data.iValue); //电网恢复延时
    }

    //判断是否有功率因数
    if (MemDb::GetRealDataWrited(realData, regStart + 11, true)) {
        tmp = realData.pv.data.dbValue * 100;
        ret = client.PresetRS485SingleRegister(addr, 53621, tmp); //功率因数
    }

    //判断是否有有功功率
    if (MemDb::GetRealDataWrited(realData, regStart + 12, true)) {
        tmp = realData.pv.data.dbValue * 0.001 * 10;
        ret = client.PresetRS485SingleRegister(addr, 53622, tmp); //有功功率
    }

    //判断是否有无功功率
    if (MemDb::GetRealDataWrited(realData, regStart + 13, true)) {
        tmp = realData.pv.data.dbValue * 0.001 * 10;
        ret = client.PresetRS485SingleRegister(addr, 53623, tmp); //无功功率
    }

    //判断是否有有功功率变化速率
    if (MemDb::GetRealDataWrited(realData, regStart + 14, true)) {
        tmp = realData.pv.data.dbValue * 0.001 * 10;
        ret = client.PresetRS485SingleRegister(addr, 53624,
                                               tmp); //有功功率变化速率
    }

    //判断是否有无功变化速率
    if (MemDb::GetRealDataWrited(realData, regStart + 15, true)) {
        tmp = realData.pv.data.dbValue * 0.001 * 10;
        ret = client.PresetRS485SingleRegister(addr, 53625, tmp); //无功变化速率
    }

    //判断是否有功率变化模式
    if (MemDb::GetRealDataWrited(realData, regStart + 16, true)) {
        ret = client.PresetRS485SingleRegister(
            addr, 53626, realData.pv.data.iValue); //电网恢复延时
    }

    //判断是否有离网电压范围
    if (MemDb::GetRealDataWrited(realData, regStart + 17, true)) {
        tmp = realData.pv.data.dbValue * 100;
        ret = client.PresetRS485SingleRegister(addr, 53628, tmp); //离网电压范围
    }

    //判断是否有离网电压频率
    if (MemDb::GetRealDataWrited(realData, regStart + 18, true)) {
        tmp = realData.pv.data.dbValue * 100;
        ret = client.PresetRS485SingleRegister(addr, 53629, tmp); //离网电压频率
    }

    //判断是否直流控制模式
    if (MemDb::GetRealDataWrited(realData, regStart + 19, true)) {
        ret = client.PresetRS485SingleRegister(
            addr, 53650, realData.pv.data.iValue); //直流控制模式
    }

    //判断是否有直流充放电电流
    if (MemDb::GetRealDataWrited(realData, regStart + 20, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret =
            client.PresetRS485SingleRegister(addr, 53651, tmp); //直流充放电电流
    }

    //判断是否有直流充放电功率
    if (MemDb::GetRealDataWrited(realData, regStart + 21, true)) {
        tmp = realData.pv.data.dbValue * 10 * 0.001;
        ret =
            client.PresetRS485SingleRegister(addr, 53652, tmp); //直流充放电功率
    }

    //判断是否有电池EOD电压
    if (MemDb::GetRealDataWrited(realData, regStart + 22, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53653, tmp); //电池EOD电压
    }

    //判断是否有放电起始电压
    if (MemDb::GetRealDataWrited(realData, regStart + 23, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53654, tmp); //放电起始电压
    }

    //判断是否有放电终止电压
    if (MemDb::GetRealDataWrited(realData, regStart + 24, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53655, tmp); //放电终止电压
    }

    //判断是否有预充电电压
    if (MemDb::GetRealDataWrited(realData, regStart + 25, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53656, tmp); //预充电电压
    }

    //判断是否有预充转快充电压
    if (MemDb::GetRealDataWrited(realData, regStart + 26, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret =
            client.PresetRS485SingleRegister(addr, 53657, tmp); //预充转快充电压
    }

    //判断是否有预充电时间
    if (MemDb::GetRealDataWrited(realData, regStart + 27, true)) {
        tmp = realData.pv.data.dbValue;
        ret = client.PresetRS485SingleRegister(addr, 53658, tmp); //预充电时间
    }

    //判断是否有电池浮充电压
    if (MemDb::GetRealDataWrited(realData, regStart + 28, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53659, tmp); //电池浮充电压
    }

    //判断是否有电池均充电压
    if (MemDb::GetRealDataWrited(realData, regStart + 29, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53660, tmp); //电池均充电压
    }

    //判断是否有均充转浮充电流
    if (MemDb::GetRealDataWrited(realData, regStart + 30, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret =
            client.PresetRS485SingleRegister(addr, 53661, tmp); //均充转浮充电流
    }

    //判断是否有充电截止电流
    if (MemDb::GetRealDataWrited(realData, regStart + 31, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53662, tmp); //充电截止电流
    }

    //判断是否有最大充电电流
    if (MemDb::GetRealDataWrited(realData, regStart + 32, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53663, tmp); //最大充电电流
    }

    //判断是否有最大放电电流
    if (MemDb::GetRealDataWrited(realData, regStart + 33, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53664, tmp); //最大放电电流
    }

    //判断是否有预充最大电流
    if (MemDb::GetRealDataWrited(realData, regStart + 34, true)) {
        tmp = realData.pv.data.dbValue * 10;
        ret = client.PresetRS485SingleRegister(addr, 53665, tmp); //预充最大电流
    }
    return ret;
}
