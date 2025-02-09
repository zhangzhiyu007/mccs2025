/*
 * ChromaLoad.cpp
 *
 *  创建日期: 2016-6-20
 *  作          者: 刘海涛
 */

#include "ChromaLoad.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

ChromaLoad::ChromaLoad()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

ChromaLoad::~ChromaLoad()
{
	// TODO Auto-generated destructor stub
}

void ChromaLoad::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void ChromaLoad::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int ChromaLoad::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_63804:
		ret = this->Chroma63804Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaLoad::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_63804:
		ret = this->Chroma63804Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaLoad::Chroma63804Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma63800Read(dev);
	return ret;
}

int ChromaLoad::Chroma63804Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma63800Preset(dev);
	return ret;
}

int ChromaLoad::Chroma63800Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int regStart = dev->regStart;
	int value = 0;
	int mode = 0;
	float tmp = 0.0;

	//实时值
	//获取实时电压
	ret = GetVol(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 22, tmp, false);//电压
	}
	msleep(200);

	//获取实时电流
	ret = GetCur(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 23, tmp, false);//电流
	}
	msleep(200);

	//获取实时输出有功功率值
	ret = GetPower(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 25, tmp, false);//有功功率
	}
	msleep(200);

	//获取实时输出无功功率值
	ret = GetQPower(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 26, tmp, false);//无功功率
	}
	msleep(200);

	//获取实时输出视在功率值
	ret = GetSPower(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 27, tmp, false);//视在功率
	}
	msleep(200);

	//获取实时输出频率值
	ret = GetFreq(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 24, tmp, false);//频率
	}
	msleep(200);

	//获取实时输出功率因数值
	ret = GetPF(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 28, tmp, false);//功率因数
	}
	msleep(200);

	//获取实时故障码
	ret = GetFault(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		if(value != 0)
		{
			MemDb::SetRealData(regStart + 20, 1, false);//故障状态
			MemDb::SetRealData(regStart + 21, value, false);//故障码
		}
		else
		{
			MemDb::SetRealData(regStart + 20, 0, false);//故障状态
			MemDb::SetRealData(regStart + 21, value, false);//故障码
		}

	}
	msleep(200);

	//反馈值
	//获取启动停止设置
	ret = GetStart(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 1, value, false);//启动/停止
		MemDb::SetRealData(regStart + 18, value, false);//运行状态
	}
	msleep(200);

	//获取工作模式设置
	ret = GetOutMode(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 3, value, false);//工作模式
		MemDb::SetRealData(regStart + 19, value, false);//运行模式
	}
	msleep(200);

	//获取设置IpMax
	ret = GetIpMax(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 15, tmp, false);//IpMax
	}

	//获取设置IrmsMax
	ret = GetIrmsMax(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 16, tmp, false);//IrmsMax
	}

	//获取工作模式
	MemDb::GetRealData(mode, regStart + 3);//工作模式
	if(mode == 1)//CC
	{
		//获取设置短路模拟
		ret = GetScirCuit(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 4, value, false);//短路模拟
		}
		msleep(200);

		//获取设置CC模式电流值
		ret = GetCCCur(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 5, tmp, false);//电流
		}
	}
	else if(mode == 2)//CP
	{
		//获取设置短路模拟
		ret = GetScirCuit(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 4, value, false);//短路模拟
		}
		msleep(200);

		//获取设置CP模式功率值
		ret = GetCPPow(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 6, tmp, false);//功率
		}
	}
	else if(mode == 3)//CR
	{
		//获取设置短路模拟
		ret = GetScirCuit(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 4, value, false);//短路模拟
		}
		msleep(200);

		//获取设置CR模式电阻值
		ret = GetCRRes(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 7, tmp, false);//电阻
		}
	}
	else if(mode == 4)//RLC
	{
		//获取设置RLC模式电容值
		ret = GetRLCC(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 8, tmp, false);//电容
		}
		msleep(200);

		//获取设置RLC模式Ls值
		ret = GetRLCLs(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 9, tmp, false);//Ls
		}
		msleep(200);

		//获取设置RLC模式Rl值
		ret = GetRLCRl(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 10, tmp, false);//Rl
		}
		msleep(200);

		// 获取设置RLC模式Rs值
		ret = GetRLCRs(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 11, tmp, false);//Rs
		}
	}
	else if(mode == 5)//RLC定功率
	{
		// 获取设置RLC定功率模式功率值
		ret = GetRLCCPPow(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 12, tmp, false);//功率
		}
		msleep(200);

		// 获取设置RLC定功率模式功率因数
		ret = GetRLCCPPf(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 13, tmp, false);//功率因数
		}
	}
	else if(mode == 6)//RLC浪涌电流
	{
		//获取设置INR(RLC浪涌电流)模式电容值
		ret = GetINRC(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 8, tmp, false);//电容
		}
		msleep(200);

		//获取设置INR(RLC浪涌电流)模式Ls值
		ret = GetINRLs(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 9, tmp, false);//Ls
		}
		msleep(200);

		//获取设置INR(RLC浪涌电流)模式Rl值
		ret = GetINRRl(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 10, tmp, false);//Rl
		}
		msleep(200);

		// 获取设置INR(RLC浪涌电流)模式Rs值
		ret = GetINRRs(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 11, tmp, false);//Rs
		}
		msleep(200);

		// 获取INR(RLC浪涌电流)模式启动相位值
		ret = GetINRPhas(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 14, tmp, false);//启动相位值
		}
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

	return ret;
}

