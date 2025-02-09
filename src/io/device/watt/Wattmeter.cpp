/*
 * Wattmeter.cpp
 *
 *   创建日期: 2013-6-24
 *   作         者: mzh
 */

#include "Wattmeter.h"
#include "../../../db/MemDb.h"
#include "../../protocol/dlt645/Dlt645.h"
#include "../../protocol/dlt645/Dlt645_1997.h"
#include "../../protocol/dlt645/Dlt645_2007.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "Acrel.h"
#include "HcWatt.h"
#include "TaikaiWatt.h"

Wattmeter::Wattmeter() {
    // TODO 构造函数
    m_com       = NULL;
    m_tcpClient = NULL;
    m_isTcp     = false;
}

Wattmeter::~Wattmeter() {
    // TODO 析构函数
}

//设置是否采用以太网通讯
void Wattmeter::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

void Wattmeter::SetCom(Com *com) { m_com = com; }

//设置TcpClient指针
void Wattmeter::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int Wattmeter::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_WASION: //威盛
        ret = Dlt645_1997Read(dev);
        break;
    case DevManu::Manu_HC: //惠测
        ret = hcRead(dev);
        break;
    case DevManu::Manu_TAIKAI: 
        ret = TaikaiRead(dev);
        break;
    case DevManu::Manu_ACREL:
        ret = AcrelRead(dev);
        break;
    default:
        break;
    }

    return ret;
}

int Wattmeter::hcRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    HcWatt hcwatt;
    hcwatt.SetIsTcp(m_isTcp);
    hcwatt.SetTcp(m_tcpClient);
    ret = hcwatt.Read(dev);
    return ret;
}

int Wattmeter::TaikaiRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    TaikaiWatt watt;
    watt.SetTcp(m_tcpClient);
    ret = watt.Read(dev);
    return ret;
}

int Wattmeter::AcrelRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    Acrel meter;

    meter.SetTcp(m_tcpClient);

    ret = meter.Read(dev);

    return ret;
}

