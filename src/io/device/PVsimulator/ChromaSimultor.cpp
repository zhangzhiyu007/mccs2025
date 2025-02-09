/*
 * ChromaSimultor.cpp
 *
 *  创建日期: 2016-6-17
 *  作          者: zl
 */

#include "ChromaSimultor.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

ChromaSimultor::ChromaSimultor()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

ChromaSimultor::~ChromaSimultor()
{
	// TODO Auto-generated destructor stub
}

void ChromaSimultor::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void ChromaSimultor::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int ChromaSimultor::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_62150H_1000S:
		ret = this->Chroma62150Read(dev);
		break;
	case DevModel::Model_CHROMA_62020H_150S:
		ret = this->Chroma62020Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaSimultor::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_62150H_1000S:
		ret = this->Chroma62150Preset(dev);
		break;
	case DevModel::Model_CHROMA_62020H_150S:
		ret = this->Chroma62020Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaSimultor::Chroma62020Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma62000Read(dev);
	return ret;
}

int ChromaSimultor::Chroma62150Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma62000Read(dev);
	return ret;
}

int ChromaSimultor::Chroma62020Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma62000Preset(dev);
	return ret;
}

int ChromaSimultor::Chroma62150Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma62000Preset(dev);
	return ret;
}

int ChromaSimultor::Chroma62000Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int regStart = dev->regStart;
	float voltage = 0.0;
	float current = 0.0;
	float power = 0.0;
	float setCurr = 0.0;
	float setVol = 0.0;
	float setVoc = 0.0;
	float setIsc = 0.0;
	float setPmpI = 0.0;
	float setPmpV = 0.0;
	float setPmpP = 0.0;
	float irr = 0.0;
	float irrRef = 0.0;
	float beta = 0.0;
	float tmpRef = 0.0;
	float ff = 0.0;
	float tmp = 0.0;
	int runSta = 0;
	int mode = 0;
	int fault = 0;
	int value = 0;


	// printf("99999999\n");
	// printf("99999999\n");
	// printf("99999999\n");

	//获取系统状态
	ret = GetState(fault, runSta, mode);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 22, runSta, false);//运行状态
		MemDb::SetRealData(regStart + 23, mode, false);//运行模式
		if (fault != 0)
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
	msleep(200);

	//获取电压
	ret = GetVol(voltage);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 28, voltage, false);//电压
	}
	msleep(200);

	//获取电流
	ret = GetCur(current);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 29, current, false);//电流
	}
	msleep(200);

	//获取功率
	ret = GetPow(power);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 30, power, false);//功率
	}
	msleep(200);

	//获取设置反馈值
	//获取设置启动停止
	ret = GetStart(runSta);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 26, runSta, false);//设置启动停止
	}
	msleep(200);

	//获取设置输出工作模式
	ret = GetOutMode(mode);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 27, mode, false);//工作模式
	}
	msleep(200);

	//获取设置输出电流值
	ret = GetCurrent(setCurr);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 32, setCurr, false);//设置电流值
	}
	msleep(200);

	//获取设置输出电压值
	ret = GetVoltage(setVol);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 31, setVol, false);//设置电压值
	}
	msleep(200);

	//获取设置开路电压值
	ret = GetVoc(setVoc);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 33, setVoc, false);//设置开路电压
	}
	msleep(200);

	//获取设置短路电流值
	ret = GetIsc(setIsc);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 34, setIsc, false);//设置短路电流
	}
	msleep(200);

	//获取最大功率点电压
	ret = GetPmpV(setPmpV);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 35, setPmpV, false);//最大功率点电压
	}
	msleep(200);

	//获取最大功率点电流
	ret = GetPmpI(setPmpI);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 36, setPmpI, false);//最大功率点电流
	}
	msleep(200);

	//获取EN50530最大功率点功率
	ret = GetPmpP(setPmpP);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 37, setPmpP, false);//EN50530最大功率点功率
	}
	msleep(200);

	//获取设置EN50530模式填充因子值
	ret = GetTech(value);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 38, value, false);//填充因子
	}
	msleep(200);

	//获取设置EN50530模式光照幅度值
	ret = GetIrr(irr);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 39, irr, false);//光照幅度
	}
	msleep(200);

	//获取SANDIA模式设置光照幅度参考值
	ret = GetIrrRef(irrRef);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 41, irrRef, false);//光照幅度参考值
	}
	msleep(200);

	//获取设置SANDIA模式温度参考值
	ret = GetTmpRef(tmpRef);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 45, tmpRef, false);//温度参考值
	}
	msleep(200);

	//获取设置SANDIA模式电压温度系数
	ret = GetBeta(beta);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 43, beta, false);//电压温度系数
	}
	msleep(200);

	//获取设置SANDIA模式填充因数
	ret = GetFF(ff);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 42, ff, false);//填充因数
	}
	msleep(200);

	//获取设置SANDIA模式温度
	ret = GetSandiaTmp(tmp);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 44, tmp, false);//温度
	}
	msleep(200);

	//获取设置SANDIA模式光照幅度
	ret = GetSandiaIrr(irr);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 46, irr, false);//SANDIA模式光照幅度
	}
	msleep(200);

	//获取设置SANDIA模式最大功率点功率
	ret = GetSandiaPmpP(setPmpP);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 40, setPmpP, false);//SANDIA模式最大功率点功率
	}
	msleep(200);

	//获取设置SANDIA模式最大功率点电压
	ret = GetSandiaPmpV(setPmpV);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 47, setPmpV, false);//SANDIA模式最大功率点电压
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int ChromaSimultor::Chroma62000Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	int outMode = 0;
	int run = 0;
	RealData realData;

	//判断是否有启停设定值
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		// ret = SetTrig();//更新波形
		ret = SetStart(realData.pv.data.iValue);
		// ret = SetTrig();//更新波形

	}

	//判断是否有输出模式设定值
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		ret = SetOutMode(realData.pv.data.iValue);
	}

	MemDb::GetRealData(outMode, regStart + 2);
	MemDb::GetRealData(run, regStart + 22);//启停状态

	if (outMode == 1)//CVCC
	{
		//判断是否电压设定值
		if (MemDb::GetRealData(realData, regStart + 5, true))
		{
			ret = SetVoltage(realData.pv.data.dbValue);
		}

		//判断是否电流设定值
		if (MemDb::GetRealData(realData, regStart + 6, true))
		{
			ret = SetCurrent(realData.pv.data.dbValue);
		}
	}
	else if (outMode == 2)//SAS模式
	{
		//判断是否开路电压设定值
		if (MemDb::GetRealData(realData, regStart + 7, true))
		{
			ret = SetVoc(realData.pv.data.dbValue);
			if (run == 1)//启动
			{
				ret = SetTrig();//更新波形
			}
		}

		//判断是否短路电流设定值
		if (MemDb::GetRealData(realData, regStart + 8, true))
		{
			ret = SetIsc(realData.pv.data.dbValue);
			if (run == 1)//启动
			{
				// ret = SetTrig();//更新波形
			}
		}

		//判断是否最大功率点电压设定值
		if (MemDb::GetRealData(realData, regStart + 9, true))
		{
			ret = SetPmpV(realData.pv.data.dbValue);
			if (run == 1)//启动
			{
				ret = SetTrig();//更新波形
			}
		}

		//判断是否最大功率点电流设定值
		if (MemDb::GetRealData(realData, regStart + 10, true))
		{
			ret = SetPmpI(realData.pv.data.dbValue);
			if (run == 1)//启动
			{
				ret = SetTrig();//更新波形
			}
		}
		// ret = SetTrig();//更新波形
	}
	else if (outMode == 3)//EN50530模式
	{
		//判断是否最大功率点功率设定值
		if (MemDb::GetRealData(realData, regStart + 11, true))
		{
			ret = SetPmpP(realData.pv.data.dbValue);
			if (run == 1)//启动
			{
				ret = SetTrig();//更新波形
			}
		}

		//判断是否最大功率点电压设定值
		if (MemDb::GetRealData(realData, regStart + 9, true))
		{
			ret = SetPmpV(realData.pv.data.dbValue);
			if (run == 1)//启动
			{
				ret = SetTrig();//更新波形
			}
		}

		//判断是否填充因数设定值
		if (MemDb::GetRealData(realData, regStart + 12, true))
		{
			int i = realData.pv.data.iValue;
//			zlog_warn(Util::m_zlog,"下发数据================ivalue:%f",i);
			ret = SetTech(i);
		}

		//判断是否光照幅度设定值
		if (MemDb::GetRealData(realData, regStart + 13, true))
		{
			ret = SetIrr(realData.pv.data.dbValue);
		}
	}
	else if (outMode == 4)//SANDIA模式
	{
		//判断是否最大功率点功率设定值
		if (MemDb::GetRealData(realData, regStart + 14, true))
		{
			ret = SetSandiaPmpP(realData.pv.data.dbValue);
			if (run == 1)//启动
			{
				ret = SetTrig();//更新波形
			}
		}

		//判断是否最大功率点电压设定值
		if (MemDb::GetRealData(realData, regStart + 21, true))
		{
			ret = SetSandiaPmpV(realData.pv.data.dbValue);
			if (run == 1)//启动
			{
				ret = SetTrig();//更新波形
			}
		}

		//判断是否光照幅度参考值设定值
		if (MemDb::GetRealData(realData, regStart + 15, true))
		{
			ret = SetIrrRef(realData.pv.data.dbValue);
		}

		//判断是否填充因数设定值
		if (MemDb::GetRealData(realData, regStart + 16, true))
		{
			ret = SetFF(realData.pv.data.dbValue);
		}

		//判断是否电压温度系数设定值
		if (MemDb::GetRealData(realData, regStart + 17, true))
		{
			ret = SetBeta(realData.pv.data.dbValue);
		}

		//判断是否温度设定值
		if (MemDb::GetRealData(realData, regStart + 18, true))
		{
			ret = SetSandiaTmp(realData.pv.data.dbValue);
		}

		//判断是否温度参考值设定值
		if (MemDb::GetRealData(realData, regStart + 19, true))
		{
			ret = SetTmpRef(realData.pv.data.dbValue);
		}

		//判断是否光照幅度设定值
		if (MemDb::GetRealData(realData, regStart + 20, true))
		{
			ret = SetSandiaIrr(realData.pv.data.dbValue);
		}
	}

	return ret;
}

