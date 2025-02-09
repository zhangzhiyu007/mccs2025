/*
 * ShWenShunLoad.cpp
 *
 *  创建日期: 2016-6-16
 *  作          者: 刘海涛
 */

#include "ShWenShunLoad.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

ShWenShunLoad::ShWenShunLoad()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

ShWenShunLoad::~ShWenShunLoad()
{
	// TODO Auto-generated destructor stub
}

void ShWenShunLoad::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void ShWenShunLoad::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int ShWenShunLoad::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_WSTF_LDJ_RLC://上海文顺RLC负载
		ret = this->ShWenShunRLCRead(dev);
		break;
	case DevModel::Model_WSTF_LDJ_ISLAND_RLC://上海文顺孤岛负载
		ret = this->ShWenShunIsLandRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ShWenShunLoad::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_WSTF_LDJ_RLC://上海文顺RLC负载
		ret = this->ShWenShunRLCPreset(dev);
		break;
	case DevModel::Model_WSTF_LDJ_ISLAND_RLC://上海文顺孤岛负载
		ret = this->ShWenShunIsLandPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ShWenShunLoad::ShWenShunRLCRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 103;
	int length = 1;
	float tmp = 0.0;
	int data = 0;
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
	FloatArray values;
	values.clear();
	UshortArray array;
	array.clear();

	ret = client.ReadRs485OutputRegisters(array,addr,start,length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;

		MemDb::SetRealData(regStart + 39, (short)array[0], false);//已加载时间
	}

	start = 104;
	length = 40;
	ret = client.ReadRLCFloatRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::SetRealData(regStart + 46, values[0], false);//A相电压
		MemDb::SetRealData(regStart + 47, values[1], false);//B相电压
		MemDb::SetRealData(regStart + 48, values[2], false);//C相电压
		MemDb::SetRealData(regStart + 49, values[6], false);//A相电流
		MemDb::SetRealData(regStart + 50, values[7], false);//B相电流
		MemDb::SetRealData(regStart + 51, values[8], false);//C相电流
		tmp = values[9] * 1000;
		MemDb::SetRealData(regStart + 40, tmp, false);//A相有功功率
		tmp = values[13] * 1000;
		MemDb::SetRealData(regStart + 41, tmp, false);//A相无功功率
		tmp = values[10] * 1000;
		MemDb::SetRealData(regStart + 42, tmp, false);//B相有功功率
		tmp = values[14] * 1000;
		MemDb::SetRealData(regStart + 43, tmp, false);//B相无功功率
		tmp = values[11] * 1000;
		MemDb::SetRealData(regStart + 44, tmp, false);//C相有功功率
		tmp = values[15] * 1000;
		MemDb::SetRealData(regStart + 45, tmp, false);//C相无功功率
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s数据失败", dev->name.c_str());
	}

	//读取设定值反馈
	start = 98;
	length = 4;
	ret = client.ReadRLCFloatRegisters(values,addr,start,length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 1000;
		MemDb::SetRealData(regStart + 14, tmp, false);//设定视在功率
		MemDb::SetRealData(regStart + 15, values[1], false);//设定功率因数
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设定值反馈数据失败", dev->name.c_str());
	}

	//读取设定加载时间反馈
	start = 102;
	length = 1;
	ret = client.ReadRs485OutputRegisters(array,addr,start,length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (short)array[0];
		MemDb::SetRealData(regStart + 3, (float)data, false);//设定加载时间
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设定加载时间反馈失败", dev->name.c_str());
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int ShWenShunLoad::ShWenShunRLCPreset(Device::SlaveDev* dev)
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
	FloatArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	int regStart = dev->regStart;

	//判断是否有加载/卸载命令
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		if(realData.pv.data.iValue == 1)//加载
		{
			client.ForceRS485SingleCoil(addr, 4106, true);
			MemDb::SetRealData(regStart + 36, 1, false);//运行
		}
		else if(realData.pv.data.iValue == 0)//卸载
		{
			client.ForceRS485SingleCoil(addr, 4106, false);
			MemDb::SetRealData(regStart + 36, 0, false);//停止
		}
	}

	//判断是否远程急停命令
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		client.ForceRS485SingleCoil(addr, 4111, true);//远程急停
	}

	//判断是否设定加载时间设置
	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		client.PresetRS485SingleRegister(addr, 102, (int)realData.pv.data.dbValue);
	}

	//判断是否有设定视在功率
	if (MemDb::GetRealDataWrited(realData, regStart + 14, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 98, 2, values);
	}

	//判断是否有设定功率因数
	if (MemDb::GetRealDataWrited(realData, regStart + 15, true))
	{
		tmp = realData.pv.data.dbValue;
		if(tmp < 0)
		{
			tmp = 0.0;
		}
		if(tmp > 1.0)
		{
			tmp = 1.0;
		}
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 100, 2, values);
	}

	return ret;
}

