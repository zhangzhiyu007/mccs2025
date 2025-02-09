/*
 * HuaWeiPvs.cpp
 *
 *  Created on: 2014-12-25
 *      Author: liuhaitao
 */

#include "HuaWeiPvs.h"

#include "../../com/Com.h"

HuaWeiPvs::HuaWeiPvs()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_limitPower = 0;
	m_powerFlag = 0;
	m_controlStart.Initialize();
}

HuaWeiPvs::~HuaWeiPvs()
{
	// TODO Auto-generated destructor stub
}

void HuaWeiPvs::SetCom(Com* com)
{
	assert(com!=NULL);
	m_com = com;
}

int HuaWeiPvs::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
//	case DevModel::Model_SUN200010KTL:
//		ret = this->Sun200010KtlRead(dev);
//		break;
	default:
		break;
	};

	return ret;
}

int HuaWeiPvs::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
//	case DevModel::Model_SUN200010KTL:
//		ret = this->Sun200010KtlPreset(dev);
//		break;
	default:
		break;
	};

	return ret;
}

int HuaWeiPvs::Sun200010KtlRead(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取华为PVS数据");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int start = 40500;
	int length = 12;
	int regStart = dev->regStart;
	float tmp = 0;
	float tmpI = 0;
	float tmpV = 0;
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
			ret = rtu.ReadOutputRegisters(values,addr,start,length);
			if(ret == ErrorInfo::ERR_OK)
			{
				break;
			}
		}

		if (ErrorInfo::ERR_OK == ret)
		{
			//保存通讯正常
			comm = true;
			tmpV = ((short)values[0]) * 0.1;//PV1输入电压
			MemDb::SetRealData(regStart + 14, tmpV, false);//直流电压
			tmpI = ((short)values[6]) * 0.01;//PV1输入电流
			MemDb::SetRealData(regStart + 15, tmpI, false);//直流电流
			tmp = tmpI * tmpV;
			MemDb::SetRealData(regStart + 16, tmp, false);//直流功率
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		start = 40523;
		length = 4;
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
			data = ((((short)values[2])&0xFFFF)<<16)|(((short)values[3])&0xFFFF);
			MemDb::SetRealData(regStart + 26, (float)data, false);//有功功率
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		start = 40532;
		length = 2;
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
			tmp = ((short)values[0]) * 0.001;
			MemDb::SetRealData(regStart + 28, tmp, false);//功率因数
			tmp = ((short)values[1]) * 0.1;
			MemDb::SetRealData(regStart + 13, tmp, false);//机内温度
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		start = 40544;
		length = 3;
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
			data = ((((short)values[0])&0xFFFF)<<16)|(((short)values[1])&0xFFFF);
			MemDb::SetRealData(regStart + 27, (float)data, false);//无功功率
			tmp = ((short)values[2]) * 0.01;
			MemDb::SetRealData(regStart + 29, tmp, false);//电网频率
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		start = 40560;
		length = 4;
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
			tmp = (((((short)values[0])&0xFFFF)<<16)|(((short)values[1])&0xFFFF)) * 0.01;
			MemDb::SetRealData(regStart + 10, tmp, false);//总发电量
			tmp = (((((short)values[2])&0xFFFF)<<16)|(((short)values[3])&0xFFFF)) * 0.01;
			MemDb::SetRealData(regStart + 9, tmp, false);//日发电量
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		start = 40572;
		length = 3;
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
			tmp = ((short)values[0]) * 0.01;
			MemDb::SetRealData(regStart + 20, tmp, false);//A相电流
			tmp = ((short)values[1]) * 0.01;
			MemDb::SetRealData(regStart + 21, tmp, false);//B相电流
			tmp = ((short)values[2]) * 0.01;
			MemDb::SetRealData(regStart + 22, tmp, false);//C相电流
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		start = 40577;
		length = 3;
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
			tmp = ((short)values[0]) * 0.01;
			MemDb::SetRealData(regStart + 17, tmp, false);//A相电压
			tmp = ((short)values[1]) * 0.01;
			MemDb::SetRealData(regStart + 18, tmp, false);//B相电压
			tmp = ((short)values[2]) * 0.01;
			MemDb::SetRealData(regStart + 19, tmp, false);//C相电压
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}


		start = 40685;
		length = 1;
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
			tmp = ((short)values[0]) * 0.01;
			MemDb::SetRealData(regStart + 30, tmp, false);//逆变器效率
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		start = 40939;
		length = 1;
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
			data = (short)values[0];
			switch(data)
			{
			case 0x0000://待机：初始化
			case 0x0001://待机：绝缘阻抗检测
			case 0x0002://待机：光照检测
			case 0xA000://待机：无光照
				tmp1 = 4;//待机
				break;
			case 0x0100://启动
				tmp1 = 6;
				break;
			case 0x0200://并网
			case 0x0401://电网调度：cosψ-P 曲线
			case 0x0402://电网调度：Q_U曲线
			case 0x0201://并网：限功率
				tmp1 = 1;
				break;
			case 0x0300://关机：异常关机
			case 0x0301://关机：指令关机
				tmp1 = 0;
			}
			MemDb::SetRealData(regStart + 5, tmp1, false);//运行状态
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		start = 50000;
		length = 10;
		//读取三次，如果三次都不成功认为通讯失败
		for(int i =0;i<3;i++)
		{
			ret = rtu.ReadOutputRegisters(values, addr, start, length);
			if(ret == ErrorInfo::ERR_OK)
			{
				break;
			}
		}

		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			data = 0;
			if(Util::GetBitValue(values[0],8)  == 1)
			{
				Util::SetBitValue(data, 2, true);//内部通讯故障
			}

			if((Util::GetBitValue(values[1], 3)  == 1)
					|| (Util::GetBitValue(values[1], 4)  == 1)
					|| (Util::GetBitValue(values[1], 6)  == 1)
					|| (Util::GetBitValue(values[1], 15)  == 1)
					|| (Util::GetBitValue(values[2], 10)  == 1)
					|| (Util::GetBitValue(values[2], 15)  == 1)
					|| (Util::GetBitValue(values[3], 0)  == 1)
					|| (Util::GetBitValue(values[6], 6)  == 1)
					|| (Util::GetBitValue(values[6], 10)  == 1)
					|| (Util::GetBitValue(values[6], 12)  == 1)
					|| (Util::GetBitValue(values[7], 1)  == 1)
					|| (Util::GetBitValue(values[7], 5)  == 1)
					|| (Util::GetBitValue(values[9], 6)  == 1))
			{
				Util::SetBitValue(data, 0, true);//控制器故障
			}

			if((Util::GetBitValue(values[1], 8)  == 1)
					|| (Util::GetBitValue(values[1], 9)  == 1)
					|| (Util::GetBitValue(values[2], 6)  == 1)
					|| (Util::GetBitValue(values[4], 1)  == 1))
			{
				Util::SetBitValue(data, 29, true);//过温保护
			}

			if((Util::GetBitValue(values[1], 7)  == 1)
					|| (Util::GetBitValue(values[2], 0)  == 1)
					|| (Util::GetBitValue(values[2], 5)  == 1)
					|| (Util::GetBitValue(values[8], 10)  == 1))
			{
				Util::SetBitValue(data, 9, true);//接地故障
			}

			if((Util::GetBitValue(values[3], 9)  == 1)
					|| (Util::GetBitValue(values[3], 10)  == 1)
					|| (Util::GetBitValue(values[3], 11)  == 1))
			{
				Util::SetBitValue(data, 12, true);//直流过压
			}

			if((Util::GetBitValue(values[5], 0)  == 1)
					|| (Util::GetBitValue(values[5], 1)  == 1)
					|| (Util::GetBitValue(values[5], 4)  == 1)
					|| (Util::GetBitValue(values[5], 5)  == 1))
			{
				Util::SetBitValue(data, 14, true);//直流电压异常
			}

			if((Util::GetBitValue(values[5], 11)  == 1))
			{
				Util::SetBitValue(data, 23, true);//三相电流不平衡
			}

			if((Util::GetBitValue(values[7], 2)  == 1)
					|| (Util::GetBitValue(values[8], 6)  == 1))
			{
				Util::SetBitValue(data, 18, true);//电网过频
			}

			if((Util::GetBitValue(values[7], 3)  == 1)
					|| (Util::GetBitValue(values[8], 7)  == 1))
			{
				Util::SetBitValue(data, 19, true);//电网欠频
			}

			if((Util::GetBitValue(values[7], 6)  == 1))
			{
				Util::SetBitValue(data, 27, true);//频率异常
			}

			if((Util::GetBitValue(values[7], 13)  == 1)
					|| (Util::GetBitValue(values[7], 14)  == 1)
					|| (Util::GetBitValue(values[7], 15)  == 1)
					|| (Util::GetBitValue(values[8], 3)  == 1)
					|| (Util::GetBitValue(values[8], 4)  == 1)
					|| (Util::GetBitValue(values[8], 5)  == 1)
					|| (Util::GetBitValue(values[8], 11)  == 1)
					|| (Util::GetBitValue(values[8], 8)  == 1))
			{
				Util::SetBitValue(data, 16, true);//电网过压
			}

			if((Util::GetBitValue(values[8], 0)  == 1)
					|| (Util::GetBitValue(values[8], 1)  == 1)
					|| (Util::GetBitValue(values[8], 2)  == 1)
					|| (Util::GetBitValue(values[8], 13)  == 1)
					|| (Util::GetBitValue(values[8], 14)  == 1)
					|| (Util::GetBitValue(values[8], 15)  == 1))
			{
				Util::SetBitValue(data, 17, true);//电网欠压
			}

			if((Util::GetBitValue(values[8], 9)  == 1))
			{
				Util::SetBitValue(data, 10, true);//孤岛保护
			}

			if (data != 0)
			{
				MemDb::SetRealData(regStart + 7, 1, false);//故障
				MemDb::SetRealData(regStart + 8, data, false);//故障码
			}
			else
			{
				MemDb::SetRealData(regStart + 7, 0, false);//无故障
				MemDb::SetRealData(regStart + 8, data, false);//故障码
			}
		}
		else
		{
			zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
		}

		MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
		return ret;
}

