/*
 * JSHJSimultor.cpp
 *
 *  Created on: 2015-12-9
 *      Author: liuhaitao
 */

#include "JSHJSimultor.h"

#include "../../com/Com.h"

JSHJSimultor::JSHJSimultor()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

JSHJSimultor::~JSHJSimultor()
{
	// TODO Auto-generated destructor stub
}

void JSHJSimultor::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

int JSHJSimultor::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PVS1000:
		ret = this->PVS1000Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int JSHJSimultor::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_PVS1000:
		ret = this->PVS1000Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int JSHJSimultor::PVS1000Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int regStart = dev->regStart;
	float voltage = 0.0;
	float current = 0.0;
	float power = 0.0;
	float setCurr = 0.0;
	float setVol = 0.0;
	float setPower = 0.0;
	float setResi = 0.0;
	int runSta = 0;
	int mode = 0;
	int fault = 0;
	PVivCurve value;

	//获取系统状态
	ret = GetSysSta(runSta, mode, fault);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 22, runSta, false);//运行状态
		MemDb::SetRealData(regStart + 23, mode, false);//运行模式
		if(fault != 0)
		{
			MemDb::SetRealData(regStart + 24, 1, false);//有故障
			MemDb::SetRealData(regStart + 25, fault, false);//故障码
		}
		else
		{
			MemDb::SetRealData(regStart + 24, 0, false);//无故障
			MemDb::SetRealData(regStart + 25, fault, false);//故障码
		}
	}

	//获取电压电流
	ret = GetData(voltage, current);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 26, voltage, false);//电压
		MemDb::SetRealData(regStart + 27, current, false);//电流
		power = voltage * current;
		if(power < 50)
		{
			power = 0;
		}
		MemDb::SetRealData(regStart + 28, power, false);//电流
	}

	//获取设置反馈值
	//获取设置电流值
	ret = GetCurrent(setCurr);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 5, setCurr, false);//设置电流
	}

	//获取设置电压值
	ret = GetVoltage(setVol);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 4, setVol, false);//设置电压
	}

	//获取设置功率值
	ret = GetPower(setPower);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 6, setPower, false);//设置功率
	}

	//获取设置内阻值
	ret = GetResistance(setResi);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 7, setResi, false);//设置内阻
	}

	//获取设置光伏曲线参数
//	ret = GetSAS(value, regStart);
	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

#if 0
	float maxVol = 0.0;
	float maxCur = 0.0;
	float maxPow = 0.0;
	int accuracyV = 0;
	int accuracyI = 0;
	int accuracyP = 0;
	int powerUnit = 0;
	//设备可提供输出能力
	ret = OutPutMaxDc(maxVol, maxCur, maxPow);
	//获取设备精度
	ret = GetSysSta(accuracyV, accuracyI, accuracyP, powerUnit);
#endif
	return ret;
}

