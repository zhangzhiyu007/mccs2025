/*
 * AntiReflux.cpp
 *
 *  Created on: 2014-3-7
 *      Author: liuhaitao
 */

#include "AntiReflux.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

AntiReflux::AntiReflux()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
}

AntiReflux::~AntiReflux()
{
	// TODO Auto-generated destructor stub
}

void AntiReflux::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void AntiReflux::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int AntiReflux::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devManu)
	{
	case 1:
		ret = this->EpciRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int AntiReflux::EpciRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0;
	int length = 10;
	int data = 0;
	float tmp = 0.0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		tmp = ((short)(values[0]))*0.1;
		MemDb::SetRealData(regStart + 0, tmp, false);//线电压
		data = (short)(values[1]);
		MemDb::SetRealData(regStart + 1, (float)data, false);//电流
		data = ((short)(values[2]))*1000;
		MemDb::SetRealData(regStart + 2, (float)data, false);//有功功率
		data = ((short)(values[3]))*1000;
		MemDb::SetRealData(regStart + 3, (float)data, false);//总无功
		tmp = ((short)(values[4]))*0.01;
		MemDb::SetRealData(regStart + 4, tmp, false);//频率
		data = ((short)(values[8]))*1000;
		MemDb::SetRealData(regStart + 8, (float)data, false);//三相最小功率
		data = ((short)(values[9]))*1000;
		MemDb::SetRealData(regStart + 9, (float)data, false);//总有功功率
	}

	return ret;
}
