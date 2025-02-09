/*
 * ChromaDcLoad.cpp
 *
 *  创建日期: 2016-6-20
 *  作          者: 刘海涛
 */

#include "ChromaDcLoad.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

ChromaDcLoad::ChromaDcLoad()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

ChromaDcLoad::~ChromaDcLoad()
{
	// TODO Auto-generated destructor stub
}

void ChromaDcLoad::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void ChromaDcLoad::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int ChromaDcLoad::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_63204A_150_400:
		ret = this->Chroma63204Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaDcLoad::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_CHROMA_63204A_150_400:
		ret = this->Chroma63204Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ChromaDcLoad::Chroma63204Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma63200Read(dev);
	return ret;
}

int ChromaDcLoad::Chroma63204Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	ret = Chroma63200Preset(dev);
	return ret;
}

int ChromaDcLoad::Chroma63200Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int regStart = dev->regStart;
	int value = 0;
	int data = 0;
	int mode = 0;
	float tmp = 0.0;

	//实时值
	//获取实时电压
	ret = GetVol(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 51, tmp, false);//电压
	}

	//获取实时功率
	ret = GetPower(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 53, tmp, false);//功率
	}

	//获取实时电流
	ret = GetCur(tmp);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 52, tmp, false);//电流
	}

	//获取故障码
	ret = GetFault(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		if(value != 0)
		{
			MemDb::SetRealData(regStart + 49, 1, false);//有故障
			MemDb::SetRealData(regStart + 50, value, false);//故障码
		}
		else
		{
			MemDb::SetRealData(regStart + 49, 0, false);//无故障
			MemDb::SetRealData(regStart + 50, value, false);//故障码
		}
	}
	//反馈值
	//获取启动停止设置
	ret = GetStart(value);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 1, value, false);//启动/停止
	}

	//获取工作模式设置
	ret = GetOutMode(value, data);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		MemDb::SetRealData(regStart + 3, value, false);//工作模式
		MemDb::SetRealData(regStart + 4, data, false);//模式档位
	}

	//获取工作模式
	MemDb::GetRealData(mode, regStart + 3);//工作模式

	if(mode == 1)//恒流模式
	{
		//获取CC模式L1电流
		ret = GetCCL1Cur(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 8, tmp, false);//L1电流
		}

		//获取CC模式L2电流
		ret = GetCCL2Cur(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 9, tmp, false);//L2电流
		}

		//获取CC模式电流上升斜率
		ret = GetCCCurRisSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 10, tmp, false);//电流上升斜率
		}

		//获取CC模式电流下降斜率
		ret = GetCCCurFallSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 11, tmp, false);//电流下降斜率
		}

		//获取CC模式电压量测档位
		ret = GetCCVolGear(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 12, value, false);//电压量测档位
		}
	}
	else if(mode == 5)//CCD（电流动态负载模式）
	{
		//获取CCD模式L1电流
		ret = GetCCDL1Cur(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 13, tmp, false);//L1电流
		}

		//获取CCD模式L2电流
		ret = GetCCDL2Cur(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 14, tmp, false);//L2电流
		}

		//获取CCD模式T1
		ret = GetCCDT1(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 15, tmp, false);//T1
		}

		//获取CCD模式T2
		ret = GetCCDT2(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 16, tmp, false);//T2
		}

		//获取CCD模式重复次数
		ret = GetCCDRepeat(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 17, value, false);//重复次数
		}

		//获取CCD模式电流上升斜率
		ret = GetCCDCurRisSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 18, tmp, false);//电流上升斜率
		}

		//获取CCD模式电流下降斜率
		ret = GetCCDCurFallSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 19, tmp, false);//电流下降斜率
		}

		//获取CCD模式电压量测档位
		ret = GetCCDVolGear(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 20, value, false);//电压量测档位
		}
	}
	else if(mode == 3)//CR
	{
		//获取CR模式L1电阻
		ret = GetCRL1R(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 21, tmp, false);//L1电阻
		}

		//获取CR模式L2电阻
		ret = GetCRL2R(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 22, tmp, false);//L2电阻
		}

		//获取CR模式电流上升斜率
		ret = GetCRCurRisSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 23, tmp, false);//电流上升斜率
		}

		//获取CR模式电流下降斜率
		ret = GetCRCurFallSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 24, tmp, false);//电流下降斜率
		}

		//获取CR模式电流量测档位
		ret = GetCRCurGear(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 25, value, false);//电流量测档位
		}
	}
	else if(mode == 6)//CRD（动态电阻变动模式）
	{
		//获取CRD模式L1电阻
		ret = GetCRDL1R(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 26, tmp, false);//L1电阻
		}

		//获取CRD模式L2电阻
		ret = GetCRDL2R(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 27, tmp, false);//L2电阻
		}

		//获取CRD模式T1
		ret = GetCRDT1(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 28, tmp, false);//T1
		}

		//获取CRD模式T2
		ret = GetCRDT2(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 29, tmp, false);//T2
		}

		//获取CRD模式重复次数
		ret = GetCRDRepeat(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 30, value, false);//重复次数
		}

		//获取CRD模式电流上升斜率
		ret = GetCRDCurRisSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 31, tmp, false);//电流上升斜率
		}

		//获取CRD模式电流下降斜率
		ret = GetCRDCurFallSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 32, tmp, false);//电流下降斜率
		}

		//获取CRD模式电流量测档位
		ret = GetCRDCurGear(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 33, value, false);//电流量测档位
		}
	}
	else if(mode == 2)//CV
	{
		//获取CV模式L1电压
		ret = GetCVL1Vol(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 34, tmp, false);//L1电压
		}

		//获取CV模式L2电压
		ret = GetCVL2Vol(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 35, tmp, false);//L2电压
		}

		//获取CV模式限电流
		ret = GetCVCurLimit(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 36, tmp, false);//限电流
		}

		//获取CV模式反应速度
		ret = GetCVVolReact(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 37, value, false);//反应速度
		}

		//获取CV模式电流量测档位
		ret = GetCVCurGear(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 38, value, false);//电流量测档位
		}
	}
	else if(mode == 4)//CP
	{
		//获取CP模式L1功率
		ret = GetCPL1Pow(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			tmp = tmp * 0.001;
			MemDb::SetRealData(regStart + 39, tmp, false);//L1功率
		}

		//获取CP模式L2功率
		ret = GetCPL2Pow(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			tmp = tmp * 0.001;
			MemDb::SetRealData(regStart + 40, tmp, false);//L2功率
		}

		//获取CP模式电流上升斜率
		ret = GetCPCurRisSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 41, tmp, false);//电流上升斜率
		}

		//获取CP模式电流下降斜率
		ret = GetCPCurFallSlope(tmp);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 42, tmp, false);//电流下降斜率
		}

		//获取CP模式电压量测档位
		ret = GetCPVolGear(value);
		if(ret == ErrorInfo::ERR_OK)
		{
			comm = true;
			MemDb::SetRealData(regStart + 43, value, false);//电压量测档位
		}
	}
	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int ChromaDcLoad::Chroma63200Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	int mode = 0;
	int modeGear = 0;
	RealData realData;

	//判断是否有启停设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		ret = SetStart(realData.pv.data.iValue);
		MemDb::SetRealData(regStart + 47, realData.pv.data.iValue, false);//运行状态
	}

	//判断是否有故障清除设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		ret = ClearFault();
	}

	//获取模式档位
	MemDb::GetRealData(modeGear, regStart + 4);//模式档位
	//判断是否有工作模式设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		ret = SetOutMode(realData.pv.data.iValue, modeGear);
		MemDb::SetRealData(regStart + 48, realData.pv.data.iValue, false);//工作模式
	}

	//判断是否有远程本地设定值
	if(MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{
		ret = SetRemAndLoc(realData.pv.data.iValue);
	}

	//获取工作模式
	MemDb::GetRealData(mode, regStart + 3);//工作模式
	if(mode == 1)//CC
	{
		//判断是否有CC模式L1设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 8, true))
		{
			ret = SetCCL1Cur(realData.pv.data.dbValue);
		}

		//判断是否有CC模式L2设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 9, true))
		{
			ret = SetCCL2Cur(realData.pv.data.dbValue);
		}

		//判断是否有CC模式电流上升斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 10, true))
		{
			ret = SetCCCurRisSlope(realData.pv.data.dbValue);
		}

		//判断是否有CC模式电流下降斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 11, true))
		{
			ret = SetCCCurFallSlope(realData.pv.data.dbValue);
		}

		//判断是否有CC模式电压量测档位设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 12, true))
		{
			ret = SetCCVolGear(realData.pv.data.iValue);
		}
	}
	else if(mode == 2)//CV
	{
		//判断是否有CV模式L1设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 34, true))
		{
			ret = SetCVL1Vol(realData.pv.data.dbValue);
		}

		//判断是否有CV模式L2设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 35, true))
		{
			ret = SetCVL2Vol(realData.pv.data.dbValue);
		}

		//判断是否有CV模式限电流设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 36, true))
		{
			ret = SetCVCurLimit(realData.pv.data.dbValue);
		}

		//判断是否有CV模式反应速度设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 37, true))
		{
			ret = SetCVVolReact(realData.pv.data.iValue);
		}

		//判断是否有CV模式电流量测档位设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 38, true))
		{
			ret = SetCVCurGear(realData.pv.data.iValue);
		}
	}
	else if(mode == 3)//CR
	{
		//判断是否有CR模式L1设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 21, true))
		{
			ret = SetCRL1R(realData.pv.data.dbValue);
		}

		//判断是否有CR模式L2设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 22, true))
		{
			ret = SetCRL2R(realData.pv.data.dbValue);
		}

		//判断是否有CR模式电流上升斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 23, true))
		{
			ret = SetCRCurRisSlope(realData.pv.data.dbValue);
		}

		//判断是否有CR模式电流下降斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 24, true))
		{
			ret = SetCRCurFallSlope(realData.pv.data.dbValue);
		}

		//判断是否有CR模式电流量测档位设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 12, true))
		{
			ret = SetCRCurGear(realData.pv.data.iValue);
		}
	}
	else if(mode == 4)//CP
	{
		//判断是否有CP模式L1设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 39, true))
		{
			ret = SetCPL1Pow(realData.pv.data.dbValue);
		}

		//判断是否有CP模式L2设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 40, true))
		{
			ret = SetCPL2Pow(realData.pv.data.dbValue);
		}

		//判断是否有CP模式电流上升斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 41, true))
		{
			ret = SetCPCurRisSlope(realData.pv.data.dbValue);
		}

		//判断是否有CP模式电流下降斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 42, true))
		{
			ret = SetCPCurFallSlope(realData.pv.data.dbValue);
		}

		//判断是否有CP模式电压量测档位设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 43, true))
		{
			ret = SetCPVolGear(realData.pv.data.iValue);
		}
	}
	else if(mode == 5)//CCD
	{
		//判断是否有CCD模式L1设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 13, true))
		{
			ret = SetCCDL1Cur(realData.pv.data.dbValue);
		}

		//判断是否有CCD模式L2设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 14, true))
		{
			ret = SetCCDL2Cur(realData.pv.data.dbValue);
		}

		//判断是否有CCD模式T1设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 15, true))
		{
			ret = SetCCDT1(realData.pv.data.dbValue);
		}

		//判断是否有CCD模式T2设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 16, true))
		{
			ret = SetCCDT2(realData.pv.data.dbValue);
		}

		//判断是否有CCD模式重复次数设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 17, true))
		{
			ret = SetCCDRepeat(realData.pv.data.iValue);
		}

		//判断是否有CCD模式电流上升斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 18, true))
		{
			ret = SetCCDCurRisSlope(realData.pv.data.dbValue);
		}

		//判断是否有CCD模式电流下降斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 19, true))
		{
			ret = SetCCDCurFallSlope(realData.pv.data.dbValue);
		}

		//判断是否有CCD模式电压量测档位设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 20, true))
		{
			ret = SetCCDVolGear(realData.pv.data.iValue);
		}
	}
	else if(mode == 6)//CRD
	{
		//判断是否有CRD模式L1设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 26, true))
		{
			ret = SetCRDL1R(realData.pv.data.dbValue);
		}

		//判断是否有CRD模式L2设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 27, true))
		{
			ret = SetCRDL2R(realData.pv.data.dbValue);
		}

		//判断是否有CRD模式T1设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 28, true))
		{
			ret = SetCRDT1(realData.pv.data.dbValue);
		}

		//判断是否有CRD模式T2设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 29, true))
		{
			ret = SetCRDT2(realData.pv.data.dbValue);
		}

		//判断是否有CRD模式重复次数设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 30, true))
		{
			ret = SetCRDRepeat(realData.pv.data.iValue);
		}

		//判断是否有CRD模式电流上升斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 31, true))
		{
			ret = SetCRDCurRisSlope(realData.pv.data.dbValue);
		}

		//判断是否有CRD模式电流下降斜率设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 32, true))
		{
			ret = SetCRDCurFallSlope(realData.pv.data.dbValue);
		}

		//判断是否有CRD模式电流量测档位设定值
		if(MemDb::GetRealDataWrited(realData, regStart + 33, true))
		{
			ret = GetCRDCurGear(realData.pv.data.iValue);
		}
	}
	return ret;
}

