/*
 * ApcACSource.cpp
 *
 *  Created on: 2014-1-8
 *      Author: liuhaitao
 */

#include "ApcACSource.h"

ApcACSource::ApcACSource()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

ApcACSource::~ApcACSource()
{
	// TODO Auto-generated destructor stub
}

void ApcACSource::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
	m_retries = 3;//默认3次
	m_delayTime = 1000;//延时默认1000ms
}

int ApcACSource::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 28:
		ret = this->AcstN33030Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ApcACSource::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 28:
		ret = this->AcstN33030Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ApcACSource::AcstN33030Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取比亚迪PCS数据开始");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	unsigned int i = 0;
	int byte = 0;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	String reply;
	String query;
	query.clear();
	reply.clear();
	reply.resize(64);
	float tmp = 0;
	int data = 0;

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

	//读取实时数据
	//封装查询包
	query.push_back(0x40);
	query.push_back(0x15);
	query.push_back(0xea);

	ret = m_com->TxRxMsg(reply, query, 64);
	//判断查询有无错误
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		values.resize(40);
		byte = 0;
		for(i = 2;i < reply.size()-2;i+=2)
		{
			values[byte] = (reply[i]<<8)|(reply[i+1]);
			byte++;
		}
	}

	tmp = ((short)values[0])*0.01;
	MemDb::SetRealData(regStart+26,tmp,false);//A相电压
	tmp = ((short)values[1])*0.01;
	MemDb::SetRealData(regStart+27,tmp,false);//B相电压
	tmp = ((short)values[2])*0.01;
	MemDb::SetRealData(regStart+28,tmp,false);//C相电压
	tmp = ((short)values[3])*0.01;
	MemDb::SetRealData(regStart+29,tmp,false);//A相电流
	tmp = ((short)values[4])*0.01;
	MemDb::SetRealData(regStart+30,tmp,false);//B相电流
	tmp = ((short)values[5])*0.01;
	MemDb::SetRealData(regStart+31,tmp,false);//C相电流
	tmp = ((short)values[9])*0.01;
	MemDb::SetRealData(regStart+32,tmp,false);//A相有功功率
	tmp = ((short)values[10])*0.33;
	MemDb::SetRealData(regStart+30,tmp,false);//B相有功功率
	tmp = ((short)values[11])*0.01;
	MemDb::SetRealData(regStart+34,tmp,false);//C相有功功率
	data = (short)values[38];
	MemDb::SetRealData(regStart+37,data,false);//总功率因数
	tmp = ((short)values[39])*0.01;
	MemDb::SetRealData(regStart+38,tmp,false);//电网频率

	MemDb::SetRealData(regStart+0,comm,false);//通讯状态
	return ret;
}

int ApcACSource::AcstN33030Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	return ret;
}
