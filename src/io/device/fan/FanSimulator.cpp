/*
 * FanSimulator.cpp
 *
 *  Created on: 2014-2-23
 *      Author: liuhaitao
 */

#include "FanSimulator.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

#include "EpciFanSimulator.h"

FanSimulator::FanSimulator()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
}

FanSimulator::~FanSimulator()
{
	// TODO Auto-generated destructor stub
}

void FanSimulator::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void FanSimulator::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}


int FanSimulator::Read(Device::SlaveDev* dev)
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
	case DevManu::Manu_STGK://盛通高科
		ret = this->EpciFanRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int FanSimulator::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"FanSimulator::Preset：NULL == dev");
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devManu)
	{
	case DevManu::Manu_STGK://盛通高科
		ret = this->EpciFanPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int FanSimulator::EpciFanRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	EpciFanSimulator fan;
	fan.SetTcp(this->m_tcpClient);
	ret = fan.Read(dev);
	return ret;
}

int FanSimulator::EpciFanPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	EpciFanSimulator fan;
	fan.SetTcp(this->m_tcpClient);
	ret = fan.Preset(dev);
	return ret;
}
