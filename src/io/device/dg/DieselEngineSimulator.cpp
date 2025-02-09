/*
 * DieselEngineSimulator.cpp
 *
 *  Created on: 2014-2-24
 *      Author: liuhaitao
 */

#include "DieselEngineSimulator.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

#include "EpciDieselEngineSimulator.h"

DieselEngineSimulator::DieselEngineSimulator()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
}

DieselEngineSimulator::~DieselEngineSimulator()
{
	// TODO Auto-generated destructor stub
}

void DieselEngineSimulator::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void DieselEngineSimulator::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int DieselEngineSimulator::Read(Device::SlaveDev* dev)
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
		ret = this->EpciDieselRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int DieselEngineSimulator::Preset(Device::SlaveDev* dev)
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
	case DevManu::Manu_STGK://盛通高科
		ret = this->EpciDieselPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int DieselEngineSimulator::EpciDieselRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	EpciDieselEngineSimulator diesel;
	diesel.SetTcp(this->m_tcpClient);
	ret = diesel.Read(dev);
	return ret;
}

int DieselEngineSimulator::EpciDieselPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	EpciDieselEngineSimulator diesel;
	diesel.SetTcp(this->m_tcpClient);
	ret = diesel.Preset(dev);
	return ret;
}
