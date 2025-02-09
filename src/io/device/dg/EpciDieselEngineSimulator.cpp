/*
 * EpciDieselEngineSimulator.cpp
 *
 *  Created on: 2014-2-24
 *      Author: liuhaitao
 */

#include "EpciDieselEngineSimulator.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

EpciDieselEngineSimulator::EpciDieselEngineSimulator()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

EpciDieselEngineSimulator::~EpciDieselEngineSimulator()
{
	// TODO Auto-generated destructor stub
}

void EpciDieselEngineSimulator::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void EpciDieselEngineSimulator::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int EpciDieselEngineSimulator::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HN_DSLM_30:
		ret = this->Dgs4030Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int EpciDieselEngineSimulator::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HN_DSLM_30:
		ret = this->Dgs4030Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int EpciDieselEngineSimulator::Dgs4030Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 50;
	int length = 74;
	int data = 0;
	float tmp = 0;

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
		data = (short)values[0];
		MemDb::SetRealData(regStart + 0,data,false);//通讯状态
		data = (short)values[1];
		MemDb::SetRealData(regStart + 30,data,false);//运行状态
		data = (short)values[2];
		MemDb::SetRealData(regStart + 31,data,false);//当前工作模式
		data = (short)values[3];
		MemDb::SetRealData(regStart + 32,data,false);//当前执行方式
		data = (short)values[4];
		MemDb::SetRealData(regStart + 33,data,false);//同步并网完成信号
		data = (short)values[5];
		MemDb::SetRealData(regStart + 34,data,false);//合分闸状态
		data = (short)values[6];
		MemDb::SetRealData(regStart + 35,data,false);//故障状态
		data = (short)values[11];
		MemDb::SetRealData(regStart + 36,data,false);//变频器报警状态1
		data = (short)values[12];
		MemDb::SetRealData(regStart + 37,data,false);//变频器报警状态2
		data = (short)values[13];
		MemDb::SetRealData(regStart + 38,data,false);//变频器报警状态3
		data = (short)values[14];
		MemDb::SetRealData(regStart + 39,data,false);//变频器报警状态4
		data = (short)values[17];
		MemDb::SetRealData(regStart + 40,data,false);//控制器报警状态1
		data = (short)values[18];
		MemDb::SetRealData(regStart + 41,data,false);//控制器报警状态2
		data = (short)values[19];
		MemDb::SetRealData(regStart + 42,data,false);//控制器报警状态3
		data = (short)values[20];
		MemDb::SetRealData(regStart + 43,data,false);//控制器报警状态4
		data = (short)values[22];
		MemDb::SetRealData(regStart + 44,data,false);//系统报警状态1
		tmp = (short)values[30] * 0.01;
		MemDb::SetRealData(regStart + 45,tmp,false);//市电频率
		tmp = (short)values[31] * 0.01;
		MemDb::SetRealData(regStart + 46,tmp,false);//发电频率
		data = (short)values[32];
		MemDb::SetRealData(regStart + 47,data,false);//实时转速
		tmp = (short)values[33] * 0.01;
		MemDb::SetRealData(regStart + 48,tmp,false);//输出扭矩
		tmp = (short)values[34] * 0.1;
		MemDb::SetRealData(regStart + 49,tmp,false);//当前加速时间
		tmp = (short)values[35] * 0.1;
		MemDb::SetRealData(regStart + 50,tmp,false);//当前减速时间
		tmp = (short)values[36] * 0.1;
		MemDb::SetRealData(regStart + 51,tmp,false);//模拟器输入电压
		tmp = (short)values[42] * 0.1;
		MemDb::SetRealData(regStart + 52,tmp,false);//模拟器输入电流
		data = (short)values[48];
		MemDb::SetRealData(regStart + 53,(float)data,false);//模拟器输入总有功功率
		data = (short)values[52];
		MemDb::SetRealData(regStart + 54,(float)data,false);//模拟器输入总无功功率
		tmp = (short)values[53] * 0.001;
		MemDb::SetRealData(regStart + 55,tmp,false);//市电平均功率因数
		tmp = (short)values[54] * 0.1;
		MemDb::SetRealData(regStart + 56,tmp,false);//模拟器输出电压
		tmp = (short)values[60] * 0.1;
		MemDb::SetRealData(regStart + 57,tmp,false);//模拟器输出电流
		data = (short)values[66];
		MemDb::SetRealData(regStart + 58,(float)data,false);//模拟器输出总有功功率
		data = (short)values[70];
		MemDb::SetRealData(regStart + 59,(float)data,false);//模拟器输出总无功功率
		tmp = (short)values[71] * 0.001;
		MemDb::SetRealData(regStart + 60,tmp,false);//模拟器平均功率因数
		tmp = (short)values[72] * 0.1;
		MemDb::SetRealData(regStart + 61,tmp,false);//模拟器有功输出效率
		tmp = (short)values[73] * 0.1;
		MemDb::SetRealData(regStart + 62,tmp,false);//模拟器无功输出效率
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取柴油模拟器地址：%d数据失败", dev->regStart);
	}

	//读取设置反馈值
	start = 0;
	length = 43;
	ret = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		data = (short)values[0];
		MemDb::SetRealData(regStart + 1,data,false);//启动停止
		data = (short)values[1];
		MemDb::SetRealData(regStart + 2,data,false);//工作模式
		tmp = (short)values[2] * 0.1;
		MemDb::SetRealData(regStart + 3,tmp,false);//加速时间
		tmp = (short)values[3] * 0.1;
		MemDb::SetRealData(regStart + 4,tmp,false);//减速时间
		data = (short)values[4];
		MemDb::SetRealData(regStart + 5,data,false);//市电合闸信号
		data = (short)values[8];
		MemDb::SetRealData(regStart + 6,data,false);//并网控制器故障复位
		data = (short)values[20];
		MemDb::SetRealData(regStart + 7,data,false);//功率输出方式
		data = (short)values[21];
		MemDb::SetRealData(regStart + 8,(float)data,false);//有功功率设定值
		data = (short)values[22];
		MemDb::SetRealData(regStart + 9,data,false);//通用曲线类型
		data = (short)values[23];
		MemDb::SetRealData(regStart + 10,data,false);//通用曲线周期
		data = (short)values[24];
		MemDb::SetRealData(regStart + 11,(float)data,false);//通用曲线幅值
		data = (short)values[25];
		MemDb::SetRealData(regStart + 12,data,false);//通用曲线控制周期
		data = (short)values[26];
		MemDb::SetRealData(regStart + 13,data,false);//历史曲线地址
		data = (short)values[27];
		MemDb::SetRealData(regStart + 14,data,false);//功率曲线执行方式
		data = (short)values[28];
		MemDb::SetRealData(regStart + 15,data,false);//曲线开始执行时间
		data = (short)values[29];
		MemDb::SetRealData(regStart + 16,data,false);//开始曲线
		data = (short)values[30];
		MemDb::SetRealData(regStart + 17,data,false);//结束曲线
		data = (short)values[31];
		MemDb::SetRealData(regStart + 18,data,false);//开始曲线执行时间（年）
		data = (short)values[32];
		MemDb::SetRealData(regStart + 19,data,false);//开始曲线执行时间（月）
		data = (short)values[33];
		MemDb::SetRealData(regStart + 20,data,false);//开始曲线执行时间（日）
		data = (short)values[34];
		MemDb::SetRealData(regStart + 21,data,false);//开始曲线执行时间（时）
		data = (short)values[35];
		MemDb::SetRealData(regStart + 22,data,false);//开始曲线执行时间（分）
		data = (short)values[36];
		MemDb::SetRealData(regStart + 23,data,false);//开始曲线执行时间（秒）
		data = (short)values[37];
		MemDb::SetRealData(regStart + 24,data,false);//结束曲线执行时间（年）
		data = (short)values[38];
		MemDb::SetRealData(regStart + 25,data,false);//结束曲线执行时间（月）
		data = (short)values[39];
		MemDb::SetRealData(regStart + 26,data,false);//结束曲线执行时间（日）
		data = (short)values[40];
		MemDb::SetRealData(regStart + 27,data,false);//结束曲线执行时间（时）
		data = (short)values[41];
		MemDb::SetRealData(regStart + 28,data,false);//结束曲线执行时间（分）
		data = (short)values[42];
		MemDb::SetRealData(regStart + 29,data,false);//结束曲线执行时间（秒）
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取柴油模拟器地址：%d数据失败", dev->regStart);
	}

	zlog_debug(Util::m_zlog,"读取中电建投柴油模拟器DGS-4030数据结束");
	return ret;
}