//获取实时输出电流值
int ChromaSimultor::GetCur(float& current)
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
	sprintf(tmp, "FETC:CURR?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	current = (atof(packData.c_str()));
	return ret;
}

//获取实时输出电压值
int ChromaSimultor::GetVol(float& voltage)
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
	sprintf(tmp, "FETC:VOLT?\r\n");
	str = tmp;

	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	voltage = atof(packData.c_str());
	return ret;
}

//获取实时输出功率值
int ChromaSimultor::GetPow(float& power)
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
	sprintf(tmp, "FETC:POW?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	power = atof(packData.c_str());
	return ret;
}

//获取状态
int ChromaSimultor::GetState(int& fault, int& state, int& mode)
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
	str1.clear();
	str2.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "FETC:STAT?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	sprintf(temp, ",");
	str1 = temp;
	len = packData.find(str1.c_str());
	str2 = packData.substr(0, len);
	fault = atoi(str2.c_str());
	packData.erase(0, len + 1);

	str2.clear();
	len = packData.find(str1.c_str());
	str2 = packData.substr(0, len);
	if (str2.compare("ON") == 0)
	{
		state = 1;
	}
	else if (str2.compare("OFF") == 0)
	{
		state = 0;
	}
	packData.erase(0, len + 1);

	str2.clear();
	str2 = packData.substr(0, 2);
	if (str2.compare("CC\n") == 0)
	{
		mode = 2;
	}
	else if (str2.compare("CV\n") == 0)
	{
		mode = 1;
	}

	return ret;
}

