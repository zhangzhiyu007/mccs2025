/*
 * ChromaBattery.cpp
 *
 *  创建日期: 2016-12-16
 *  作          者: zl
 */

#include "ChromaBattery.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

ChromaBattery::ChromaBattery()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
	m_flag = 0;
	count1 = 0;
}

ChromaBattery::~ChromaBattery()
{
	// TODO Auto-generated destructor stub
}

void ChromaBattery::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void ChromaBattery::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int ChromaBattery::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_17040:
		ret = this->Chroma17040Read(dev);
		break;
	case DevModel::Model_CHROMA_62180D:
		ret = this->Chroma62180DRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaBattery::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_17040:
		ret = this->Chroma17040Preset(dev);
		break;
	case DevModel::Model_CHROMA_62180D:
		ret = this->Chroma62180DPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaBattery::Chroma17040Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int regStart = dev->regStart;
	int value = 0;
	int fault = 0;
	float tmp = 0.0;
	FloatArray temp;
	temp.clear();

	//实时值
	//获取实时电压
	ret = GetVoltage(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 34, tmp, false);//电压
	}
	msleep(100);

#if 1
	//获取实时电流
	ret = GetCur(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 35, tmp, false);//电流
	}
	msleep(100);

	//获取实时输出有功功率值
	ret = GetPower(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 36, tmp, false);//有功功率
	}
	msleep(100);

	//获取实时可放电容量
	ret = GetAh(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 37, tmp, false);//可放电容量
	}
	msleep(100);

#if 0
	//获取实时内阻值
	ret = GetDcir(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 38, tmp, false);//内阻值
	}
	msleep(100);

	//获取实时温度
	ret = GetTemp(temp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;

		for(int i = 0; i < 8; i ++)
		{
			tmp = temp.at(i);
			MemDb::SetRealData(regStart + 39 + i, tmp, false);//温度值
		}
	}
	msleep(100);

	//获取实时操作状态
	ret = GetOperat(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 30, value, false);//操作状态
	}
	msleep(100);
#endif

	ret = GetStat(fault);
	if(fault != 0)
	{
		MemDb::SetRealData(regStart + 31, 1, false);//故障状态
		MemDb::SetRealData(regStart + 33, fault, false);//故障码2
	}
	else
	{
		MemDb::SetRealData(regStart + 31, 0, false);//故障状态
		MemDb::SetRealData(regStart + 33, fault, false);//故障码2
	}
	msleep(100);

	//反馈值
	//获取启动停止设置
	ret = GetStart(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 1, value, false);//启动/停止
	}
	msleep(100);

	//获取工作模式设置
	ret = GetOutMode(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 2, value, false);//工作模式
	}
	msleep(100);

	//获取初始模拟器设置值
	ret = GetInit(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 6, value, false);//初始模拟器设置值
	}
	msleep(100);

	//获取选择电池模拟器参数
	ret = GetPara(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 7, value, false);//电池模拟器参数
	}
	msleep(100);

	//获取初始CAP参数
	ret = GetInitCap(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 8, tmp, false);//初始CAP参数
	}
	msleep(100);

	//获取初始电压参数
	ret = GetInitVolt(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 9, tmp, false);//初始电压参数
	}
	msleep(100);

	//获取CAP
	ret = GetCap(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 13, tmp, false);//CAP
	}
	msleep(100);

	//获取设置模拟器OCP值
	ret = GetOcp(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 14, tmp, false);//OCP
	}
	msleep(100);

	//获取设置模拟器ESR值
	ret = GetEsr(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 15, tmp, false);//ESR
	}
	msleep(100);

	//获取设置模拟器BCH值
	ret = GetBch(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 16, tmp, false);//BCH
	}
	msleep(100);

	//获取设置模拟器BCL值
	ret = GetBcl(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 17, tmp, false);//BCL
	}
	msleep(100);

	//获取设置模拟器BVH值
	ret = GetBvh(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 18, tmp, false);//BVH
	}
	msleep(100);

	//获取设置模拟器BVL值
	ret = GetBvl(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 19, tmp, false);//BVL
	}
	msleep(100);

	//获取设置模拟器VH值
	ret = GetVh(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 20, tmp, false);//VH
	}
	msleep(100);

	//获取设置模拟器VL值
	ret = GetVl(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 21, tmp, false);//VL
	}
	msleep(100);

	//获取设置模拟器EFFCHG值
	ret = GetEffchg(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 22, tmp, false);//EFFCHG
	}
	msleep(100);

	//获取设置模拟器EFFDSG值
	ret = GetEffdsg(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 23, tmp, false);//EFFDSG
	}
	msleep(100);

	//获取设置模拟器BOH值
	ret = GetBoh(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 24, tmp, false);//BOH
	}
	msleep(100);

	//获取设置模拟器BOL值
	ret = GetBol(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 25, tmp, false);//BOL
	}
	msleep(100);

	//获取设置模拟器VOH值
	ret = GetVoh(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 26, tmp, false);//VOH
	}
	msleep(100);

	//获取设置模拟器VOL值
	ret = GetVol(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 27, tmp, false);//VOL
	}