int EpciDieselEngineSimulator::Dgs4030Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;

	//判断网络是否打开
	if (!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcp);

	//判断是否有启动停止命令设置
	ret = SetInt(addr,regStart + 1,0,client);

	//判断是否有工作模式命令设置
	ret = SetInt(addr,regStart + 2,1,client);

	//判断是否有加速时间命令设置
	ret = SetFloatRange10(addr,regStart + 3,2,client,0,20);

	//判断是否有减速时间命令设置
	ret = SetFloatRange10(addr,regStart + 4,3,client,0,20);

	//判断是否有并网控制器故障复位命令设置
	ret = SetInt(addr,regStart + 6,8,client);

	//判断是否有功率输出方式命令设置
	ret = SetInt(addr,regStart + 7,20,client);

	//判断是否有有功功率设定值命令设置
	ret = SetInt(addr,regStart + 8,21,client);

	//判断是否有通用曲线类型命令设置
	ret = SetInt(addr,regStart + 9,22,client);

	//判断是否有通用曲线周期命令设置
	ret = SetInt(addr,regStart + 10,23,client);

	//判断是否有通用曲线幅值命令设置
	ret = SetInt(addr,regStart + 11,24,client);

	//判断是否有通用曲线控制周期命令设置
	ret = SetInt(addr,regStart + 12,25,client);

	//判断是否有历史曲线地址命令设置
	ret = SetInt(addr,regStart + 13,26,client);

	//判断是否有功率曲线执行方式命令设置
	ret = SetInt(addr,regStart + 14,27,client);

	//判断是否有曲线开始执行时间命令设置
	ret = SetInt(addr,regStart + 15,28,client);

	//判断是否有开始曲线命令设置
	ret = SetInt(addr,regStart + 16,29,client);

	//判断是否有结束曲线命令设置
	ret = SetInt(addr,regStart + 17,30,client);

	//判断是否有曲线开始执行时间（年）命令设置
	ret = SetInt(addr,regStart + 18,31,client);

	//判断是否有曲线开始执行时间（月）命令设置
	ret = SetInt(addr,regStart + 19,32,client);

	//判断是否有曲线开始执行时间（日）命令设置
	ret = SetInt(addr,regStart + 20,33,client);

	//判断是否有曲线开始执行时间（时）命令设置
	ret = SetInt(addr,regStart + 21,34,client);

	//判断是否有曲线开始执行时间（分）命令设置
	ret = SetInt(addr,regStart + 22,35,client);

	//判断是否有曲线开始执行时间（秒）命令设置
	ret = SetInt(addr,regStart + 23,36,client);

	//判断是否有曲线结束执行时间（年）命令设置
	ret = SetInt(addr,regStart + 24,37,client);

	//判断是否有曲线结束执行时间（月）命令设置
	ret = SetInt(addr,regStart + 25,38,client);

	//判断是否有曲线结束执行时间（日）命令设置
	ret = SetInt(addr,regStart + 26,39,client);

	//判断是否有曲线结束执行时间（时）命令设置
	ret = SetInt(addr,regStart + 27,40,client);

	//判断是否有曲线结束执行时间（分）命令设置
	ret = SetInt(addr,regStart + 28,41,client);

	//判断是否有曲线结束执行时间（秒）命令设置
	ret = SetInt(addr,regStart + 29,42,client);

	zlog_info(Util::m_zlog,"设置命令结束");
	return ret;
}

