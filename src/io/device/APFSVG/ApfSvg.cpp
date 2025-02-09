/*
 * ApfSvg.cpp
 *
 *  创建日期: 2016-6-20
 *  作          者: 刘海涛
 */

#include "ApfSvg.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

#include "SinexcelApfSvg.h"

ApfSvg::ApfSvg()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
}

ApfSvg::~ApfSvg()
{
	// TODO Auto-generated destructor stub
}

void ApfSvg::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void ApfSvg::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int ApfSvg::Read(Device::SlaveDev* dev)
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
	case DevManu::Manu_SINEXCEL://深圳盛弘
		ret = this->SinexcelRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ApfSvg::Preset(Device::SlaveDev* dev)
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
	case DevManu::Manu_SINEXCEL://深圳盛弘
		ret = this->SinexcelPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ApfSvg::SinexcelRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	SinexcelApfSvg apf;
	apf.SetCom(this->m_com);
	ret = apf.Read(dev);
	return ret;
}

int ApfSvg::SinexcelPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	SinexcelApfSvg apf;
	apf.SetCom(this->m_com);
	ret = apf.Preset(dev);
	return ret;
}
