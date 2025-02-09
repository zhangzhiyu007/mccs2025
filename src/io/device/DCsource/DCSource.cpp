/*
 * DCSource.cpp
 *
 *  Created on: 2014-1-15
 *      Author: liuhaitao
 */

#include "DCSource.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

#include "ApcDCSource.h"
DCSource::DCSource()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
}

DCSource::~DCSource()
{
	// TODO Auto-generated destructor stub
}

void DCSource::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void DCSource::SetTcp(TcpClient* tcpClient)
{
	assert(NULL!=tcpClient);
	m_tcpClient = tcpClient;
}

int DCSource::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	if(NULL==dev)
	{
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devManu)
	{
	case 16://艾普斯
		ret = this->ApcDcSourceRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int DCSource::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	if (NULL == dev)
	{
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devManu)
	{
	case 16://艾普斯
		ret = this->ApcDcSourcePreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int DCSource::ApcDcSourceRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ApcDCSource source;
	source.SetCom(this->m_com);
	ret = source.Read(dev);
	return ret;
}

int DCSource::ApcDcSourcePreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ApcDCSource source;
	source.SetCom(this->m_com);
	ret = source.Preset(dev);
	return ret;
}

