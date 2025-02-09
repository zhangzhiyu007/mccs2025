/*
 * QunLingDcLoad.cpp
 *
 *  Created on: 2013-10-11
 *      Author: liuhaitao
 */

#include "QunLingDcLoad.h"
#include "../../com/Com.h"

QunLingDcLoad::QunLingDcLoad()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_retries = 3;//默认3次
	m_delayTime = 1000;//延时默认1000ms
}

QunLingDcLoad::~QunLingDcLoad()
{
	// TODO Auto-generated destructor stub
}

void QunLingDcLoad::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

void QunLingDcLoad::SetRetries(int retries)
{
	m_retries = retries;
}

int QunLingDcLoad::GetRetries()
{
	return m_retries;
}

void QunLingDcLoad::SetDelayTime(int delayTime)
{
	m_delayTime = delayTime;
}

int QunLingDcLoad::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 2:
		ret = this->Bdct5000Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int QunLingDcLoad::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 2:
		ret = this->Bdct5000Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int QunLingDcLoad::Bdct5000Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取直流负载数据开始");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	UshortArray data;
	values.clear();
//	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	String query;
	String reply;
	float tmp = 0;
//	int runState = 0;//当前运行状态
//	RealData realData;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		zlog_warn(Util::m_zlog,"串口没有打开");
		return ErrorInfo::ERR_OPENED;
	}

	//读取实时状态数据
	data.clear();
	PackData(0x0035,0,data,query);
	ret = m_com->TxRxMsg(reply,query,42);

	//查询无错误
	if (ret == ErrorInfo::ERR_OK)
	{
		if (((reply[7]<<8)|(reply[8]&0xFF)) != 0x0035)
		{
			ret = ErrorInfo::ERR_INV_RESP;
			zlog_warn(Util::m_zlog,"应答错误");
		}
		else
		{
			comm = true;
			values.resize(9);
			//zlog_warn(Util::m_zlog,"直流负载状态字：reply[11] = %02x, reply[12] = %02x",reply[11],reply[12]);
			values[0] =(reply[11]<<8)|(reply[12]&0xFF);//状态字
			values[1] = (reply[13] << 24) | (reply[14] << 16) | (reply[15] << 8)
					| (reply[16]);//电压
			values[2]  = (reply[17] << 24) | (reply[18] << 16) | (reply[19] << 8)
					| (reply[20]);//电流
			values[3] = (reply[21] << 24) | (reply[22] << 16) | (reply[23] << 8)
					| (reply[24]);//容量
			values[4] = (reply[25] << 24) | (reply[26] << 16) | (reply[27] << 8)
					| (reply[28]);//功率
			values[5] = (reply[29] << 24) | (reply[30] << 16) | (reply[31] << 8)
					| (reply[32]);//阻值
			values[6] = (reply[33] << 8) | reply[34];//温度
			values[7] = (reply[35] << 24) | (reply[36] << 16) | (reply[37] << 8)
					| (reply[38]);//秒
			values[8] = (reply[39] << 8) | reply[40];//报警状态

			MemDb::SetRealData(regStart + 10, values[0],false);//运行状态
			tmp = values[1]*0.01;
			if(tmp < 2)
			{
				tmp = 0;//防止精度不准确
			}
			MemDb::SetRealData(regStart + 13, tmp,false);//电压
			tmp = values[2]*0.01;
			MemDb::SetRealData(regStart + 14, tmp,false);//电流
			tmp = values[3]*0.01;
			MemDb::SetRealData(regStart + 17,tmp,false);//容量
//			tmp = values[4]*0.001;

			MemDb::SetRealData(regStart + 16, (float)(values[4]),false);//功率
			tmp = values[5]*0.01;
			MemDb::SetRealData(regStart + 15, tmp,false);//阻值
			tmp = values[6] * 0.1;
			MemDb::SetRealData(regStart + 18, tmp,false);//温度
			MemDb::SetRealData(regStart + 19, values[7],false);//秒
			if (values[8] != 0)
			{
				MemDb::SetRealData(regStart + 11, 1,false);
				MemDb::SetRealData(regStart + 12, values[8],false);
			}
			else
			{
				MemDb::SetRealData(regStart + 11, 0,false);
				MemDb::SetRealData(regStart + 12, values[8],false);
			}
		}
	}
	else
	{
		zlog_warn(Util::m_zlog,"直流负载读取失败");
	}

