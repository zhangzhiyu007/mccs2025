/*
 * HzConverterPvs.cpp
 *
 *  创建日期: 2016-6-7
 *  作          者: 刘海涛
 */

#include "HzConverterPvs.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Check.h"

HzConverterPvs::HzConverterPvs()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

HzConverterPvs::~HzConverterPvs()
{
	// TODO Auto-generated destructor stub
}

void HzConverterPvs::SetCom(Com* com)
{
	assert(com!=NULL);
	m_com = com;
}

//设置TcpClient指针
void HzConverterPvs::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int HzConverterPvs::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_SPC030KTL:
		ret = this->Spc030KtlRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int HzConverterPvs::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_SPC030KTL:
		ret = this->Spc030KtlPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int HzConverterPvs::Spc030KtlRead(Device::SlaveDev* dev)
{
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int error;
	float data = 0.0;
	int value = 0;
	int mode = 0;
	int runSta = 0;
	int fault = 0;
	int fault1 = 0;

	String query;//查询命令数据
	String reply;////应答字符串
	String crcQuery;

	query.clear();
	reply.clear();
	crcQuery.clear();

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

	unsigned short crc16;

	//modbus message
	query.push_back(0xAA);
	query.push_back(0x55);
	query.push_back((char)(addr >> 8)&0xFF);
	query.push_back((char)(addr & 0xFF));
	query.push_back(0x04);//功能码
	query.push_back(0x00);
	query.push_back(0x00);
	query.push_back(0x00);
	query.push_back(0x16);

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 >> 8)&0xFF);
	query.push_back((char)(crc16 & 0xFF));

	error = m_com->TxRxMsg(reply, query, 53);
	if (error == ErrorInfo::ERR_OK)
	{
		comm = true;
		data = ((reply[7]<<8) | (reply[8])) * 0.1;
		MemDb::SetRealData(regStart + 10, data, false);//温度
		data = ((reply[9]<<8) | (reply[10])) * 0.1;
		MemDb::SetRealData(regStart + 6, data, false);//PV1电压
		data = ((reply[11]<<8) | (reply[12])) * 0.1;
		MemDb::SetRealData(regStart + 7, data, false);//PV2电压
		data = ((reply[13]<<8) | (reply[14])) * 0.1;
		MemDb::SetRealData(regStart + 8, data, false);//PV1电流
		data = ((reply[15]<<8) | (reply[16])) * 0.1;
		MemDb::SetRealData(regStart + 9, data, false);//PV2电流
		data = ((reply[17]<<24) | (reply[18]<<16) | (reply[19]<<8) | (reply[20])) * 0.1;
		MemDb::SetRealData(regStart + 19, data, false);//总发电量
		data = ((reply[21]<<24) | (reply[22]<<16) | (reply[23]<<8) | (reply[24]));
		MemDb::SetRealData(regStart + 20, data, false);//总运行时间
		data = ((reply[25]<<8) | (reply[26]));
		MemDb::SetRealData(regStart + 18, data, false);//有功功率
		data = ((reply[27]<<8) | (reply[28])) * 0.01;
		MemDb::SetRealData(regStart + 21, data, false);//日发电量
		data = ((reply[29]<<8) | (reply[30])) * 0.1;
		MemDb::SetRealData(regStart + 12, data, false);//A相电流
		data = ((reply[31]<<8) | (reply[32])) * 0.1;
		MemDb::SetRealData(regStart + 11, data, false);//A相电压
		data = ((reply[33]<<8) | (reply[34])) * 0.01;
		MemDb::SetRealData(regStart + 17, data, false);//频率
		data = ((reply[35]<<8) | (reply[36])) * 0.1;
		MemDb::SetRealData(regStart + 14, data, false);//B相电流
		data = ((reply[37]<<8) | (reply[38])) * 0.1;
		MemDb::SetRealData(regStart + 13, data, false);//B相电压
		data = ((reply[39]<<8) | (reply[40])) * 0.1;
		MemDb::SetRealData(regStart + 16, data, false);//C相电流
		data = ((reply[41]<<8) | (reply[42])) * 0.1;
		MemDb::SetRealData(regStart + 15, data, false);//C相电压
		value = ((reply[43]<<8) | (reply[44]));
		if(value == 0)
		{
			mode = 1;
			runSta = 0;
		}
		else if(value == 1)
		{
			mode = 2;
			runSta = 1;
		}
		else if(value == 2)
		{
			mode = 3;
			runSta = 0;
		}
		else if(value == 5)
		{
			mode = 4;
			runSta = 0;
		}
		MemDb::SetRealData(regStart + 1, runSta, false);//运行状态
		MemDb::SetRealData(regStart + 2, mode, false);//工作模式

		fault = ((reply[45]<<24) | (reply[46]<<16) | (reply[47]<<8) | (reply[48]));
		fault1 = ((reply[49]<<8) | (reply[50]));
		if((fault > 0) || (fault1 > 0))
		{
			MemDb::SetRealData(regStart + 3, 1, false);//故障状态
			MemDb::SetRealData(regStart + 4, fault, false);//故障码1
			MemDb::SetRealData(regStart + 5, fault1, false);//故障码2
		}
		else
		{
			MemDb::SetRealData(regStart + 3, 0, false);//故障状态
			MemDb::SetRealData(regStart + 4, fault, false);//故障码1
			MemDb::SetRealData(regStart + 5, fault1, false);//故障码2
		}
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int HzConverterPvs::Spc030KtlPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;


	return ret;
}