int Wattmeter::Dlt645_1997Read(Device::SlaveDev *dev) {
    zlog_info(Util::m_zlog, "读取7758数据");
    bool comm = false;
    int ret   = ErrorInfo::ERR_OK;
    UshortArray data;
    data.clear();

    if (NULL == dev) {
        return ErrorInfo::ERR_NULL;
    }

    Dlt645_1997 dlt;
    dlt.SetCom(m_com);
    //	dlt.SetIsTcp(m_isTcp);
    //	dlt.SetTcp(m_tcpClient);
    dlt.SetLeadBytesResp(m_leadBytesResp);

    UshortArray values;
    values.clear();

    Dlt645::DataTemp *dataTemp = NULL;
    int tmp                    = 0;
    int tmp1                   = 0;
    float tmp2                 = 0;
    float tmp3                 = 0;
    unsigned int i             = 0;
    PtrArray array;
    array.clear();
    for (i = 0; i < ARRAY_LENGTH_128; i++) {
        dataTemp = new Dlt645::DataTemp();
        array.push_back(dataTemp);
    }

    int regStart = dev->regStart + 0;
    // 1、读取电表数据
    ret = dlt.ReadRegisters(array, dev->slaveAddr);
    if (ErrorInfo::ERR_OK == ret) {
        //对数据进行处理,增加变比
        //频率2

        //电压3-5，原始V，转换为0.1V，乘以变比
        for (i = 3; i < 6; i++) {
            dataTemp = (Dlt645::DataTemp *)array[i];
            if (dataTemp->flag) {
                dataTemp->data = dataTemp->data * 10;
                tmp2           = dataTemp->data * 0.1;
                zlog_debug(Util::m_zlog, "电压原始值=%.2f,实际值 = %.2f",
                           dataTemp->data, tmp2);
                MemDb::SetRealData(regStart + i + 0, tmp2, false);
            }
        }

        //电流6-8，原始0.01A
        for (i = 6; i < 9; i++) {
            dataTemp = (Dlt645::DataTemp *)array[i];
            if (dataTemp->flag) {
                tmp2 = dataTemp->data * dev->rate * 0.01;
                zlog_debug(Util::m_zlog, "电流原始值=%.2f,实际值 = %.2f",
                           dataTemp->data, tmp2);
                MemDb::SetRealData(regStart + i + 0, tmp2, false);
            }
        }

        //有功、9-12，原始0.0001kw
        for (i = 9; i < 12; i++) {
            dataTemp = (Dlt645::DataTemp *)array[i];
            if (dataTemp->flag) {
                tmp2 = dataTemp->data * dev->rate * 0.0001 * 1000;
                tmp3 += tmp2;
                zlog_debug(Util::m_zlog, "有功原始值=%.2f,实际值 = %.2f",
                           dataTemp->data, tmp2);
                MemDb::SetRealData(regStart + i + 0, tmp2, false);
            }
        }
        MemDb::SetRealData(regStart + 12, tmp3, false); //总有功功率

        //无功13-16，原始0.01kvar
        tmp3 = 0;
        for (i = 13; i < 16; i++) {
            dataTemp = (Dlt645::DataTemp *)array[i];
            if (dataTemp->flag) {
                tmp2 = dataTemp->data * dev->rate * 0.01 * 1000;
                tmp3 += tmp2;
                zlog_debug(Util::m_zlog, "无功功原始值=%.2f,实际值 = %.2f",
                           dataTemp->data, tmp2);
                MemDb::SetRealData(regStart + i + 0, tmp2, false);
            }
        }
        MemDb::SetRealData(regStart + 16, tmp3, false); //总无功功率

        //视在功率17-20
        tmp3 = 0;
        for (i = 17; i < 20; i++) {
            dataTemp = (Dlt645::DataTemp *)array[i];
            if (dataTemp->flag) {
                tmp2 = dataTemp->data * dev->rate * 0.01 * 1000;
                tmp3 += tmp2;
                zlog_debug(Util::m_zlog, "无功功原始值=%.2f,实际值 = %.2f",
                           dataTemp->data, tmp2);
                MemDb::SetRealData(regStart + i + 0, tmp2, false);
            }
        }
        MemDb::SetRealData(regStart + 20, tmp3, false); //总视在功率

        //系统功率因数PF,原始0.001,转换为0.001
        dataTemp = (Dlt645::DataTemp *)array[21];
        if (dataTemp->flag) {
            tmp2 = dataTemp->data * 0.001;
            MemDb::SetRealData(regStart + 25, tmp2, false);
            zlog_debug(Util::m_zlog, "功率因数：读取=%d,码值=%d，实际值= %.2f",
                       dataTemp->flag, dataTemp->data, tmp2);
        }

        //正向、反向有功无功电能21-24，原始0.01kwh
        for (i = 26; i < 30; i++) {
            dataTemp = (Dlt645::DataTemp *)array[i];
            if (dataTemp->flag) {
                tmp2 = dataTemp->data * dev->rate * 0.01;
                MemDb::SetRealData(regStart + i + 0 - 5, tmp2, false);
                zlog_debug(Util::m_zlog, "读取=%d,有功=%d", dataTemp->flag,
                           dataTemp->data);
            }
        }

        //保存通讯正常
        comm = true;
        //		for(i=0;i<26;i++)
        //		{
        //			dataTemp = (Dlt645::DataTemp*) array[i];
        //			if (dataTemp->flag)
        //			{
        //				tmp1 = dataTemp->data;
        //				zlog_info(Util::m_zlog,"%d ==
        //%d",i,tmp1); 				MemDb::SetRealData(regStart +
        // i+0, tmp1, false);
        //			}
        //		}
        //
        //		for (i = 0; i < 4; i++)
        //		{
        //			dataTemp = (Dlt645::DataTemp*) array[26 + i];
        //			if (dataTemp->flag)
        //			{
        //				tmp = regStart + 2* i + 26;
        //				tmp1 = dataTemp->data;
        //				MemDb::SetRealData(tmp, (tmp1 >> 16),
        // false); 				MemDb::SetRealData(tmp + 1, (tmp1
        // & 0xFFFF), false);
        //			}
        //		}
    }

    //清除内存
    for (i = 0; i < array.size(); i++) {
        dataTemp = (Dlt645::DataTemp *)array[i];
        delete dataTemp;
    }
    array.clear();

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}