//	//读取加载参数数据
//	data.clear();
//	PackData(0x0036, 0, data, query);
//	ret = m_com->TxRxMsg(reply, query, 68);
//	//查询无错误
//	if (ret == ErrorInfo::ERR_OK)
//	{
//		if (((reply[7]<<8)|(reply[8]&0xFF)) != 0x0036)
//		{
//			ret = ErrorInfo::ERR_INV_RESP;
//			zlog_warn(Util::m_zlog,"应答错误");
//		}
//		else
//		{
//			comm = true;
//			values.resize(23);
//			//无错误返回查询数组
//			//恒流加载模式
//			values[0] = (reply[11] << 8) | reply[12];//电压
//			values[1] = (reply[13] << 8) | reply[14];//电流上限
//			values[2] = (reply[15] << 8) | reply[16];//波段数
//			values[3] = (reply[17] << 8) | reply[18];//放电容量
//			values[4] = (reply[19] << 8) | reply[20];//电流小值
//			values[5] = (reply[21] << 8) | reply[22];//电流大值
//			values[6] = (reply[23] << 24) | (reply[24] << 16) | (reply[25] << 8)
//					| (reply[26]);//每段时间秒
//
//			//恒阻加载模式
//			values[7] = (reply[27] << 8) | reply[28];//电压
//			values[8] = (reply[29] << 8) | reply[30];//电流上限
//			values[9] = (reply[31] << 8) | reply[32];//波段数
//			values[10] = (reply[33] << 8) | reply[34];//放电容量
//			values[11] = (reply[35] << 24) | (reply[36] << 16) | (reply[37] << 8)
//					| (reply[38]);//电阻小值
//			values[12] = (reply[39] << 24) | (reply[40] << 16) | (reply[41] << 8)
//					| (reply[42]);//电阻大值
//			values[13] = (reply[43] << 24) | (reply[44] << 16) | (reply[45] << 8)
//					| (reply[46]);//每段时间秒
//
//			//恒功率加载模式
//			values[14] = (reply[47] << 8) | reply[48];//电压
//			values[15] = (reply[49] << 8) | reply[50];//电流上限
//			values[16] = (reply[51] << 8) | reply[52];//波段数
//			values[17] = (reply[53] << 8) | reply[54];//放电容量
//			values[18] = (reply[55] << 8) | reply[56];//功率小值
//			values[19] = (reply[57] << 8) | reply[58];//功率大值
//			values[20] = (reply[59] << 24) | (reply[60] << 16) | (reply[61] << 8)
//					| (reply[62]);//每段时间秒
//
//			values[21] = (reply[63] << 8) | (reply[64]);//电压
//			values[22] = (reply[65] << 8) | (reply[66]);//电流
//		}
//		MemDb::GetRealData(realData,regStart+2);
//		if(realData.pv.type == RealData::INT)
//		{
//			runState = realData.pv.data.iValue;
//		}
//		else if(realData.pv.type == RealData::DB)
//		{
//			zlog_warn(Util::m_zlog,"运行状态取值错误");
//			ret = ErrorInfo::ERR_READ;
//		}
//		if(runState == 1)
//		{
//			//恒流加载参数
//			zlog_info(Util::m_zlog,"恒流加载参数");
//			MemDb::SetRealData(regStart + 3, (float)values[0],false);//电压
//			MemDb::SetRealData(regStart + 4, (float)values[1],false);//电流上限
//			MemDb::SetRealData(regStart + 5, values[2],false);//波段数
//			MemDb::SetRealData(regStart + 6, (float)values[3],false);//放电容量
//			tmp = values[4]*0.1;
//			MemDb::SetRealData(regStart + 7, tmp,false);//电流小值
//			tmp = values[5]*0.1;
//			MemDb::SetRealData(regStart + 8, tmp,false);//电流大值
//			MemDb::SetRealData(regStart + 9, values[6],false);//每段时间（s）
//		}
//		else if(runState == 2)
//		{
//			//恒阻加载参数
//			zlog_info(Util::m_zlog,"恒阻加载参数");
//			MemDb::SetRealData(regStart + 3, (float)values[7],false);//电压
//			MemDb::SetRealData(regStart + 4, (float)values[8],false);//电流上限
//			MemDb::SetRealData(regStart + 5, values[9],false);//波段数
//			MemDb::SetRealData(regStart + 6, (float)values[10],false);//放电容量
//			tmp = values[11]*0.1;
//			MemDb::SetRealData(regStart + 7, tmp,false);//电阻小值
//			tmp = values[12]*0.1;
//			MemDb::SetRealData(regStart + 8, tmp,false);//电阻大值
//			MemDb::SetRealData(regStart + 9, values[13],false);//每段时间（s）
//		}
//		else if(runState ==3)
//		{
//			//恒功率加载参数
//			zlog_info(Util::m_zlog,"恒功率加载参数");
//			MemDb::SetRealData(regStart + 3, (float)values[14],false);//电压
//			MemDb::SetRealData(regStart + 4, (float)values[15],false);//电流上限
//			MemDb::SetRealData(regStart + 5, values[16],false);//波段数
//			MemDb::SetRealData(regStart + 6, (float)values[17],false);//放电容量
//			MemDb::SetRealData(regStart + 7, (float)values[18] * 10,false);//功率小值
//			MemDb::SetRealData(regStart + 8, (float)values[19] * 10,false);//功率大值
//			MemDb::SetRealData(regStart + 9, values[20],false);//每段时间（s
//		}
//		else
//		{
//			zlog_warn(Util::m_zlog,"当前不是恒流、恒阻、恒功加载模式");
//		}
//	}

	MemDb::SetRealData(regStart, comm,false);
	zlog_info(Util::m_zlog,"查询直流负载实时数据结束");
	return ret;
}

