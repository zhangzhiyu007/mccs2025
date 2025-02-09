/*
 * DcLoad.cpp
 *
 *  Created on: 2013-10-11
 *      Author: liuhaitao
 */

#include "DcLoad.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"
#include "QunLingDcLoad.h"
#include "ChromaDcLoad.h"
#include "GreeKongtiao.h"
#include "TaikaiPcms.h"
#include "EADCLoad.h"
#include "ZJNCLoad.h"

DcLoad::DcLoad()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_isTcp = false;
	m_tcpClient = NULL;
}

DcLoad::~DcLoad()
{
	// TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void DcLoad::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

void DcLoad::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void DcLoad::SetTcp(TcpClient* tcpClient)
{
	assert(NULL!=tcpClient);
	m_tcpClient = tcpClient;
}


int DcLoad::Read(Device::SlaveDev* dev)
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
	case DevManu::Manu_CHROMA:
		ret = this->ChromaDcLoadRead(dev);
		break;
	case DevManu::Manu_GREE:
		ret = this->GreeKongtiaoRead(dev);
		break;
	case DevManu::Manu_TAIKAI:
		ret = this->TaikaiPcmsRead(dev);
		break;
	case DevManu::Manu_EA:
		ret = this->EADCLoadRead(dev);
		break;
	case DevManu::Manu_ZJNC:
		ret = this->ZJNCLoadRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int DcLoad::Preset(Device::SlaveDev* dev)
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
	case DevManu::Manu_CHROMA:
		ret = this->ChromaDcLoadPreset(dev);
		break;
	case DevManu::Manu_TAIKAI:
		ret = this->TaikaiPcmsPreset(dev);
		break;
	case DevManu::Manu_EA:
		ret = this->EADCLoadPreset(dev);
		break;
	case DevManu::Manu_ZJNC:
		ret = this->ZJNCLoadPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}


int DcLoad::QunlingDcLoadRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	QunLingDcLoad dcLoad;
	dcLoad.SetCom(this->m_com);
	ret = dcLoad.Read(dev);
	return ret;
}

int DcLoad::QunlingDcLoadPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	QunLingDcLoad dcLoad;
	dcLoad.SetCom(this->m_com);
	ret = dcLoad.Preset(dev);
	return ret;
}

int DcLoad::ChromaDcLoadRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ChromaDcLoad dcLoad;
	dcLoad.SetTcp(this->m_tcpClient);
	ret = dcLoad.Read(dev);
	return ret;
}

int DcLoad::ChromaDcLoadPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ChromaDcLoad dcLoad;
	dcLoad.SetTcp(this->m_tcpClient);
	ret = dcLoad.Preset(dev);
	return ret;
}

int DcLoad::GreeKongtiaoRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	GreeKongtiao load;
	load.SetTcp(this->m_tcpClient);
	load.SetIsTcp(m_isTcp);
	ret = load.Read(dev);
	return ret;
}

int DcLoad::TaikaiPcmsRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	TaikaiPcms load;
	load.SetTcp(this->m_tcpClient);
	load.SetIsTcp(m_isTcp);
	ret = load.Read(dev);
	return ret;
}

int DcLoad::TaikaiPcmsPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	TaikaiPcms load;
	load.SetTcp(this->m_tcpClient);
	load.SetIsTcp(m_isTcp);
	ret = load.Preset(dev);
	return ret;
}

int DcLoad::EADCLoadRead(Device::SlaveDev* dev){
	int ret = ErrorInfo::ERR_OK;
	EADCLoad eaDcLoad;
	eaDcLoad.SetTcp(this->m_tcpClient);

	ret = eaDcLoad.Read(dev);
	return ret;

}

int DcLoad::EADCLoadPreset(Device::SlaveDev* dev){
	int ret = ErrorInfo::ERR_OK;

	EADCLoad eaDcLoad;
	eaDcLoad.SetTcp(this->m_tcpClient);

	ret = eaDcLoad.Preset(dev);
	return ret;

}

int DcLoad::ZJNCLoadRead(Device::SlaveDev* dev) {
	int ret = ErrorInfo::ERR_OK;
	ZJNCLoad load;
	load.SetTcp(this->m_tcpClient);

	ret = load.Read(dev);
	return ret;
}

int DcLoad::ZJNCLoadPreset(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	ZJNCLoad load;

	load.SetTcp(this->m_tcpClient);

	ret = load.Preset(dev);

	return ret;
}