#endif
	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}


int ChromaBattery::Chroma62180DRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int regStart = dev->regStart;
	int value = 0;
	int fault = 0;
	float tmp = 0.0;
	FloatArray temp;
	temp.clear();

	 float VH = 0.0;
	 float VL = 0.0;
	 float VHVL = 0.0;
	 float VoltageBattNow = 0.0;
	 float BattSOC = 0.0;

	 int model = 0;

//设置成储能模拟模式
//	 if( count1 == 0)
//	 {
//		 int ret = ErrorInfo::ERR_OK;
//		 	char tmp[ARRAY_LENGTH_512];
//		 	string str;
//		 	String values;
//
//		 	str.clear();
//		 	values.clear();
//
//		 	//封装控制语句
//		 	sprintf(tmp, "BATT:MODEL \r\n");
//		 	str = tmp;
//		 	zlog_warn(Util::m_zlog,"下发控制语句==============：%s",str.c_str());
//		 	for(unsigned int i = 0; i < str.size(); i++)
//		 	{
//		 		values.push_back(str.at(i));
//		 	}
//
//		 	//发送数据
//		 	m_tcp->SendFlush(values);
//		 	msleep(500);
//		 	count1++;
//			zlog_warn(Util::m_zlog,"下发控制计数器count==============：%d",count1);
//    }

//设置完成


//计算实时SOC
    MemDb::GetRealData(VH, regStart + 28);
    MemDb::GetRealData(VL, regStart + 29);
    VHVL = VH - VL;
    MemDb::GetRealData(VoltageBattNow, regStart + 15);
    VoltageBattNow = VoltageBattNow - VL;
    BattSOC = (VoltageBattNow/VHVL) * 100.0;
	zlog_debug(Util::m_zlog, "电网并网状态储能电池SOC=%f" ,BattSOC);
	MemDb::SetRealData(regStart + 30, BattSOC, false);//电压
 //计算结束

	//实时值
	//获取实时电压
	ret = GetVoltage(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 15, tmp, false);//电压
	}
	msleep(100);

#if 1

//获取是否运行在储能模式
//	ret = GetModel(model);
//	if(ret == ErrorInfo::ERR_OK)
//	{
//		comm = true;
//		MemDb::SetRealData(regStart + 31, tmp, false);//读储能模式
//	}
//	msleep(100);
//结束


	//获取实时电流
	ret = GetCur(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 16, tmp, false);//电流
	}
	msleep(100);

	//获取实时输出有功功率值
	ret = GetPower(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 17, tmp, false);//有功功率
	}
	msleep(100);

	ret = GetStat(fault);
	if(fault != 0)
	{
		MemDb::SetRealData(regStart + 18, 1, false);//故障状态
		MemDb::SetRealData(regStart + 19, fault, false);//故障码2
	}
	else
	{
		MemDb::SetRealData(regStart + 18, 0, false);//故障状态
		MemDb::SetRealData(regStart + 19, fault, false);//故障码2
	}
	msleep(100);

	//反馈值
	//获取启动停止设置
	ret = GetStart(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 20, value, false);//启动/停止
	}
	msleep(100);

	//获取初始模拟器设置值
	ret = GetInit62180(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 21, value, false);//初始模拟器设置值
	}
	msleep(100);

	//获取初始CAP参数
	ret = GetInitCap(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 22, tmp, false);//初始CAP参数
	}
	msleep(100);

	//获取初始电压参数
	ret = GetInitVolt(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 23, tmp, false);//初始电压参数
	}
	msleep(100);

	//获取CAP
	ret = GetCap(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 24, tmp, false);//CAP
	}
	msleep(100);

	//获取设置模拟器Sourced的OCP值
	ret = GetSourceOcp(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 25, tmp, false);//OCP
	}
	msleep(100);

	//获取设置模拟器Load的OCP值
	ret = GetLoadOcp(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 26, tmp, false);//OCP
	}
	msleep(100);

	//获取设置模拟器ESR值
	ret = GetEsr(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 27, tmp, false);//ESR
	}
	msleep(100);


	//获取设置模拟器VH值
	ret = GetVh(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 28, tmp, false);//VH
	}
	msleep(100);

	//获取设置模拟器VL值
	ret = GetVl(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 29, tmp, false);//VL
	}
	msleep(100);
