/*
 * EcmPcs.cpp
 *
 *  Created on: 2019-4-24
 *      Author: zl
 */

#include "EcmHFIPcs.h"
// #include "../../../ctrl/sys/CtrlMccs.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusTcpClient.h"

EcmHFIPcs::EcmHFIPcs()
{
    // TODO Auto-generated constructor stub
    m_isTcp = false;
    m_com = NULL;
    m_tcpClient = NULL;
    m_state = 0;
}

EcmHFIPcs::~EcmHFIPcs()
{
    // TODO Auto-generated destructor stub
}

// 设置是否采用以太网通讯
void EcmHFIPcs::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

void EcmHFIPcs::SetCom(Com *com) { m_com = com; }

// 设置TcpClient指针
void EcmHFIPcs::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int EcmHFIPcs::Read(Device::SlaveDev *dev)
{
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev)
    {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devModel)
    {
    case DevModel::Model_ECM_HFI_DCAC: // PCS
        ret = this->EcmHFIPcsRead(dev);
        break;
    default:
        break;
    };

    return ret;
}

int EcmHFIPcs::Preset(Device::SlaveDev *dev)
{
    int ret = ErrorInfo::ERR_OK;
    if (NULL == dev)
    {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devModel)
    {
    case DevModel::Model_ECM_HFI_DCAC: // PCS
        ret = this->EcmHFIPcsPreset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int EcmHFIPcs::EcmHFIPcsRead(Device::SlaveDev *dev)
{
    int ret = ErrorInfo::ERR_OK;
    bool comm = false;
    int addr = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start = 0x0000;
    int length = 0;
    int data = 0;
    float f_data = 0.0;

    if (NULL == dev)
    {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    // 判断串口是否打开
    if (!m_tcpClient->IsOpened())
    {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusTcpClient client;
    client.SetTcpClient(this->m_tcpClient);

    UshortArray values;
    values.clear();

    // 读取实时数据
    start = 0x1001;
    length = 10;
    ret = client.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
        comm = true;
        data = (unsigned short)values[0];
        if (data>4)
        {
             MemDb::SetRealData(regStart + 1, data,false);
                                   // 读设备状态，4—短静置，5-停机，6-故障，
                                   // 10-软起，11-恒压运行，12-恒流运行，
                                   // 13-待机，14-离网逆变运行， 16-AC恒功率运行
        }
        
       
        if (data==13)
        {
            MemDb::SetRealData(regStart + 35, 1,false);
            MemDb::SetRealData(regStart + 36, 0,false);

        }
        else if (data==6)
        {
            MemDb::SetRealData(regStart + 36, 1,false);
            MemDb::SetRealData(regStart + 35, 0,false);

        }
        

        data = (unsigned short)values[1];
        MemDb::SetRealData(regStart + 2, data, false); // 告警代码
        data = (unsigned short)values[2];
        MemDb::SetRealData(regStart + 3, data, false); // 故障代码
        if (data == 268 || data == 269 || data == 270)
        {
            MemDb::SetRealData(regStart + 34, 1); //输出过流过流故障 
        }

        f_data = ((values[3] * 65536) + values[4]) * 0.001; // 直流电压
        MemDb::SetRealData(regStart + 4, f_data, false);

        f_data = ((values[5] * 65536) + values[6]) * 0.001;
        MemDb::SetRealData(regStart + 5, f_data, false); // 直流电流

        f_data = ((values[7] * 65536) + values[8]);
        MemDb::SetRealData(regStart + 6, f_data, false); // 直流功率

        // 读取实时数据
        start = 0x1016;
        length = 6;
        values.clear();
        ret = client.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]) * 0.001; // wh
        MemDb::SetRealData(regStart + 7, f_data, false);    // 充电累计

        f_data = ((values[4] * 65536) + values[5]) * 0.001; // wh
        MemDb::SetRealData(regStart + 8, f_data, false);    // 放电累计

        // 读取实时数据
        start = 0x1038;
        length = 20;
        values.clear();
        ret = client.ReadOutputRegisters(values, addr, start, length);

        f_data = ((values[0] * 65536) + values[1]) * 0.001;
        MemDb::SetRealData(regStart + 9, f_data, false); // 电网U相电压

        f_data = ((values[2] * 65536) + values[3]) * 0.001;
        MemDb::SetRealData(regStart + 10, f_data, false); // 电网V相电压

        f_data = ((values[4] * 65536) + values[5]) * 0.001;
        MemDb::SetRealData(regStart + 11, f_data, false); // 电网W相电压

        f_data = ((values[6] * 65536) + values[7]) * 0.001;
        MemDb::SetRealData(regStart + 12, f_data, false); // 电网U相电流

        f_data = ((values[8] * 65536) + values[9]) * 0.001;
        MemDb::SetRealData(regStart + 13, f_data, false); // 电网V相电流

        f_data = ((values[10] * 65536) + values[11]) * 0.001;
        MemDb::SetRealData(regStart + 14, f_data, false); // 电网W相电流

        f_data = ((values[12] * 65536) + values[13]);
        MemDb::SetRealData(regStart + 15, f_data, false); // 系统有功功率

        f_data = ((values[14] * 65536) + values[15]);
        MemDb::SetRealData(regStart + 16, f_data, false); // 系统无功功率

        f_data = ((values[16] * 65536) + values[17]);
        MemDb::SetRealData(regStart + 17, f_data, false); // 系统视在功率

        // 读取实时数据
        start = 0x104A;
        length = 20;
        values.clear();
        ret = client.ReadOutputRegisters(values, addr, start, length);

        f_data = ((values[0] * 65536) + values[1]) * 0.001;

        MemDb::SetRealData(regStart + 18, f_data, false); // 负载U相电压

        f_data = ((values[2] * 65536) + values[3]) * 0.001;

        MemDb::SetRealData(regStart + 19, f_data, false); // 负载V相电压

        f_data = ((values[4] * 65536) + values[5]) * 0.001;

        MemDb::SetRealData(regStart + 20, f_data, false); // 负载W相电压

        f_data = ((values[6] * 65536) + values[7]) * 0.001;

        MemDb::SetRealData(regStart + 21, f_data, false); // 负载U相电流

        f_data = ((values[8] * 65536) + values[9]) * 0.001;

        MemDb::SetRealData(regStart + 22, f_data, false); // 负载V相电流

        f_data = ((values[10] * 65536) + values[11]) * 0.001;

        MemDb::SetRealData(regStart + 23, f_data, false); // 负载W相电流

        f_data = ((values[12] * 65536) + values[13]);

        MemDb::SetRealData(regStart + 24, f_data, false); // 负载有功功率

        f_data = ((values[14] * 65536) + values[15]);

        MemDb::SetRealData(regStart + 25, f_data, false); // 负载无功功率

        f_data = ((values[16] * 65536) + values[17]);

        MemDb::SetRealData(regStart + 26, f_data, false); // 负载视在功率

        f_data = (short)values[18] * 0.001;

        MemDb::SetRealData(regStart + 27, f_data, false); // PF

        data = (unsigned short)values[19] * 0.01;
        MemDb::SetRealData(regStart + 28, data, false); // 频率

        // 读取实时数据
        start = 0x1119;
        length = 1;
        values.clear();
        ret = client.ReadOutputRegisters(values, addr, start, length);
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 29, f_data, false); // 母线电压

        // 读取设定的工作状态数据
        start = 0x2013;
        length = 2;
        values.clear();
        ret = client.ReadOutputRegisters(values, addr, start, length);
        data = ((unsigned short)(values[0] << 16) + (unsigned short)values[1]);
        MemDb::SetRealData(regStart + 30, data); // 读设定的工作模式：14

        // 读取设定的离网工作电压与频率
        start = 0x2015;
        length = 6;
        values.clear();
        ret = client.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] << 16) + values[1]) * 0.001;
        MemDb::SetRealData(regStart + 31, f_data); // 设定的离网电压
        f_data = ((values[2] << 16) + values[3]) * 0.001;
        MemDb::SetRealData(regStart + 32, f_data); // 设定的离网频率
        f_data = ((values[2] << 16) + values[5]) * 0.001;
        MemDb::SetRealData(regStart + 33, f_data); // 设定的离网频率
    }
    else
    {
        comm = false;
        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false); // 通讯状态

    return ret;
}

