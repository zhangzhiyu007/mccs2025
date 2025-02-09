/*
 * SunGrowPvs.cpp
 *
 *  Created on: 2013-10-16
 *      Author: liuhaitao
 */

#include "SunGrowPvs.h"

#include "../../com/Com.h"

SunGrowPvs::SunGrowPvs()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

SunGrowPvs::~SunGrowPvs()
{
	// TODO Auto-generated destructor stub
}

void SunGrowPvs::SetCom(Com* com)
{
	assert(com!=NULL);
	m_com = com;
}

int SunGrowPvs::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 21:
		ret = this->Sg30KtlRead(dev);
		break;
	case 25:
		ret = this->Sg20KtlRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SunGrowPvs::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 21:
		ret = this->Sg30KtlPreset(dev);
		break;
	case 25:
		ret = this->Sg20KtlPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SunGrowPvs::Sg30KtlRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = this->SgKtlRead(dev);
	return ret;
}

int SunGrowPvs::Sg30KtlPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = this->SgKtlPreset(dev);
	return ret;
}

int SunGrowPvs::Sg20KtlRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = this->SgKtlRead(dev);
	return ret;
}

int SunGrowPvs::Sg20KtlPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = this->SgKtlPreset(dev);
	return ret;
}

int SunGrowPvs::SgKtlRead(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取合肥阳光PVS数据");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int start = 4999;
	int length = 46;
	int regStart = dev->regStart;
	float tmp = 0;
	int data = 0;
	int tmp1 = 0;

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

	//读取PVS数据
	//读取三次，如果三次都不成功认为通讯失败
	for(int i =0;i<3;i++)
	{
		ret = rtu.ReadInputRegisters(values,addr,start,length);
		if(ret == ErrorInfo::ERR_OK)
		{
			break;
		}
	}

	if (ErrorInfo::ERR_OK == ret)
	{
		//保存通讯正常
		comm = true;

		tmp = ((short)values[3]) * 0.1;
		MemDb::SetRealData(regStart + 9, tmp, false);//日发电量
		data = ((((short)values[5])&0xFFFF)<<16)|(((short)values[4])&0xFFFF);
		MemDb::SetRealData(regStart + 10, (float)data, false);//总发电量
		data = ((((short)values[7])&0xFFFF)<<16)|(((short)values[6])&0xFFFF);
		MemDb::SetRealData(regStart + 12, (float)data, false);//总运行时间
		tmp = ((short) values[8]) * 0.1;
		MemDb::SetRealData(regStart + 13, tmp, false);//机内空气温度
		tmp = ((short)values[11]) * 0.1;
		MemDb::SetRealData(regStart + 14, tmp, false);//直流电压
		tmp = ((short)values[12]) * 0.1 + ((short)values[14]) * 0.1;
		MemDb::SetRealData(regStart + 15, tmp, false);//直流电流
		data = ((((short)values[18])&0xFFFF)<<16)|(((short)values[17])&0xFFFF);
		MemDb::SetRealData(regStart + 16, (float)data, false);//总直流功率
		if(((short)values[2]) == 0)//两相
		{
			zlog_info(Util::m_zlog,"两相");
		}
		else if(((short)values[2]) == 1)//三相四线
		{
			zlog_info(Util::m_zlog,"三相四线");

			tmp = ((short)values[19]) * 0.1;
			MemDb::SetRealData(regStart + 17, tmp, false);//A相电压
			tmp = ((short)values[20]) * 0.1;
			MemDb::SetRealData(regStart + 18, tmp, false);//B相电压
			tmp = ((short)values[21]) * 0.1;
			MemDb::SetRealData(regStart + 19, tmp, false);//C相电压
			tmp = ((short)values[22]) * 0.1;
			MemDb::SetRealData(regStart + 20, tmp, false);//A相电流
			tmp = ((short)values[23]) * 0.1;
			MemDb::SetRealData(regStart + 21, tmp, false);//B相电流
			tmp = ((short)values[24]) * 0.1;
			MemDb::SetRealData(regStart + 22, tmp, false);//C相电流
		}
		else if(((short)values[2]) == 2)//三相三线
		{
			zlog_info(Util::m_zlog,"三相三线");

			tmp = ((short)values[22]) * 0.1;
			MemDb::SetRealData(regStart + 20, tmp, false);//A相电流
			tmp = ((short)values[23]) * 0.1;
			MemDb::SetRealData(regStart + 21, tmp, false);//B相电流
			tmp = ((short)values[24]) * 0.1;
			MemDb::SetRealData(regStart + 22, tmp, false);//C相电流
		}
		data = ((((short)values[32])&0xFFFF)<<16)|(((short)values[31])&0xFFFF);
		MemDb::SetRealData(regStart + 26, (float)data, false);//总有功功率
		data = ((((short)values[34])&0xFFFF)<<16)|(((short)values[33])&0xFFFF);
		MemDb::SetRealData(regStart + 27, (float)data, false);//总无功功率
		tmp = ((short)values[35])*0.001;
		MemDb::SetRealData(regStart + 28, tmp, false);//功率因数
		tmp = ((short)values[36]) * 0.1;
		MemDb::SetRealData(regStart + 29, tmp, false);//电网频率
		tmp = ((short)values[37]) * 0.1;
		MemDb::SetRealData(regStart + 30, tmp, false);//逆变器效率

		//故障信息
		zlog_info(Util::m_zlog,"设备状态 = %02x",values[38]);
		zlog_info(Util::m_zlog,"状态数据1 = %02x",values[45]);

		if ((values[38] == 0x0000) || (values[38] == 0x8000)
				|| (values[38] == 0x1200) || (values[38] == 0x1300)
				|| (values[38] == 0x1400) || (values[38] == 0x1500)
				|| (values[38] == 0x1600) || (values[38] == 0x5000)
				|| (values[38] == 0x5100) || (values[38] == 0x5200)
				|| (values[38] == 0x5300) || (values[38] == 0x5400)
				|| (values[38] == 0x8100) || (values[38] == 0x8200)
				|| (values[38] == 0xFFFF) || (values[38] == 0x4013))
		{
			data = values[38];
			switch(data)
			{
			case 0x0000://运行
				MemDb::SetRealData(regStart + 5, 1, false);//运行
				break;
			case 0x8000://停机
				MemDb::SetRealData(regStart + 5, 0, false);//停机
				break;
			case 0x1200://初始待机
				MemDb::SetRealData(regStart + 5, 2, false);//初始待机
				break;
			case 0x1300://按键关机
				MemDb::SetRealData(regStart + 5, 3, false);//按键关机
				break;
			case 0x1400://待机
				MemDb::SetRealData(regStart + 5, 4, false);//待机
				break;
			case 0x1500://紧急停机
				MemDb::SetRealData(regStart + 5, 5, false);//紧急停机
				break;
			case 0x1600://启动中
				MemDb::SetRealData(regStart + 5, 6, false);//启动中
				break;
			case 0x8100://降额运行
				MemDb::SetRealData(regStart + 5, 7, false);//降额运行
				break;
			case 0x8200://调度运行
				MemDb::SetRealData(regStart + 5, 8, false);//调度运行
				break;
			case 0x9100://告警运行
				MemDb::SetRealData(regStart + 5, 9, false);//告警运行
				break;
			default:
				break;
			}
			MemDb::SetRealData(regStart + 7, 0, false);//无故障
			MemDb::SetRealData(regStart + 8, 0, false);//故障码
		}
		else if((values[38] == 0x5500)||(values[38] == 0x9100))//故障、告警运行
		{
			if((values[45] == 2)||(values[45] == 3))//电网过压
			{
				tmp1 = tmp1 | 0x00010000;
			}
			else if((values[45] == 4)||(values[45] == 5))//电网欠压
			{
				tmp1 = tmp1 | 0x00020000;
			}
			else if((values[45] == 6)||(values[45] == 7))//交流过流
			{
				tmp1 = tmp1 | 0x00400000;
			}
			else if(values[45] == 8)//电网过频
			{
				tmp1 = tmp1 | 0x00040000;
			}
			else if(values[45] == 9)//电网欠频
			{
				tmp1 = tmp1 | 0x00080000;
			}
			else if(values[45] == 10)//孤岛
			{
				tmp1 = tmp1 | 0x00000400;
			}
			else if((values[45] == 11)||(values[45] == 21)
					||(values[45] == 22))//直流过流
			{
				tmp1 = tmp1 | 0x00008000;
			}
			else if(values[45] == 12)//接地故障
			{
				tmp1 = tmp1 | 0x00000200;
			}
			else if((values[45] == 16)||(values[45] == 76)
					||(values[45] == 77))//过载保护
			{
				tmp1 = tmp1 | 0x01000000;
			}
			else if((values[45] == 36)||(values[45] == 37))//过温保护
			{
				tmp1 = tmp1 | 0x20000000;
			}
			else if(values[45] == 40)//IGBT故障
			{
				tmp1 = tmp1 | 0x00000002;
			}
			else if((values[45] == 41)||(values[45] == 48)
					||(values[45] == 49)||(values[45] == 50))//采样通道故障
			{
				tmp1 = tmp1 | 0x00000010;
			}
			else if(values[45] == 42)//三相电流不平衡
			{
				tmp1 = tmp1 | 0x00800000;
			}

			MemDb::SetRealData(regStart + 7, 1, false);//故障
			MemDb::SetRealData(regStart + 8, tmp1, false);//故障码
		}
	}
	else
	{
		if(addr == 1)
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}
	}

	//读取设定反馈值
	start = 4999;
	length = 20;
	//读取三次，如果三次都不成功认为通讯失败
	for(int i =0;i<3;i++)
	{
		ret = rtu.ReadOutputRegisters(values, addr, start, length);
		if(ret == ErrorInfo::ERR_OK)
		{
			break;
		}
	}

	if (ErrorInfo::ERR_OK == ret)
	{
		//保存通讯正常
		comm = true;
		if (values[6] == 0xCF)
		{
			MemDb::SetRealData(regStart + 1, 1, false);//启动
		}
		else if (values[6] == 0xCE)
		{
			MemDb::SetRealData(regStart + 1, 0, false);//停止
		}
		tmp = ((short)values[8]) * 0.1;
		MemDb::SetRealData(regStart + 3, tmp, false);//限功率设置
		tmp = ((short)values[19]) * 0.1;
		MemDb::SetRealData(regStart + 4, tmp, false);//功率因数设置
	}
	else
	{
		if(addr == 1)
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}
	}
	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int SunGrowPvs::SgKtlPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	UshortArray data;
	data.clear();
	int tmp = 0;
	float tmp1 = 0.0;

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	int regStart = dev->regStart;

	//判断是否有启停命令
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		zlog_info(Util::m_zlog,"设置启动停止");
		if (realData.pv.type == RealData::INT)
		{
			if (realData.pv.data.iValue == 1)
			{
				//启动
				ret = rtu.PresetSingleRegister(addr, 5005, 0xCF);
			}
			else if (realData.pv.data.iValue == 0)
			{
				//停止
				ret = rtu.PresetSingleRegister(addr, 5005, 0xCE);
			}
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_info(Util::m_zlog,"发送数据有错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否有限功率限制命令
	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		zlog_info(Util::m_zlog,"设置有功功率限制");
		if (realData.pv.type == RealData::INT)
		{
			//限功率开关
			ret = rtu.PresetSingleRegister(addr, 5006, 0xAA);
			//限功率设置
			if (realData.pv.data.iValue == 0)
			{
				tmp = 1;
			}
			else
			{
				tmp = realData.pv.data.iValue;
			}
			ret = rtu.PresetSingleRegister(addr, 5007, tmp * 10);
		}
		else if (realData.pv.type == RealData::DB)
		{

			//开启限功率开关
			ret = rtu.PresetSingleRegister(addr, 5006, 0xAA);
			//限功率设置
			if ((int) realData.pv.data.dbValue == 0)
			{
				tmp1 = 1.0;
			}
			else
			{
				tmp1 = realData.pv.data.dbValue;
			}
			ret = rtu.PresetSingleRegister(addr, 5007, tmp1 * 10);
		}
	}

	//判断是否有功率因数设置命令
	if (MemDb::GetRealDataWrited(realData, regStart + 4, true))
	{
		zlog_info(Util::m_zlog,"设置有功功率因数");
		if (realData.pv.type == RealData::INT)
		{
			//功率因数设置
			ret = rtu.PresetSingleRegister(addr, 5018, realData.pv.data.iValue
					* 10);
		}
		else if (realData.pv.type == RealData::DB)
		{
			//功率因数设置
			ret = rtu.PresetSingleRegister(addr, 5018, realData.pv.data.iValue
					* 10);
		}
	}

	zlog_info(Util::m_zlog,"设置PVS参数结束");
	return ret;
}