#endif
	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int ChromaBattery::Chroma17040Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	RealData realData;
	float value = 0.0;

	if(m_flag == 0)
	{
		//获取电池设备信息
		ret = GetBattSpecAll(m_battPara);
		m_flag = 1;
	}

	//判断是否有启停设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		ret = SetStart(realData.pv.data.iValue);
	}

	//判断是否有工作模式设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		ret = SetOutMode(realData.pv.data.iValue);
	}

	//判断是否有初始模拟器设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 6, true))
	{
		ret = SetInit(realData.pv.data.iValue);
	}

	//判断是否选择电池模拟器参数设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 7, true))
	{
		ret = SetPara(realData.pv.data.iValue);
	}

	//判断是否选择输出CAP参数设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 8, true))
	{
		ret = SetInitCap(realData.pv.data.dbValue);
	}

	//判断是否选择初始电压设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 9, true))
	{

		value = realData.pv.data.dbValue;

		//ret = SetInitVolt(value);
	}

	//判断是否选择CAP设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 13, true))
	{
		ret = SetCap(realData.pv.data.dbValue);
	}

	//判断是否选择OCP设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 14, true))
	{
		ret = SetOcp(realData.pv.data.dbValue);
	}

	//判断是否选择ESR设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 15, true))
	{
		if(realData.pv.data.dbValue > m_battPara.esrMax)
		{
			value = m_battPara.esrMax;
		}
		else if(realData.pv.data.dbValue < m_battPara.esrMin)
		{
			value = m_battPara.esrMin;
		}
		else
		{
			value = realData.pv.data.dbValue;
		}
		ret = SetEsr(value);
	}
	//判断是否选择BCH设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 16, true))
	{
		ret = SetBch(realData.pv.data.dbValue);
	}

	//判断是否选择BCL设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 17, true))
	{
		ret = SetBcl(realData.pv.data.dbValue);
	}

	//判断是否选择BVH设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 18, true))
	{
		if(realData.pv.data.dbValue > m_battPara.bvhMax)
		{
			value = m_battPara.bvhMax;
		}
		else if(realData.pv.data.dbValue < m_battPara.bvhMin)
		{
			value = m_battPara.bvhMin;
		}
		else
		{
			value = realData.pv.data.dbValue;
		}
		ret = SetBvh(value);
	}

	//判断是否选择BVL设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 19, true))
	{
		if(realData.pv.data.dbValue > m_battPara.bvlMax)
		{
			value = m_battPara.bvlMax;
		}
		else if(realData.pv.data.dbValue < m_battPara.bvlMin)
		{
			value = m_battPara.bvlMin;
		}
		else
		{
			value = realData.pv.data.dbValue;
		}
		ret = SetBvl(value);
	}

	//判断是否选择VH设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 20, true))
	{
		if(realData.pv.data.dbValue > m_battPara.vhMax)
		{
			value = m_battPara.vhMax;
		}
		else if(realData.pv.data.dbValue < m_battPara.vhMin)
		{
			value = m_battPara.vhMin;
		}
		else
		{
			value = realData.pv.data.dbValue;
		}
		ret = SetVh(value);
	}

	//判断是否选择VL设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 21, true))
	{
		if(realData.pv.data.dbValue > m_battPara.vlMax)
		{
			value = m_battPara.vlMax;
		}
		else if(realData.pv.data.dbValue < m_battPara.vlMin)
		{
			value = m_battPara.vlMin;
		}
		else
		{
			value = realData.pv.data.dbValue;
		}
		ret = SetVl(value);
	}

	//判断是否选择EFFCHG设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 22, true))
	{
		ret = SetEffchg(realData.pv.data.dbValue);
	}

	//判断是否选择EFFDSG设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 23, true))
	{
		ret = SetEffdsg(realData.pv.data.dbValue);
	}

	//判断是否选择BOH设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 24, true))
	{
		ret = SetBoh(realData.pv.data.dbValue);
	}

	//判断是否选择BOL设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 25, true))
	{
		ret = SetBol(realData.pv.data.dbValue);
	}

	//判断是否选择VOH设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 26, true))
	{
		if(realData.pv.data.dbValue > m_battPara.vohMax)
		{
			value = m_battPara.vohMax;
		}
		else if(realData.pv.data.dbValue < m_battPara.vohMin)
		{
			value = m_battPara.vohMin;
		}
		else
		{
			value = realData.pv.data.dbValue;
		}
		ret = SetVoh(value);
	}

	//判断是否选择VOL设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 27, true))
	{
		if(realData.pv.data.dbValue > m_battPara.volMax)
		{
			value = m_battPara.volMax;
		}
		else if(realData.pv.data.dbValue < m_battPara.volMin)
		{
			value = m_battPara.volMin;
		}
		else
		{
			value = realData.pv.data.dbValue;
		}
		ret = SetVol(value);
	}

	return ret;
}