int JSHJSimultor::PVS1000Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	float power = 0.0;
	RealData realData;
	RealData realDataI;
	RealData realDataV;
	RealData voc;
	RealData isc;
	RealData pmp;
	RealData vmp;//最大功率点电压
	RealData imp;//最大功率点电流
	RealData fillFactorV;//电压填充因子
	RealData fillFactorI;//电流填充因子
	RealData bt;//湿度
	RealData ta;//温度
	RealData irr;//光照幅度
	int mode = 0;
	int type = 0;
	PVivCurve data;

	//判断是否有启停设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		ret = SetStart(realData.pv.data.iValue);
	}

	//判断是否有输出模式设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		ret = SetOutMode(realData.pv.data.iValue);
	}

	//获取设备工作模式
	MemDb::GetRealData(mode, regStart + 23);
	if(mode == 0)//正常模式
	{
		//判断是否有电流设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 5, true))
		{
			ret = SetCurrent(realData.pv.data.dbValue);

			msleep(1500);
			MemDb::GetRealData(realDataV, regStart + 4);//获取电压值
			if(realDataV.pv.data.dbValue != 0)
			{
				power = (realData.pv.data.dbValue * realDataV.pv.data.dbValue) + 100.0;
				ret = SetPower(power);
			}

		}

		msleep(1500);
		//判断是否有电压设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 4, true))
		{
			ret = SetVoltage(realData.pv.data.dbValue);

			msleep(1500);
			MemDb::GetRealData(realDataI, regStart + 5);//获取电流值
			if(realDataI.pv.data.dbValue != 0)
			{
				power = (realData.pv.data.dbValue * realDataI.pv.data.dbValue) + 100.0;
				ret = SetPower(power);
			}
		}

		msleep(1500);
		//判断是否有内阻设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 7, true))
		{
			ret = SetResistance(realData.pv.data.dbValue);
		}

#if 0
		//判断是否有功率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 6, true))
		{
			ret = SetPower(realData.pv.data.dbValue);

		}
#endif
	}
	else if(mode == 2)//SAS模式
	{
		MemDb::GetRealData(type, regStart + 3);//光伏模拟曲线类型
		if(type == 0)//实际曲线
		{
			if((MemDb::GetRealDataWrited(vmp, regStart + 10, true))//最大功率点电压
					|| (MemDb::GetRealDataWrited(imp, regStart + 11, true))//最大功率点电流
					|| (MemDb::GetRealDataWrited(fillFactorV, regStart + 13, true))//电压填充因子
					|| (MemDb::GetRealDataWrited(fillFactorI, regStart + 14, true))//电流填充因子
					|| (MemDb::GetRealDataWrited(bt, regStart + 15, true))//电压温度系数
					|| (MemDb::GetRealDataWrited(ta, regStart + 16, true))//温度
					|| (MemDb::GetRealDataWrited(irr, regStart + 17, true)))//光照幅度
			{
				MemDb::GetRealDataWrited(vmp, regStart + 10, true);//最大功率点电压
				MemDb::GetRealDataWrited(imp, regStart + 11, true);//最大功率点电流
				MemDb::GetRealDataWrited(fillFactorV, regStart + 13, true);//电压填充因子
				MemDb::GetRealDataWrited(fillFactorI, regStart + 14, true);//电流填充因子
				MemDb::GetRealDataWrited(bt, regStart + 15, true);//电压温度系数
				MemDb::GetRealDataWrited(ta, regStart + 16, true);//温度
				MemDb::GetRealDataWrited(irr, regStart + 17, true);//光照幅度

				MemDb::GetRealData(vmp, regStart + 10);//最大功率点电压
				MemDb::GetRealData(imp, regStart + 11);//最大功率点电流
				MemDb::GetRealData(fillFactorV, regStart + 13);//电压填充因子
				MemDb::GetRealData(fillFactorI, regStart + 14);//电流填充因子
				MemDb::GetRealData(bt, regStart + 15);//电压温度系数
				MemDb::GetRealData(ta, regStart + 16);//温度
				MemDb::GetRealData(irr, regStart + 17);//光照幅度

				if(fillFactorV.pv.data.dbValue == 0)
				{
					fillFactorV.pv.data.dbValue = 0.8;
				}

				if(fillFactorI.pv.data.dbValue == 0)
				{
					fillFactorI.pv.data.dbValue = 0.9;
				}

				if(bt.pv.data.dbValue == 0)
				{
					bt.pv.data.dbValue = (-0.25);
				}

				if(ta.pv.data.dbValue == 0)
				{
					ta.pv.data.dbValue = 25;
				}

				if(irr.pv.data.dbValue == 0)
				{
					irr.pv.data.dbValue = 1000;
				}

				data.num = 0;
				data.vmp = vmp.pv.data.dbValue;
				data.imp = imp.pv.data.dbValue;
				data.fillFactorV = fillFactorV.pv.data.dbValue;
				data.fillFactorI = fillFactorI.pv.data.dbValue;
				data.bt = bt.pv.data.dbValue;
				data.ta = ta.pv.data.dbValue;
				data.irr = irr.pv.data.dbValue;

				ret = SetIVCU(type, data);
			}
		}
		else if(type == 1)//理想曲线
		{
			if((MemDb::GetRealDataWrited(voc, regStart + 8, true))//开路电压
					|| (MemDb::GetRealDataWrited(isc, regStart + 9, true))//短路电流
					|| (MemDb::GetRealDataWrited(pmp, regStart + 12, true)))//最大功率点功率
			{
				MemDb::GetRealDataWrited(voc, regStart + 8, true);//开路电压
				MemDb::GetRealDataWrited(isc, regStart + 9, true);//短路电流
				MemDb::GetRealDataWrited(pmp, regStart + 12, true);//最大功率点功率

				MemDb::GetRealData(voc, regStart + 8);//开路电压
				MemDb::GetRealData(isc, regStart + 9);//短路电流
				MemDb::GetRealData(pmp, regStart + 12);//最大功率点功率
				data.voc = voc.pv.data.dbValue;
				data.isc = isc.pv.data.dbValue;
				data.power = pmp.pv.data.dbValue;
				data.num = 0;

				ret = SetIVCU(type, data);
			}
		}
	}

	return ret;
}