int ShWenShunLoad::ShWenShunIsLandRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 119;
	int length = 1;
	float tmp = 0.0;
	int data = 0;

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
	FloatArray values;
	values.clear();
	UshortArray array;
	array.clear();

	ret = client.ReadRs485OutputRegisters(array,addr,start,length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::SetRealData(regStart + 39, (short)array[0], false);//已加载时间
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s已加载时间失败", dev->name.c_str());
	}

	start = 120;
	length = 58;
	ret = client.ReadRLCFloatRegisters(values,addr,start,length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::SetRealData(regStart + 46, values[0], false);//A相电压
		MemDb::SetRealData(regStart + 47, values[1], false);//B相电压
		MemDb::SetRealData(regStart + 48, values[2], false);//C相电压
		MemDb::SetRealData(regStart + 49, values[6], false);//A相电流
		MemDb::SetRealData(regStart + 50, values[7], false);//B相电流
		MemDb::SetRealData(regStart + 51, values[8], false);//C相电流
		tmp = values[9] * 1000;
		MemDb::SetRealData(regStart + 40, tmp, false);//A相有功功率
		tmp = values[13] * 1000;
		MemDb::SetRealData(regStart + 41, tmp, false);//A相无功功率
		tmp = values[10] * 1000;
		MemDb::SetRealData(regStart + 42, tmp, false);//B相有功功率
		tmp = values[14] * 1000;
		MemDb::SetRealData(regStart + 43, tmp, false);//B相无功功率
		tmp = values[11] * 1000;
		MemDb::SetRealData(regStart + 44, tmp, false);//C相有功功率
		tmp = values[15] * 1000;
		MemDb::SetRealData(regStart + 45, tmp, false);//C相无功功率
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s实时数据失败", dev->name.c_str());
	}

	//读取故障码
	start = 4116;
	length = 10;
	array.clear();
	ret = client.ReadRLCFloatRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(unsigned int i = 0; i < array.size(); i++)
		{
			Util::SetBitValue(data, i, array[i]);
		}

		if(data > 0)
		{
			MemDb::SetRealData(regStart + 37, 1, false);//有故障
			MemDb::SetRealData(regStart + 38, data, false);//故障码
		}
		else
		{
			MemDb::SetRealData(regStart + 37, 0, false);//无故障
			MemDb::SetRealData(regStart + 38, data, false);//故障码
		}
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s故障数据失败", dev->name.c_str());
	}

	//读取设置反馈量
	start = 100;
	length = 18;
	ret = client.ReadRLCFloatRegisters(values,addr,start,length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 1000;
		MemDb::SetRealData(regStart + 5, tmp, false);//A相R
		tmp = values[6] * 1000;
		MemDb::SetRealData(regStart + 6, tmp, false);//A相C
		tmp = values[3] * 1000;
		MemDb::SetRealData(regStart + 7, tmp, false);//A相L
		tmp = values[1] * 1000;
		MemDb::SetRealData(regStart + 8, tmp, false);//B相R
		tmp = values[7] * 1000;
		MemDb::SetRealData(regStart + 9, tmp, false);//B相C
		tmp = values[4] * 1000;
		MemDb::SetRealData(regStart + 10, tmp, false);//B相L
		tmp = values[2] * 1000;
		MemDb::SetRealData(regStart + 11, tmp, false);//C相R
		tmp = values[8] * 1000;
		MemDb::SetRealData(regStart + 12, tmp, false);//C相C
		tmp = values[5] * 1000;
		MemDb::SetRealData(regStart + 13, tmp, false);//C相L
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设置反馈量失败", dev->name.c_str());
	}

	//读取设定加载时间反馈
	start = 118;
	length = 1;
	ret = client.ReadRs485OutputRegisters(array,addr,start,length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (short)array[0];
		MemDb::SetRealData(regStart + 3, (float)data, false);//设定加载时间
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设定加载时间反馈失败", dev->name.c_str());
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int ShWenShunLoad::ShWenShunIsLandPreset(Device::SlaveDev* dev)
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

	FloatArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	int regStart = dev->regStart;

	//判断是否有加载/卸载命令
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		if(realData.pv.data.iValue == 1)//加载
		{
			client.ForceRS485SingleCoil(addr, 4106, true);
			MemDb::SetRealData(regStart + 36, 1, false);//运行
		}
		else if(realData.pv.data.iValue == 0)//卸载
		{
			client.ForceRS485SingleCoil(addr, 4106, false);
			MemDb::SetRealData(regStart + 36, 0, false);//停止
		}
	}

	//判断是否远程急停命令
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		client.ForceRS485SingleCoil(addr, 4111, true);//远程急停
	}

	//判断是否设定加载时间设置
	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		client.PresetRS485SingleRegister(addr, 118, (int)realData.pv.data.dbValue);
	}

	//判断是否有设定A相R功率
	if (MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 100, 2, values);
	}

	//判断是否有设定A相C功率
	if (MemDb::GetRealDataWrited(realData, regStart + 6, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 112, 2, values);
	}

	//判断是否有设定A相L功率
	if (MemDb::GetRealDataWrited(realData, regStart + 7, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 106, 2, values);
	}

	//判断是否有设定B相R功率
	if (MemDb::GetRealDataWrited(realData, regStart + 8, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 102, 2, values);
	}

	//判断是否有设定B相C功率
	if (MemDb::GetRealDataWrited(realData, regStart + 9, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 114, 2, values);
	}

	//判断是否有设定B相L功率
	if (MemDb::GetRealDataWrited(realData, regStart + 10, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 108, 2, values);
	}

	//判断是否有设定C相R功率
	if (MemDb::GetRealDataWrited(realData, regStart + 11, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 104, 2, values);
	}

	//判断是否有设定C相C功率
	if (MemDb::GetRealDataWrited(realData, regStart + 12, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 116, 2, values);
	}

	//判断是否有设定C相L功率
	if (MemDb::GetRealDataWrited(realData, regStart + 13, true))
	{
		tmp = realData.pv.data.dbValue * 0.001;
		values.clear();
		values.push_back(tmp);
		client.PresetRLCFloatRegisters(addr, 110, 2, values);
	}

	return ret;
}