int ChromaBattery::Chroma62180DPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	RealData realData;
	float value = 0.0;
	//int valuee = 0;


	//判断是否有启停设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		ret = SetStart(realData.pv.data.iValue);
	}


	//判断是否有初始模拟器设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{

		value = realData.pv.data.iValue * 1.0;
		zlog_debug(Util::m_zlog,"SOC or CAP：=%f",value);
		ret = SetInit62180(value);
	}

	//判断是否选择输出CAP初始参数设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{

		value = realData.pv.data.iValue * 1.0;
		zlog_debug(Util::m_zlog,"CAP初始参数设定：=%f",value);
		ret = SetInitCap(value);
	}

	//判断是否选择初始电压设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 4, true))
	{
		value = realData.pv.data.iValue * 1.0 ;
		zlog_debug(Util::m_zlog,"选择初始电压设定值：=%f",value);
		ret = SetInitVolt(value);
	}

	//判断是否选择CAP设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{

		value = realData.pv.data.iValue * 1.0;
		zlog_debug(Util::m_zlog,"选择CAP设定值：=%f",value);
		ret = SetCap(value);
	}

	//判断是否选择source OCP设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 6, true))
	{
		//zlog_warn(Util::m_zlog,"realData.pv.data.iValue：=%d",realData.pv.data.iValue);
		value = realData.pv.data.iValue * 1.0;
		//zlog_warn(Util::m_zlog,"source-ocp：=%d",value);
		zlog_debug(Util::m_zlog,"source-ocp：=%f",value);
		ret = SetSourceOcp(value);
	}

	//判断是否选择load OCP设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 7, true))
	{

		value = realData.pv.data.iValue * 1.0;
		zlog_debug(Util::m_zlog,"source-ocp：=%f",value);
		ret = SetLoadOcp(value);
	}

	//判断是否选择ESR设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 8, true))
	{

		value = realData.pv.data.dbValue;
		zlog_debug(Util::m_zlog,"选择ESR设定值：=%f",value);
		ret = SetEsr(value);
	}

	//判断是否选择VH设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 9, true))
	{
		value = realData.pv.data.dbValue;
		zlog_debug(Util::m_zlog,"选择VH设定值：=%f",value);
		ret = SetVh(value);
	}

	//判断是否选择VL设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 10, true))
	{
		value = realData.pv.data.dbValue;
		zlog_debug(Util::m_zlog,"选择VL设定值：=%f",value);
		ret = SetVl(value);
	}

	return ret;
}



//实时值
//获取实时操作状态
int ChromaBattery::GetOperat(int& oper)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:OPER?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"操作状态命令返回：%s",packData.c_str());
	oper = atoi(packData.c_str());
	return ret;
}

//获取用户告警状态
int ChromaBattery::GetUserAlarm(int& alarm)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:ALARM?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"用户告警命令返回：%s",packData.c_str());
	alarm = atoi(packData.c_str());
	return ret;
}

//获取保护状态
int ChromaBattery::GetStat(int& stat)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "FETC:STAT?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"保护状态命令返回：%s",packData.c_str());
	stat = atoi(packData.c_str());
	return ret;
}

//获取实时输出电流值
int ChromaBattery::GetCur(float& current)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:CURR?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"电流查询命令返回：%s",packData.c_str());
	current = atof(packData.c_str());
	return ret;
}