//获取实时输出电流值
int ChromaDcLoad::GetCur(float& current)
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

	current = atof(packData.c_str());
	return ret;
}

//获取实时输出电压值
int ChromaDcLoad::GetVol(float& voltage)
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

	voltage = atof(packData.c_str());
	return ret;
}

//获取实时输出功率值
int ChromaDcLoad::GetPower(float& power)
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

	power = atof(packData.c_str());
	return ret;
}

//获取实时故障码
int ChromaDcLoad::GetFault(int& fault)
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

	fault = atoi(packData.c_str());
	return ret;
}

//设置本地远程
int ChromaDcLoad::SetRemAndLoc(int value)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	if(value == 1)
	{
		sprintf(tmp, "SYST:REM\r\n");
	}
	else if(value == 2)
	{
		sprintf(tmp, "SYST:LOC\r\n");
	}
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//设置启动/停止
int ChromaDcLoad::SetStart(int runSta)
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
	m_tcp->Send(values);

	return ret;
}

//获取设置启动停止
int ChromaDcLoad::GetStart(int& runSta)
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

	if(packData.compare("ON\n") == 0)
	{
		runSta = 1;
	}
	else if(packData.compare("OFF\n") == 0)
	{
		runSta = 0;
	}
	return ret;
}

//设置输出工作模式
int ChromaDcLoad::SetOutMode(int mode, int modeGear)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	if(modeGear == 1)//低
	{
		if(mode == 1)//CC
		{
			sprintf(tmp, "MODE CCL\r\n");
		}
		else if(mode == 2)//CV
		{
			sprintf(tmp, "MODE CVL\r\n");
		}
		else if(mode == 3)//CR
		{
			sprintf(tmp, "MODE CRL\r\n");
		}
		else if(mode == 4)//CP
		{
			sprintf(tmp, "MODE CPL\r\n");
		}
		else if(mode == 5)//CCD
		{
			sprintf(tmp, "MODE CCDL\r\n");
		}
		else if(mode == 6)//CRD
		{
			sprintf(tmp, "MODE CRDL\r\n");
		}
	}
	else if(modeGear == 2)//中
	{
		if(mode == 1)//CC
		{
			sprintf(tmp, "MODE CCM\r\n");
		}
		else if(mode == 2)//CV
		{
			sprintf(tmp, "MODE CVM\r\n");
		}
		else if(mode == 3)//CR
		{
			sprintf(tmp, "MODE CRM\r\n");
		}
		else if(mode == 4)//CP
		{
			sprintf(tmp, "MODE CPM\r\n");
		}
		else if(mode == 5)//CCD
		{
			sprintf(tmp, "MODE CCDM\r\n");
		}
		else if(mode == 6)//CRD
		{
			sprintf(tmp, "MODE CRDM\r\n");
		}
	}
	else if(modeGear == 3)//高
	{
		if(mode == 1)//CC
		{
			sprintf(tmp, "MODE CCH\r\n");
		}
		else if(mode == 2)//CV
		{
			sprintf(tmp, "MODE CVH\r\n");
		}
		else if(mode == 3)//CR
		{
			sprintf(tmp, "MODE CRH\r\n");
		}
		else if(mode == 4)//CP
		{
			sprintf(tmp, "MODE CPH\r\n");
		}
		else if(mode == 5)//CCD
		{
			sprintf(tmp, "MODE CCDH\r\n");
		}
		else if(mode == 6)//CRD
		{
			sprintf(tmp, "MODE CRDH\r\n");
		}
	}


	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置输出工作模式