int EpciDieselEngineSimulator::SetInt(int addr,int regStart,int start,ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;

	if (MemDb::GetRealDataWrited(realData, regStart, true))
	{
		zlog_warn(Util::m_zlog,"设置命令柴油发电机模拟器状态数据");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr, start, realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			ret = client.PresetSingleRegister(addr, start, realData.pv.data.dbValue);
		}
	}

	return ret;
}

int EpciDieselEngineSimulator::SetFloat10(int addr,int regStart,int start,ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart, true))
	{
		zlog_warn(Util::m_zlog,"设置命令柴油发电机模拟器设定值数据");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue * 10;
		}
		else if(realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue * 10);
		}
		ret = client.PresetSingleRegister(addr,start,tmp);
	}

	return ret;
}

int EpciDieselEngineSimulator::SetFloatRange10(int addr,int regStart,int start,ModbusTcpClient client,int minValue,int maxValue)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart, true))
	{
		zlog_warn(Util::m_zlog,"设置命令柴油发电机模拟器设定值数据");
		if (realData.pv.type == RealData::INT)
		{
			if((realData.pv.data.iValue >= minValue)
					&& (realData.pv.data.iValue <= maxValue))
			{
				tmp = realData.pv.data.iValue * 10;
				ret = client.PresetSingleRegister(addr,start,tmp);
			}

		}
		else if(realData.pv.type == RealData::DB)
		{
			if((realData.pv.data.dbValue >= minValue)
					&& (realData.pv.data.dbValue <= maxValue))
			{
				tmp = (int)(realData.pv.data.dbValue * 10);
				ret = client.PresetSingleRegister(addr,start,tmp);
			}
		}
	}
	return ret;
}