int ChromaLoad::Chroma63800Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	int mode = 0;
	RealData realData;

	//判断是否有启停设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		ret = SetStart(realData.pv.data.iValue);
	}

	//判断是否有故障清除设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		ret = ClearFault();
	}

	//判断是否有工作设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		ret = SetOutMode(realData.pv.data.iValue);
	}

	//获取工作模式
	MemDb::GetRealData(mode, regStart + 3);//工作模式
	if(mode == 1)//CC
	{
		//判断是否有短路模拟值
		if(MemDb::GetRealDataWrited(realData, regStart + 4, true))
		{
			ret = SetScirCuit(realData.pv.data.iValue);
		}

		//判断是否有IpMax设定
		if(MemDb::GetRealDataWrited(realData, regStart + 15, true))
		{
			ret = SetIpMax(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否有电流值
		if(MemDb::GetRealDataWrited(realData, regStart + 5, true))
		{
			ret = SetCCCur(realData.pv.data.dbValue);
		}
	}
	else if(mode == 2)//CP
	{
		//判断是否有短路模拟值
		if(MemDb::GetRealDataWrited(realData, regStart + 4, true))
		{
			ret = SetScirCuit(realData.pv.data.iValue);
		}

		//判断是否有IpMax设定
		if(MemDb::GetRealDataWrited(realData, regStart + 15, true))
		{
			ret = SetIpMax(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否有功率值
		if(MemDb::GetRealDataWrited(realData, regStart + 6, true))
		{
			ret = SetCPPow(realData.pv.data.dbValue);
		}
	}
	else if(mode == 3)//CR
	{
		//判断是否有短路模拟值
		if(MemDb::GetRealDataWrited(realData, regStart + 4, true))
		{
			ret = SetScirCuit(realData.pv.data.iValue);
		}

		//判断是否有IrmsMax设定
		if(MemDb::GetRealDataWrited(realData, regStart + 16, true))
		{
			ret = SetIrmsMax(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否有电阻值
		if(MemDb::GetRealDataWrited(realData, regStart + 7, true))
		{
			ret = SetCRRes(realData.pv.data.dbValue);
		}
	}
	else if(mode == 4)//RLC
	{
		//判断是否有IpMax设定
		if(MemDb::GetRealDataWrited(realData, regStart + 15, true))
		{
			ret = SetIpMax(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否RLC模式电容值
		if(MemDb::GetRealDataWrited(realData, regStart + 8, true))
		{
			ret = SetRLCC(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否RLC模式Ls值
		if(MemDb::GetRealDataWrited(realData, regStart + 9, true))
		{
			ret = SetRLCLs(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否RLC模式Rl值
		if(MemDb::GetRealDataWrited(realData, regStart + 10, true))
		{
			ret = SetRLCRl(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否RLC模式Rs值
		if(MemDb::GetRealDataWrited(realData, regStart + 11, true))
		{
			ret = SetRLCRs(realData.pv.data.dbValue);
		}
	}
	else if(mode == 5)//RLC定功率
	{
		//判断是否有IpMax设定
		if(MemDb::GetRealDataWrited(realData, regStart + 15, true))
		{
			ret = SetIpMax(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否RLC定功率模式功率值
		if(MemDb::GetRealDataWrited(realData, regStart + 12, true))
		{
			ret = SetRLCCPPow(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否RLC定功率模式功率因数
		if(MemDb::GetRealDataWrited(realData, regStart + 13, true))
		{
			ret = SetRLCCPPf(realData.pv.data.dbValue);
		}
	}
	else if(mode == 6)//RLC浪涌电流
	{
		//判断是否有IpMax设定
		if(MemDb::GetRealDataWrited(realData, regStart + 15, true))
		{
			ret = SetIpMax(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否INR(RLC浪涌电流)模式电容值
		if(MemDb::GetRealDataWrited(realData, regStart + 8, true))
		{
			ret = SetINRC(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否INR(RLC浪涌电流)模式Ls值
		if(MemDb::GetRealDataWrited(realData, regStart + 9, true))
		{
			ret = SetINRLs(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否INR(RLC浪涌电流)模式Rl值
		if(MemDb::GetRealDataWrited(realData, regStart + 10, true))
		{
			ret = SetINRRl(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否INR(RLC浪涌电流)模式Rs值
		if(MemDb::GetRealDataWrited(realData, regStart + 11, true))
		{
			ret = SetINRRs(realData.pv.data.dbValue);
		}
		msleep(200);

		//判断是否INR(RLC浪涌电流)模式启动相位值
		if(MemDb::GetRealDataWrited(realData, regStart + 14, true))
		{
			ret = SetINRPhas(realData.pv.data.dbValue);
		}
	}
	return ret;
}

//获取实时输出电流值
int ChromaLoad::GetCur(float& current)
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
int ChromaLoad::GetVol(float& voltage)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;
	int len = 0;

	str.clear();
	packData.clear();
	values.clear();
	reply.clear();

	//封装下发语句
	sprintf(tmp, "MEAS:VOLT?\r\n");
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
int ChromaLoad::GetPower(float& power)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	string packData;
	String values;
	String reply;
	int len = 0;

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

//获取实时输出无功功率值
int ChromaLoad::GetQPower(float& qPower)
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
	sprintf(tmp, "MEAS:POW:REAC?\r\n");
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

	zlog_debug(Util::m_zlog,"无功查询命令返回：%s",packData.c_str());
	qPower = atof(packData.c_str());
	return ret;
}

//获取实时输出视在功率值
int ChromaLoad::GetSPower(float& sPower)
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
	sprintf(tmp, "MEAS:POW:APP?\r\n");
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

	zlog_debug(Util::m_zlog,"视在功率查询命令返回：%s",packData.c_str());
	sPower = atof(packData.c_str());
	return ret;
}

//获取实时输出频率值
int ChromaLoad::GetFreq(float& freq)
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
	sprintf(tmp, "MEAS:FREQ?\r\n");
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

	zlog_debug(Util::m_zlog,"频率查询命令返回：%s",packData.c_str());
	freq = atof(packData.c_str());
	return ret;
}

//获取实时输出功率因数值
int ChromaLoad::GetPF(float& pf)
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
	sprintf(tmp, "MEAS:POW:PFAC?\r\n");
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

	zlog_debug(Util::m_zlog,"功率因数查询命令返回：%s",packData.c_str());
	pf = atof(packData.c_str());
	return ret;
}

//获取实时故障码
int ChromaLoad::GetFault(int& fault)
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
	sprintf(tmp, "PROT?\r\n");
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

	zlog_debug(Util::m_zlog,"故障码查询命令返回：%s",packData.c_str());
	fault = atoi(packData.c_str());
	return ret;
}

//设置启动/停止
int ChromaLoad::SetStart(int runSta)
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
		sprintf(tmp, "LOAD ON\r\n");
	}
	else if(runSta == 0)
	{
		sprintf(tmp, "LOAD OFF\r\n");
	}
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置启动停止
int ChromaLoad::GetStart(int& runSta)
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
	sprintf(tmp, "LOAD?\r\n");
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
	if((packData.compare("ON\n") == 0) || (atoi(packData.c_str()) == 1))
	{
		runSta = 1;
	}
	else if((packData.compare("OFF\n") == 0) || (atoi(packData.c_str()) == 0))//OFF
	{
		runSta = 0;
	}
	return ret;
}

//设置输出工作模式
int ChromaLoad::SetOutMode(int mode)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	if(mode == 1)//CC
	{
		sprintf(tmp, "MODE CURR\r\n");
	}
	else if(mode == 2)//CP
	{
		sprintf(tmp, "MODE POW\r\n");
	}
	else if(mode == 3)//CR
	{
		sprintf(tmp, "MODE RES\r\n");
	}
	else if(mode == 4)//RLC
	{
		sprintf(tmp, "MODE RLC\r\n");
	}
	else if(mode == 5)//RLC定功率
	{
		sprintf(tmp, "MODE RLCP\r\n");
	}
	else if(mode == 6)//RLC浪涌电流
	{
		sprintf(tmp, "MODE INRUSH\r\n");
	}
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置输出工作模式
int ChromaLoad::GetOutMode(int& mode)
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
	sprintf(tmp, "MODE?\r\n");
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
	if(packData.compare("CURR\n") == 0)
	{
		mode = 1;
	}
	else if(packData.compare("POW\n") == 0)
	{
		mode = 2;
	}
	else if(packData.compare("RES\n") == 0)
	{
		mode = 3;
	}
	else if(packData.compare("RLC\n") == 0)
	{
		mode = 4;
	}
	else if(packData.compare("RLCP\n") == 0)
	{
		mode = 5;
	}
	else if(packData.compare("INRUSH\n") == 0)//INRUSH
	{
		mode = 6;
	}
	return ret;
}

//清除故障
int ChromaLoad::ClearFault()
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "PROT:CLE\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//设置短路模拟
int ChromaLoad::SetScirCuit(int value)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "SCIR %d\r\n", value);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置短路模拟
int ChromaLoad::GetScirCuit(int& value)
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
	sprintf(tmp, "SCIR?\r\n");
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

	zlog_debug(Util::m_zlog,"短路模拟反馈命令返回：%s",packData.c_str());
	value = atoi(packData.c_str());
	return ret;
}

//设置CC模式电流值
int ChromaLoad::SetCCCur(float current)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "CURR %f\r\n", current);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//设置IpMax值
int ChromaLoad::SetIpMax(float ipMax)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "CURR:PEAK:MAX %f\r\n", ipMax);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//设置IrmsMax
int ChromaLoad::SetIrmsMax(float IrmsMax)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "CURR:MAX %f\r\n", IrmsMax);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置IpMax
int ChromaLoad::GetIpMax(float& ipMax)
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
	sprintf(tmp, "CURR:PEAK:MAX?\r\n");
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

	zlog_debug(Util::m_zlog,"IpMax反馈命令返回：%s",packData.c_str());
	ipMax = atof(packData.c_str());
	return ret;
}

//获取设置IrmsMax
int ChromaLoad::GetIrmsMax(float& IrmsMax)
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
	sprintf(tmp, "CURR:MAX?\r\n");
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

	zlog_debug(Util::m_zlog,"IrmsMax反馈命令返回：%s",packData.c_str());
	IrmsMax = atof(packData.c_str());
	return ret;
}

//获取设置CC模式电流值
int ChromaLoad::GetCCCur(float& current)
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
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"CC电流值反馈命令返回：%s",packData.c_str());
	current = atof(packData.c_str());
	return ret;
}

//设置CP模式功率值
int ChromaLoad::SetCPPow(float power)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "POW %f\r\n", power);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置CP模式功率值
int ChromaLoad::GetCPPow(float& power)
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
	ret = m_tcp->TxRxMsg(reply, values, 100);
	if(ret == ErrorInfo::ERR_OK)
	{
		for(unsigned int i = 0; i < reply.size(); i++)
		{
			packData.push_back(reply[i]);
		}
	}

	zlog_debug(Util::m_zlog,"CP功率值反馈命令返回：%s",packData.c_str());
	power = atof(packData.c_str());
	return ret;
}

//设置CR模式电阻值
int ChromaLoad::SetCRRes(float res)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RES %f\r\n", res);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置CR模式电阻值
int ChromaLoad::GetCRRes(float& res)
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
	sprintf(tmp, "RES?\r\n");
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

	zlog_debug(Util::m_zlog,"CR电阻值反馈命令返回：%s",packData.c_str());
	res = atof(packData.c_str());
	return ret;
}

//设置RLC模式电容值
int ChromaLoad::SetRLCC(float cap)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RLC:CAP %f\r\n", cap);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置RLC模式电容值
int ChromaLoad::GetRLCC(float& cap)
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
	sprintf(tmp, "RLC:CAP?\r\n");
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

	zlog_debug(Util::m_zlog,"RLC电容值反馈命令返回：%s",packData.c_str());
	cap = atof(packData.c_str());
	return ret;
}

//设置RLC模式Ls值
int ChromaLoad::SetRLCLs(float ls)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RLC:LS %f\r\n", ls);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置RLC模式Ls值
int ChromaLoad::GetRLCLs(float& ls)
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
	sprintf(tmp, "RLC:LS?\r\n");
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

	zlog_debug(Util::m_zlog,"RLCls反馈命令返回：%s",packData.c_str());
	ls = atof(packData.c_str());
	return ret;
}

//设置RLC模式Rl值
int ChromaLoad::SetRLCRl(float rl)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RLC:RL %f\r\n", rl);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置RLC模式Rl值
int ChromaLoad::GetRLCRl(float& rl)
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
	sprintf(tmp, "RLC:RL?\r\n");
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

	zlog_debug(Util::m_zlog,"RLCrl反馈命令返回：%s",packData.c_str());
	rl = atof(packData.c_str());
	return ret;
}

//设置RLC模式Rs值
int ChromaLoad::SetRLCRs(float rs)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RLC:RS %f\r\n", rs);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置RLC模式Rs值
int ChromaLoad::GetRLCRs(float& rs)
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
	sprintf(tmp, "RLC:RS?\r\n");
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

	zlog_debug(Util::m_zlog,"RLCRs反馈命令返回：%s",packData.c_str());
	rs = atof(packData.c_str());
	return ret;
}

//设置RLC定功率模式功率值
int ChromaLoad::SetRLCCPPow(float power)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RLC:POW %f\r\n", power);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置RLC定功率模式功率值
int ChromaLoad::GetRLCCPPow(float& power)
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
	sprintf(tmp, "RLC:POW?\r\n");
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

	zlog_debug(Util::m_zlog,"RLC定功率功率值反馈命令返回：%s",packData.c_str());
	power = atof(packData.c_str());
	return ret;
}

