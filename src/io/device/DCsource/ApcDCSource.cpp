/*
 * ApcDCSource.cpp
 *
 *  Created on: 2014-1-11
 *      Author: liuhaitao
 */

#include "ApcDCSource.h"

#include "../../com/Com.h"

ApcDCSource::ApcDCSource()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_delayTime = 1000;
	m_retries = 3;
}

ApcDCSource::~ApcDCSource()
{
	// TODO Auto-generated destructor stub
}

void ApcDCSource::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

int ApcDCSource::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 27://DCST-30
		ret = this->Dcst30Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ApcDCSource::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 27://DCST-30
		this->Dcst30Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ApcDCSource::Dcst30Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取直流稳定电源数据");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int data = 0;
	int data1 = 0;
	float tmp = 0;
	int length = 0;
	int function = 0;

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

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	length = 2;
	function = 0x04;
	values.push_back(0x01);
	ret = rtu.PresetDCSourceRegisters(addr,function,length,values);
	if(ret == ErrorInfo::ERR_OK)
	{
		//zlog_warn(Util::m_zlog,"远控成功");
	}

	//读取实时数据
	zlog_info(Util::m_zlog,"读取实时数据");
	//读取三次，如果三次都不成功认为通讯失败
	for(int i =0;i<3;i++)
	{
		ret = rtu.ReadDCSourceRegisters(values, addr, 0, 9);
		if(ret == ErrorInfo::ERR_OK)
		{
			break;
		}
	}

	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = ((short)(values[0]<<8))|((short)values[1]);//故障信息

		if((data&0x00000001)>0)//过压
		{
			data1 = data1|0x00000004;
		}
		if((data&0x00000002)>0)//过温
		{
			data1 = data1|0x00000008;
		}
		if((data&0x00000004)>0)//过流
		{
			data1 = data1|0x00000010;
		}

		if(data1>0)//故障
		{
			MemDb::SetRealData(regStart + 12, 1, false);
			MemDb::SetRealData(regStart + 13, data1, false);
		}
		else
		{
			MemDb::SetRealData(regStart + 12, 0, false);
			MemDb::SetRealData(regStart + 13, data1, false);
		}

		float udc=0.0;
		float idc=0.0;
		tmp = (short)values[6]*0.1;
		if(tmp < 10)
		{
			tmp = 0;//防止精度不准确
		}
		MemDb::SetRealData(regStart + 14, tmp, false);//电压输出值
		udc=tmp;
		tmp = (short)values[7]*0.1;
		MemDb::SetRealData(regStart + 15, tmp, false);//电流输出值
		idc=tmp;
		MemDb::SetRealData(regStart + 16, (float)(udc*idc), false);//直流功率输出值

		tmp = (short)values[8]*0.1;
		MemDb::SetRealData(regStart + 17, tmp, false);//A相电压输入

		tmp = (short)values[9]*0.1;
		MemDb::SetRealData(regStart + 18, tmp, false);//B相电压输入

		tmp = (short)values[10]*0.1;
		MemDb::SetRealData(regStart + 19, tmp, false);//C相电压输入
	}
	else
	{
		zlog_warn(Util::m_zlog,"直流源通信失败");
	}

	MemDb::SetRealData(regStart,comm,false);//通讯状态
	return ret;
}

int ApcDCSource::Dcst30Preset(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"设置直流稳定电源数据");
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int length = 0;
	int function = 0;
	int data = 0;
	RealData realData;
	RealData realDatai;
	RealData realDatav;

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	//判断是否有启停设置
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			if(realData.pv.data.iValue == 1)//启动
			{
				values.push_back(0x14);
				length = 0x02;
				function = 0x01;
			}
			else if(realData.pv.data.iValue == 0)//停止
			{
				values.push_back(0x00);
				length = 0;
				function = 0x02;
			}
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"启停命令命令错误");
			ret = ErrorInfo::ERR_WRITE;
		}

		ret = rtu.PresetDCSourceRegisters(addr,function,length,values);
	}

	//判断是否有运行模式、电压、电流变化
	if ((MemDb::GetRealDataWrited(realData, regStart + 2, true))
				|| (MemDb::GetRealDataWrited(realDatav, regStart + 3, true))
				|| (MemDb::GetRealDataWrited(realDatai, regStart + 4, true)))
	{
		MemDb::GetRealData(realData, regStart + 2);//运行模式
		MemDb::GetRealData(realDatav, regStart + 3);//电压
		MemDb::GetRealData(realDatai, regStart + 4);//电流
		values.resize(3);

		if (realData.pv.type == RealData::INT)
		{
			if(realData.pv.data.iValue == 1)//恒压模式
			{
//				zlog_error(Util::m_zlog,"恒压模式");
				values[0] = 0x07;//功能标志

			}
			else if(realData.pv.data.iValue == 2)//恒流模式
			{
//				zlog_error(Util::m_zlog,"恒流模式");
				values[0] = 0x06;//功能标志
			}
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_info(Util::m_zlog,"设定模式命令错误");
			ret = ErrorInfo::ERR_WRITE;
		}

		if (realDatav.pv.type == RealData::INT)
		{
//			zlog_error(Util::m_zlog,"电压值%d",realDatav.pv.data.iValue);
			data = realDatav.pv.data.iValue*10;
			values[2] = data;
		}
		else if(realDatav.pv.type == RealData::DB)
		{
//			zlog_error(Util::m_zlog,"电压值%.2f",realDatav.pv.data.dbValue);
			data = (int)(realDatav.pv.data.dbValue*10);
			values[2] = data;
		}

		if (realDatai.pv.type == RealData::INT)
		{
//			zlog_error(Util::m_zlog,"电流值%d",realDatai.pv.data.iValue);
			data = realDatai.pv.data.iValue*10;
			values[1] = data;
		}
		else if(realDatai.pv.type == RealData::DB)
		{
//			zlog_error(Util::m_zlog,"电流值%.2f",realDatai.pv.data.dbValue);
			data = (int)(realDatai.pv.data.dbValue*10);
			values[1] = data;
		}
//		for(int i = 0;i<values.size();i++)
//		{
//			zlog_error(Util::m_zlog,"values[%d] = %d",i,values[i]);
//		}

		ret = rtu.PresetDCSourceRegisters(addr, 0x41, 0x0A, values);
	}

	zlog_info(Util::m_zlog,"设置直流稳定电压结束");
	return ret;
}