//获取实时输出电压值
int ChromaBattery::GetVoltage(float& voltage)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:VOLT?\r\n"); //下发检测电压
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"电压查询命令返回：%s",packData.c_str());
	voltage = atof(packData.c_str());
	return ret;
}

//获取实时输出有功功率值
int ChromaBattery::GetPower(float& power)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:POW?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"有功功率查询命令返回：%s",packData.c_str());

	power = atof(packData.c_str());
	return ret;

}

//获取实时内阻值
int ChromaBattery::GetDcir(float& dcir)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:DCIR?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"内阻查询命令返回：%s",packData.c_str());

	dcir = atof(packData.c_str());
	return ret;

}

//获取实时温度
int ChromaBattery::GetTemp(FloatArray& temp)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	char tmp1[ARRAY_LENGTH_512];
	string str;
	string str1;
	string str2;
	string packData;
	String values;
	String reply;
	unsigned int len = 0;
	float data = 0.0;

	str.clear();
	str1.clear();
	str2.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:TEMP?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);

	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
		sprintf(tmp1, ",");
		str1 = tmp1;
		while(true)
		{
			len = packData.find(str1.c_str());
			if(len == string::npos)
			{
				data = atof(str2.c_str());
				temp.push_back(data);
				break;
			}
			str2 = packData.substr(0, len);
			data = atof(str2.c_str());
			packData.erase(0, len + 1);
			temp.push_back(data);
		}
	}

	return ret;

}

//获取可放电容量
int ChromaBattery::GetAh(float& ah)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:AH?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"可放电容量查询命令返回：%s",packData.c_str());

	ah = atof(packData.c_str());
	return ret;

}

//获取电池模拟器设备规格
int ChromaBattery::GetBattSpecAll(BattPara& battPara)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	char temp[ARRAY_LENGTH_512];
	float data = 0.0;
	unsigned int len = 0;
	string str;
	string str1;
	string str2;
	string packData;
	String values;
	String reply;
	FloatArray para;

	str.clear();
	str1.clear();
	str2.clear();
	packData.clear();
	values.clear();
	reply.clear();
	para.clear();

	//封装下发语句
	sprintf(tmp, "BATT:SPEC:ALL?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 150);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}

		zlog_debug(Util::m_zlog,"限制值反馈命令返回：%s",packData.c_str());
		sprintf(temp, ",");
		str1 = temp;

		while(true)
		{
			len = packData.find(str1.c_str());
			if(len == string::npos)
			{
				data = atof(str2.c_str());
				para.push_back(data);
				break;
			}
			str2 = packData.substr(0, len);
			data = atof(str2.c_str());
			packData.erase(0, len + 1);
			para.push_back(data);
		}

		//存储参数
		battPara.volaMax = para.at(0);
		battPara.volaMin = para.at(1);
		battPara.rateI = para.at(2);
		battPara.ratePower = para.at(3);
		battPara.slewRateI = para.at(4);
		battPara.esrMax = para.at(5);
		battPara.esrMin = para.at(6);
		battPara.bvhMax = para.at(7);
		battPara.bvhMin = para.at(8);
		battPara.bvlMax = para.at(9);
		battPara.bvlMin = para.at(10);
		battPara.vhMax = para.at(11);
		battPara.vhMin = para.at(12);
		battPara.vlMax = para.at(13);
		battPara.vlMin = para.at(14);
		battPara.vohMax = para.at(15);
		battPara.vohMin = para.at(16);
		battPara.volMax = para.at(17);
		battPara.volMin = para.at(18);
	}

	return ret;
}

//设置启动/停止
int ChromaBattery::SetStart(int runSta)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:OUTP %d\r\n", runSta);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置启动停止
int ChromaBattery::GetStart(int& runSta)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:OUTP?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"启停反馈命令返回：%s",packData.c_str());
	if(packData.compare("ON\n") == 0)
	{
		runSta = 1;
	}
	else if(packData.compare("OFF\n") == 0)//OFF
	{
		runSta = 0;
	}
	return ret;
}

//设置输出工作模式
int ChromaBattery::SetOutMode(int mode)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "OUTP:MOD %d\r\n", mode);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置输出工作模式
int ChromaBattery::GetOutMode(int& mode)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "OUTP:MOD?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"工作模式反馈命令返回：%s",packData.c_str());
	mode = atoi(packData.c_str());
	return ret;
}

