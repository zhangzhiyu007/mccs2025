/*
 * PvAcLoad.cpp
 *
 *  Created on: 2013-10-21
 *      Author: liuhaitao
 */

#include "PvAcLoad.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

PvAcLoad::PvAcLoad()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

PvAcLoad::~PvAcLoad()
{
	// TODO Auto-generated destructor stub
}

void PvAcLoad::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void PvAcLoad::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int PvAcLoad::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PV_RLC_30:
		ret = this->PvRLCRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int PvAcLoad::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PV_RLC_30:
		ret = this->PvRLCPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int PvAcLoad::PvRLCRead(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取群菱RLC负载数据开始");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	int tmp = 0;
	float data = 0;

	if(NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断网口是否打开
	if(!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient tcp;
	tcp.SetTcpClient(this->m_tcp);


	UshortArray values;
	values.clear();
	int start = 0;
	int length = 51;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;

	ret = tcp.ReadOutputRegisters(values,addr,start,length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;

		//保存控制数据反馈值
		tmp = (short)values[0];
		MemDb::SetRealData(regStart+1,tmp,false);//运行控制
		tmp = (short)values[1];
		MemDb::SetRealData(regStart+2,tmp,false);//执行方式
		tmp = (short)values[4];
		MemDb::SetRealData(regStart+5,(float)tmp,false);//A相R功率设定值
		tmp = (short)values[5];
		MemDb::SetRealData(regStart+6,(float)tmp,false);//A相C功率设定值
		tmp = (short)values[6];
		MemDb::SetRealData(regStart+7,(float)tmp,false);//A相L功率设定值
		tmp = (short)values[7];
		MemDb::SetRealData(regStart+8,(float)tmp,false);//B相R功率设定值
		tmp = (short)values[8];
		MemDb::SetRealData(regStart+9,(float)tmp,false);//B相C功率设定值
		tmp = (short)values[9];
		MemDb::SetRealData(regStart+10,(float)tmp,false);//B相L功率设定值
		tmp = (short)values[10];
		MemDb::SetRealData(regStart+11,(float)tmp,false);//C相R功率设定值
		tmp = (short)values[11];
		MemDb::SetRealData(regStart+12,(float)tmp,false);//C相C功率设定值
		tmp = (short)values[12];
		MemDb::SetRealData(regStart+13,(float)tmp,false);//C相L功率设定值

		//保存负载实时数据
		tmp = (short)values[14];
		MemDb::SetRealData(regStart+36,tmp,false);//运行状态
		tmp = (short)values[15];
		MemDb::SetRealData(regStart+37,tmp,false);//负载通讯状态
		tmp = (short)values[18];
		MemDb::SetRealData(regStart+40,(float)tmp,false);//A相有功功率
		tmp = (short)values[19];
		MemDb::SetRealData(regStart+41,(float)tmp,false);//A相无功功率
		tmp = (short)values[20];
		MemDb::SetRealData(regStart+42,(float)tmp,false);//B相有功功率
		tmp = (short)values[21];
		MemDb::SetRealData(regStart+43,(float)tmp,false);//B相无功功率
		tmp = (short)values[22];
		MemDb::SetRealData(regStart+44,(float)tmp,false);//C相有功功率
		tmp = (short)values[23];
		MemDb::SetRealData(regStart+45,(float)tmp,false);//C相无功功率
		data = ((short)values[24]) * 0.1;
		MemDb::SetRealData(regStart+46,(float)data,false);//A相电压
		data = ((short)values[25]) * 0.1;
		MemDb::SetRealData(regStart+47,(float)data,false);//B相电压
		data = ((short)values[26]) * 0.1;
		MemDb::SetRealData(regStart+48,(float)data,false);//C相电压
		data = ((short)values[27])*0.1;
		MemDb::SetRealData(regStart+49,data,false);//A相电流
		data = ((short)values[28])*0.1;
		MemDb::SetRealData(regStart+50,data,false);//B相电流
		data = ((short)values[29])*0.1;
		MemDb::SetRealData(regStart+51,data,false);//C相电流

		//设置曲线反馈值
		tmp = (short)values[30];
		MemDb::SetRealData(regStart+14,tmp,false);//曲线执行时间
		tmp = (short)values[31];
		MemDb::SetRealData(regStart+15,tmp,false);//曲线执行方式
		tmp = (short)values[32];
		MemDb::SetRealData(regStart+16,tmp,false);//优先级
		tmp = (short)values[33];
		MemDb::SetRealData(regStart+17,tmp,false);//开始曲线
		tmp = (short)values[34];
		MemDb::SetRealData(regStart+18,tmp,false);//结束曲线
		tmp = (short)values[35];
		MemDb::SetRealData(regStart+19,tmp,false);//年（开始时间）
		tmp = (short)values[36];
		MemDb::SetRealData(regStart+20,tmp,false);//月（开始时间）
		tmp = (short)values[37];
		MemDb::SetRealData(regStart+21,tmp,false);//日（开始时间）
		tmp = (short)values[38];
		MemDb::SetRealData(regStart+22,tmp,false);//时（开始时间）
		tmp = (short)values[39];
		MemDb::SetRealData(regStart+23,tmp,false);//分（开始时间）
		tmp = (short)values[40];
		MemDb::SetRealData(regStart+24,tmp,false);//秒（开始时间）
		tmp = (short)values[41];
		MemDb::SetRealData(regStart+25,tmp,false);//年（结束时间）
		tmp = (short)values[42];
		MemDb::SetRealData(regStart+26,tmp,false);//月（结束时间）
		tmp = (short)values[43];
		MemDb::SetRealData(regStart+27,tmp,false);//日（结束时间）
		tmp = (short)values[44];
		MemDb::SetRealData(regStart+28,tmp,false);//时（结束时间）
		tmp = (short)values[45];
		MemDb::SetRealData(regStart+29,tmp,false);//分（结束时间）
		tmp = (short)values[46];
		MemDb::SetRealData(regStart+30,tmp,false);//秒（结束时间）
		tmp = (short)values[47];
		MemDb::SetRealData(regStart+31,tmp,false);//通用曲线类型
		tmp = (short)values[48];
		MemDb::SetRealData(regStart+32,tmp,false);//通用曲线周期
		tmp = (short)values[49];
		MemDb::SetRealData(regStart+33,tmp,false);//通用曲线幅值
		tmp = (short)values[50];
		MemDb::SetRealData(regStart+34,tmp,false);//通用曲线类型控制周期

		zlog_info(Util::m_zlog,"获取RLC负载数据结束");
	}
	else
	{
		zlog_warn(Util::m_zlog, "地址%d读取读取数据失败，结果=%d",addr,ret);
	}
	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

	zlog_info(Util::m_zlog,"获取RLC负载数据结束");
	return ret;
}

int PvAcLoad::PvRLCPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	UshortArray data;
	data.clear();

	//判断网口是否打开
	if(!m_tcp->IsOpened())
	{
		zlog_warn(Util::m_zlog,"设置RLC负载：以太网没有打开");
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient tcp;
	tcp.SetTcpClient(this->m_tcp);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	int regStart = dev->regStart;
	int tmp = 0;

	//判断是否有运行控制命令

	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,0,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_info(Util::m_zlog,"RLC负载加载/卸载命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有执行方式命令
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		//zlog_warn(Util::m_zlog,"设置RLC负载：执行方式命令");
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,1,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"RLC负载执行方式命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}

		//zlog_warn(Util::m_zlog,"设置RLC负载：执行方式命令=%d",regStart + 2);
	}

	//判断是否有A相R功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,4,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,4,tmp);
		}
	}

	//判断是否有A相C功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 6, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,5,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,5,tmp);
		}
	}

	//判断是否有A相L功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 7, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,6,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,6,tmp);
		}
	}

	//判断是否有B相R功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 8, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,7,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,7,tmp);
		}
	}

	//判断是否有B相C功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 9, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,8,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,8,tmp);
		}
	}

	//判断是否有B相L功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 10, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,9,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,9,tmp);
		}
	}

	//判断是否有C相R功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 11, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,10,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,10,tmp);
		}
	}

	//判断是否有C相C功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 12, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,11,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,11,tmp);
		}
	}

	//判断是否有C相L功率设定值命令
	if (MemDb::GetRealDataWrited(realData, regStart + 13, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
			ret = tcp.PresetSingleRegister(addr,12,tmp);
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
			ret = tcp.PresetSingleRegister(addr,12,tmp);
		}
	}

	//判断是否曲线执行时间控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 14, true))
	{
		zlog_error(Util::m_zlog,"曲线执行时间控制命令：%d",realData.pv.type);
		if (realData.pv.type == RealData::INT)
		{
			zlog_error(Util::m_zlog,"曲线执行时间控制命令：%d",realData.pv.data.iValue);
			ret = tcp.PresetSingleRegister(addr,30,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载曲线执行时间命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有曲线执行方式控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 15, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			zlog_error(Util::m_zlog,"曲线执行方式控制命令：%d",realData.pv.data.iValue);
			ret = tcp.PresetSingleRegister(addr,31,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载曲线执行方式命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有优先级控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 16, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,32,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载优先级命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有开始曲线控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 17, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,33,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载开始曲线命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有结束曲线控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 18, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,34,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载结束曲线命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有开始时间年控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 19, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,35,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载开始时间年命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有开始时间月控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 20, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,36,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载开始时间月命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有开始时间日控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 21, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,37,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载开始时间日命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有开始时间时控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 22, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,38,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载开始时间时命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有开始时间分控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 23, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,39,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载开始时间分命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有开始时间秒控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 24, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,40,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_info(Util::m_zlog,"RLC负载开始时间秒命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有结束时间年控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 25, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,41,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载结束时间年命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有结束时间月控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 26, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,42,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载结束时间月命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有结束时间日控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 27, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,43,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载结束时间日命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有结束时间时控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 28, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,44,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载结束时间时命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有结束时间分控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 29, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,45,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载结束时间分命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有结束时间秒控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 30, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,46,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载结束时间秒命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有通用曲线类型控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 31, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,47,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载通用曲线类型命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有通用曲线周期控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 32, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,48,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载通用曲线周期命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有通用曲线幅值控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 33, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,49,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载通用曲线幅值命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有通用曲线控制周期控制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 34, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = tcp.PresetSingleRegister(addr,50,realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"RLC负载通用曲线控制周期命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	zlog_info(Util::m_zlog,"RLC负载设置结束");
	return ret;
}
