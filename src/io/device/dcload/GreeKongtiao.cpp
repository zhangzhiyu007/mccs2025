/*
 * GreeKongtiao.cpp
 *
 *  创建日期: 2019-7-31
 *  作          者: 刘海涛
 */

#include "GreeKongtiao.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

GreeKongtiao::GreeKongtiao()
{
	// TODO Auto-generated constructor stub
	m_isTcp = false;
	m_com = NULL;
	m_tcpClient = NULL;
}

GreeKongtiao::~GreeKongtiao()
{
	// TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void GreeKongtiao::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

void GreeKongtiao::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void GreeKongtiao::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int GreeKongtiao::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	if(NULL==dev)
	{
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_KONGTIAO:
		ret = this->GreeKongtiaoRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int GreeKongtiao::GreeKongtiaoRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 0;
	int data = 0;
	float tmp = 0.0;

	start = 0x0000;
	length = 60;
	//读取电能模块实时数据
	ret = rtu.ReadOutputRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		tmp = ((short) values[0]) * 2.0;
		MemDb::SetRealData(regStart + 1, tmp, false);//外机模块1电网侧输入相电压有效值
		tmp = ((short) values[1]) * 1.0;
		MemDb::SetRealData(regStart + 2, tmp, false);//外机模块1光伏直流母线电压
		tmp = (values[2] << 8) + ((values[2] & 0xFF) / 256.0);
		MemDb::SetRealData(regStart + 3, tmp, false);//外机模块1光伏直流母线电压
		tmp = (((short) values[3]) - 32768) * 10.0;
		MemDb::SetRealData(regStart + 4, tmp, false);//外机模块1电网侧并网功率
		tmp = ((short) values[4]) * 10.0;
		MemDb::SetRealData(regStart + 5, tmp, false);//外机模块1光伏发电功率
		tmp = (((short) values[5]) - 32768) / 3600.0;
		MemDb::SetRealData(regStart + 6, tmp, false);//外机模块1电网侧并网电量
		tmp = ((short) values[6]) / 3600000.0;
		MemDb::SetRealData(regStart + 7, tmp, false);//外机模块1光伏发电电量
		tmp = (values[7] << 8) + ((values[7] & 0xFF) / 256.0);
		MemDb::SetRealData(regStart + 8, tmp, false);//外机模块1光伏侧输入电流
		tmp = ((short) values[8]) / 10.0;
		MemDb::SetRealData(regStart + 9, tmp, false);//外机模块1光伏侧限制功率百分比设定

		tmp = ((short) values[9]) * 2.0;
		MemDb::SetRealData(regStart + 10, tmp, false);//外机模块2电网侧输入相电压有效值
		tmp = ((short) values[10]) * 1.0;
		MemDb::SetRealData(regStart + 11, tmp, false);//外机模块2光伏直流母线电压
		tmp = (values[11] << 8) + ((values[11] & 0xFF) / 256.0);
		MemDb::SetRealData(regStart + 12, tmp, false);//外机模块2光伏直流母线电压
		tmp = (((short) values[12]) - 32768) * 10.0;
		MemDb::SetRealData(regStart + 13, tmp, false);//外机模块2电网侧并网功率
		tmp = ((short) values[13]) * 10.0;
		MemDb::SetRealData(regStart + 14, tmp, false);//外机模块2光伏发电功率
		tmp = (((short) values[14]) - 32768) / 3600.0;
		MemDb::SetRealData(regStart + 15, tmp, false);//外机模块2电网侧并网电量
		tmp = ((short) values[15]) / 3600000.0;
		MemDb::SetRealData(regStart + 16, tmp, false);//外机模块2光伏发电电量
		tmp = (values[16] << 8) + ((values[16] & 0xFF) / 256.0);
		MemDb::SetRealData(regStart + 17, tmp, false);//外机模块2光伏侧输入电流
		tmp = ((short) values[17]) / 10.0;
		MemDb::SetRealData(regStart + 18, tmp, false);//外机模块2光伏侧限制功率百分比设定

		tmp = ((short) values[18]) * 2.0;
		MemDb::SetRealData(regStart + 19, tmp, false);//外机模块3电网侧输入相电压有效值
		tmp = ((short) values[19]) * 1.0;
		MemDb::SetRealData(regStart + 20, tmp, false);//外机模块3光伏直流母线电压
		tmp = (values[20] << 8) + ((values[20] & 0xFF) / 256.0);
		MemDb::SetRealData(regStart + 21, tmp, false);//外机模块3光伏直流母线电压
		tmp = (((short) values[21]) - 32768) * 10.0;
		MemDb::SetRealData(regStart + 22, tmp, false);//外机模块3电网侧并网功率
		tmp = ((short) values[22]) * 10.0;
		MemDb::SetRealData(regStart + 23, tmp, false);//外机模块3光伏发电功率
		tmp = (((short) values[23]) - 32768) / 3600.0;
		MemDb::SetRealData(regStart + 24, tmp, false);//外机模块3电网侧并网电量
		tmp = ((short) values[24]) / 3600000.0;
		MemDb::SetRealData(regStart + 25, tmp, false);//外机模块3光伏发电电量
		tmp = (values[25] << 8) + ((values[25] & 0xFF) / 256.0);
		MemDb::SetRealData(regStart + 26, tmp, false);//外机模块3光伏侧输入电流
		tmp = ((short) values[26]) / 10.0;
		MemDb::SetRealData(regStart + 27, tmp, false);//外机模块3光伏侧限制功率百分比设定

		tmp = ((short) values[27]) * 2.0;
		MemDb::SetRealData(regStart + 28, tmp, false);//外机模块4电网侧输入相电压有效值
		tmp = ((short) values[28]) * 1.0;
		MemDb::SetRealData(regStart + 29, tmp, false);//外机模块4光伏直流母线电压
		tmp = (values[29] << 8) + ((values[29] & 0xFF) / 256.0);
		MemDb::SetRealData(regStart + 30, tmp, false);//外机模块4光伏直流母线电压
		tmp = (((short) values[30]) - 32768) * 10.0;
		MemDb::SetRealData(regStart + 31, tmp, false);//外机模块4电网侧并网功率
		tmp = ((short) values[31]) * 10.0;
		MemDb::SetRealData(regStart + 32, tmp, false);//外机模块4光伏发电功率
		tmp = (((short) values[32]) - 32768) / 3600.0;
		MemDb::SetRealData(regStart + 33, tmp, false);//外机模块4电网侧并网电量
		tmp = ((short) values[33]) / 3600000.0;
		MemDb::SetRealData(regStart + 34, tmp, false);//外机模块4光伏发电电量
		tmp = (values[34] << 8) + ((values[34] & 0xFF) / 256.0);
		MemDb::SetRealData(regStart + 35, tmp, false);//外机模块4光伏侧输入电流
		tmp = ((short) values[35]) / 10.0;
		MemDb::SetRealData(regStart + 36, tmp, false);//外机模块4光伏侧限制功率百分比设定

		tmp = ((((short)values[36]) * 65536) + ((short)values[37])) * 0.01;
		MemDb::SetRealData(regStart + 37, tmp, false);//外机模块1电网侧并网总供电量
		tmp = ((((short)values[38]) * 65536) + ((short)values[39])) * 0.01;
		MemDb::SetRealData(regStart + 38, tmp, false);//外机模块1电网侧并网总馈电量
		tmp = ((((short)values[40]) * 65536) + ((short)values[41])) * 0.01;
		MemDb::SetRealData(regStart + 39, tmp, false);//外机模块1光伏总发电电量

		tmp = ((((short)values[42]) * 65536) + ((short)values[43])) * 0.01;
		MemDb::SetRealData(regStart + 40, tmp, false);//外机模块2电网侧并网总供电量
		tmp = ((((short)values[44]) * 65536) + ((short)values[45])) * 0.01;
		MemDb::SetRealData(regStart + 41, tmp, false);//外机模块2电网侧并网总馈电量
		tmp = ((((short)values[46]) * 65536) + ((short)values[47])) * 0.01;
		MemDb::SetRealData(regStart + 42, tmp, false);//外机模块2光伏总发电电量

		tmp = ((((short)values[48]) * 65536) + ((short)values[49])) * 0.01;
		MemDb::SetRealData(regStart + 43, tmp, false);//外机模块3电网侧并网总供电量
		tmp = ((((short)values[50]) * 65536) + ((short)values[51])) * 0.01;
		MemDb::SetRealData(regStart + 44, tmp, false);//外机模块3电网侧并网总馈电量
		tmp = ((((short)values[52]) * 65536) + ((short)values[53])) * 0.01;
		MemDb::SetRealData(regStart + 45, tmp, false);//外机模块3光伏总发电电量

		tmp = ((((short)values[54]) * 65536) + ((short)values[55])) * 0.01;
		MemDb::SetRealData(regStart + 46, tmp, false);//外机模块4电网侧并网总供电量
		tmp = ((((short)values[56]) * 65536) + ((short)values[57])) * 0.01;
		MemDb::SetRealData(regStart + 47, tmp, false);//外机模块4电网侧并网总馈电量
		tmp = ((((short)values[58]) * 65536) + ((short)values[59])) * 0.01;
		MemDb::SetRealData(regStart + 48, tmp, false);//外机模块4光伏总发电电量
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取%s实时数据失败",dev->name.c_str());
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}
