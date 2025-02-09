/*
 * BosPcs.cpp
 *
 *  Created on: 2015-5-20
 *      Author: liuhaitao
 */

#include "BosPcs.h"
#include "../../../etc/socket/TcpClient.h"

BosPcs::BosPcs()
{
	// TODO Auto-generated constructor stub
	m_tcp = NULL;
}

BosPcs::~BosPcs()
{
	// TODO Auto-generated destructor stub
}

//设置TcpClient指针
void BosPcs::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int BosPcs::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HN_PCS_15:
		ret = this->Bos10Read(dev);//博奥斯10kWPCS
		break;
	default:
		break;
	};

	return ret;
}

int BosPcs::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HN_PCS_15:
		ret = this->Bos10Preset(dev);//博奥斯10kWPCS
		break;
	default:
		break;
	};

	return ret;
}

int BosPcs::Bos10Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取博奥斯10kWPCS数据开始");
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0;
	int length = 27;
	int data = 0;
	float data1 = 0.0;
	int value  = 0;
	int tmp = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断网络是否打开
	if (!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcp);
	//读取实时反馈信息

	ret = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (short)values[20];
		if(data == 0)
		{
			MemDb::SetRealData(regStart + 13, 3, false);//待机
		}
		else if(data == 1)
		{
			MemDb::SetRealData(regStart + 13, 4, false);//运行
		}
		data = (short)values[0];
		MemDb::SetRealData(regStart + 26, (float)data, false);//直流电压
		data = (short)values[1];
		MemDb::SetRealData(regStart + 27, (float)data, false);//直流电流
		data = (short)values[0] * (short)values[1];
		MemDb::SetRealData(regStart + 28, (float)data, false);//直流功率
		data = (short)values[2];
		MemDb::SetRealData(regStart + 29, (float)data, false);//A电压
		data = (short)values[3];
		MemDb::SetRealData(regStart + 32, (float)data, false);//A电流
		data = (short)values[4];
		MemDb::SetRealData(regStart + 30, (float)data, false);//B电压
		data = (short)values[5];
		MemDb::SetRealData(regStart + 33, (float)data, false);//B电流
		data = (short)values[6];
		MemDb::SetRealData(regStart + 31, (float)data, false);//C电压
		data = (short)values[7];
		MemDb::SetRealData(regStart + 34, (float)data, false);//C电流
		data1 = (short)values[16] * 0.1;
		MemDb::SetRealData(regStart + 41, data1, false);//频率
		data1 = (short)values[18] * 0.1 * 1000;
		MemDb::SetRealData(regStart + 38, data1, false);//有功功率
		data1 = (((short)values[24]) << 16) | ((short)values[25]);
		MemDb::SetRealData(regStart + 22, data1, false);//累计输出电能
		data1 = (((short)values[22]) << 16) | ((short)values[23]);
		MemDb::SetRealData(regStart + 21, data1, false);//累计输入电能
		data = (short)values[15];
		MemDb::SetRealData(regStart + 16, data, false);//并离网状态
		data = (short)values[26];
		MemDb::SetRealData(regStart + 15, data, false);//并离网手自动状态
		data1 = (short)values[12] * 0.1 * 1000;
		MemDb::SetRealData(regStart + 39, data1, false);//无功功率
		data1 = (short)values[13] * 0.1;
		MemDb::SetRealData(regStart + 37, data1, false);//逆变效率
		data1 = (short)values[11] * 0.01;
		MemDb::SetRealData(regStart + 40, data1, false);//功率因数

		data = (short)values[19];
		value = 0;
		switch(data)
		{
		case 1://直流过压
			Util::SetBitValue(value, 12, true);
			break;
		case 2://直流欠压
			Util::SetBitValue(value, 13, true);
			break;
		case 3://A相过压
		case 5://B相过压
		case 7://C相过压
			Util::SetBitValue(value, 16, true);
			break;
		case 4://A相欠压
		case 6://B相欠压
		case 8://C相欠压
			Util::SetBitValue(value, 17, true);
			break;
		case 9://频率故障
			Util::SetBitValue(value, 27, true);
			break;
		case 10://A相过流
		case 11://B相过流
		case 12://C相过流
		case 15://限流保护
			Util::SetBitValue(value, 22, true);
			break;
		case 14://过热
			Util::SetBitValue(value, 29, true);
			break;
		case 17://模块保护
			Util::SetBitValue(value, 31, true);
			break;
		case 18://直流过压
			Util::SetBitValue(value, 12, true);
			break;
		case 19://系统报错
			Util::SetBitValue(value, 30, true);
			break;
		default:
			break;
		}
		if(value != 0)
		{
			MemDb::SetRealData(regStart + 17, 1, false);//有故障
			MemDb::SetRealData(regStart + 18, value, false);//故障码1
			MemDb::SetRealData(regStart + 19, tmp, false);//故障码2
		}
		else
		{
			MemDb::SetRealData(regStart + 17, 0, false);//无故障
			MemDb::SetRealData(regStart + 18, value, false);//故障码1
			MemDb::SetRealData(regStart + 19, tmp, false);//故障码2
		}

		data = (short)values[21];
		switch(data)
		{
		case 1://并机模式
			MemDb::SetRealData(regStart + 14, 5, false);//并机模式
			break;
		case 2://直流电压源
			MemDb::SetRealData(regStart + 14, 4, false);//恒压模式
			break;
		case 3://恒直流电流充放电
			MemDb::SetRealData(regStart + 14, 3, false);//恒流模式
			break;
		case 4://恒交流功率充放电
			MemDb::SetRealData(regStart + 14, 1, false);//恒功率模式
			break;
		case 5://离网模式
			MemDb::SetRealData(regStart + 14, 2, false);//离网模式
			break;
		default:
			break;
		}
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取%s数据失败,故障码 = %d", dev->name.c_str(), ret);
	}

	//设置反馈值
	start = 30;
	length = 9;
	ret = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = (short)values[0];
		MemDb::SetRealData(regStart + 1, data, false);//停机
		data1 = (short)values[3] * 100;
		MemDb::SetRealData(regStart + 6, data1, false);//恒功率功率
		data1 = (short)values[4] * 0.01;
		MemDb::SetRealData(regStart + 9, data1, false);//功率因数
		data = (short)values[6];
		MemDb::SetRealData(regStart + 10, (float)data, false);//恒流电流
		data = (short)values[8];
		MemDb::SetRealData(regStart + 2, data, false);//并离网手自动切换
		data1 = (short)values[5];
		MemDb::SetRealData(regStart + 11, data1, false);//恒压电压设定值
		data1 = (short)values[7];
		MemDb::SetRealData(regStart + 7, data1, false);//交流电压设定值

		data = (short)values[2];
		switch(data)
		{
		case 1://并机模式
			MemDb::SetRealData(regStart + 3, 5, false);//并机模式
			break;
		case 2://直流电压源
			MemDb::SetRealData(regStart + 3, 4, false);//恒压模式
			break;
		case 3://恒直流电流充放电
			MemDb::SetRealData(regStart + 3, 3, false);//恒流模式
			break;
		case 4://恒交流功率充放电
			MemDb::SetRealData(regStart + 3, 1, false);//恒功率模式
			break;
		case 5://离网模式
			MemDb::SetRealData(regStart + 3, 2, false);//离网模式
			break;
		default:
			break;
		}
	}

	MemDb::SetRealData(regStart + 0, comm, false);

	return ret;
}

