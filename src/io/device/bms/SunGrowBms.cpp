/*
 * SunGrowBms.cpp
 *
 *  Created on: 2013-10-15
 *      Author: liuhaitao
 */

#include "SunGrowBms.h"

#include "../../com/Com.h"

SunGrowBms::SunGrowBms()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

SunGrowBms::~SunGrowBms()
{
	// TODO Auto-generated destructor stub
}

void SunGrowBms::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

int SunGrowBms::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 24:
		ret = this->SunGrow30KWRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SunGrowBms::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 24:
		ret = this->SunGrow30KWPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SunGrowBms::SunGrow30KWRead(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取BMS数据开始");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0001;
	int length = 20;
	float tmp = 0;
	int data = 0;
	UshortArray values;
	values.clear();
	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

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

	//读取BMS数据
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = ((short)values[0])*0.1;
		zlog_debug(Util::m_zlog,"系统总压:%.2f",tmp);
		MemDb::SetRealData(regStart+12,tmp,false);//系统总压
		tmp = ((short)values[1])*0.1;
		zlog_debug(Util::m_zlog,"SOC:%.2f",tmp);
		MemDb::SetRealData(regStart+17,tmp,false);//SOC
		tmp = (short)(values[2]*0.01-300);
		zlog_debug(Util::m_zlog,"系统电流:%.2f",tmp);
		MemDb::SetRealData(regStart+13,tmp,false);//系统电流
		tmp = (short)(values[4]*0.1 - 40);
		zlog_debug(Util::m_zlog,"最高温度:%.2f",tmp);
		MemDb::SetRealData(regStart+22,tmp,false);//最高温度
		tmp = (short)(values[5]*0.1 - 40);
		zlog_debug(Util::m_zlog,"最低温度:%.2f",tmp);
		MemDb::SetRealData(regStart+23,tmp,false);//最低温度
		tmp = ((short)values[6])*0.001;
		zlog_debug(Util::m_zlog,"最高电压:%.2f",tmp);
		MemDb::SetRealData(regStart+20,tmp,false);//最高电压
		tmp = ((short)values[7])*0.001;
		zlog_debug(Util::m_zlog,"最低电压:%.2f",tmp);
		MemDb::SetRealData(regStart+21,tmp,false);//最低电压
		data = ((((short)values[10])&0xFFFF)<<16)|(((short)values[11])&0xFFFF);
		zlog_debug(Util::m_zlog,"高电压箱内位置:%d",data);
		MemDb::SetRealData(regStart+24,data,false);//最高电压箱号/箱内位置
		data = ((((short)values[12])&0xFFFF)<<16)|(((short)values[13])&0xFFFF);
		zlog_debug(Util::m_zlog,"低电压箱内位置:%d",data);
		MemDb::SetRealData(regStart+25,data,false);//最低电压箱号/箱内位置
		data = ((((short)values[14])&0xFFFF)<<16)|(((short)values[15])&0xFFFF);
		zlog_debug(Util::m_zlog,"高温度箱内位置:%d",data);
		MemDb::SetRealData(regStart+26,data,false);//最高温度箱号/箱内位置
		data = ((((short)values[16])&0xFFFF)<<16)|(((short)values[17])&0xFFFF);
		zlog_debug(Util::m_zlog,"低温度箱内位置:%d",data);
		MemDb::SetRealData(regStart+27,data,false);//最低温度箱号/箱内位置
		data = 0;
		//		zlog_info(Util::m_zlog,"电池状态：%d,%d",values[18],values[19]);
		if (values[18] == 1)//禁止充电
		{
			data = data & 0xFFFFFFFE;
		}
		else if (values[18] == 0)//允许充电
		{
			data = data | 0x00000001;
		}
		if (values[19] == 1)//禁止放电
		{
			data = data & 0xFFFFFFFD;
		}
		else if (values[19] == 0)//允许放电
		{
			data = data | 0x00000002;
		}
		//		zlog_info(Util::m_zlog,"电池状态：%d",data);
		MemDb::SetRealData(regStart + 4, data, false);//电池状态

		data = 0;
		//故障信息
		if (((values[8] & 0x0001) > 0) || ((values[9] & 0x0001) > 0))
		{
			data = data | 0x00000008;//电池串欠压告警
		}
		if (((values[8] & 0x0002) > 0) || ((values[9] & 0x0002) > 0))
		{
			data = data | 0x00000002;//电池串过压告警
		}
		if (((values[8] & 0x0010) > 0) || ((values[9] & 0x0010) > 0))
		{
			data = data | 0x00000020;//电池模块低温告警
		}
		if (((values[8] & 0x0020) > 0) || ((values[9] & 0x0020) > 0))
		{
			data = data | 0x00000010;//电池模块高温告警
		}
		if (((values[8] & 0x0040) > 0) || ((values[9] & 0x0040) > 0))
		{
			data = data | 0x00000004;//单体电池欠压告警
		}
		if (((values[8] & 0x0080) > 0) || ((values[9] & 0x0080) > 0))
		{
			data = data | 0x00000001;//单体电池过压告警
		}
		if (((values[8] & 0x1000) > 0) || ((values[9] & 0x1000) > 0))
		{
			data = data | 0x02000000;//BMS与PCS通信故障
		}
		if (((values[8] & 0x0400) > 0)//子CAN通信丢失警告
				|| ((values[8] & 0x0800) > 0)//高压板通信丢失警告
				|| ((values[8] & 0x2000) > 0)//烟雾警告
				|| ((values[9] & 0x0400) > 0)//子CAN通信丢失故障
				|| ((values[9] & 0x0800) > 0)//高压板通信丢失故障
				|| ((values[9] & 0x2000) > 0))//烟雾故障
		{
			data = data | 0x01000000;//BMS装置内部异常报警
		}
		if (data != 0)
		{
			zlog_info(Util::m_zlog,"故障：%d",data);
			MemDb::SetRealData(regStart+8,1,false);//故障
			MemDb::SetRealData(regStart+9,data,false);//故障码1
			MemDb::SetRealData(regStart+10,0,false);//故障码2
		}
		else
		{
			MemDb::SetRealData(regStart + 8, 0, false);//无故障
			MemDb::SetRealData(regStart + 9, data, false);//故障码1
			MemDb::SetRealData(regStart + 10, 0, false);//故障码2
		}
	}
	else
	{
		zlog_warn(Util::m_zlog,"地址%d读取读取实时数据失败，结果=%d",addr,ret);
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int SunGrowBms::SunGrow30KWPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	return ret;
}