int QunLingDcLoad::Bdct5000Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	//	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	RealData realData1;
	RealData realData2;
	RealData realData3;
	RealData realData4;
	RealData realData5;
	RealData realData6;
	RealData realData7;
	int regStart = dev->regStart;
	String query;
	String reply;
	int tmp = 0;
	int retry = 0;

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		zlog_warn(Util::m_zlog,"直流负载设定：串口没有打开");
		return ErrorInfo::ERR_OPENED;
	}

	//判断是否有启停命令
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			if (realData.pv.data.iValue == 0)
			{
				values.push_back(0x00);
			}
			else if (realData.pv.data.iValue == 1)
			{
				MemDb::GetRealData(realData, regStart + 2);
				values.push_back(realData.pv.data.iValue);
			}
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"启停命令错误");
			ret = ErrorInfo::ERR_WRITE;
		}
		PackData(0x0029, 2, values, query);

		ret = m_com->TxRxMsg(reply, query, 14);

		for (int i = 0; i < query.size(); i++)
		{
			printf("%02X ", query[i]);
		}
		printf("\n");

		if (ret == ErrorInfo::ERR_OK)
		{
			zlog_warn(Util::m_zlog,"设定成功");
		}
		else
		{
			zlog_warn(Util::m_zlog,"设定失败 %d",ret);
		}
	}

	//判断是否加载参数设置命令
	if ((MemDb::GetRealDataWrited(realData1, regStart + 3, true))
			||(MemDb::GetRealDataWrited(realData2, regStart + 4, true))
			||(MemDb::GetRealDataWrited(realData3, regStart + 5, true))
			||(MemDb::GetRealDataWrited(realData4, regStart + 6, true))
			||(MemDb::GetRealDataWrited(realData5, regStart + 7, true))
			||(MemDb::GetRealDataWrited(realData6, regStart + 8, true))
			||(MemDb::GetRealDataWrited(realData7, regStart + 9, true)))
	{
		MemDb::GetRealData(realData, regStart + 2,true);//工作模式 先判断设置什么模式，在设置参数
		MemDb::GetRealData(realData1, regStart + 3,true);//电压
		MemDb::GetRealData(realData2, regStart + 4,true);//电流上限
		MemDb::GetRealData(realData3, regStart + 5,true);//波段数
		MemDb::GetRealData(realData4, regStart + 6,true);//放电容量
		MemDb::GetRealData(realData5, regStart + 7,true);//电流、电阻、功率小值
		MemDb::GetRealData(realData6, regStart + 8,true);//电流、电阻、功率大值
		MemDb::GetRealData(realData7, regStart + 9,true);//每段时间

		MemDb::GetRealData(realData, regStart + 2);//工作模式 先判断设置什么模式，在设置参数
		MemDb::GetRealData(realData1, regStart + 3);//电压
		MemDb::GetRealData(realData2, regStart + 4);//电流上限
		MemDb::GetRealData(realData3, regStart + 5);//波段数
		MemDb::GetRealData(realData4, regStart + 6);//放电容量
		MemDb::GetRealData(realData5, regStart + 7);//电流、电阻、功率小值
		MemDb::GetRealData(realData6, regStart + 8);//电流、电阻、功率大值
		MemDb::GetRealData(realData7, regStart + 9);//每段时间

		zlog_warn(Util::m_zlog,"直流负载设定：设置定=%d",realData.pv.data.iValue);
		if (realData.pv.data.iValue == 1)//恒流加载
		{
			values.push_back(realData1.pv.data.dbValue);//电压
			values.push_back(realData2.pv.data.dbValue);//电流上限
			values.push_back(realData3.pv.data.iValue);//波段数
			values.push_back(realData4.pv.data.dbValue);//放电容量
			tmp = realData5.pv.data.dbValue * 10;
			values.push_back(tmp);//电流小值
			tmp = realData6.pv.data.dbValue * 10;
			values.push_back(tmp);//电流大值
			values.push_back((realData7.pv.data.iValue >> 16) & 0xFFFF);//时间高字节
			values.push_back(realData7.pv.data.iValue & 0xFFFF);//时间低字节
			PackData(0x0027, 16, values, query);
			//PackData(0x0027, 0x0E, values, query);

			zlog_warn(Util::m_zlog,"恒流加载:电压=%.2f,电流上限=%.2f,波段数=%d,放电容量=%.2f,电流小值=%.2f,电流大值=%.2f,每段时间=%d",
					realData1.pv.data.dbValue,
					realData2.pv.data.dbValue,
					realData3.pv.data.iValue,
					realData4.pv.data.dbValue,
					realData5.pv.data.dbValue,
					realData6.pv.data.dbValue,
					realData7.pv.data.iValue);
		}
		else if (realData.pv.data.iValue == 2)//恒阻加载
		{
			values.push_back(realData1.pv.data.dbValue);//电压
			values.push_back(realData2.pv.data.dbValue);//电流上限
			values.push_back(realData3.pv.data.iValue);//波段数
			values.push_back(realData4.pv.data.dbValue);//放电容量
			tmp = realData5.pv.data.dbValue * 10;
			values.push_back((tmp >> 16) & 0xFFFF);//电阻小值高字节
			values.push_back(tmp & 0xFFFF);//电阻小值低字节
			tmp = realData6.pv.data.dbValue * 10;
			values.push_back((tmp >> 16) & 0xFFFF);//电阻大值高字节
			values.push_back(tmp & 0xFFFF);//电阻小值低字节
			values.push_back((realData7.pv.data.iValue >> 16) & 0xFFFF);//每段时间高字节
			values.push_back(realData7.pv.data.iValue & 0xFFFF);//每段时间低字节
			PackData(0x0028, 20, values, query);
			zlog_warn(Util::m_zlog,"恒阻加载:电压=%.2f,电流上限=%.2f,波段数=%d,放电容量=%.2f,电阻小值=%.2f,电阻大值=%.2f,每段时间=%d",
					realData1.pv.data.dbValue,
					realData2.pv.data.dbValue,
					realData3.pv.data.iValue,
					realData4.pv.data.dbValue,
					realData5.pv.data.dbValue,
					realData6.pv.data.dbValue,
					realData7.pv.data.iValue);
		}
		else if (realData.pv.data.iValue == 3)//恒功率加载
		{
			values.push_back(realData1.pv.data.dbValue);//电压
			values.push_back(realData2.pv.data.dbValue);//电流上限
			values.push_back(realData3.pv.data.iValue);//波段数
			values.push_back(realData4.pv.data.dbValue);//放电容量
			tmp = realData5.pv.data.dbValue * 100 * 0.001;
			values.push_back(tmp);//功率小值
			tmp = realData6.pv.data.dbValue * 100 * 0.001;
			values.push_back(tmp);//功率大值
			values.push_back((realData7.pv.data.iValue >> 16) & 0xFFFF);//每段时间高字节
			values.push_back(realData7.pv.data.iValue & 0xFFFF);//每段时间低字节
			PackData(0x0024, 16, values, query);
			zlog_warn(Util::m_zlog,"恒功率加载:电压=%.2f,电流上限=%.2f,波段数=%d,放电容量=%.2f,功率小值=%.2f,功率大值=%.2f,每段时间=%d",
					realData1.pv.data.dbValue,
					realData2.pv.data.dbValue,
					realData3.pv.data.iValue,
					realData4.pv.data.dbValue,
					realData5.pv.data.dbValue,
					realData6.pv.data.dbValue,
					realData7.pv.data.iValue);
		}
		else
		{
			return ret;
		}

		for (int i = 0; i < query.size(); i++)
		{
			printf("%02X ", query[i]);
		}
		printf("\n");

		ret = m_com->TxRxMsg(reply, query, 14);
		if (ret == ErrorInfo::ERR_OK)
		{
			zlog_warn(Util::m_zlog,"设定成功");
		}
		else
		{
			zlog_warn(Util::m_zlog,"设定失败 %d",ret);
		}

	}

	return ret;
}

void QunLingDcLoad::PackData(int function,int length,UshortArray values,String& query)
{
	unsigned short crc16 = 0;
	unsigned int i =0;

	//封装数据帧
	query.clear();
	query.push_back(0x7E);//信息头
	query.push_back(0x01);//信息头
	query.push_back(0x00);//信息头
	query.push_back(0x00);//信息头
	query.push_back(0x02);//信息头
	query.push_back(0x10);//信息头
	query.push_back(0x02);//信息头
	query.push_back(function>>8);//功能码高字节
	query.push_back(function&0xFF);//功能码低字节
	query.push_back(length>>8);//数据区长度
	query.push_back(length&0xFF);//数据区长度

	for(i= 0;i<values.size();i++)
	{
		query.push_back((values[i]>>8)&0xFF);
		query.push_back(values[i]&0xFF);
	}

	//Crc16校验 包括信息头、功能码、帧长度
	crc16 = Check::Crc16(query);
	query.push_back(crc16&0xFF);//校验低字节
	query.push_back(crc16>>8);//校验高字节
	query.push_back(0x0D);//结束帧

	zlog_info(Util::m_zlog,"封装结束");
}
