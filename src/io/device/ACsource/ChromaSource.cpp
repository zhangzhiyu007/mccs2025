/*
 * ChromaSource.cpp
 *
 *  创建日期: 2016-6-28
 *  作          者: 刘海涛
 */

#include "ChromaSource.h"
#include "../../../etc/socket/TcpClient.h"

ChromaSource::ChromaSource()
{
	// TODO Auto-generated constructor stub
	m_tcp = NULL;
	m_flag = 0;
}

ChromaSource::~ChromaSource()
{
	// TODO Auto-generated destructor stub
}

//设置TcpClient指针
void ChromaSource::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int ChromaSource::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_61860:
		ret = this->Chroma61860Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaSource::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_61860:
		ret = this->Chroma61860Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaSource::Chroma61860Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma61800Read(dev);
	return ret;
}

int ChromaSource::Chroma61860Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma61800Preset(dev);
	return ret;
}

int ChromaSource::Chroma61800Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int regStart = dev->regStart;
	int value = 0;
	float data = 0.0;
	float tmp[3];
	string packData;
	string str;
	char temp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();


	//实时值
	//获取故障状态
	ret = GetFault(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		if(value != 0)
		{
			MemDb::SetRealData(regStart + 65, 1, false);//有故障
			MemDb::SetRealData(regStart + 66, value, false);//故障码
		}
		else
		{
			MemDb::SetRealData(regStart + 65, 0, false);//无故障
			MemDb::SetRealData(regStart + 66, value, false);//故障码
		}
	}
	msleep(200);

	//获取电压
	ret = GetVolt(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(unsigned int i = 0; i < 3; i++)
		{
			data = (float)tmp[i];
			MemDb::SetRealData(regStart + 67 + i, data, false);//电压
		}
	}
	msleep(200);

	//获取电流
	ret = GetCurr(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(unsigned int i = 0; i < 3; i++)
		{
			data = (float)tmp[i];
			MemDb::SetRealData(regStart + 70 + i, data, false);//电流
		}
	}
	msleep(200);

	//获取频率
	ret = GetHz(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(unsigned int i = 0; i < 3; i++)
		{
			data = (float)tmp[i];
			MemDb::SetRealData(regStart + 73 + i, data, false);//频率
		}
	}
	msleep(200);

	//获取功率因数
	ret = Getpf(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(unsigned int i = 0; i < 3; i++)
		{
			data = (float)tmp[i];
			MemDb::SetRealData(regStart + 76 + i, data, false);//功率因数
		}
	}
	msleep(200);

	//获取有功功率
	ret = GetPow(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(unsigned int i = 0; i < 3; i++)
		{
			data = (float)tmp[i];
			MemDb::SetRealData(regStart + 79 + i, data, false);//有功功率
		}
	}
	msleep(200);

	//获取无功功率
	ret = GetQPow(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(unsigned int i = 0; i < 3; i++)
		{
			data = (float)tmp[i];
			MemDb::SetRealData(regStart + 82 + i, data, false);//无功功率
		}
	}
	msleep(200);

	//获取视在功率
	ret = GetSPow(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(unsigned int i = 0; i < 3; i++)
		{
			data = (float)tmp[i];
			MemDb::SetRealData(regStart + 85 + i, data, false);//视在功率
		}
	}
	msleep(200);

	//设置反馈值
	//获取启动停止设置反馈量
	memset(temp, 0, sizeof(temp));
	str.clear();
	sprintf(temp, "OUTP?\r\n");
	str = temp;
	ret = ReadPack(str, packData);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		zlog_debug(Util::m_zlog,"查询启动设置反馈量:%s", packData.c_str());
		if(packData.compare("ON\n") == 0)
		{
			value = 1;
		}
		else if(packData.compare("OFF\n") == 0)//OFF
		{
			value = 0;
		}
		MemDb::SetRealData(regStart + 63, value, false);//运行模式
		MemDb::SetRealData(regStart + 1, value, false);//启动停止
	}
	msleep(200);

	//获取工作模式设置反馈量
	memset(temp, 0, sizeof(temp));
	str.clear();
	sprintf(temp, "OUTP:MODE?\r\n");
	str = temp;
	ret = ReadPack(str, packData);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		zlog_debug(Util::m_zlog,"查询工作模式设置反馈量:%s", packData.c_str());
		if(packData.compare("FIXED\n") == 0)
		{
			value = 1;
		}
		else if(packData.compare("LIST\n") == 0)
		{
			value = 2;
		}
		else if(packData.compare("PULSE\n") == 0)
		{
			value = 3;
		}
		else if(packData.compare("STEP\n") == 0)
		{
			value = 4;
		}
		else if(packData.compare("SYNTH\n") == 0)
		{
			value = 5;
		}
		else if(packData.compare("LIST\n") == 0)
		{
			value = 6;
		}
		MemDb::SetRealData(regStart + 64, value, false);//运行模式
		MemDb::SetRealData(regStart + 3, value, false);//工作模式
	}
	msleep(200);

	//获取输出模式设置反馈量
	memset(temp, 0, sizeof(temp));
	str.clear();
	sprintf(temp, "INST:PHAS?\r\n");
	str = temp;
	ret = ReadPack(str, packData);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		zlog_debug(Util::m_zlog,"查询输出模式设置反馈量:%s", packData.c_str());
		if(packData.compare("THREE\n") == 0)
		{
			value = 2;
		}
		else if(packData.compare("SINGLE\n") == 0)
		{
			value = 1;
		}
		MemDb::SetRealData(regStart + 4, value, false);//输出模式
	}
	msleep(200);

	//获取输出信号耦合设定设置反馈量
	memset(temp, 0, sizeof(temp));
	str.clear();
	sprintf(temp, "OUTP:COUP?\r\n");
	str = temp;
	ret = ReadPack(str, packData);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		zlog_debug(Util::m_zlog,"查询输出耦合设定设置反馈量:%s", packData.c_str());
		if(packData.compare("AC\n") == 0)
		{
			value = 1;
		}
		else if(packData.compare("DC\n") == 0)
		{
			value = 2;
		}
		else if(packData.compare("ACDC\n") == 0)
		{
			value = 3;
		}
		MemDb::SetRealData(regStart + 5, value, false);//输出耦合设定
	}
	msleep(200);

	//获取输出阻抗控制设置反馈量
	memset(temp, 0, sizeof(temp));
	str.clear();
	sprintf(temp, "OUTP:IMP:STAT?\r\n");
	str = temp;
	ret = ReadPack(str, packData);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		zlog_debug(Util::m_zlog,"查询阻抗控制设置反馈量:%s", packData.c_str());
		if(packData.compare("ON\n") == 0)
		{
			value = 1;
		}
		else if(packData.compare("OFF\n") == 0)
		{
			value = 0;
		}
		MemDb::SetRealData(regStart + 6, value, false);//输出阻抗控制
	}
	msleep(200);

	//获取输出阻抗电阻值设置反馈量
	memset(temp, 0, sizeof(temp));
	str.clear();
	sprintf(temp, "OUTP:IMP:RES?\r\n");
	str = temp;
	ret = ReadPack(str, packData);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		zlog_debug(Util::m_zlog,"查询电阻设置反馈量:%s", packData.c_str());
		data = atof(packData.c_str());
		MemDb::SetRealData(regStart + 7, data, false);//输出阻抗电阻值
	}
	msleep(200);

	//获取输出阻抗电阻值设置反馈量
	memset(temp, 0, sizeof(temp));
	str.clear();
	sprintf(temp, "OUTP:IMP:IDN?\r\n");
	str = temp;
	ret = ReadPack(str, packData);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		zlog_debug(Util::m_zlog,"查询电感设置反馈量:%s", packData.c_str());
		data = atof(packData.c_str());
		MemDb::SetRealData(regStart + 8, data, false);//输出阻抗电感值
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int ChromaSource::Chroma61800Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	float data = 0.0;
	RealData realData;
	char tmp[ARRAY_LENGTH_512];
	string str;
	str.clear();

	if(m_flag == 0)
	{
		//设置电网模拟器初始化
		ret = SetRst();
		msleep(200);
		//设置使用所有命令
		ret = SetCouple(1);

		m_flag = 1;
	}

	//判断是否有启动停止设置
	if(MemDb::GetRealData(realData, regStart + 1, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		if(realData.pv.data.iValue == 0)
		{
			sprintf(tmp, "OUTP OFF\r\n");
		}
		else if(realData.pv.data.iValue == 1)
		{
			sprintf(tmp, "OUTP ON\r\n");
		}

		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有故障清除设置
	if(MemDb::GetRealData(realData, regStart + 2, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "OUTP:PROT:CLE\r\n");
		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有工作模式设置
	if(MemDb::GetRealData(realData, regStart + 3, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		if(realData.pv.data.iValue == 1)
		{
			sprintf(tmp, "OUTP:MODE FIXED\r\n");
		}
		else if(realData.pv.data.iValue == 2)
		{
			sprintf(tmp, "OUTP:MODE LIST\r\n");
		}
		else if(realData.pv.data.iValue == 3)
		{
			sprintf(tmp, "OUTP:MODE PULSE\r\n");
		}
		else if(realData.pv.data.iValue == 4)
		{
			sprintf(tmp, "OUTP:MODE STEP\r\n");
		}
		else if(realData.pv.data.iValue == 5)
		{
			sprintf(tmp, "OUTP:MODE SYNTH\r\n");
		}
		else if(realData.pv.data.iValue == 5)
		{
			sprintf(tmp, "OUTP:MODE INTERHAR\r\n");
		}

		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有输出模式设置
	if(MemDb::GetRealData(realData, regStart + 4, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		if(realData.pv.data.iValue == 1)
		{
			sprintf(tmp, "INST:PHAS SINGLE\r\n");
		}
		else if(realData.pv.data.iValue == 2)
		{
			sprintf(tmp, "INST:PHAS THREE\r\n");
		}

		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有输出信号耦合设置
	if(MemDb::GetRealData(realData, regStart + 5, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		if(realData.pv.data.iValue == 1)
		{
			sprintf(tmp, "OUTP:COUP AC\r\n");
		}
		else if(realData.pv.data.iValue == 2)
		{
			sprintf(tmp, "OUTP:COUP DC\r\n");
		}
		else if(realData.pv.data.iValue == 3)
		{
			sprintf(tmp, "OUTP:COUP ACDC\r\n");
		}
		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有输出阻抗控制设置
	if(MemDb::GetRealData(realData, regStart + 6, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		if(realData.pv.data.iValue == 1)
		{
			sprintf(tmp, "OUTP:IMP:STAT ON\r\n");
		}
		else if(realData.pv.data.iValue == 0)
		{
			sprintf(tmp, "OUTP:IMP:STAT OFF\r\n");
		}

		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有输出阻抗电阻值设置
	if(MemDb::GetRealData(realData, regStart + 7, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		if(realData.pv.data.dbValue < 0.00)
		{
			data = 0.00;
		}
		else if(realData.pv.data.dbValue > 1.00)
		{
			data = 1.00;
		}
		else
		{
			data = realData.pv.data.dbValue;
		}
		sprintf(tmp, "OUTP:IMP:RES %f\r\n", data);
		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有输出阻抗电感值设置
	if(MemDb::GetRealData(realData, regStart + 8, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		if(realData.pv.data.dbValue < 0.00)
		{
			data = 0.00;
		}
		else if(realData.pv.data.dbValue > 1.00)
		{
			data = 1.00;
		}
		else
		{
			data = realData.pv.data.dbValue;
		}
		sprintf(tmp, "OUTP:IMP:IDN %f\r\n", data);
		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有电压设置
	if(MemDb::GetRealData(realData, regStart + 10, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "VOLT:AC %f\r\n", realData.pv.data.dbValue);
		str = tmp;
		ret = PresetPack(str);
	}

	//判断是否有频率设置
	if(MemDb::GetRealData(realData, regStart + 13, true))
	{
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "FREQ %f\r\n", realData.pv.data.dbValue);
		str = tmp;
		ret = PresetPack(str);
	}

	return ret;
}

int ChromaSource::ReadPack(string str, string& packData)
{
	int ret = ErrorInfo::ERR_OK;
	String values;
	String reply;

	values.clear();
	reply.clear();
	packData.clear();

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

	return ret;
}

int ChromaSource::PresetPack(string str)
{
	int ret = ErrorInfo::ERR_OK;

	String values;
	values.clear();
	zlog_debug(Util::m_zlog,"====================================控制数据下发数据：%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//选择相位
int ChromaSource::SelPhase(int phase)
{
	int ret = ErrorInfo::ERR_OK;
	string str;
	char tmp[ARRAY_LENGTH_512];
	String values;
	values.clear();
	str.clear();

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "INST:NSEL %d\r\n", phase);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

int ChromaSource::SetCouple(int couple)
{
	int ret = ErrorInfo::ERR_OK;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();

	memset(tmp, 0, sizeof(tmp));
	if(couple == 0)
	{
		sprintf(tmp, "INST:COUP NONE\r\n");
	}
	else if(couple == 1)
	{
		sprintf(tmp, "INST:COUP ALL\r\n");
	}

	str = tmp;
	ret = PresetPack(str);

	return ret;
}

int ChromaSource::SetRst()
{
	int ret = ErrorInfo::ERR_OK;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "*RST\r\n");

	str = tmp;
	ret = PresetPack(str);

	return ret;
}

int ChromaSource::GetVolt(float volt[3])
{
	int ret = ErrorInfo::ERR_OK;
	int phase = 0;
	string packData;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();

	for(phase = 1; phase < 4; phase++)
	{
		//选择相位
		ret = SelPhase(phase);

		//读取相位对应数据
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "FETC:VOLT:AC?\r\n");
		str = tmp;
		ret = ReadPack(str, packData);

		zlog_debug(Util::m_zlog,"查询%d相电压值:%s", phase, packData.c_str());
		volt[phase - 1] = atof(packData.c_str());
	}

	return ret;
}

int ChromaSource::GetCurr(float curr[3])
{
	int ret = ErrorInfo::ERR_OK;
	int phase = 0;
	string packData;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();

	for(phase = 1; phase < 4; phase++)
	{
		//选择相位
		ret = SelPhase(phase);

		//读取相位对应数据
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "FETC:CURR:AC?\r\n");
		str = tmp;
		ret = ReadPack(str, packData);

		zlog_debug(Util::m_zlog,"查询%d相电流值:%s", phase, packData.c_str());
		curr[phase - 1] = atof(packData.c_str());
	}

	return ret;
}

int ChromaSource::GetPow(float power[3])
{
	int ret = ErrorInfo::ERR_OK;
	int phase = 0;
	string packData;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();

	for(phase = 1; phase < 4; phase++)
	{
		//选择相位
		ret = SelPhase(phase);

		//读取相位对应数据
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "FETC:POW:AC?\r\n");
		str = tmp;
		ret = ReadPack(str, packData);

		zlog_debug(Util::m_zlog,"查询%d相有功功率值:%s", phase, packData.c_str());
		power[phase - 1] = atof(packData.c_str());
	}

	return ret;
}

int ChromaSource::GetSPow(float sPower[3])
{
	int ret = ErrorInfo::ERR_OK;
	int phase = 0;
	string packData;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();

	for(phase = 1; phase < 4; phase++)
	{
		//选择相位
		ret = SelPhase(phase);

		//读取相位对应数据
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "FETC:POW:AC:APP?\r\n");
		str = tmp;
		ret = ReadPack(str, packData);

		zlog_debug(Util::m_zlog,"查询%d相视在功率值:%s", phase, packData.c_str());
		sPower[phase - 1] = atof(packData.c_str());
	}

	return ret;
}

int ChromaSource::GetQPow(float qPower[3])
{
	int ret = ErrorInfo::ERR_OK;
	int phase = 0;
	string packData;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();

	for(phase = 1; phase < 4; phase++)
	{
		//选择相位
		ret = SelPhase(phase);

		//读取相位对应数据
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "FETC:POW:AC:REAC?\r\n");
		str = tmp;
		ret = ReadPack(str, packData);

		zlog_debug(Util::m_zlog,"查询%d相无功功率值:%s", phase, packData.c_str());
		qPower[phase - 1] = atof(packData.c_str());
	}

	return ret;
}

int ChromaSource::Getpf(float pf[3])
{
	int ret = ErrorInfo::ERR_OK;
	int phase = 0;
	string packData;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();

	for(phase = 1; phase < 4; phase++)
	{
		//选择相位
		ret = SelPhase(phase);

		//读取相位对应数据
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "FETC:POW:AC:PFAC?\r\n");
		str = tmp;
		ret = ReadPack(str, packData);

		zlog_debug(Util::m_zlog,"查询%d相功率因数值:%s", phase, packData.c_str());
		pf[phase - 1] = atof(packData.c_str());
	}

	return ret;
}

int ChromaSource::GetHz(float hz[3])
{
	int ret = ErrorInfo::ERR_OK;
	int phase = 0;
	string packData;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();

	for(phase = 1; phase < 4; phase++)
	{
		//选择相位
		ret = SelPhase(phase);

		//读取相位对应数据
		memset(tmp, 0, sizeof(tmp));
		str.clear();
		sprintf(tmp, "FETC:FREQ?\r\n");
		str = tmp;
		ret = ReadPack(str, packData);

		zlog_debug(Util::m_zlog,"查询%d相频率值:%s", phase, packData.c_str());
		hz[phase - 1] = atof(packData.c_str());
	}

	return ret;
}

int ChromaSource::GetFault(int fault)
{
	int ret = ErrorInfo::ERR_OK;
	string packData;
	string str;
	char tmp[ARRAY_LENGTH_512];

	str.clear();
	packData.clear();

	memset(tmp, 0, sizeof(tmp));
	str.clear();
	sprintf(tmp, "STAT:QUES:NTR?\r\n");
	str = tmp;
	ret = ReadPack(str, packData);
	fault = atoi(packData.c_str());

	return ret;
}