int ChromaDcLoad::GetOutMode(int& mode, int& modeGear)
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

	//CCL、CCM、CCH、CVL、CVM、CVH、CRL、CRM、CRH、CPL、CPM、CPH、CCDL、CCDM、CCDH、CRDL、CRDM、CRDH
	if(packData.compare("CCL\n") == 0)
	{
		mode = 1;
		modeGear = 1;
	}
	else if(packData.compare("CCM\n") == 0)
	{
		mode = 1;
		modeGear = 2;
	}
	else if(packData.compare("CCH\n") == 0)
	{
		mode = 1;
		modeGear = 3;
	}
	else if(packData.compare("CVL\n") == 0)
	{
		mode = 2;
		modeGear = 1;
	}
	else if(packData.compare("CVM\n") == 0)
	{
		mode = 2;
		modeGear = 2;
	}
	else if(packData.compare("CVH\n") == 0)
	{
		mode = 2;
		modeGear = 3;
	}
	else if(packData.compare("CRL\n") == 0)
	{
		mode = 3;
		modeGear = 1;
	}
	else if(packData.compare("CRM\n") == 0)
	{
		mode = 3;
		modeGear = 2;
	}
	else if(packData.compare("CRH\n") == 0)
	{
		mode = 3;
		modeGear = 3;
	}
	else if(packData.compare("CPL\n") == 0)
	{
		mode = 4;
		modeGear = 1;
	}
	else if(packData.compare("CPM\n") == 0)
	{
		mode = 4;
		modeGear = 2;
	}
	else if(packData.compare("CPH\n") == 0)
	{
		mode = 4;
		modeGear = 3;
	}
	else if(packData.compare("CCDL\n") == 0)
	{
		mode = 5;
		modeGear = 1;
	}
	else if(packData.compare("CCDM\n") == 0)
	{
		mode = 5;
		modeGear = 2;
	}
	else if(packData.compare("CCDH\n") == 0)
	{
		mode = 5;
		modeGear = 3;
	}
	else if(packData.compare("CRDL\n") == 0)
	{
		mode = 6;
		modeGear = 1;
	}
	else if(packData.compare("CRDM\n") == 0)
	{
		mode = 6;
		modeGear = 2;
	}
	else if(packData.compare("CRDH\n") == 0)
	{
		mode = 6;
		modeGear = 3;
	}

	return ret;
}