/*菊水皇家光伏模拟器通讯协议*/
//生成一条可供调用的IV曲线
int JSHJSimultor::SetIVCU(int type, PVivCurve data)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	if (type == 0)//实际曲线
	{
		sprintf(
				tmp,
				"SASI:IVCU \"No=%d\",\"Vp=%.1f\",\"Ip=%.1f\",\"FFv=%.1f\",\"FFi=%.1f\",\"BT=%.2f\",\"Ta=%.1f\",\"Irr=%.1f\"/n",
				data.num, data.vmp, data.imp, data.fillFactorV,
				data.fillFactorI, data.bt, data.ta, data.irr);
	}
	else if (type == 1)//理想曲线
	{
		sprintf(
				tmp,
				"SASI:IVCU \"No=%d\",\"Voc=%.1f\",\"Isc=%.1f\",\"Pmp=%.1f\"/n",
				data.num, data.voc, data.isc, data.power);
	}

	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->Write(values);

	sleep(1);
	//调用执行指定曲线
	ret = CallSepCur(data.num);

	return ret;
}

//调用执行指定曲线
int JSHJSimultor::CallSepCur(int num)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SASI:CALL %d/n", num);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->Write(values);

	return ret;
}

//设置输出工作模式
int JSHJSimultor::SetOutMode(int mode)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "OUTP:MODE %d/r/n", mode);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->Write(values);

	return ret;
}

//设置启动/停止
int JSHJSimultor::SetStart(int runSta)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	if(runSta == 1)
	{
		sprintf(tmp, "OUTP ON");
	}
	else if(runSta == 0)
	{
		sprintf(tmp, "OUTP OFF");
	}
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->Write(values);

	return ret;
}

//设置输出电流值
int JSHJSimultor::SetCurrent(float current)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "CURR %f/r/n", current);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->Write(values);

	return ret;
}

//设置输出电压值
int JSHJSimultor::SetVoltage(float voltage)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "VOLT %f/r/n", voltage);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->Write(values);

	return ret;
}

//设置输出功率值
int JSHJSimultor::SetPower(float power)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "POW %.0f/r/n", power);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->Write(values);

	return ret;
}

//设置输出内阻值
int JSHJSimultor::SetResistance(float resistance)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//内阻范围0-1.2
	if(resistance < 0)
	{
		resistance = 0;
	}

	if(resistance > 1.2)
	{
		resistance = 1.2;
	}

	//封装控制语句
	sprintf(tmp, "RESI %f/r/n", resistance);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->Write(values);

	return ret;
}

