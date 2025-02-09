 /*
 * Env.cpp
 *
 *   创建日期: 2013-6-24
 *   作         者: mzh
 */

#include "Env.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"

Env::Env()
{
	// TODO 构造函数
	m_com = NULL;
	m_tcpClient = NULL;
}

Env::~Env()
{
	// TODO 析构函数
}

void Env::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void Env::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int Env::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devManu)
	{
	case DevManu::Manu_JZYG://锦州阳光
		ret = Pc4Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int Env::Pc4Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog, "读取环境监测装置数据");
	bool comm = true;
	int ret = ErrorInfo::ERR_FAILED;
	UshortArray data;
	data.clear();

	if(NULL==dev)
	{
		zlog_info(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int start = 0;
	int length = 10;
	float tmp = 0;

	int regStart = dev->regStart;
	//1、读取保持寄存器
	ret = rtu.ReadOutputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = ((values[0]*0.001)/3.6)*1000;//辐射日累计值MJ转换成Wh
		MemDb::SetRealData(regStart+2,tmp,false);//日累计幅度
//		tmp = ((short)(values[1]))*0.1;//温度
//		MemDb::SetRealData(regStart+7,tmp,false);
		tmp = ((short)(values[2]))*0.1;//环境温度
		MemDb::SetRealData(regStart+6,tmp,false);
		tmp = (short)(values[3]*0.1);//环境湿度
		MemDb::SetRealData(regStart+10,tmp,false);
		tmp = values[4]*0.1;
		MemDb::SetRealData(regStart+8,tmp,false);
		//2分钟平均风速
		//10分钟平均风速
		//风向
		MemDb::SetRealData(regStart+9,values[7],false);
		//辐射瞬时值
		MemDb::SetRealData(regStart+4,values[8],false);
		//电量
	}

	MemDb::SetRealData(regStart + 0, comm, false);

	return ret;
}

int Env::Jz322Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog, "读取环境监测装置数据");
	bool comm = false;
	int ret = ErrorInfo::ERR_FAILED;
	UshortArray data;
	data.clear();

	if(NULL==dev)
	{
		zlog_info(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int start = 0;
	int length = 0x0A;
	int tmp = 0;
	short dataTmp=0;

	int regStart = dev->regStart;
	//1、读取保持寄存器
	//zlog_info(Util::m_zlog, "读取环境监测装置数据：查询命令,地址=%d,起始地址=%d,长度=%d",addr,start,length);
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	//zlog_warn(Util::m_zlog, "读取环境监测装置数据：查询结果=%d",ret);
	if (ErrorInfo::ERR_OK == ret)
	{

		//保存通讯正常
		comm = true;
		ret = ErrorInfo::ERR_OK;

//		//0辐射日累计
//		tmp = values[0] * 100 / 36;//从kj转换成wh
//		MemDb::SetRealData(regStart + 2, tmp, false);
//
//		//1背板温度、
//		MemDb::SetRealData(regStart + 7, values[1], false);
//		//3环境温度、
//		MemDb::SetRealData(regStart + 6, values[3], false);
//		//4环境湿度、
//
//		//5风速瞬时值、
//		MemDb::SetRealData(regStart + 8, values[5], false);
//		//6,2分钟平均风速、
//		//7\10分钟平均风速、
//		//8\风向、
//		MemDb::SetRealData(regStart + 9, values[8] * 10, false);
//		//9\辐射瞬时值
//		MemDb::SetRealData(regStart + 4, values[9] * 10, false);
//
//		for(int i=0;i<10;i++)
//		{
//			zlog_warn(Util::m_zlog, "读取环境监测装置数据：i%d=%d",i,values[i]);
//		}
		//0辐射日累计
		tmp = values[0] * 100 / 36;//从kj转换成wh
		MemDb::SetRealData(regStart + 2, tmp, false);

		//1背板温度、
		dataTmp=(short)values[2];
		if(values[8]>5000)
		{
			dataTmp+=230;
		}
		else
		{
			dataTmp+=60;
		}

		MemDb::SetRealData(regStart + 7, dataTmp, false);
		//3环境温度、
		MemDb::SetRealData(regStart + 6, values[2], false);
		//4环境湿度、

		//5风速瞬时值、
		MemDb::SetRealData(regStart + 8, values[4], false);
		//6,2分钟平均风速、
		//7\10分钟平均风速、
		//8\风向、
		MemDb::SetRealData(regStart + 9, values[7] * 10, false);
		//9\辐射瞬时值
		MemDb::SetRealData(regStart + 4, values[8] * 10, false);
	}

	MemDb::SetRealData(regStart + 0, comm, false);

	return ret;
}