//清除故障
int ChromaDcLoad::ClearFault()
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "LOAD:PORT:CLE\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//CC模式
//设置CC模式L1电流值
int ChromaDcLoad::SetCCL1Cur(float current)
{
	int ret = ErrorInfo::ERR_OK;
	float curMax = 0.0;
	float curMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L1最大值
	ret = GetCCL1CurMax(curMin);

	//获取L1最小值
	ret = GetCCL1CurMin(curMax);

	if(current > curMax)
	{
		current = curMax;
	}

	if(current < curMin)
	{
		current = curMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:STAT:L1 %f\r\n", current);
	str = tmp;
	zlog_warn(Util::m_zlog,"%s", str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CC模式L1电流值
int ChromaDcLoad::GetCCL1Cur(float& current)
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
	sprintf(tmp, "CURR:STAT:L1?\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//获取设置CC模式L1电流值最大值
int ChromaDcLoad::GetCCL1CurMax(float& current)
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
	sprintf(tmp, "CURR:STAT:L1? MAX\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 1);
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

//获取设置CC模式L1电流值最小值
int ChromaDcLoad::GetCCL1CurMin(float& current)
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
	sprintf(tmp, "CURR:STAT:L1? MIN\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//设置CC模式L2电流值
int ChromaDcLoad::SetCCL2Cur(float current)
{
	int ret = ErrorInfo::ERR_OK;
	float curMax = 0.0;
	float curMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L2最大值
	ret = GetCCL2CurMax(curMax);

	//获取L2最小值
	ret = GetCCL2CurMin(curMin);

	if(current > curMax)
	{
		current = curMax;
	}

	if(current < curMin)
	{
		current = curMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:STAT:L2 %f\r\n", current);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CC模式L2电流值
int ChromaDcLoad::GetCCL2Cur(float& current)
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
	sprintf(tmp, "CURR:STAT:L2?\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//获取设置CC模式L2电流值最大值
int ChromaDcLoad::GetCCL2CurMax(float& current)
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
	sprintf(tmp, "CURR:STAT:L2? MAX\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//获取设置CC模式L2电流值最小值
int ChromaDcLoad::GetCCL2CurMin(float& current)
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
	sprintf(tmp, "CURR:STAT:L2? MIN\r\n");
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	ret = m_tcp->TxRxMsg(reply, values, 1);
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

//设置CC模式电流上升斜率
int ChromaDcLoad::SetCCCurRisSlope(float rise)
{
	int ret = ErrorInfo::ERR_OK;
	float riseMax = 0.0;
	float riseMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取上升斜率最大值
	ret = GetCCCurRisSlopeMax(riseMax);

	//获取上升斜率最小值
	ret = GetCCCurRisSlopeMin(riseMin);

	if(rise > riseMax)
	{
		rise = riseMax;
	}

	if(rise < riseMin)
	{
		rise = riseMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:STAT:RISE %f\r\n", rise);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CC模式电流上升斜率
int ChromaDcLoad::GetCCCurRisSlope(float& rise)
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
	sprintf(tmp, "CURR:STAT:RISE?\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CC模式电流上升斜率最大值
int ChromaDcLoad::GetCCCurRisSlopeMax(float& rise)
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
	sprintf(tmp, "CURR:STAT:RISE? MAX\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CC模式电流上升斜率最小值
int ChromaDcLoad::GetCCCurRisSlopeMin(float& rise)
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
	sprintf(tmp, "CURR:STAT:RISE? MIN\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//设置CC模式电流下降斜率
int ChromaDcLoad::SetCCCurFallSlope(float fall)
{
	int ret = ErrorInfo::ERR_OK;
	float fallMax = 0.0;
	float fallMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取下降斜率最大值
	ret = GetCCCurFallSlopeMax(fallMax);

	//获取下降斜率最小值
	ret = GetCCCurFallSlopeMin(fallMin);

	if(fall > fallMax)
	{
		fall = fallMax;
	}

	if(fall < fallMin)
	{
		fall = fallMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:STAT:FALL %f\r\n", fall);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CC模式电流下降斜率
int ChromaDcLoad::GetCCCurFallSlope(float& fall)
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
	sprintf(tmp, "CURR:STAT:FALL?\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CC模式电流下降斜率最大值
int ChromaDcLoad::GetCCCurFallSlopeMax(float& fall)
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
	sprintf(tmp, "CURR:STAT:FALL? MAX\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CC模式电流下降斜率最小值
int ChromaDcLoad::GetCCCurFallSlopeMin(float& fall)
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
	sprintf(tmp, "CURR:STAT:FALL? MIN\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//设置CC模式电压量测档位
int ChromaDcLoad::SetCCVolGear(int gear)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "CURR:STAT:VRNG %d\r\n", gear);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CC模式电压量测档位
int ChromaDcLoad::GetCCVolGear(int& gear)
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
	sprintf(tmp, "CURR:STAT:VRNG?\r\n");
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

	if(packData.compare("LOW\n") == 0)
	{
		gear = 3;
	}
	else if(packData.compare("MIDDLE\n") == 0)
	{
		gear = 2;
	}
	else if(packData.compare("HIGH\n") == 0)
	{
		gear = 1;
	}

	return ret;
}

//CCD模式
//设置CCD模式L1电流值
int ChromaDcLoad::SetCCDL1Cur(float current)
{
	int ret = ErrorInfo::ERR_OK;
	float curMax = 0.0;
	float curMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L1最大值
	ret = GetCCDL1CurMax(curMax);

	//获取L1最小值
	ret = GetCCDL1CurMin(curMin);

	if(current > curMax)
	{
		current = curMax;
	}

	if(current < curMin)
	{
		current = curMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:DYN:L1 %f\r\n", current);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CCD模式L1电流值
int ChromaDcLoad::GetCCDL1Cur(float& current)
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
	sprintf(tmp, "CURR:DYN:L1?\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式L1电流值最大值
int ChromaDcLoad::GetCCDL1CurMax(float& current)
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
	sprintf(tmp, "CURR:DYN:L1? MAX\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式L1电流值最小值
int ChromaDcLoad::GetCCDL1CurMin(float& current)
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
	sprintf(tmp, "CURR:DYN:L1? MIN\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//设置CCD模式L2电流值
int ChromaDcLoad::SetCCDL2Cur(float current)
{
	int ret = ErrorInfo::ERR_OK;
	float curMax = 0.0;
	float curMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L2最大值
	ret = GetCCDL2CurMax(curMax);

	//获取L2最小值
	ret = GetCCDL2CurMin(curMin);

	if(current > curMax)
	{
		current = curMax;
	}

	if(current < curMin)
	{
		current = curMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:DYN:L2 %f\r\n", current);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CCD模式L2电流值
int ChromaDcLoad::GetCCDL2Cur(float& current)
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
	sprintf(tmp, "CURR:DYN:L2?\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式L2电流值最大值
int ChromaDcLoad::GetCCDL2CurMax(float& current)
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
	sprintf(tmp, "CURR:DYN:L2? MAX\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式L2电流值最小值
int ChromaDcLoad::GetCCDL2CurMin(float& current)
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
	sprintf(tmp, "CURR:DYN:L2? MIN\r\n");
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

	current = atof(packData.c_str());
	return ret;
}

//设置CCD模式T1
int ChromaDcLoad::SetCCDT1(float time)
{
	int ret = ErrorInfo::ERR_OK;
	float timeMax = 0.0;
	float timeMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取T1最大值
	ret = GetCCDT1Max(timeMax);

	//获取T1最小值
	ret = GetCCDT1Min(timeMin);

	if(time > timeMax)
	{
		time = timeMax;
	}

	if(time < timeMin)
	{
		time = timeMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:DYN:T1 %f\r\n", time);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CCD模式T1
int ChromaDcLoad::GetCCDT1(float& time)
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
	sprintf(tmp, "CURR:DYN:T1?\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式T1最大值
int ChromaDcLoad::GetCCDT1Max(float& time)
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
	sprintf(tmp, "CURR:DYN:T1? MAX\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式T1最小值
int ChromaDcLoad::GetCCDT1Min(float& time)
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
	sprintf(tmp, "CURR:DYN:T1? MIN\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//设置CCD模式T2
int ChromaDcLoad::SetCCDT2(float time)
{
	int ret = ErrorInfo::ERR_OK;
	float timeMax = 0.0;
	float timeMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取T2最大值
	ret = GetCCDT2Max(timeMax);

	//获取T2最小值
	ret = GetCCDT2Min(timeMin);

	if(time > timeMax)
	{
		time = timeMax;
	}

	if(time < timeMin)
	{
		time = timeMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:DYN:T2 %f\r\n", time);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CCD模式T2
int ChromaDcLoad::GetCCDT2(float& time)
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
	sprintf(tmp, "CURR:DYN:T2?\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式T2最大值
int ChromaDcLoad::GetCCDT2Max(float& time)
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
	sprintf(tmp, "CURR:DYN:T2? MAX\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式T2最小值
int ChromaDcLoad::GetCCDT2Min(float& time)
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
	sprintf(tmp, "CURR:DYN:T2? MIN\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//设置CCD模式重复次数
int ChromaDcLoad::SetCCDRepeat(int num)
{
	int ret = ErrorInfo::ERR_OK;
	int numMax = 0;
	int numMin = 0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取T2最大值
	ret = GetCCDRepeatMax(numMax);

	//获取T2最小值
	ret = GetCCDRepeatMin(numMin);

	if(num > numMax)
	{
		num = numMax;
	}

	if(num < numMin)
	{
		num = numMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:DYN:REP %d\r\n", num);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CCD模式重复次数
int ChromaDcLoad::GetCCDRepeat(int& num)
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
	sprintf(tmp, "CURR:DYN:REP?\r\n");
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

	num = atoi(packData.c_str());
	return ret;
}

//获取设置CCD模式重复次数最大值
int ChromaDcLoad::GetCCDRepeatMax(int& num)
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
	sprintf(tmp, "CURR:DYN:REP? MAX\r\n");
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

	num = atoi(packData.c_str());
	return ret;
}

//获取设置CCD模式重复次数最小值
int ChromaDcLoad::GetCCDRepeatMin(int& num)
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
	sprintf(tmp, "CURR:DYN:REP? MIN\r\n");
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

	num = atoi(packData.c_str());
	return ret;
}

//设置CCD模式电流上升斜率
int ChromaDcLoad::SetCCDCurRisSlope(float rise)
{
	int ret = ErrorInfo::ERR_OK;
	float riseMax = 0.0;
	float riseMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取上升斜率最大值
	ret = GetCCDCurRisSlopeMax(riseMax);

	//获取上升斜率最小值
	ret = GetCCDCurRisSlopeMax(riseMin);

	if(rise > riseMax)
	{
		rise = riseMax;
	}

	if(rise < riseMin)
	{
		rise = riseMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:DYN:RISE %f\r\n", rise);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CCD模式电流上升斜率
int ChromaDcLoad::GetCCDCurRisSlope(float& rise)
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
	sprintf(tmp, "CURR:DYN:RISE?\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式上升斜率最大值
int ChromaDcLoad::GetCCDCurRisSlopeMax(float& rise)
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
	sprintf(tmp, "CURR:DYN:RISE? MAX\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式上升斜率最小值
int ChromaDcLoad::GetCCDCurRisSlopeMin(float& rise)
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
	sprintf(tmp, "CURR:DYN:RISE? MIN\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//设置CCD模式电流下降斜率
int ChromaDcLoad::SetCCDCurFallSlope(float fall)
{
	int ret = ErrorInfo::ERR_OK;
	float fallMax = 0.0;
	float fallMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取下降斜率最大值
	ret = GetCCDCurFallSlopeMax(fallMax);

	//获取下降斜率最小值
	ret = GetCCDCurFallSlopeMin(fallMin);

	if(fall > fallMax)
	{
		fall = fallMax;
	}

	if(fall < fallMin)
	{
		fall = fallMin;
	}

	//封装控制语句
	sprintf(tmp, "CURR:DYN:FALL %f\r\n", fall);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CCD模式电流下降斜率
int ChromaDcLoad::GetCCDCurFallSlope(float& fall)
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
	sprintf(tmp, "CURR:DYN:FALL?\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式下降斜率最大值
int ChromaDcLoad::GetCCDCurFallSlopeMax(float& fall)
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
	sprintf(tmp, "CURR:DYN:FALL? MAX\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CCD模式下降斜率最小值
int ChromaDcLoad::GetCCDCurFallSlopeMin(float& fall)
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
	sprintf(tmp, "CURR:DYN:FALL? MIN\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//设置CCD模式电压量测档位
int ChromaDcLoad::SetCCDVolGear(int gear)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "CURR:DYN:VRNG %d\r\n", gear);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CCD模式电压量测档位
int ChromaDcLoad::GetCCDVolGear(int& gear)
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
	sprintf(tmp, "CURR:DYN:VRNG?\r\n");
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

	if(packData.compare("LOW\n") == 0)
	{
		gear = 3;
	}
	else if(packData.compare("MIDDLE\n") == 0)
	{
		gear = 2;
	}
	else if(packData.compare("HIGH\n") == 0)
	{
		gear = 1;
	}
	return ret;
}

//CR模式
//设置CR模式L1电阻值
int ChromaDcLoad::SetCRL1R(float resistance)
{
	int ret = ErrorInfo::ERR_OK;
	float resMax = 0.0;
	float resMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L1最大值
	ret = GetCRL1RMax(resMax);

	//获取L1最小值
	ret = GetCRL1RMin(resMin);

	if(resistance > resMax)
	{
		resistance = resMax;
	}

	if(resistance < resMin)
	{
		resistance = resMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:STAT:L1 %f\r\n", resistance);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CR模式L1电阻值
int ChromaDcLoad::GetCRL1R(float& resistance)
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
	sprintf(tmp, "RES:STAT:L1?\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//获取设置CR模式L1电阻最大值
int ChromaDcLoad::GetCRL1RMax(float& resistance)
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
	sprintf(tmp, "RES:STAT:L1? MAX\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//获取设置CR模式L1电阻最小值
int ChromaDcLoad::GetCRL1RMin(float& resistance)
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
	sprintf(tmp, "RES:STAT:L1? MIN\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//设置CR模式L2电阻值
int ChromaDcLoad::SetCRL2R(float resistance)
{
	int ret = ErrorInfo::ERR_OK;
	float resMax = 0.0;
	float resMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L2最大值
	ret = GetCRL2RMax(resMax);

	//获取L2最小值
	ret = GetCRL2RMin(resMin);

	if(resistance > resMax)
	{
		resistance = resMax;
	}

	if(resistance < resMin)
	{
		resistance = resMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:STAT:L2 %f\r\n", resistance);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CR模式L2电阻值
int ChromaDcLoad::GetCRL2R(float& resistance)
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
	sprintf(tmp, "RES:STAT:L2?\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

// 获取设置CR模式L2电阻最大值
int ChromaDcLoad::GetCRL2RMax(float& resistance)
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
	sprintf(tmp, "RES:STAT:L2? MAX\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//获取设置CR模式L2电阻最小值
int ChromaDcLoad::GetCRL2RMin(float& resistance)
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
	sprintf(tmp, "RES:STAT:L2? MIN\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//设置CR模式电流上升斜率
int ChromaDcLoad::SetCRCurRisSlope(float rise)
{
	int ret = ErrorInfo::ERR_OK;
	float riseMax = 0.0;
	float riseMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取上升斜率最大值
	ret = GetCRCurRisSlopeMax(riseMax);

	//获取上升斜率最小值
	ret = GetCRCurRisSlopeMin(riseMin);

	if(rise > riseMax)
	{
		rise = riseMax;
	}

	if(rise < riseMin)
	{
		rise = riseMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:STAT:RISE %f\r\n", rise);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CR模式电流上升斜率
int ChromaDcLoad::GetCRCurRisSlope(float& rise)
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
	sprintf(tmp, "RES:STAT:RISE?\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CR模式上升斜率最大值
int ChromaDcLoad::GetCRCurRisSlopeMax(float& rise)
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
	sprintf(tmp, "RES:STAT:RISE? MAX\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CR模式上升斜率最小值
int ChromaDcLoad::GetCRCurRisSlopeMin(float& rise)
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
	sprintf(tmp, "RES:STAT:RISE? MIN\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//设置CR模式电流下降斜率
int ChromaDcLoad::SetCRCurFallSlope(float fall)
{
	int ret = ErrorInfo::ERR_OK;
	float fallMax = 0.0;
	float fallMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取下降斜率最大值
	ret = GetCRCurFallSlopeMax(fallMax);

	//获取下降斜率最小值
	ret = GetCRCurFallSlopeMin(fallMin);

	if(fall > fallMax)
	{
		fall = fallMax;
	}

	if(fall < fallMin)
	{
		fall = fallMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:STAT:FALL %f\r\n", fall);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CR模式电流下降斜率
int ChromaDcLoad::GetCRCurFallSlope(float& fall)
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
	sprintf(tmp, "RES:STAT:FALL?\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CR模式下降斜率最大值
int ChromaDcLoad::GetCRCurFallSlopeMax(float& fall)
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
	sprintf(tmp, "RES:STAT:FALL? MAX\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CR模式下降斜率最小值
int ChromaDcLoad::GetCRCurFallSlopeMin(float& fall)
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
	sprintf(tmp, "RES:STAT:FALL? MIN\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//设置CR模式电流量测档位
int ChromaDcLoad::SetCRCurGear(int gear)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RES:STAT:IRNG %d\r\n", gear);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CR模式电流量测档位
int ChromaDcLoad::GetCRCurGear(int& gear)
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
	sprintf(tmp, "RES:STAT:IRNG?\r\n");
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

	if(packData.compare("LOW\n") == 0)
	{
		gear = 3;
	}
	else if(packData.compare("MIDDLE\n") == 0)
	{
		gear = 2;
	}
	else if(packData.compare("HIGH\n") == 0)
	{
		gear = 1;
	}
	return ret;
}

//CRD模式
//设置CRD模式L1电阻值
int ChromaDcLoad::SetCRDL1R(float resistance)
{
	int ret = ErrorInfo::ERR_OK;
	float resMax = 0.0;
	float resMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L1最大值
	ret = GetCRDL1RMax(resMax);

	//获取L1最小值
	ret = GetCRDL1RMin(resMin);

	if(resistance > resMax)
	{
		resistance = resMax;
	}

	if(resistance < resMin)
	{
		resistance = resMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:DYN:L1 %f\r\n", resistance);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CRD模式L1电阻值
int ChromaDcLoad::GetCRDL1R(float& resistance)
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
	sprintf(tmp, "RES:DYN:L1?\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式L1电阻最大值
int ChromaDcLoad::GetCRDL1RMax(float& resistance)
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
	sprintf(tmp, "RES:DYN:L1? MAX\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式L1电阻最小值
int ChromaDcLoad::GetCRDL1RMin(float& resistance)
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
	sprintf(tmp, "RES:DYN:L1? MIN\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//设置CRD模式L2电阻值
int ChromaDcLoad::SetCRDL2R(float resistance)
{
	int ret = ErrorInfo::ERR_OK;
	float resMax = 0.0;
	float resMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L2最大值
	ret = GetCRDL2RMax(resMax);

	//获取L2最小值
	ret = GetCRDL2RMin(resMin);

	if(resistance > resMax)
	{
		resistance = resMax;
	}

	if(resistance < resMin)
	{
		resistance = resMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:DYN:L2 %f\r\n", resistance);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CRD模式L2电阻值
int ChromaDcLoad::GetCRDL2R(float& resistance)
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
	sprintf(tmp, "RES:DYN:L2?\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式L2电阻最大值
int ChromaDcLoad::GetCRDL2RMax(float& resistance)
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
	sprintf(tmp, "RES:DYN:L2? MAX\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式L2电阻最小值
int ChromaDcLoad::GetCRDL2RMin(float& resistance)
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
	sprintf(tmp, "RES:DYN:L2? MIN\r\n");
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

	resistance = atof(packData.c_str());
	return ret;
}

//设置CRD模式T1
int ChromaDcLoad::SetCRDT1(float time)
{
	int ret = ErrorInfo::ERR_OK;
	float timeMax = 0.0;
	float timeMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取T1最大值
	ret = GetCRDT1Max(timeMax);

	//获取T1最小值
	ret = GetCRDT1Min(timeMin);

	if(time > timeMax)
	{
		time = timeMax;
	}

	if(time < timeMin)
	{
		time = timeMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:DYN:T1 %f\r\n", time);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CRD模式T1
int ChromaDcLoad::GetCRDT1(float& time)
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
	sprintf(tmp, "RES:DYN:T1?\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式T1最大值
int ChromaDcLoad::GetCRDT1Max(float& time)
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
	sprintf(tmp, "RES:DYN:T1? MAX\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式T1最小值
int ChromaDcLoad::GetCRDT1Min(float& time)
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
	sprintf(tmp, "RES:DYN:T1? MIN\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//设置CRD模式T2
int ChromaDcLoad::SetCRDT2(float time)
{
	int ret = ErrorInfo::ERR_OK;
	float timeMax = 0.0;
	float timeMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取T2最大值
	ret = GetCRDT2Max(timeMax);

	//获取T2最小值
	ret = GetCRDT2Min(timeMin);

	if(time > timeMax)
	{
		time = timeMax;
	}

	if(time < timeMin)
	{
		time = timeMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:DYN:T2 %f\r\n", time);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CRD模式T2
int ChromaDcLoad::GetCRDT2(float& time)
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
	sprintf(tmp, "RES:DYN:T2?\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式T2最大值
int ChromaDcLoad::GetCRDT2Max(float& time)
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
	sprintf(tmp, "RES:DYN:T2? MAX\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式T2最小值
int ChromaDcLoad::GetCRDT2Min(float& time)
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
	sprintf(tmp, "RES:DYN:T2? MIN\r\n");
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

	time = atof(packData.c_str());
	return ret;
}

//设置CRD模式重复次数
int ChromaDcLoad::SetCRDRepeat(int num)
{
	int ret = ErrorInfo::ERR_OK;
	int numMax = 0;
	int numMin = 0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取T2最大值
	ret = GetCRDRepeatMax(numMax);

	//获取T2最小值
	ret = GetCRDRepeatMin(numMin);

	if(num > numMax)
	{
		num = numMax;
	}

	if(num < numMin)
	{
		num = numMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:DYN:REP %d\r\n", num);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CRD模式重复次数
int ChromaDcLoad::GetCRDRepeat(int& num)
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
	sprintf(tmp, "RES:DYN:REP?\r\n");
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

	num = atoi(packData.c_str());
	return ret;
}

//获取设置CRD模式重复次数最大值
int ChromaDcLoad::GetCRDRepeatMax(int& num)
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
	sprintf(tmp, "RES:DYN:REP? MAX\r\n");
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

	num = atoi(packData.c_str());
	return ret;
}

//获取设置CRD模式重复次数最小值
int ChromaDcLoad::GetCRDRepeatMin(int& num)
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
	sprintf(tmp, "RES:DYN:REP? MIN\r\n");
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

	num = atoi(packData.c_str());
	return ret;
}

//设置CRD模式电流上升斜率
int ChromaDcLoad::SetCRDCurRisSlope(float rise)
{
	int ret = ErrorInfo::ERR_OK;
	float riseMax = 0.0;
	float riseMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取上升斜率最大值
	ret = GetCRDCurRisSlopeMax(riseMax);

	//获取上升斜率最小值
	ret = GetCRDCurRisSlopeMin(riseMin);

	if(rise > riseMax)
	{
		rise = riseMax;
	}

	if(rise < riseMin)
	{
		rise = riseMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:DYN:RISE %f\r\n", rise);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CRD模式电流上升斜率
int ChromaDcLoad::GetCRDCurRisSlope(float& rise)
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
	sprintf(tmp, "RES:DYN:RISE?\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式上升斜率最大值
int ChromaDcLoad::GetCRDCurRisSlopeMax(float& rise)
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
	sprintf(tmp, "RES:DYN:RISE? MAX\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式上升斜率最小值
int ChromaDcLoad::GetCRDCurRisSlopeMin(float& rise)
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
	sprintf(tmp, "RES:DYN:RISE? MIN\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//设置CRD模式电流下降斜率
int ChromaDcLoad::SetCRDCurFallSlope(float fall)
{
	int ret = ErrorInfo::ERR_OK;
	float fallMax = 0.0;
	float fallMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取下降斜率最大值
	ret = GetCRDCurFallSlopeMax(fallMax);

	//获取下降斜率最小值
	ret = GetCRDCurFallSlopeMin(fallMin);

	if(fall > fallMax)
	{
		fall = fallMax;
	}

	if(fall < fallMin)
	{
		fall = fallMin;
	}

	//封装控制语句
	sprintf(tmp, "RES:DYN:FALL %f\r\n", fall);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CRD模式电流下降斜率
int ChromaDcLoad::GetCRDCurFallSlope(float& fall)
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
	sprintf(tmp, "RES:DYN:FALL?\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式下降斜率最大值
int ChromaDcLoad::GetCRDCurFallSlopeMax(float& fall)
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
	sprintf(tmp, "RES:DYN:FALL? MAX\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CRD模式下降斜率最小值
int ChromaDcLoad::GetCRDCurFallSlopeMin(float& fall)
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
	sprintf(tmp, "RES:DYN:FALL? MIN\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//设置CRD模式电流量测档位
int ChromaDcLoad::SetCRDCurGear(int gear)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "RES:DYN:IRNG %d\r\n", gear);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CRD模式电流量测档位
int ChromaDcLoad::GetCRDCurGear(int& gear)
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
	sprintf(tmp, "RES:DYN:IRNG?\r\n");
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

	if(packData.compare("LOW\n") == 0)
	{
		gear = 3;
	}
	else if(packData.compare("MIDDLE\n") == 0)
	{
		gear = 2;
	}
	else if(packData.compare("HIGH\n") == 0)
	{
		gear = 1;
	}
	return ret;
}

//CV模式
//设置CV模式L1电压值
int ChromaDcLoad::SetCVL1Vol(float vol)
{
	int ret = ErrorInfo::ERR_OK;
	float volMax = 0.0;
	float volMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L1最大值
	ret = GetCVL1VolMax(volMax);

	//获取L1最小值
	ret = GetCVL1VolMin(volMin);

	if(vol > volMax)
	{
		vol = volMax;
	}

	if(vol < volMin)
	{
		vol = volMin;
	}

	//封装控制语句
	sprintf(tmp, "VOLT:STAT:L1 %f\r\n", vol);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CV模式L1电压值
int ChromaDcLoad::GetCVL1Vol(float& vol)
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
	sprintf(tmp, "VOLT:STAT:L1?\r\n");
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

	vol = atof(packData.c_str());
	return ret;
}

//获取设置CV模式L1电压最大值
int ChromaDcLoad::GetCVL1VolMax(float& vol)
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
	sprintf(tmp, "VOLT:STAT:L1? MAX\r\n");
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

	vol = atof(packData.c_str());
	return ret;
}

//获取设置CV模式L1电压最小值
int ChromaDcLoad::GetCVL1VolMin(float& vol)
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
	sprintf(tmp, "VOLT:STAT:L1? MIN\r\n");
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

	vol = atof(packData.c_str());
	return ret;
}

//设置CV模式L2电压值
int ChromaDcLoad::SetCVL2Vol(float vol)
{
	int ret = ErrorInfo::ERR_OK;
	float volMax = 0.0;
	float volMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L2最大值
	ret = GetCVL2VolMax(volMax);

	//获取L2最小值
	ret = GetCVL2VolMin(volMin);

	if(vol > volMax)
	{
		vol = volMax;
	}

	if(vol < volMin)
	{
		vol = volMin;
	}

	//封装控制语句
	sprintf(tmp, "VOLT:STAT:L2 %f\r\n", vol);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CV模式L2电压值
int ChromaDcLoad::GetCVL2Vol(float& vol)
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
	sprintf(tmp, "VOLT:STAT:L2?\r\n");
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

	vol = atof(packData.c_str());
	return ret;
}

// 获取设置CV模式L2电压最大值
int ChromaDcLoad::GetCVL2VolMax(float& vol)
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
	sprintf(tmp, "VOLT:STAT:L2? MAX\r\n");
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

	vol = atof(packData.c_str());
	return ret;
}

//获取设置CV模式L2电压最小值
int ChromaDcLoad::GetCVL2VolMin(float& vol)
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
	sprintf(tmp, "VOLT:STAT:L2? MIN\r\n");
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

	vol = atof(packData.c_str());
	return ret;
}

//设置CV模式限电流
int ChromaDcLoad::SetCVCurLimit(float limitCur)
{
	int ret = ErrorInfo::ERR_OK;
	float limitCurMax = 0.0;
	float limitCurMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取CV模式限电流最大值
	ret = GetCVCurLimitMax(limitCurMax);

	//获取CV模式限电流最小值
	ret = GetCVCurLimitMin(limitCurMin);

	if(limitCur > limitCurMax)
	{
		limitCur = limitCurMax;
	}

	if(limitCur < limitCurMin)
	{
		limitCur = limitCurMin;
	}

	//封装控制语句
	sprintf(tmp, "VOLT:STAT:ILIM %f\r\n", limitCur);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CV模式限电流
int ChromaDcLoad::GetCVCurLimit(float& limitCur)
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
	sprintf(tmp, "VOLT:STAT:ILIM?\r\n");
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

	limitCur = atof(packData.c_str());
	return ret;
}

//获取设置CV模式限电流最大值
int ChromaDcLoad::GetCVCurLimitMax(float& limitCur)
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
	sprintf(tmp, "VOLT:STAT:ILIM? MAX\r\n");
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

	limitCur = atof(packData.c_str());
	return ret;
}

//获取设置CV模式限电流最小值
int ChromaDcLoad::GetCVCurLimitMin(float& limitCur)
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
	sprintf(tmp, "VOLT:STAT:ILIM? MIN\r\n");
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

	limitCur = atof(packData.c_str());
	return ret;
}

//设置CV模式电压反应速度
int ChromaDcLoad::SetCVVolReact(int react)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "VOLT:STAT:RES %d\r\n", react);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CV模式电压反应速度
int ChromaDcLoad::GetCVVolReact(int& react)
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
	sprintf(tmp, "VOLT:STAT:RES?\r\n");
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

	if(packData.compare("SLOW\n") == 0)
	{
		react = 0;
	}
	else if(packData.compare("NORMAL\n") == 0)
	{
		react = 1;
	}
	else if(packData.compare("FAST\n") == 0)
	{
		react = 2;
	}

	return ret;
}

//设置CV模式电流量测档位
int ChromaDcLoad::SetCVCurGear(int gear)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "VOLT:STAT:IRNG %d\r\n", gear);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CV模式电流量测档位
int ChromaDcLoad::GetCVCurGear(int& gear)
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
	sprintf(tmp, "VOLT:STAT:IRNG?\r\n");
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

	if(packData.compare("LOW\n") == 0)
	{
		gear = 3;
	}
	else if(packData.compare("MIDDLE\n") == 0)
	{
		gear = 2;
	}
	else if(packData.compare("HIGH\n") == 0)
	{
		gear = 1;
	}
	return ret;
}
//CP模式
//设置CP模式L1功率值
int ChromaDcLoad::SetCPL1Pow(float power)
{
	int ret = ErrorInfo::ERR_OK;
	float powMax = 0.0;
	float powMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L1最大值
	ret = GetCPL1PowMax(powMax);

	//获取L1最小值
	ret = GetCPL1PowMin(powMin);

	if(power > powMax)
	{
		power = powMax;
	}

	if(power < powMin)
	{
		power = powMin;
	}

	//封装控制语句
	sprintf(tmp, "POW:STAT:L1 %f\r\n", power);
	str = tmp;
	zlog_warn(Util::m_zlog,"%s",str.c_str());
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CP模式L1功率值
int ChromaDcLoad::GetCPL1Pow(float& power)
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
	sprintf(tmp, "POW:STAT:L1?\r\n");
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

	power = atof(packData.c_str());
	return ret;
}

//获取设置CP模式L1功率值最大值
int ChromaDcLoad::GetCPL1PowMax(float& power)
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
	sprintf(tmp, "POW:STAT:L1? MAX\r\n");
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

	power = atof(packData.c_str());
	return ret;
}

//获取设置CP模式L1功率值最小值
int ChromaDcLoad::GetCPL1PowMin(float& power)
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
	sprintf(tmp, "POW:STAT:L1? MIN\r\n");
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

	power = atof(packData.c_str());
	return ret;
}

//设置CP模式L2功率值
int ChromaDcLoad::SetCPL2Pow(float power)
{
	int ret = ErrorInfo::ERR_OK;
	float powMax = 0.0;
	float powMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取L2最大值
	ret = GetCCL2CurMax(powMax);

	//获取L2最小值
	ret = GetCCL2CurMin(powMin);

	if(power > powMax)
	{
		power = powMax;
	}

	if(power < powMin)
	{
		power = powMin;
	}

	//封装控制语句
	sprintf(tmp, "POW:STAT:L2 %f\r\n", power);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CP模式L2功率值
int ChromaDcLoad::GetCPL2Pow(float& power)
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
	sprintf(tmp, "POW:STAT:L2?\r\n");
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

	power = atof(packData.c_str());
	return ret;
}

//获取设置CP模式L2功率值最大值
int ChromaDcLoad::GetCPL2PowMax(float& power)
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
	sprintf(tmp, "POW:STAT:L2? MAX\r\n");
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

	power = atof(packData.c_str());
	return ret;
}

//获取设置CP模式L2功率值最小值
int ChromaDcLoad::GetCPL2PowMin(float& power)
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
	sprintf(tmp, "POW:STAT:L2? MIN\r\n");
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

	power = atof(packData.c_str());
	return ret;
}

//设置CP模式电流上升斜率
int ChromaDcLoad::SetCPCurRisSlope(float rise)
{
	int ret = ErrorInfo::ERR_OK;
	float riseMax = 0.0;
	float riseMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取上升斜率最大值
	ret = GetCPCurRisSlopeMax(riseMax);

	//获取上升斜率最小值
	ret = GetCPCurRisSlopeMin(riseMin);

	if(rise > riseMax)
	{
		rise = riseMax;
	}

	if(rise < riseMin)
	{
		rise = riseMin;
	}

	//封装控制语句
	sprintf(tmp, "POW:STAT:RISE %f\r\n", rise);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CP模式电流上升斜率
int ChromaDcLoad::GetCPCurRisSlope(float& rise)
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
	sprintf(tmp, "POW:STAT:RISE?\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CP模式电流上升斜率最大值
int ChromaDcLoad::GetCPCurRisSlopeMax(float& rise)
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
	sprintf(tmp, "POW:STAT:RISE? MAX\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//获取设置CP模式电流上升斜率最小值
int ChromaDcLoad::GetCPCurRisSlopeMin(float& rise)
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
	sprintf(tmp, "POW:STAT:RISE? MIN\r\n");
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

	rise = atof(packData.c_str());
	return ret;
}

//设置CP模式电流下降斜率
int ChromaDcLoad::SetCPCurFallSlope(float fall)
{
	int ret = ErrorInfo::ERR_OK;
	float fallMax = 0.0;
	float fallMin = 0.0;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//获取下降斜率最大值
	ret = GetCPCurFallSlopeMax(fallMax);

	//获取下降斜率最小值
	ret = GetCPCurFallSlopeMin(fallMin);

	if(fall > fallMax)
	{
		fall = fallMax;
	}

	if(fall < fallMin)
	{
		fall = fallMin;
	}

	//封装控制语句
	sprintf(tmp, "POW:STAT:FALL %f\r\n", fall);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CP模式电流下降斜率
int ChromaDcLoad::GetCPCurFallSlope(float& fall)
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
	sprintf(tmp, "POW:STAT:FALL?\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CP模式电流下降斜率最大值
int ChromaDcLoad::GetCPCurFallSlopeMax(float& fall)
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
	sprintf(tmp, "POW:STAT:FALL? MAX\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//获取设置CP模式电流下降斜率最小值
int ChromaDcLoad::GetCPCurFallSlopeMin(float& fall)
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
	sprintf(tmp, "POW:STAT:FALL? MIN\r\n");
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

	fall = atof(packData.c_str());
	return ret;
}

//设置CP模式电压量测档位
int ChromaDcLoad::SetCPVolGear(int gear)
{
	int ret = ErrorInfo::ERR_OK;
	char tmp[ARRAY_LENGTH_512];
	string str;
	String values;

	str.clear();
	values.clear();

	//封装控制语句
	sprintf(tmp, "POW:STAT:VRNG %d\r\n", gear);
	str = tmp;
	for(unsigned int i = 0; i < str.size(); i++)
	{
		values.push_back(str.at(i));
	}

	//发送数据
	m_tcp->Send(values);

	return ret;
}

//获取设置CP模式电压量测档位
int ChromaDcLoad::GetCPVolGear(int& gear)
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
	sprintf(tmp, "POW:STAT:VRNG?\r\n");
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

	if(packData.compare("LOW\n") == 0)
	{
		gear = 3;
	}
	else if(packData.compare("MIDDLE\n") == 0)
	{
		gear = 2;
	}
	else if(packData.compare("HIGH\n") == 0)
	{
		gear = 1;
	}
	return ret;
}
