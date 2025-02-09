/*
 * AcrelBox.cpp
 *
 *  创建日期: 2016-3-25
 *  作          者: 刘海涛
 */

#include "AcrelBox.h"

AcrelBox::AcrelBox()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

AcrelBox::~AcrelBox()
{
	// TODO Auto-generated destructor stub
}

void AcrelBox::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

int AcrelBox::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_M4FTRA_KV_T2:
		ret = this->AcrelM4FTRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int AcrelBox::AcrelM4FTRead(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"开始读取%s数据",dev->name.c_str());

	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int start = 7;
	int length = 20;
	int regStart = dev->regStart;
	float tmp = 0;
	int data = 0;

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

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	//读取汇流箱数据
	ret = rtu.ReadOutputRegisters(values,addr,start,length);

	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = ((short)(values[0])) & 0xFF;
		MemDb::SetRealData(regStart + 4, (float)data, false);//温度
		tmp = ((short)(values[8])) * 0.1;
		MemDb::SetRealData(regStart + 1, tmp, false);//电压
		tmp = ((short)(values[9])) * 0.1;
		MemDb::SetRealData(regStart + 2, tmp, false);//电流
		tmp = ((short)(values[10])) * 0.1 * 1000;
		MemDb::SetRealData(regStart + 3, tmp, false);//功率
		tmp = ((short)(values[11])) * 0.01;
		MemDb::SetRealData(regStart + 5, tmp, false);//第一路电流
		tmp = ((short)(values[12])) * 0.01;
		MemDb::SetRealData(regStart + 6, tmp, false);//第二路电流
	}
	else
	{
		zlog_warn(Util::m_zlog,"%s读取电流数据错误",dev->name.c_str());
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

	zlog_info(Util::m_zlog,"读取%s数据完毕",dev->name.c_str());
	return ret;
}