int HuaWeiPvs::Sun200010KtlPreset(Device::SlaveDev* dev)
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
				ret = rtu.PresetSingleRegister(addr, 40200, 0);
			}
			else if (realData.pv.data.iValue == 0)
			{
				//停止
				ret = rtu.PresetSingleRegister(addr, 40201, 0);
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
		zlog_warn(Util::m_zlog,"设置有功功率限制");
		if (realData.pv.type == RealData::DB)
		{
			//限功率设置
			if(((int) realData.pv.data.dbValue == 100))
			{
				ret = rtu.PresetSingleRegister(addr, 40118, 0);//关闭光伏限功率
				m_powerFlag = 0;
			}
			else
			{
				if ((int) realData.pv.data.dbValue == 0)
				{
					m_limitPower = 1;
				}
				else
				{
					m_limitPower = (int)realData.pv.data.dbValue;
				}
				ret = rtu.PresetSingleRegister(addr, 40118, 1);//开启光伏限功率
				ret = rtu.PresetSingleRegister(addr, 40234, m_limitPower);
				m_powerFlag = 1;
				m_controlStart.SetStartTime();
			}
		}
	}

	//重复发送限功率指令
	if(m_powerFlag == 1)
	{
		m_controlStart.SetEndTime();
		if (m_controlStart.GetMilliSeconds() > 50000)
		{
			ret = rtu.PresetSingleRegister(addr, 40234, m_limitPower);
			m_controlStart.SetStartTime();
		}
	}

	//判断是否有功率因数设置命令
	if (MemDb::GetRealDataWrited(realData, regStart + 4, true))
	{
		zlog_info(Util::m_zlog,"设置有功功率因数");
		if (realData.pv.type == RealData::INT)
		{
			//功率因数设置
			ret = rtu.PresetSingleRegister(addr, 40237, realData.pv.data.iValue
					* 1000);
		}
		else if (realData.pv.type == RealData::DB)
		{
			//功率因数设置
			ret = rtu.PresetSingleRegister(addr, 40237, realData.pv.data.iValue
					* 1000);
		}
	}

	zlog_info(Util::m_zlog,"设置PVS参数结束");
	return ret;
}