//设置初始模拟器设置值
int ChromaBattery::SetInit(int initValue)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;
	int initValuetmp = 0;
	initValuetmp = initValue;
	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:INIT %d\r\n", initValue);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}
int ChromaBattery::SetInit62180(int initValue)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;
	int initValuetmp = 0;
	initValuetmp = initValue;
	str.clear();
	values.clear();

	//封装控制语句
	if(initValue == 0)
	{sprintf(tmp, "BATT:INIT SOC\r\n");} //0是soc模式
	else if (initValue == 1)
	{sprintf(tmp, "BATT:INIT VOL\r\n"); } //1是vol模式
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置初始模式设置值
int ChromaBattery::GetInit62180(int& initValue)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;
	int value = 0;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:INIT?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"初始模式设置反馈命令返回：%s",packData.c_str());
	if(packData.compare("SOC\n") == 0)
	{
		value = 0;
	}
	else if (packData.compare("VOL\n") == 0)
	{
		value = 1;
	}
	//initValue = atoi(packData.c_str());
	initValue = value;
	return ret;
}


int ChromaBattery::GetModel(int& initValue)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;
	int value = 0;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:MODEL?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"初始模式设置反馈命令返回：%s",packData.c_str());
//	if(packData.compare("SOC\n") == 0)
//	{
//		value = 0;
//	}
//	else if (packData.compare("VOL\n") == 0)
//	{
//		value = 1;
//	}
//	//initValue = atoi(packData.c_str());
	initValue = 0;
	return ret;
}





//获取设置初始模式设置值
int ChromaBattery::GetInit(int& initValue)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:INIT?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"初始模式设置反馈命令返回：%s",packData.c_str());
	initValue = atoi(packData.c_str());
	return ret;
}

//设置模拟器参数设置值
int ChromaBattery::SetPara(int para)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:PARA %d\r\n", para);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器参数设置值
int ChromaBattery::GetPara(int& para)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:PARA?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"模拟器参数反馈命令返回：%s",packData.c_str());
	para = atoi(packData.c_str());
	return ret;
}

//设置模拟器初始CAP值
int ChromaBattery::SetInitCap(float initCap)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:INIT:CAP %f\r\n", initCap);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器初始CAP值
int ChromaBattery::GetInitCap(float& initCap)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:INIT:CAP?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"初始CAP值反馈命令返回：%s",packData.c_str());
	initCap = atof(packData.c_str());
	return ret;
}

//设置模拟器初始VOLT值
int ChromaBattery::SetInitVolt(float initVolt)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	//sprintf(tmp, "BATT:INIT:VOLT %d\r\n", initVolt);
	sprintf(tmp, "BATT:INIT:VOLT %f\r\n", initVolt);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器初始VOLT值
int ChromaBattery::GetInitVolt(float& initVolt)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:INIT:VOLT?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"初始VOLT值反馈命令返回：%s",packData.c_str());
	initVolt = atof(packData.c_str());
	return ret;
}

//设置模拟器CAP值
int ChromaBattery::SetCap(float cap)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:CAP %f\r\n", cap);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器CAP值
int ChromaBattery::GetCap(float& cap)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:CAP?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器CAP值反馈命令返回：%s",packData.c_str());
	cap = atof(packData.c_str());
	return ret;
}

//设置模拟器OCP值
int ChromaBattery::SetOcp(float ocp)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:OCP %f\r\n", ocp);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//设置模拟器OCP值
int ChromaBattery::SetSourceOcp(float ocp)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:SOUR:OCP %f\r\n", ocp);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	zlog_debug(Util::m_zlog,"source ocp：%f",ocp);
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//设置模拟器OCP值
int ChromaBattery::SetLoadOcp(float ocp)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:LOAD:OCP %f\r\n", ocp);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器OCP值
int ChromaBattery::GetSourceOcp(float& ocp)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:SOUR:OCP?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器source-OCP值反馈命令返回：%s",packData.c_str());
	ocp = atof(packData.c_str());
	return ret;
}

//获取设置模拟器OCP值
int ChromaBattery::GetOcp(float& ocp)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:OCP?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器source-OCP值反馈命令返回：%s",packData.c_str());
	ocp = atof(packData.c_str());
	return ret;
}


//获取设置模拟器OCP值
int ChromaBattery::GetLoadOcp(float& ocp)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:LOAD:OCP?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器load-OCP值反馈命令返回：%s",packData.c_str());
	ocp = atof(packData.c_str());
	return ret;
}


