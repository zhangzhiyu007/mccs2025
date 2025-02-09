/*
 * ParwaFaultSimultor.cpp
 *
 *  创建日期: 2017-12-10
 *  作          者: 刘海涛
 */

#include "ParwaFaultSimultor.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

ParwaFaultSimultor::ParwaFaultSimultor()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

ParwaFaultSimultor::~ParwaFaultSimultor()
{
	// TODO Auto-generated destructor stub
}

void ParwaFaultSimultor::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void ParwaFaultSimultor::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int ParwaFaultSimultor::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PARWA_GZMN:
		ret = this->FaultSimRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ParwaFaultSimultor::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PARWA_GZMN:
		ret = this->FaultSimPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ParwaFaultSimultor::FaultSimRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	if (dev->commType == 2)
	{
		ret = this->PrawaTcpFaultSimRead(dev);
	}
	else if(dev->commType == 1)
	{
		ret = this->PrawaRs485FaultSimRead(dev);
	}

	return ret;
}

int ParwaFaultSimultor::FaultSimPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	if (dev->commType == 2)
	{
		ret = this->PrawaTcpFaultSimPreset(dev);
	}
	else if(dev->commType == 1)
	{
		ret = this->PrawaRs485FaultSimPreset(dev);
	}
	return ret;
}

int ParwaFaultSimultor::PrawaTcpFaultSimRead(Device::SlaveDev* dev)
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

	//读取短路模式
	ret = client.ReadRs485OutputRegisters(values, addr, 0x9c48, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 2, data, false);//短路模式
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器短路模式失败");
	}

	//读取短路线路
	ret = client.ReadRs485OutputRegisters(values, addr, 0x9c4E, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 3, data, false);//短路线路
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器短路线路失败");
	}

	//读取设置时间
	ret = client.ReadRs485OutputRegisters(values, addr, 0x9c4C, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 5, data, false);//设置时间
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器设置时间失败");
	}

	//读取限流电阻
	ret = client.ReadRs485OutputRegisters(values, addr, 0x9c4A, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 4, data, false);//限流电阻
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器限流电阻失败");
	}

	//读取剩余时间
	ret = client.ReadRs485OutputRegisters(values, addr, 0xC350, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 7, data, false);//剩余时间
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器剩余时间失败");
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
		MemDb::SetRealData(regStart + 6, run, false);//运行状态
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器运行状态失败");
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

	return ret;
}

int ParwaFaultSimultor::PrawaTcpFaultSimPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

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

	//判断是否有设置短路时间命令
	if (MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		client.PresetRS485MultipleRegisters(addr, 0x9c4c, 2, values);
	}

	//判断是否有设置电阻命令
	if (MemDb::GetRealDataWrited(realData, regStart + 4, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		client.PresetRS485MultipleRegisters(addr, 0x9c4a, 2, values);
	}

	//判断是否有设置短路模式
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		client.PresetRS485MultipleRegisters(addr, 0x9c48, 2, values);
	}

	//判断是否有设置短路线路
	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		client.PresetRS485MultipleRegisters(addr, 0x9c4E, 2, values);
	}

	return ret;
}


int ParwaFaultSimultor::PrawaRs485FaultSimRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = 1;
	int data = 0;
	int regStart = dev->regStart;
	int run = 0;
	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断网口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);
	UshortArray values;
	values.clear();

	//读取短路模式
	ret = rtu.ReadOutputRegisters(values, addr, 0x9c48, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 2, data, false);//短路模式
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器短路模式失败");
	}

	//读取短路线路
	ret = rtu.ReadOutputRegisters(values, addr, 0x9c4E, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 3, data, false);//短路线路
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器短路线路失败");
	}

	//读取设置时间
	ret = rtu.ReadOutputRegisters(values, addr, 0x9c4C, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 5, data, false);//设置时间
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器设置时间失败");
	}

	//读取限流电阻
	ret = rtu.ReadOutputRegisters(values, addr, 0x9c4A, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 4, data, false);//限流电阻
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器限流电阻失败");
	}

	//读取剩余时间
	ret = rtu.ReadOutputRegisters(values, addr, 0xC350, 2);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (values[0] << 16) | values[1];
		MemDb::SetRealData(regStart + 7, data, false);//剩余时间
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器剩余时间失败");
	}

	//读取运行状态
	ret = rtu.ReadOutputRegisters(values, addr, 0x7530, 2);
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
		MemDb::SetRealData(regStart + 6, run, false);//运行状态
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取故障模拟器运行状态失败");
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

	return ret;
}

int ParwaFaultSimultor::PrawaRs485FaultSimPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	//判断网口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

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
			rtu.ForceSingleCoil(addr, 0, true);
		}
		else if (realData.pv.data.iValue == 0)//停止
		{
			rtu.ForceSingleCoil(addr, 0, false);
		}
	}

	//判断是否有设置短路时间命令
	if (MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		rtu.PresetMultipleRegisters(addr, 0x9c4c, 2, values);
	}

	//判断是否有设置电阻命令
	if (MemDb::GetRealDataWrited(realData, regStart + 4, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		rtu.PresetMultipleRegisters(addr, 0x9c4a, 2, values);
	}

	//判断是否有设置短路模式
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		rtu.PresetMultipleRegisters(addr, 0x9c48, 2, values);
	}

	//判断是否有设置短路线路
	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		values.push_back((realData.pv.data.iValue >> 16)& 0xFFFF);
		values.push_back(realData.pv.data.iValue & 0xFFFF);
		rtu.PresetMultipleRegisters(addr, 0x9c4E, 2, values);
	}

	return ret;
}
