/*
 * ParwaImpedanceSimulator.cpp
 *
 *  创建日期: 2017-12-10
 *  作          者: 刘海涛
 */

#include "ParwaImpedanceSimulator.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"
ParwaImpedanceSimulator::ParwaImpedanceSimulator()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

ParwaImpedanceSimulator::~ParwaImpedanceSimulator()
{
	// TODO Auto-generated destructor stub
}

void ParwaImpedanceSimulator::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void ParwaImpedanceSimulator::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int ParwaImpedanceSimulator::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PARWA_XLZK:
		ret = this->ParwaImpSimRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ParwaImpedanceSimulator::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PARWA_XLZK:
		ret = this->ParwaImpSimPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}


int ParwaImpedanceSimulator::ParwaImpSimRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = 1;
	int data = 0;
	int run = 0;
	int regStart = dev->regStart;
	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断网口是否打开
	if (!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcp);
	UshortArray values;
	values.clear();

	//读取线路长度
	ret = client.ReadRs485OutputRegisters(values, addr, 0x9c4A, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 3, data, false);//线路长度
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障线路阻抗线路长度失败");
	}

	//读取运行状态
	ret = client.ReadRs485OutputRegisters(values, addr, 0x7530, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		if(data == 0x01)
		{
			run = 0;
		}
		else if(data == 0x02)
		{
			run = 1;
		}
		else if(data == 0x04)
		{
			run = 2;
		}
		else if(data == 0x08)
		{
			run = 3;
		}
		else if(data == 0x10)
		{
			run = 4;
		}
		MemDb::SetRealData(regStart + 4, run, false);//运行状态
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障线路阻抗运行状态失败");
	}

	//读取电路模式
	ret = client.ReadRs485OutputRegisters(values, addr, 0x9c48, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 2, data, false);//电路模式
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障线路阻抗电路模式失败");
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

	return ret;
}

int ParwaImpedanceSimulator::ParwaImpSimPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	float tmp = 0.0;

	//判断网口是否打开
	if (!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcp);

	UshortArray values;
	values.clear();
	int addr = 1;
	RealData realData;
	int regStart = dev->regStart;
	//判断是否有启动/停止命令
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		if (realData.pv.data.iValue == 1)//启动
		{
			client.ForceRS485SingleCoil(addr, 0, true);
		}
		else if (realData.pv.data.iValue == 0)//停止
		{
			client.ForceRS485SingleCoil(addr, 0, false);
		}
	}

	//判断是否有设置电路模式命令
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		client.PresetRS485MultipleRegisters(addr, 0x9c48, 2, values);
	}

	//判断是否有线路长度命令
	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		client.PresetRS485MultipleRegisters(addr, 0x9c4a, 2, values);
	}
	return ret;
}