//设置模拟器ESR值
int ChromaBattery::SetEsr(float esr)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:ESR %f\r\n", esr);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器ESR值
int ChromaBattery::GetEsr(float& esr)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:ESR?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器ESR值反馈命令返回：%s",packData.c_str());
	esr = atof(packData.c_str());
	return ret;
}

//设置模拟器BCH值
int ChromaBattery::SetBch(float bch)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:BCH %f\r\n", bch);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器BCH值
int ChromaBattery::GetBch(float& bch)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:BCH?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器BCH值反馈命令返回：%s",packData.c_str());
	bch = atof(packData.c_str());
	return ret;
}

//设置模拟器BCL值
int ChromaBattery::SetBcl(float bcl)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:BCL %f\r\n", bcl);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器BCL值
int ChromaBattery::GetBcl(float& bcl)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:BCL?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器BCL值反馈命令返回：%s",packData.c_str());
	bcl = atof(packData.c_str());
	return ret;
}

//设置模拟器BVH值
int ChromaBattery::SetBvh(float bvh)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:BVH %f\r\n", bvh);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器BVH值
int ChromaBattery::GetBvh(float& bvh)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:BVH?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器BVH值反馈命令返回：%s",packData.c_str());
	bvh = atof(packData.c_str());
	return ret;
}

//设置模拟器BVL值
int ChromaBattery::SetBvl(float bvl)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:BVL %f\r\n", bvl);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器BVL值
int ChromaBattery::GetBvl(float& bvl)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:BVL?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器BVL值反馈命令返回：%s",packData.c_str());
	bvl = atof(packData.c_str());
	return ret;
}

//设置模拟器VH值
int ChromaBattery::SetVh(float vh)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:VH %f\r\n", vh);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器VH值
int ChromaBattery::GetVh(float& vh)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:VH?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器VH值反馈命令返回：%s",packData.c_str());
	vh = atof(packData.c_str());
	return ret;
}

//设置模拟器VL值
int ChromaBattery::SetVl(float vl)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:VL %f\r\n", vl);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器VL值
int ChromaBattery::GetVl(float& vl)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:VL?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器VL值反馈命令返回：%s",packData.c_str());
	vl = atof(packData.c_str());
	return ret;
}

//设置模拟器EFFCHG值
int ChromaBattery::SetEffchg(float effchg)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:EFFCHG %f\r\n", effchg);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器EFFCHG值
int ChromaBattery::GetEffchg(float& effchg)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:EFFCHG?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器EFFCHG值反馈命令返回：%s",packData.c_str());
	effchg = atof(packData.c_str());
	return ret;
}

//设置模拟器EFFDSG值
int ChromaBattery::SetEffdsg(float effdsg)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:EFFDSG %f\r\n", effdsg);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器EFFDSG值
int ChromaBattery::GetEffdsg(float& effdsg)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:EFFDSG?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器EFFDSG值反馈命令返回：%s",packData.c_str());
	effdsg = atof(packData.c_str());
	return ret;
}

//设置模拟器BOH值
int ChromaBattery::SetBoh(float boh)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:BOH %f\r\n", boh);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器BOH值
int ChromaBattery::GetBoh(float& boh)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:BOH?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器BOH值反馈命令返回：%s",packData.c_str());
	boh = atof(packData.c_str());
	return ret;
}

//设置模拟器BOL值
int ChromaBattery::SetBol(float bol)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:BOL %f\r\n", bol);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器BOL值
int ChromaBattery::GetBol(float& bol)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:BOL?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器BOL值反馈命令返回：%s",packData.c_str());
	bol = atof(packData.c_str());
	return ret;
}

//设置模拟器VOH值
int ChromaBattery::SetVoh(float voh)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:VOH %f\r\n", voh);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器VOH值
int ChromaBattery::GetVoh(float& voh)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:VOH?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器VOH值反馈命令返回：%s",packData.c_str());
	voh = atof(packData.c_str());
	return ret;
}

//设置模拟器VOL值
int ChromaBattery::SetVol(float vol)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "BATT:VOLP %f\r\n", vol);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	zlog_debug(Util::m_zlog,"下发控制语句：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置模拟器VOL值
int ChromaBattery::GetVol(float& vol)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "BATT:VOLP?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"设置模拟器VOL值反馈命令返回：%s",packData.c_str());
	vol = atof(packData.c_str());
	return ret;
}