//设置启动/停止
int ChromaSimultor::SetStart(int runSta)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	if (runSta == 1)
	{
		sprintf(tmp, "CONF:OUTP ON\r\n");
	}
	else if (runSta == 0)
	{
		sprintf(tmp, "CONF:OUTP OFF\r\n");
	}
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	//for (int i = 0; i < 3; i++)
	//{
		m_tcp->SendFlush(values);
		//msleep(50);
	//}

	return ret;
}

//获取设置启动停止
int ChromaSimultor::GetStart(int& mode)
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
	sprintf(tmp, "CONF:OUTP?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	if (packData.compare("ON\n") == 0)
	{
		mode = 1;
	}
	else if (packData.compare("OFF\n") == 0)
	{
		mode = 0;
	}

	return ret;
}

//设置输出工作模式
int ChromaSimultor::SetOutMode(int mode)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	if (mode == 1)//CVCC
	{
		sprintf(tmp, "OUTP:MODE CVCC\r\n");
	}
	else if (mode == 2)//SAS
	{
		sprintf(tmp, "OUTP:MODE SAS\r\n");
	}
	else if (mode == 3)//EN50530
	{
		sprintf(tmp, "OUTP:MODE EN50530\r\n");
	}
	else if (mode == 4)//SANDIA
	{
		sprintf(tmp, "OUTP:MODE SANDIA\r\n");
	}

	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	//for (int i = 0; i < 3; i++)
	//{
		m_tcp->SendFlush(values);
		//msleep(50);
	//}

	return ret;
}

