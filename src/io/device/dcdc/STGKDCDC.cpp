/*
 * STGKDCDC.cpp
 *
 *  Created on: 2015-12-4
 *      Author: liuhaitao
 */

#include "STGKDCDC.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"


STGKDCDC::STGKDCDC()
{
	// TODO Auto-generated constructor stub
	m_isTcp = false;
	m_com = NULL;
	m_tcpClient = NULL;
}

STGKDCDC::~STGKDCDC()
{
	// TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void STGKDCDC::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

void STGKDCDC::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void STGKDCDC::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int STGKDCDC::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	if(NULL==dev)
	{
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PV_DCDC://光伏DC/DC
		ret = this->STGkPvDcDcRead(dev);
		break;
	case DevModel::Model_BATT_DCDC://储能DC/DC
		ret = this->STGkBattDcDcRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int STGKDCDC::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	if(NULL==dev)
	{
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PV_DCDC://光伏DC/DC
		ret = this->STGkPvDcDcPreset(dev);
		break;
	case DevModel::Model_BATT_DCDC://储能DC/DC
		ret = this->STGkBattDcDcPreset(dev);
		break;
	default:
		break;
	};

	return ret;

}

int STGKDCDC::STGkBattDcDcRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 26;
	int data = 0;
	float tmp = 0.0;
	int value = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();

    ret = rtu.ReadInputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
        comm = true;
        data = (short) values[0];
        MemDb::SetRealData(regStart + 1, data, false);//运行命令设置反馈
        data = (short) values[1];
        MemDb::SetRealData(regStart + 2, data, false);//工作模式反馈
        data = (short) values[2];
        MemDb::SetRealData(regStart + 3, data, false);//故障清除设置反馈
        tmp = ((short)values[5]) * 0.1;
        MemDb::SetRealData(regStart + 6, tmp, false);//高压侧电压设定值
        tmp = (short)values[6] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 7, tmp, false);//高压侧功率设定值
        tmp = (short)values[7] * 0.1;
        MemDb::SetRealData(regStart + 8, tmp, false);//高压侧电流设定值
        tmp = (short)values[8] * 0.1;
        MemDb::SetRealData(regStart + 9, tmp, false);//高压侧放电基准电流
        tmp = (short)values[9] * 0.1;
        MemDb::SetRealData(regStart + 10, tmp, false);//高压侧充电基准电流
        data = (short)values[12];
        MemDb::SetRealData(regStart + 13, data, false);//运行状态
        data = (short)values[13];
        MemDb::SetRealData(regStart + 14, data, false);//工作状态
        data = (short)values[16];
        MemDb::SetRealData(regStart + 17, data, false);//工作模式
        tmp = (short)values[20] * 0.1;
        MemDb::SetRealData(regStart + 21, tmp, false);//直流侧电压
        tmp = (short)values[21] * 0.1;
        MemDb::SetRealData(regStart + 22, tmp, false);//直流侧电流
        tmp = (short)values[22] * 100;
        MemDb::SetRealData(regStart + 23, tmp, false);//直流侧功率
        tmp = (short)values[23] * 0.1;
        MemDb::SetRealData(regStart + 24, tmp, false);//低压侧电压
        tmp = (short)values[24] * 0.1;
        MemDb::SetRealData(regStart + 25, tmp, false);//低压侧电流
        tmp = (short)values[25] * 100;
        MemDb::SetRealData(regStart + 26, tmp, false);//低压侧功率

        data = (((short)values[15]) << 16) | ((short)values[14]);
        if (data > 0)
        {
            MemDb::SetRealData(regStart + 15, 1, false);//故障
            MemDb::SetRealData(regStart + 16, data, false);//故障码
        }
        else
        {
            MemDb::SetRealData(regStart + 15, 0, false);//无故障
            MemDb::SetRealData(regStart + 16, data, false);//故障码
        }
    }
    else
    {
        comm = false;
        zlog_warn(Util::m_zlog,"读取%s数据失败",dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

    return ret;
}

int STGKDCDC::STGkBattDcDcPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int data = 0;
	RealData realData;
	UshortArray values;
	values.clear();

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

    //判断是否有启动停止命令下发
    if (MemDb::GetRealData(realData, regStart + 1, true))
    {
        ret = rtu.PresetSingleRegister(addr, 0, realData.pv.data.iValue);
    }

    //判断是否有工作模式命令下发
    if (MemDb::GetRealData(realData, regStart + 2, true))
    {
        ret = rtu.PresetSingleRegister(addr, 1, realData.pv.data.iValue);
    }

    //判断是否有故障清除命令下发
    if (MemDb::GetRealData(realData, regStart + 3, true))
    {
        ret = rtu.PresetSingleRegister(addr, 2, realData.pv.data.iValue);
    }

    //判断是否有高压侧电压设定值下发
    if (MemDb::GetRealData(realData, regStart + 6, true))
    {
        data = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 5, data);
    }

    //判断是否有高压侧功率设定值下发
    if (MemDb::GetRealData(realData, regStart + 7, true))
    {
        data = (int)(realData.pv.data.dbValue * 10 * 0.001);
        ret = rtu.PresetSingleRegister(addr, 6, data);
    }

    //判断是否有高压侧电流设定值下发
    if (MemDb::GetRealData(realData, regStart + 8, true))
    {
        data = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 7, data);
    }

    //判断是否有高压侧放电基准电流设定值下发
    if (MemDb::GetRealData(realData, regStart + 9, true))
    {
        data = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 8, data);
    }

    //判断是否有高压侧充电基准电流设定值下发
    if (MemDb::GetRealData(realData, regStart + 10, true))
    {
        data = realData.pv.data.dbValue * 10;
        ret = rtu.PresetSingleRegister(addr, 9, data);
    }

    return ret;
}

