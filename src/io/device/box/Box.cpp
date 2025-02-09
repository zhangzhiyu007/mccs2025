/*
 * Box.cpp
 *
 *  Created on: 2013-10-16
 *      Author: liuhaitao
 */

#include "Box.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

#include "KbtBox.h"
#include "AcrelBox.h"

Box::Box()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
}

Box::~Box()
{
	// TODO Auto-generated destructor stub
}

void Box::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void Box::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}


int Box::Read(Device::SlaveDev* dev)
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
	case DevManu::Manu_KBT://科比特
		ret = this->KbtBoxRead(dev);
		break;
	case DevManu::Manu_ACREL://安科瑞
		ret = this->AcrelBoxRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int Box::KbtBoxRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	KbtBox box;
	box.SetCom(this->m_com);
	ret = box.Read(dev);
	return ret;
}

int Box::AcrelBoxRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	AcrelBox box;
	box.SetCom(this->m_com);
	ret = box.Read(dev);
	return ret;
}
