/*
 * CharingStation.cpp
 *
 *  创建日期: 2016-4-15
 *  作          者: 刘海涛
 */

#include "CharingStation.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

CharingStation::CharingStation()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
}

CharingStation::~CharingStation()
{
	// TODO Auto-generated destructor stub
}

void CharingStation::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void CharingStation::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int CharingStation::Read(Device::SlaveDev* dev, String recvData)
{
	int ret = ErrorInfo::ERR_OK;

	if(NULL==dev)
	{
		return ErrorInfo::ERR_NULL;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devManu)
	{
	case DevManu::Manu_SCHTITANS://四川泰坦豪特
		ret = this->SchtiTansChargRead(dev, recvData);
		break;
	default:
		break;
	};

	return ret;
}

int CharingStation::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	if(NULL==dev)
	{
		return ErrorInfo::ERR_NULL;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devManu)
	{
	case DevManu::Manu_SCHTITANS://四川泰坦豪特
		ret = this->SchtiTansChargPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int CharingStation::SchtiTansChargRead(Device::SlaveDev* dev, String recvData)
{
	int ret = ErrorInfo::ERR_OK;
	m_charg.SetTcp(this->m_tcpClient);
	ret = m_charg.Read(dev, recvData);
	return ret;
}

int CharingStation::SchtiTansChargPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	m_charg.SetTcp(this->m_tcpClient);
	ret = m_charg.Preset(dev);
	return ret;
}