int BosPcs::Bos10Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	RealData realData;
	int data = 0;
	float tmp = 0.0;
	int tmpV = 0;
	int runSta = 0;

	//判断以太网口是否打开
	if (!m_tcp->IsOpened())
	{
		zlog_warn(Util::m_zlog,"EpciFanSimulator::Wes6930Preset：以太网没有打开");
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcp);

	//判断是否有启动停止设置
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		if(realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr, 30, realData.pv.data.iValue);
		}
		else
		{
			zlog_warn(Util::m_zlog,"设置启动停止命令错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有工作模式设置
	//获取运行状态
	MemDb::GetRealData(runSta, regStart + 13);
	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		if(realData.pv.data.iValue == 1)//恒功率模式
		{
			ret = client.PresetSingleRegister(addr, 32, 4);//恒功率模式
		}
		else if(realData.pv.data.iValue == 2)//离网模式
		{
			ret = client.PresetSingleRegister(addr, 32, 5);//离网模式
		}
		else if(realData.pv.data.iValue == 3)//恒直流模式
		{
			ret = client.PresetSingleRegister(addr, 32, 3);//恒直流模式
		}
		else if((realData.pv.data.iValue == 4) && (runSta == 3))
		{
			ret = client.PresetSingleRegister(addr, 32, 2);//直流源
		}
		else if(realData.pv.data.iValue == 5)//并机模式
		{
			ret = client.PresetSingleRegister(addr, 32, 1);//并机模式
		}
	}

	//判断是否功率设置
	if (MemDb::GetRealDataWrited(realData, regStart + 6, true))
	{
		tmp = realData.pv.data.dbValue * 0.01;
		ret = client.PresetSingleRegister(addr, 33, (int) tmp);
	}

	//判断是否恒直流电流目标值
	if (MemDb::GetRealDataWrited(realData, regStart + 10, true))
	{
		tmp = realData.pv.data.dbValue;
		ret = client.PresetSingleRegister(addr, 36, (int) tmp);
	}

	//判断是否有超前/滞后功率因数设置
	if (MemDb::GetRealDataWrited(realData, regStart + 9, true))
	{
		if(((realData.pv.data.dbValue > (-1)) && (realData.pv.data.dbValue <= (-0.95)))
				|| ((realData.pv.data.dbValue >= (0.95)) && (realData.pv.data.dbValue <= (1))))
		{
			tmp = realData.pv.data.dbValue * 100;
		}
		else
		{
			tmp = 100;
		}

		ret = client.PresetSingleRegister(addr, 34, (int) tmp);
	}

	//判断是否并离网手自动模式切换
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		ret = client.PresetSingleRegister(addr, 38, realData.pv.data.iValue);//并离网手自动模式切换
	}

	//判断是否有恒压模式电压值设定
	if (MemDb::GetRealDataWrited(realData, regStart + 11, true))
	{
		tmp = realData.pv.data.dbValue;
		ret = client.PresetSingleRegister(addr, 35, (int) tmp);
	}

	//判断是否有离网电压值设定
	if (MemDb::GetRealDataWrited(realData, regStart + 7, true))
	{
		tmp = realData.pv.data.dbValue;
		ret = client.PresetSingleRegister(addr, 37, (int) tmp);
	}

	//判断是否有故障清除
	if (MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{
		ret = client.PresetSingleRegister(addr, 31, realData.pv.data.iValue);//故障清除
	}

	return ret;
}
