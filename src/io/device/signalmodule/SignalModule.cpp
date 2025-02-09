/*
 * SignalModule.cpp
 *
 *  Created on: 2014-4-19
 *      Author: liuhaitao
 */

#include "SignalModule.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

SignalModule::SignalModule()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

SignalModule::~SignalModule()
{
	// TODO Auto-generated destructor stub
}

void SignalModule::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void SignalModule::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}
int SignalModule::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	if(NULL==dev)
	{
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HC_33B://惠测电子
		ret = this->HC33BRead(dev);
		break;
	case DevModel::Model_HC_33BD://惠测电子HC-33BD
		ret = this->HC33BDRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SignalModule::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	if(NULL==dev)
	{
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HC_33B://惠测电子HC-33B
		ret = this->HC33BPreset(dev);
		break;
	case DevModel::Model_HC_33BD://惠测电子HC-33BD
		ret = this->HC33BDPreset(dev);
		break;
	default:
		break;
	};

	return ret;

}

int SignalModule::HC33BDRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 5;
	int U0 = 0;//电压量程
	int I01 = 0;//第一路电流量程
	float tmp = 0.0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcpClient);
	UshortArray values;
	values.clear();

	ret = client.ReadRs485OutputRegisters(values,addr,start,length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		U0 = (short)values[2];
		I01 = (short)values[3];
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取设备量程信息失败");
	}

	start = 0x0000;
	length = 3;
	ret = client.ReadRs485OutputRegisters(values,addr,start,length);
	if (ErrorInfo::ERR_OK == ret)
	{
		//		zlog_warn(Util::m_zlog,"读取直流模块");
		comm = true;
		MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
		tmp = ((short)values[0]) * 0.0001 * U0;
		MemDb::SetRealData(regStart + 1, tmp, false);//电压
		tmp = ((short)values[1]) * 0.0001 * I01;
		MemDb::SetRealData(regStart + 2, tmp, false);//电流
		tmp = ((short)values[2]) * 0.0001 * U0 * I01;
		MemDb::SetRealData(regStart + 3, tmp, false);//功率
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s状态失败",dev->name.c_str());
	}

	return ret;
}

int SignalModule::HC33BDPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	return ret;
}

int SignalModule::HC33BRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcpClient);
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 2;
	int data = 0;
	float tmp = 0.0;
	int U0 = 260;//电压量程
	int Ur = 1;//电压变比
	int I0 = 5;//电流量程
	int Ir = dev->rate;//电流变比

	start = 0x0000;
	length = 29;
	//读取电能模块实时数据
	ret = client.ReadRS485InputRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		//	zlog_warn(Util::m_zlog,"读取电能模块实时数据地址 = %d, regStart = %d",addr, regStart);
		comm = true;
		MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
		tmp = ((short) values[0]) * 0.0001 * U0 * Ur;
		MemDb::SetRealData(regStart + 3, tmp, false);//A相电压
		tmp = ((short) values[1]) * 0.0001 * U0 * Ur;
		MemDb::SetRealData(regStart + 4, tmp, false);//B相电压
		tmp = ((short) values[2]) * 0.0001 * U0 * Ur;
		MemDb::SetRealData(regStart + 5, tmp, false);//C相电压
		tmp = ((short) values[8]) * 0.0001 * I0 * Ir;
		MemDb::SetRealData(regStart + 6, tmp, false);//A相电流
		tmp = ((short) values[9]) * 0.0001 * I0 * Ir;
		MemDb::SetRealData(regStart + 7, tmp, false);//B相电流
		tmp = ((short) values[10]) * 0.0001 * I0 * Ir;
		MemDb::SetRealData(regStart + 8, tmp, false);//C相电流
		tmp = ((short) values[12]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 9, tmp, false);//A相有功功率
		tmp = ((short) values[13]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 10, tmp, false);//B相有功功率
		tmp = ((short) values[14]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 11, tmp, false);//C相有功功率
		tmp = ((short) values[15]) * 0.0001 * U0 * Ur * I0 * Ir * 3;
		MemDb::SetRealData(regStart + 12, tmp, false);//总有功功率
		tmp = ((short) values[16]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 13, tmp, false);//A相无功功率
		tmp = ((short) values[17]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 14, tmp, false);//B相无功功率
		tmp = ((short) values[18]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 15, tmp, false);//C相无功功率
		tmp = ((short) values[19]) * 0.0001 * U0 * Ur * I0 * Ir * 3;
		MemDb::SetRealData(regStart + 16, tmp, false);//总无功功率
		tmp = ((short) values[20]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 17, tmp, false);//A相视在功率
		tmp = ((short) values[21]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 18, tmp, false);//B相视在功率
		tmp = ((short) values[22]) * 0.0001 * U0 * Ur * I0 * Ir;
		MemDb::SetRealData(regStart + 19, tmp, false);//C相视在功率
		tmp = ((short) values[23]) * 0.0001 * U0 * Ur * I0 * Ir * 3;
		MemDb::SetRealData(regStart + 20, tmp, false);//总视在功率
		tmp = ((short) values[27]) * 0.0001;
		MemDb::SetRealData(regStart + 25, tmp, false);//总功率因数
		tmp = ((short) values[28]) * 0.01;
		MemDb::SetRealData(regStart + 2, tmp, false);//频率

	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%d#电能模块实时数据失败",addr);
	}
	return ret;
}

int SignalModule::SetMemDb(int addr,UshortArray values,int start,bool comm)
{
	int ret = ErrorInfo::ERR_OK;




	return ret;
}

int SignalModule::HC33BPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	return ret;
}
