/*
 * ZheDaBatt.cpp
 *
 *  创建日期: 2017-12-11
 *  作          者: 刘海涛
 */

#include "ZheDaBatt.h"

ZheDaBatt::ZheDaBatt()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

ZheDaBatt::~ZheDaBatt()
{
	// TODO Auto-generated destructor stub
}

//设置TcpClient指针
void ZheDaBatt::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

void ZheDaBatt::setCom(Com* com)
{
	m_com = com;
}

int ZheDaBatt::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_ZHEDA_BMS://浙大电池
		ret = this->BattRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ZheDaBatt::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_ZHEDA_BMS://浙大电池
		ret = this->BattPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ZheDaBatt::BattRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	float tmp = 0.0;
	int data = 0;
	int start = 0;
	int length = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcp);
	//读取电网实时反馈信息
	start = 0x1100;
	length = 32;
	ret = client.ReadRs485OutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (short)values[0];
		MemDb::SetRealData(regStart + 1, tmp, false);//基本状态
		data = (short)values[1];
		MemDb::SetRealData(regStart + 2, tmp, false);//保护状态
		data = (short)values[2];
		MemDb::SetRealData(regStart + 3, tmp, false);//告警状态
		tmp = (short)values[3] * 0.1;
		MemDb::SetRealData(regStart + 6, tmp, false);//总电压
		tmp = (((((short)values[4])&0xFFFF)<<16)|(((short)values[5])&0xFFFF)) * 0.01;
		MemDb::SetRealData(regStart + 7, tmp, false);//总电流
		tmp = (short)values[6] * 0.1;
		MemDb::SetRealData(regStart + 8, tmp, false);//温度
		data = (short)values[7];
		MemDb::SetRealData(regStart + 9, (float)data, false);//SOC
		data = (short)values[8];
		MemDb::SetRealData(regStart + 4, data, false);//循环次数
		tmp = (short)values[9] * 0.1;
		MemDb::SetRealData(regStart + 10, tmp, false);//电池组最大充电电压
		tmp = (((((short)values[10])&0xFFFF)<<16)|(((short)values[11])&0xFFFF)) * 0.01;
		MemDb::SetRealData(regStart + 11, tmp, false);//电池组最大充电电流
		tmp = (short)values[12] * 0.1;
		MemDb::SetRealData(regStart + 12, tmp, false);//电池组最小放电电压
		tmp = (((((short)values[13])&0xFFFF)<<16)|(((short)values[14])&0xFFFF)) * 0.01;
		MemDb::SetRealData(regStart + 13, tmp, false);//电池组最大放电电流
		data = (short)values[15];
		MemDb::SetRealData(regStart + 5, data, false);//开关量指示
		tmp = (short)values[16] * 0.001;
		MemDb::SetRealData(regStart + 14, tmp, false);//电池最高电压
		tmp = (short)values[17] * 0.001;
		MemDb::SetRealData(regStart + 15, tmp, false);//电池最低电压
		data = (short)values[18];
		MemDb::SetRealData(regStart + 22, data, false);//电池最高电压通道
		data = (short)values[19];
		MemDb::SetRealData(regStart + 23, data, false);//电池最低电压通道
		tmp = (short)values[20] * 0.1;
		MemDb::SetRealData(regStart + 16, tmp, false);//电池最高温度
		tmp = (short)values[21] * 0.1;
		MemDb::SetRealData(regStart + 17, tmp, false);//电池最低温度
		data = (short)values[22];
		MemDb::SetRealData(regStart + 24, data, false);//电池最高温度通道
		data = (short)values[23];
		MemDb::SetRealData(regStart + 25, data, false);//电池最低温度通道
		tmp = (short)values[24] * 0.001;
		MemDb::SetRealData(regStart + 18, tmp, false);//单板最高电压
		tmp = (short)values[25] * 0.001;
		MemDb::SetRealData(regStart + 19, tmp, false);//单板最低电压
		data = (short)values[26];
		MemDb::SetRealData(regStart + 26, data, false);//单板最高电压通道
		data = (short)values[27];
		MemDb::SetRealData(regStart + 27, data, false);//单板最低电压通道
		tmp = (short)values[28] * 0.1;
		MemDb::SetRealData(regStart + 20, tmp, false);//单板最高温度
		tmp = (short)values[29] * 0.1;
		MemDb::SetRealData(regStart + 21, tmp, false);//单板最低温度
		data = (short)values[30];
		MemDb::SetRealData(regStart + 28, data, false);//单板最高温度通道
		data = (short)values[31];
		MemDb::SetRealData(regStart + 29, data, false);//单板最低温度通道
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设备，地址=%d数据失败", dev->name.c_str(), start);
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int ZheDaBatt::BattPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	return ret;
}