//查询设备可提供输出能力
int JSHJSimultor::OutPutMaxDc(float& voltage, float& current, float& power)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;
	string packData;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "OUTP:MAX:DC?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_com->TxRxMsg(reply, values, 19);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	return ret;
}

//获取电压电流
int JSHJSimultor::GetData(float& voltage, float& current)
{
	int ret = ErrorInfo::ERR_OK;
	int len = 0;
	char tmp[ARRAY_LENGTH_512];
	char temp[ARRAY_LENGTH_512];
	string str;
	string str1;
	string str2;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "DATA?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_com->TxRxMsg(reply, values, 13);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	sprintf(temp, ",");
	str1 = temp;
	len = packData.find(str1.c_str());
	str2 = packData.substr(0, len - 1);
	voltage = atof(str2.c_str());
	packData.erase(0, len + 1);
	current = atof(packData.c_str());
	return ret;
}

//获取设置电流值
int JSHJSimultor::GetCurrent(float& current)
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
	sprintf(tmp, "CURR?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_com->TxRxMsg(reply, values, 4);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	current = atof(packData.c_str());
	return ret;
}

//获取设置电压值
int JSHJSimultor::GetVoltage(float& voltage)
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
	sprintf(tmp, "VOLT?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_com->TxRxMsg(reply, values, 5);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	voltage = atof(packData.c_str());
	return ret;
}

//获取设置功率值
int JSHJSimultor::GetPower(float& power)
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
	sprintf(tmp, "POW?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_com->TxRxMsg(reply, values, 7);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	power = atof(packData.c_str());
	return ret;
}

//获取设置内阻值
int JSHJSimultor::GetResistance(float& resistance)
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
	sprintf(tmp, "RESI?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_com->TxRxMsg(reply, values, 4);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	resistance = atof(packData.c_str());
	return ret;
}

//获取设置光伏曲线参数
int JSHJSimultor::GetSAS(PVivCurve& value, int regStart)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;
	int type = 0;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "SASI:IVCU? 0\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	MemDb::GetRealData(type, regStart + 3);//获取曲线类型
	if(type == 0)
	{
		//发送数据
		ret = m_com->TxRxMsg(reply, values, 83);
		if(ret == ErrorInfo::ERR_OK)
		{
			for(unsigned int i = 0; i < reply.size(); i++)
			{
				packData.push_back(reply[i]);
			}
		}
	}
	else
	{
		ret = m_com->TxRxMsg(reply, values, 46);
		if(ret == ErrorInfo::ERR_OK)
		{
			for(unsigned int i = 0; i < reply.size(); i++)
			{
				packData.push_back(reply[i]);
			}
		}
	}

	return ret;
}

//获取系统工作状态
int JSHJSimultor::GetSysSta(int& runSta, int& mode, int& fault)
{
	int ret = ErrorInfo::ERR_OK;
	int len = 0;
	char tmp[ARRAY_LENGTH_512];
	char temp[ARRAY_LENGTH_512];
	string str;
	string str1;
	string str2;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "SYST?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_com->TxRxMsg(reply, values, 5);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	sprintf(temp, ",");
	str1 = temp;
	len = packData.find(str1.c_str());
	str2 = packData.substr(0, len);
	runSta = atoi(str2.c_str());
	packData.erase(0, len + 1);
	str2.clear();
	sprintf(temp, ",");
	str1 = temp;
	len = packData.find(str1.c_str());
	str2 = packData.substr(0, len);
	mode = atoi(str2.c_str());
	packData.erase(0, len + 1);
	fault = atoi(packData.c_str());
	return ret;
}

//获取数据精度
int JSHJSimultor::GetSysSta(int& accuracyV, int& accuracyI, int& accuracyP, int& powerUnit)
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
	sprintf(tmp, "SYST:ACCU?\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_com->TxRxMsg(reply, values, 7);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_warn(Util::m_zlog,"获取精度字符串：%s",packData.c_str());
	return ret;
}
