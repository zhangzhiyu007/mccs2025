/*
 * TaikaiPcms.cpp
 *
 *  创建日期: 2019-7-31
 *  作          者: 刘海涛
 */

#include "TaikaiPcms.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

TaikaiPcms::TaikaiPcms()
{
	// TODO Auto-generated constructor stub
	m_isTcp = false;
	m_com = NULL;
	m_tcpClient = NULL;
}

TaikaiPcms::~TaikaiPcms()
{
	// TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void TaikaiPcms::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

void TaikaiPcms::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void TaikaiPcms::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int TaikaiPcms::Read(Device::SlaveDev* dev)
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
	case DevModel::Model_F05012:
		ret = this->TaiKaiPcmsRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int TaikaiPcms::Preset(Device::SlaveDev* dev)
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
	case DevModel::Model_F05012:
		ret = this->TaiKaiPcmsPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int TaikaiPcms::TaiKaiPcmsRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;

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
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 0;
	int data = 0;
	float tmp = 0.0;

	start = 0;
	length = 10;
	//读取实时数据
	ret = rtu.ReadOutputRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		tmp = ((short) values[0]) * 0.1 - 30;
		MemDb::SetRealData(regStart + 28, tmp, false);//温度1
		tmp = ((short) values[1]) * 0.1 - 30;
		MemDb::SetRealData(regStart + 29, tmp, false);//温度2
		tmp = ((short) values[2]) * 0.1 - 30;
		MemDb::SetRealData(regStart + 30, tmp, false);//温度3

		data = 0;
		for(int i = 0; i < 5; i++)
		{
			Util::SetBitValue(data, i, Util::GetBitValue(values[8], i));
		}
		MemDb::SetRealData(regStart + 26, data, false);//强控开关状态

		data = 0;
		for(int i = 0; i < 5; i++)
		{
			Util::SetBitValue(data, i, Util::GetBitValue(values[9], i));
		}

		MemDb::SetRealData(regStart + 27, data, false);//输出状态
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取%s实时数据失败",dev->name.c_str());
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int TaikaiPcms::TaiKaiPcmsPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	RealData realData;
	UshortArray values;
	values.clear();
	int data = 0;
	float tmp = 0;

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

    //判断是否有充热时间段1起始小时值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 2, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 20, data);
    }

    //判断是否有充热时间段1起始分钟值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 3, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 21, data);
    }

    //判断是否有充热时间段1结束小时值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 4, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 22, data);
    }

    //判断是否有充热时间段1结束分钟值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 5, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 23, data);
    }

    //判断是否有充热时间段2起始小时值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 6, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 24, data);
    }

    //判断是否有充热时间段2起始分钟值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 7, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 25, data);
    }

    //判断是否有充热时间段2结束小时值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 8, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 26, data);
    }

    //判断是否有充热时间段2结束分钟值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 9, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 27, data);
    }

    //判断是否有放热时间段1起始小时值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 10, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 28, data);
    }

    //判断是否有放热时间段1起始分钟值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 11, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 29, data);
    }

    //判断是否有放热时间段1结束小时值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 12, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 30, data);
    }

    //判断是否有放热时间段1结束分钟值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 13, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 31, data);
    }

    //判断是否有放热时间段2起始小时值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 14, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 32, data);
    }

    //判断是否有放热时间段2起始分钟值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 15, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 33, data);
    }

    //判断是否有放热时间段2结束小时值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 16, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 34, data);
    }

    //判断是否有放热时间段2结束分钟值设置命令下发
    if (MemDb::GetRealData(realData, regStart + 17, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 35, data);
    }

    //判断是否有加热器延时启动时间（秒）设置命令下发
    if (MemDb::GetRealData(realData, regStart + 18, true))
    {
        data = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, 38, data);
    }

    //判断是否有冲热启动温度设置命令下发
    if (MemDb::GetRealData(realData, regStart + 19, true))
    {
        data = realData.pv.data.dbValue;
        ret = rtu.PresetSingleRegister(addr, 36, data);
    }

    //判断是否有冲热停止温度设置命令下发
    if (MemDb::GetRealData(realData, regStart + 20, true))
    {
        data = realData.pv.data.dbValue;
        ret = rtu.PresetSingleRegister(addr, 37, data);
    }

    //判断是否有放热启动温度设置命令下发
    if (MemDb::GetRealData(realData, regStart + 21, true))
    {
        data = realData.pv.data.dbValue;
        ret = rtu.PresetSingleRegister(addr, 39, data);
    }

    //判断是否有放热停止温度设置命令下发
    if (MemDb::GetRealData(realData, regStart + 22, true))
    {
        data = realData.pv.data.dbValue;
        ret = rtu.PresetSingleRegister(addr, 40, data);
    }

    //判断是否有供暖启动温度设置命令下发
    if (MemDb::GetRealData(realData, regStart + 23, true))
    {
        data = realData.pv.data.dbValue;
        ret = rtu.PresetSingleRegister(addr, 41, data);
    }

    //判断是否有供暖停止温度设置命令下发
    if (MemDb::GetRealData(realData, regStart + 24, true))
    {
        data = realData.pv.data.dbValue;
        ret = rtu.PresetSingleRegister(addr, 42, data);
    }
	return ret;
}