//设置RLC定功率模式功率因数
int ChromaLoad::SetRLCCPPf(float pf)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RLC:PAAC %f\r\n", pf);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置RLC定功率模式功率因数
int ChromaLoad::GetRLCCPPf(float& pf)
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
	sprintf(tmp, "RLC:PAAC?\r\n");
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

	zlog_debug(Util::m_zlog,"RLC定功率功率因数反馈命令返回：%s",packData.c_str());
	pf = atof(packData.c_str());
	return ret;
}

//设置INR(RLC浪涌电流)模式电容值
int ChromaLoad::SetINRC(float cap)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "INR:CAP %f\r\n", cap);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置INR(RLC浪涌电流)模式电容值
int ChromaLoad::GetINRC(float& cap)
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
	sprintf(tmp, "INR:CAP?\r\n");
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

	zlog_debug(Util::m_zlog,"INR电容值反馈命令返回：%s",packData.c_str());
	cap = atof(packData.c_str());
	return ret;
}

//设置INR(RLC浪涌电流)模式Ls值
int ChromaLoad::SetINRLs(float ls)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "INR:LS %f\r\n", ls);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置INR(RLC浪涌电流)模式Ls值
int ChromaLoad::GetINRLs(float& ls)
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
	sprintf(tmp, "INR:LS?\r\n");
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

	zlog_debug(Util::m_zlog,"INRLS反馈命令返回：%s",packData.c_str());
	ls = atof(packData.c_str());
	return ret;
}