//获取设置输出工作模式
int ChromaSimultor::GetOutMode(int& mode)
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
	sprintf(tmp, "SAS:MODEL?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	if (packData.compare("DEFAULT\n") == 0)
	{
		mode = 2;
	}
	else if (packData.compare("EN50530\n") == 0)
	{
		mode = 3;
	}
	else if (packData.compare("SANDIA\n") == 0)
	{
		mode = 4;
	}else{
		mode = 1;
	}

	return ret;
}

//设置输出电流值
int ChromaSimultor::SetCurrent(float current)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SOUR:CURR %.2f\r\n", current);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置输出电流值
int ChromaSimultor::GetCurrent(float& current)
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
	sprintf(tmp, "SOUR:CURR?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	current = atof(packData.c_str());
	return ret;
}

//设置输出电压值
int ChromaSimultor::SetVoltage(float voltage)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SOUR:VOLT %.2f\r\n", voltage);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置输出电压值
int ChromaSimultor::GetVoltage(float& voltage)
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
	sprintf(tmp, "SOUR:VOLT?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	voltage = atof(packData.c_str());
	return ret;
}

//设置输出开路电压
int ChromaSimultor::SetVoc(float voc)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:VOC %.2f\r\n", voc);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置开路电压值
int ChromaSimultor::GetVoc(float& voc)
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
	sprintf(tmp, "SAS:VOC?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	voc = atof(packData.c_str());
	return ret;
}

//设置输出短路电流
int ChromaSimultor::SetIsc(float isc)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:ISC %.2f\r\n", isc);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}
	return ret;
}

//获取设置短路电流值
int ChromaSimultor::GetIsc(float& isc)
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
	sprintf(tmp, "SAS:ISC?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	isc = atof(packData.c_str());
	return ret;
}

//设置最大功率点电压
int ChromaSimultor::SetPmpV(float pmpV)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:VMPp %.2f\r\n", pmpV);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置最大功率点电压值
int ChromaSimultor::GetPmpV(float& pmpV)
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
	sprintf(tmp, "SAS:VMPp?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	pmpV = atof(packData.c_str());
	return ret;
}

//设置最大功率点电流
int ChromaSimultor::SetPmpI(float pmpI)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:IMPp %.2f\r\n", pmpI);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置最大功率点电流值
int ChromaSimultor::GetPmpI(float& pmpI)
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
	sprintf(tmp, "SAS:IMPp?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	pmpI = atof(packData.c_str());
	return ret;
}

//当SAS模式正在执行是，更新SASMode输出波形
int ChromaSimultor::SetTrig()
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "TRIG\r\n");
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//设置最大功率点功率
int ChromaSimultor::SetPmpP(float pmpP)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:PMPp %.2f\r\n", pmpP);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}
	return ret;
}

//获取设置最大功率点功率值
int ChromaSimultor::GetPmpP(float& pmpP)
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
	sprintf(tmp, "SAS:PMPp?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	pmpP = atof(packData.c_str());
	return ret;
}

//设置EN50530模式填充因子
int ChromaSimultor::SetTech(int mode)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();


	zlog_debug(Util::m_zlog,"下发数据================mode:%d",mode);
	//封装控制语句
	if (mode == 1)//CSI
	{
		sprintf(tmp, "SAS:TECH CSI\r\n");
	}
	else if (mode == 2)
	{
		sprintf(tmp, "SAS:TECH TF\r\n");
	}

	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置EN50530模式填充因子值
int ChromaSimultor::GetTech(int& mode)
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
	sprintf(tmp, "SAS:TECH?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	if (packData.compare("CSI\n") == 0)//CSI
	{
		mode = 1;
	}
	else if (packData.compare("TF\n") == 0)
	{
		mode = 2;
	}

	return ret;
}

//设置EN50530模式光照幅度
int ChromaSimultor::SetIrr(float irr)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	int temp = 0;
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	temp = (int) irr;
	zlog_debug(Util::m_zlog,"下发数据irr=============:%d",temp);
	sprintf(tmp, "SAS:IRR %d\r\n", temp);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置EN50530模式光照幅度值
