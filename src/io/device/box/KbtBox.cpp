/*
 * KbtBox.cpp
 *
 *  Created on: 2013-10-16
 *      Author: liuhaitao
 */

#include "KbtBox.h"

#include "../../com/Com.h"

KbtBox::KbtBox()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

KbtBox::~KbtBox()
{
	// TODO Auto-generated destructor stub
}

void KbtBox::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

int KbtBox::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 14:
		ret = this->KbtPvx8Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int KbtBox::KbtPvx8Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取科比特汇流箱数据开始");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int start = 0x0100;
	int length = 24;
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
	//读取三次，如果三次都不成功认为通讯失败
	for(int i =0;i<3;i++)
	{
		ret = rtu.ReadOutputRegisters(values,addr,start,length);
		if(ret == ErrorInfo::ERR_OK)
		{
			break;
		}
	}

	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0]*0.1;
		MemDb::SetRealData(regStart+4,tmp,false);//直流母线电压
		tmp = values[2]*0.1;
		MemDb::SetRealData(regStart+3,tmp,false);//箱内温度
		tmp = values[8]*0.01;
		MemDb::SetRealData(regStart+5,tmp,false);//第一路电流
		tmp = values[9]*0.01;
		MemDb::SetRealData(regStart+6,tmp,false);//第二路电流
		tmp = values[10]*0.01;
		MemDb::SetRealData(regStart+7,tmp,false);//第三路电流
		tmp = values[11]*0.01;
		MemDb::SetRealData(regStart+8,tmp,false);//第四路电流
		tmp = values[12]*0.01;
		MemDb::SetRealData(regStart+9,tmp,false);//第五路电流
		tmp = values[13]*0.01;
		MemDb::SetRealData(regStart+10,tmp,false);//第六路电流
		tmp = values[14]*0.01;
		MemDb::SetRealData(regStart+11,tmp,false);//第七路电流
		tmp = values[15]*0.01;
		MemDb::SetRealData(regStart+12,tmp,false);//第八路电流
		tmp = values[16]*0.01;
		MemDb::SetRealData(regStart+13,tmp,false);//第九路电流
		tmp = values[17]*0.01;
		MemDb::SetRealData(regStart+14,tmp,false);//第十路电流
		tmp = values[18]*0.01;
		MemDb::SetRealData(regStart+15,tmp,false);//第十一路电流
		tmp = values[19]*0.01;
		MemDb::SetRealData(regStart+16,tmp,false);//第十二路电流
		tmp = values[20]*0.01;
		MemDb::SetRealData(regStart+17,tmp,false);//第十三路电流
		tmp = values[21]*0.01;
		MemDb::SetRealData(regStart+18,tmp,false);//第十四路电流
		tmp = values[22]*0.01;
		MemDb::SetRealData(regStart+19,tmp,false);//第十五路电流
		tmp = values[23]*0.01;
		MemDb::SetRealData(regStart+20,tmp,false);//第十六路电流
	}
	else
	{
		zlog_warn(Util::m_zlog, "地址%d读取读取故障数据失败，结果=%d",addr,ret);
	}

	//读取汇流箱故障信息
	start = 0x014A;
	length = 1;
	ret = rtu.ReadOutputRegisters(values,addr,start,length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		if((values[0]&0x0001)>0)
		{
			data = data|0x00000001;//过压报警
		}
		if((values[0]&0x0002)>0)
		{
			data = data|0x00000002;//高温报警
		}
		if((values[0]&0x0004)>0)
		{
			data = data|0x00000004;//低温报警
		}
		if((values[0]&0x0008)>0)
		{
			data = data|0x00000008;//模拟量上限报警
		}
		if((values[0]&0x0010)>0)
		{
			data = data|0x00000010;//模拟量下限报警
		}
		if((values[0]&0x0100)>0)
		{
			data = data|0x00000020;//电流偏离平均值报警
		}
		if((values[0]&0x0200)>0)
		{
			data = data|0x00000040;//负电流报警
		}
		if((values[0]&0x8000)>0)
		{
			data = data|0x00000080;//有报警
		}
		if((values[0]&0x0020) == 1)
		{
			MemDb::SetRealData(regStart+2,1,false);//防雷器故障
		}
		else if((values[0]&0x0020) == 0)
		{
			MemDb::SetRealData(regStart+2,0,false);//防雷器无故障
		}
		MemDb::SetRealData(regStart+1,data,false);//故障状态
	}
	else
	{
		zlog_warn(Util::m_zlog, "地址%d读取读取故障数据失败，结果=%d",addr,ret);
	}

	MemDb::SetRealData(regStart,comm,false);//通讯状态
	return ret;
}