int EcmHFIPcs::EcmHFIPcsPreset(Device::SlaveDev *dev)
{
    int ret = ErrorInfo::ERR_OK;
    int addr = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    // int start = 0;
    // int length = 0;
    int data = 0;
    // int mode = 0;
    float tmp = 0.0;
    RealData realData;
    UshortArray values;
    FloatArray Fvalues;
    values.clear();
    Fvalues.clear();
    UshortArray array;
    array.clear();

    // printf("EcmHFIPcsPreset\n");
    // 判断串口是否打开
    if (!m_tcpClient->IsOpened())
    {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusTcpClient client;
    client.SetTcpClient(this->m_tcpClient);

    // 工作模式设定
    if (MemDb::GetRealData(realData, regStart + 40, true)) // 工作模式设定
    {
        values.clear();
        data = realData.pv.data.iValue;
        // INFO: 浙江电科院 用于DC恒压输出
        if (0x02 == data || 0x08 == data || 0x21 == data || 0x22 == data ||
            0x40 == data ||
            0x41 == data)
        { // 65独立逆变  独立逆变只要参数1和参数2

            values.clear();
            values.push_back(data >> 16);
            values.push_back(data & 0xFFFF);

            ret = client.PresetMultipleRegisters(addr, 0x2013, 2, values);
            zlog_warn(Util::m_zlog, "设置工作模式：%d",
                      (values[0] << 16) + values[1]);
        }
        else
        {
            zlog_warn(Util::m_zlog, "%s 设置模式错误，模式：%d",
                      dev->name.c_str(), data);
        }
    }

    if (MemDb::GetRealData(realData, regStart + 41, true)) // 设置参数1
    {
        data = (realData.pv.data.iValue * 1000) >> 16;
        values.clear();
        values.push_back(data);
        data = (realData.pv.data.iValue * 1000) & 0xFFFF;
        values.push_back(data);
        ret = client.PresetMultipleRegisters(addr, 0x2015, 2, values);
    }

    if (MemDb::GetRealData(realData, regStart + 42, true)) // 设置参数2
    {
        data = (realData.pv.data.iValue * 1000) >> 16;
        ret = client.PresetSingleRegister(addr, 0x2017, data);
        data = (realData.pv.data.iValue * 1000) & 0xFFFF;
        ret = client.PresetSingleRegister(addr, 0x2018, data);
    }

    if (MemDb::GetRealData(realData, regStart + 43, true)) // 设置参数3
    {
        data = (realData.pv.data.iValue * 1000) >> 16;
        ret = client.PresetSingleRegister(addr, 0x2019, data);
        data = (realData.pv.data.iValue * 1000) & 0xFFFF;
        ret = client.PresetSingleRegister(addr, 0x201a, data);
    }

    if (MemDb::GetRealData(realData, regStart + 44, true)) // 设置参数4
    {
        data = (realData.pv.data.iValue * 1000) >> 16;
        ret = client.PresetSingleRegister(addr, 0x201b, data);
        data = (realData.pv.data.iValue * 1000) & 0xFFFF;
        ret = client.PresetSingleRegister(addr, 0x201c, data);
    }

    if (MemDb::GetRealData(realData, regStart + 45,true)) // 启停命令 1启动 0 关闭
    {
        data = realData.pv.data.iValue;
        // printf("%d\n",data);
        // printf("%d---------------------------------------------------------------\n",data);
        // printf("%d\n",data);

        ret = client.PresetSingleRegister(addr, 0x201D, data);
    }

    if (MemDb::GetRealData(realData, regStart + 46,true)) // 故障状态清除 1-清除故障
    {
        data = realData.pv.data.iValue;
        ret = client.PresetSingleRegister(addr, 0x201e, data);
    }

    if (MemDb::GetRealData(realData, regStart + 47, true)) // 开机自启动标志
    {
        data = realData.pv.data.iValue;
        ret = client.PresetSingleRegister(addr, 0x201f, data);
    }

    if (MemDb::GetRealData(realData, regStart + 48, true)) // 设置直流最大电压
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x2020, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 49, true)) // 设置直流最小电压
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x2022, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 50, true)) // 设置最大充电电流
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x2024, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 51, true)) // 设置最大放电电流
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x2026, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 52, true)) // 设置最大充电功率
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x2028, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 53, true)) // 设置最大放电功率
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x202A, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 54, true)) // 设置交流电压上限
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x202C, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 55, true)) // 设置交流电压下限
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x202E, 2, Fvalues);
    }
    if (MemDb::GetRealData(realData, regStart + 56, true)) // 设置交流电流上限
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x2030, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 57, true)) // 设置交流频率上限
    {
        data = realData.pv.data.iValue;
        client.PresetSingleRegister(addr, 0x2034, data);
    }

    if (MemDb::GetRealData(realData, regStart + 58, true)) // 设置交流频率下限
    {
        data = realData.pv.data.iValue;
        client.PresetSingleRegister(addr, 0x2035, data);
    }

    if (MemDb::GetRealData(realData, regStart + 59, true)) // 设置直流母线电压
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x2800, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 60, true)) // 设置交流无功功率
    {
        Fvalues.clear();
        tmp = realData.pv.data.dbValue;
        Fvalues.push_back(tmp);
        ret = client.PresetRLCFloatRegisters(addr, 0x2802, 2, Fvalues);
    }

    if (MemDb::GetRealData(realData, regStart + 61,
                           true)) // 设置并离网切换模式 0:禁用，1：自动切换
    {
        data = realData.pv.data.iValue;
        client.PresetSingleRegister(addr, 0x3003, data);
    }

    return ret;
}
