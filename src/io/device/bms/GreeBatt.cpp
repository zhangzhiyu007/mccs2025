/*
 * GreeBatt.cpp
 *
 *  创建日期: 2019-7-31
 *  作          者: 刘海涛
 */

#include "GreeBatt.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

GreeBatt::GreeBatt()
{
	// TODO Auto-generated constructor stub
	m_isTcp = false;
	m_com = NULL;
	m_tcpClient = NULL;
}

GreeBatt::~GreeBatt()
{
	// TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void GreeBatt::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

void GreeBatt::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void GreeBatt::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int GreeBatt::Read(Device::SlaveDev* dev)
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
	case DevModel::Model_GREEBATT://格力储能
		ret = this->GreeBattRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int GreeBatt::Preset(Device::SlaveDev* dev)
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
	case DevModel::Model_GREEBATT://格力储能
		ret = this->GreeBattPreset(dev);
		break;
	default:
		break;
	};

	return ret;

}

int GreeBatt::GreeBattRead(Device::SlaveDev* dev)
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

	start = 4;
	length = 12;
	//读取实时数据
	ret = rtu.ReadOutputRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		tmp = ((short) values[0]) * 1.0;
		MemDb::SetRealData(regStart + 9, tmp, false);//母线电压
		tmp = ((short) values[1]) * 1.0;
		MemDb::SetRealData(regStart + 10, tmp, false);//电池电压
		tmp = ((short) values[2]) * 0.01;
		MemDb::SetRealData(regStart + 11, tmp, false);//电感电流
		tmp = ((short) values[3]) * 0.01;
		MemDb::SetRealData(regStart + 12, tmp, false);//电感电流
		tmp = ((short) values[4]) * 1.0;
		MemDb::SetRealData(regStart + 13, tmp, false);//充电功率
		tmp = ((short) values[5]) * 1.0;
		MemDb::SetRealData(regStart + 14, tmp, false);//放电功率
    	data = (((short)values[6]) * 65536) + ((short)values[7]);
    	memcpy(&tmp,&data,sizeof(int));
		MemDb::SetRealData(regStart + 16, tmp, false);//充电电量
    	data = (((short)values[8]) * 65536) + ((short)values[9]);
    	memcpy(&tmp,&data,sizeof(int));
		MemDb::SetRealData(regStart + 17, tmp, false);//放电电量
		tmp = ((short) values[10]) * 1.0;
		MemDb::SetRealData(regStart + 15, tmp, false);//SOC
		data = ((short) values[11]);
		MemDb::SetRealData(regStart + 8, data, false);//当期状态
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取%s实时数据失败",dev->name.c_str());
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int GreeBatt::GreeBattPreset(Device::SlaveDev* dev)
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

    //判断是否有充电指令设置命令下发
    if (MemDb::GetRealData(realData, regStart + 1, true))
    {
        data = realData.pv.data.iValue;
        values.push_back(data);
        ret = rtu.PresetMultipleRegisters(addr, 3, 1, values);
    }

    //判断是否有充放电目标电流设置命令下发
    if (MemDb::GetRealData(realData, regStart + 2, true))
    {
        tmp = realData.pv.data.dbValue;
        data = tmp * 100;
        values.push_back(data);
        ret = rtu.PresetMultipleRegisters(addr, 0, 1, values);
    }

    //判断是否有充电限制电流设置命令下发
    if (MemDb::GetRealData(realData, regStart + 3, true))
    {
        tmp = realData.pv.data.dbValue;
        data = tmp * 100;
        values.push_back(data);
        ret = rtu.PresetMultipleRegisters(addr, 1, 1, values);
    }

    //判断是否有放电限制电流设置命令下发
    if (MemDb::GetRealData(realData, regStart + 4, true))
    {
        tmp = realData.pv.data.dbValue;
        data = tmp * 100;
        values.push_back(data);
        ret = rtu.PresetMultipleRegisters(addr, 2, 1, values);
    }

	return ret;
}