int STGKDCDC::STGkPvDcDcRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 26;
	int data = 0;
	float tmp = 0.0;
	int value = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();

    ret = rtu.ReadInputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
        comm = true;
        data = (short) values[0];
        MemDb::SetRealData(regStart + 1, data, false);//运行命令设置反馈
        data = (short) values[2];
        MemDb::SetRealData(regStart + 3, data, false);//故障清除设置反馈
        tmp = (short) values[5] * 0.1 * 1000;
        MemDb::SetRealData(regStart + 6, tmp, false);//功率限值
        data = (short) values[12];
        MemDb::SetRealData(regStart + 13, data, false);//运行状态
        data = (short) values[13];
        MemDb::SetRealData(regStart + 14, data, false);//工作状态
        tmp = (short) values[20] * 0.1;
        MemDb::SetRealData(regStart + 21, tmp, false);//直流侧电压
        tmp = (short) values[21] * 0.1;
        MemDb::SetRealData(regStart + 22, tmp, false);//直流侧电流
        tmp = (short) values[22] * 100;
        MemDb::SetRealData(regStart + 23, tmp, false);//直流侧功率
        tmp = (short) values[23] * 0.1;
        MemDb::SetRealData(regStart + 24, tmp, false);//低压侧电压
        tmp = (short) values[24] * 0.1;
        MemDb::SetRealData(regStart + 25, tmp, false);//低压侧电流
        tmp = (short) values[25] * 100;
        MemDb::SetRealData(regStart + 26, tmp, false);//低压侧功率

        data = (((short)values[15]) << 16) | ((short)values[14]);
        if (data > 0)
        {
            MemDb::SetRealData(regStart + 15, 1, false);//故障
            MemDb::SetRealData(regStart + 16, data, false);//故障码
            zlog_warn(Util::m_zlog,"故障码 = %d",data);
        }
        else
        {
            MemDb::SetRealData(regStart + 15, 0, false);//无故障
            MemDb::SetRealData(regStart + 16, data, false);//故障码
        }
    }
    else
    {
        comm = false;
        zlog_warn(Util::m_zlog,"读取%s数据失败",dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int STGKDCDC::STGkPvDcDcPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int data = 0;
	RealData realData;
	UshortArray values;
	values.clear();

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

    //判断是否有启动停止命令下发
    if (MemDb::GetRealData(realData, regStart + 1, true))
    {
        ret = rtu.PresetSingleRegister(addr, 0, realData.pv.data.iValue);
    }

    //判断是否有故障清除命令下发
    if (MemDb::GetRealData(realData, regStart + 3, true))
    {
        ret = rtu.PresetSingleRegister(addr, 2, realData.pv.data.iValue);
    }

    //判断是否有功率限值下发
    if (MemDb::GetRealData(realData, regStart + 5, true))
    {
        data = realData.pv.data.dbValue * 10 * 0.001;
        ret = rtu.PresetSingleRegister(addr, 5, data);
    }

    return ret;
}