//设置INR(RLC浪涌电流)模式Rl值
int ChromaLoad::SetINRRl(float rl)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "INR:RL %f\r\n", rl);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置INR(RLC浪涌电流)模式Rl值
int ChromaLoad::GetINRRl(float& rl)
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
	sprintf(tmp, "INR:RL?\r\n");
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

	zlog_debug(Util::m_zlog,"INRRL反馈命令返回：%s",packData.c_str());
	rl = atof(packData.c_str());
	return ret;
}

//设置INR(RLC浪涌电流)模式Rs值
int ChromaLoad::SetINRRs(float rs)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "INR:RS %f\r\n", rs);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取设置INR(RLC浪涌电流)模式Rs值
int ChromaLoad::GetINRRs(float& rs)
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
	sprintf(tmp, "INR:RS?\r\n");
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

	zlog_debug(Util::m_zlog,"INRRS反馈命令返回：%s",packData.c_str());
	rs = atof(packData.c_str());
	return ret;
}

//设置INR(RLC浪涌电流)模式启动相位值
int ChromaLoad::SetINRPhas(float phas)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "INR:PHAS %f\r\n", phas);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->SendFlush(values);

	return ret;
}

//获取INR(RLC浪涌电流)模式启动相位值
int ChromaLoad::GetINRPhas(float& phas)
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
	sprintf(tmp, "INR:PHAS?\r\n");
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

	zlog_debug(Util::m_zlog,"INR启动相位反馈命令返回：%s",packData.c_str());
	phas = atof(packData.c_str());
	return ret;
}