int ChromaSimultor::GetIrr(float& irr)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	int temp = 0;
	string str;
	string packData;
	String values;
	String reply;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "SAS:IRR?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	 irr = atoi(packData.c_str());
	//temp = atoi(packData.c_str());
	//irr = (float) temp;
	return ret;
}

//设置SANDIA模式光照幅度参考值
int ChromaSimultor::SetIrrRef(float irrRef)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	int temp = 0;
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	temp = (int) irrRef;
	zlog_debug(Util::m_zlog,"下发数据:irrRef============%d",temp);
	sprintf(tmp, "SAS:SANDIA:IRRREF %d\r\n", temp);
	str = tmp;
	zlog_debug(Util::m_zlog,"下发数据:%s",str.c_str());
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取SANDIA模式设置光照幅度参考值
int ChromaSimultor::GetIrrRef(float& irrRef)
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
	sprintf(tmp, "SAS:SANDIA:IRRREF?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	irrRef = atof(packData.c_str());
	return ret;
}

//设置SANDIA模式温度参考值
int ChromaSimultor::SetTmpRef(float tmpRef)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:SANDIA:TMPREF %.2f\r\n", tmpRef);
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置SANDIA模式温度参考值
int ChromaSimultor::GetTmpRef(float& tmpRef)
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
	sprintf(tmp, "SAS:SANDIA:TMPREF?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	tmpRef = atof(packData.c_str());
	return ret;
}

//设置SANDIA模式电压温度系数
int ChromaSimultor::SetBeta(float beta)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:SANDIA:BETA %.2f\r\n", beta);
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置SANDIA模式电压温度系数
int ChromaSimultor::GetBeta(float& beta)
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
	sprintf(tmp, "SAS:SANDIA:BETA?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	beta = atof(packData.c_str());
	return ret;
}

//设置SANDIA模式填充因数
int ChromaSimultor::SetFF(float ff)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:SANDIA:FF %.2f\r\n", ff);
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置SANDIA模式填充因数
int ChromaSimultor::GetFF(float& ff)
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
	sprintf(tmp, "SAS:SANDIA:FF?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	ff = atof(packData.c_str());
	return ret;
}

//设置SANDIA模式光照幅度
int ChromaSimultor::SetSandiaIrr(float irr)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	int temp = 0;
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	temp = (int) irr;
	sprintf(tmp, "SAS:SANDIA:IRR %d\r\n", temp);
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置SANDIA模式光照幅度
int ChromaSimultor::GetSandiaIrr(float& irr)
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
	sprintf(tmp, "SAS:SANDIA:IRR?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	irr = atof(packData.c_str());
	return ret;
}

//设置SANDIA模式温度
int ChromaSimultor::SetSandiaTmp(float temp)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:SANDIA:TMP %.2f\r\n", temp);
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置SANDIA模式温度
int ChromaSimultor::GetSandiaTmp(float& temp)
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
	sprintf(tmp, "SAS:SANDIA:TMP?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	temp = atof(packData.c_str());
	return ret;
}

//设置SANDIA模式最大功率点功率
int ChromaSimultor::SetSandiaPmpP(float pmpP)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:SANDIA:PMP %.2f\r\n", pmpP);
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置SANDIA模式最大功率点功率
int ChromaSimultor::GetSandiaPmpP(float& pmpP)
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
	sprintf(tmp, "SAS:SANDIA:PMP?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	pmpP = atof(packData.c_str());
	return ret;
}

//设置SANDIA模式最大功率点电压
int ChromaSimultor::SetSandiaPmpV(float pmpV)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SAS:SANDIA:VMP %.2f\r\n", pmpV);
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	for (int i = 0; i < 3; i++)
	{
		m_tcp->SendFlush(values);
		msleep(50);
	}

	return ret;
}

//获取设置SANDIA模式最大功率点电压
int ChromaSimultor::GetSandiaPmpV(float& pmpV)
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
	sprintf(tmp, "SAS:SANDIA:VMP?\r\n");
	str = tmp;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	zlog_debug(Util::m_zlog,"下发数据：%s",str.c_str());
	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if (ret == ErrorInfo::ERR_OK)
	{
		for (unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}
	zlog_debug(Util::m_zlog,"上传数据：%s",packData.c_str());
	pmpV = atof(packData.c_str());
	return ret;
}
