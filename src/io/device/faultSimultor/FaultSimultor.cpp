/*
 * FaultSimultor.cpp
 *
 *  创建日期: 2017-12-10
 *  作          者: 刘海涛
 */

#include "FaultSimultor.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

#include "ParwaFaultSimultor.h"
#include "DCXG500FaultSimulator.h"

FaultSimultor::FaultSimultor()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
}

FaultSimultor::~FaultSimultor()
{
	// TODO Auto-generated destructor stub
}

void FaultSimultor::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void FaultSimultor::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int FaultSimultor::Read(Device::SlaveDev* dev)
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
	case DevManu::Manu_Parwa:
		ret = this->ParwaFaultSimRead(dev);
		break;
	case DevManu::Manu_JINGCE:
		ret = this->DCXG500Read(dev);
	default:
		break;
	};

	return ret;
}

int FaultSimultor::Preset(Device::SlaveDev* dev)
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
	case DevManu::Manu_Parwa:
		ret = this->ParwaFaultSimPreset(dev);
		break;
	case DevManu::Manu_JINGCE:
		ret = this->DCXG500Preset(dev);
	default:
		break;
	};

	return ret;
}


int FaultSimultor::ParwaFaultSimRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ParwaFaultSimultor faultSim;
	if(dev->commType == 2)
	{
		faultSim.SetTcp(this->m_tcpClient);
	}
	else if(dev->commType == 1)
	{
		faultSim.SetCom(this->m_com);
	}

	ret = faultSim.Read(dev);
	return ret;
}

int FaultSimultor::ParwaFaultSimPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ParwaFaultSimultor faultSim;
	if(dev->commType == 2)
	{
		faultSim.SetTcp(this->m_tcpClient);
	}
	else if(dev->commType == 1)
	{
		faultSim.SetCom(this->m_com);
	}
	ret = faultSim.Preset(dev);
	return ret;
}

int FaultSimultor::DCXG500Read(Device::SlaveDev* dev){
	int ret = ErrorInfo::ERR_OK;
	DCXG500FaultSimulator DCXG;
	DCXG.SetTcp(this->m_tcpClient);
	ret = DCXG.Read(dev);
	
	return ret;

}

int FaultSimultor::DCXG500Preset(Device::SlaveDev* dev){
	int ret = ErrorInfo::ERR_OK;
	DCXG500FaultSimulator DCXG;
	DCXG.SetTcp(this->m_tcpClient);
	ret = DCXG.Preset(dev);
}






