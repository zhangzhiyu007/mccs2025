/*
 * SunGrowPcs.cpp
 *
 *  Created on: 2013-10-15
 *      Author: liuhaitao
 */

#include "SunGrowPcs.h"

#include "../../com/Com.h"

SunGrowPcs::SunGrowPcs()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

SunGrowPcs::~SunGrowPcs()
{
	// TODO Auto-generated destructor stub
}

void SunGrowPcs::SetCom(Com* com)
{
	m_com = com;
}

int SunGrowPcs::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 22:
		ret = this->Sc50Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SunGrowPcs::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 22:
		ret = this->Sc50Preset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SunGrowPcs::Sc50Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取合肥阳光PCS数据开始");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 10050;
	int length = 8;
	float tmp = 0;
	int tmp1 = 0;
	int data = 0;
	int data1 = 0;

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

	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
//		zlog_info(Util::m_zlog,"输出类型：%d",values[1]);
		MemDb::SetRealData(regStart+28,values[1],false);//输出类型
		tmp = ((short) values[6])*0.1;
//		zlog_info(Util::m_zlog,"直流电压%0.2f",tmp);
		if(tmp < 10)
		{
			tmp = 0;//防止精度不准确
		}
		MemDb::SetRealData(regStart+35,tmp,false);//直流电压
		tmp = ((short) values[7])*0.1;
//		zlog_info(Util::m_zlog,"直流电流%0.2f",tmp);
		MemDb::SetRealData(regStart+36,tmp,false);//直流电流
	}

	start = 10062;
	length = 6;
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = ((short) values[0])*0.1;
//		zlog_info(Util::m_zlog,"A相电压%0.2f",tmp);
		MemDb::SetRealData(regStart+38,tmp,false);//A相电压
		tmp = ((short) values[1])*0.1;
//		zlog_info(Util::m_zlog,"B相电压%0.2f",tmp);
		MemDb::SetRealData(regStart+39,tmp,false);//B相电压
		tmp = ((short) values[2])*0.1;
//		zlog_info(Util::m_zlog,"C相电压%0.2f",tmp);
		MemDb::SetRealData(regStart+40,tmp,false);//C相电压
		tmp = ((short) values[3])*0.1;
//		zlog_info(Util::m_zlog,"A相电流%0.2f",tmp);
		MemDb::SetRealData(regStart+41,tmp,false);//A相电流
		tmp = ((short) values[4])*0.1;
//		zlog_info(Util::m_zlog,"B相电流%0.2f",tmp);
		MemDb::SetRealData(regStart+42,tmp,false);//B相电流
		tmp = ((short) values[5])*0.1;
//		zlog_info(Util::m_zlog,"C相电流%0.2f",tmp);
		MemDb::SetRealData(regStart+43,tmp,false);//C相电流
	}

	start = 10069;
	length = 2;
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = ((short) values[0])*0.01;
//		zlog_info(Util::m_zlog,"电网频率%0.2f",tmp);
		MemDb::SetRealData(regStart+49,tmp,false);//电网频率
		tmp = ((short) values[1])*0.1;
//		zlog_info(Util::m_zlog,"逆变器效率%0.2f",tmp);
		MemDb::SetRealData(regStart+50,tmp,false);//逆变器效率
	}

	start = 10189;
	length = 1;
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = ((short) values[0]) * 0.1;
//		zlog_info(Util::m_zlog,"机内温度%0.2f",tmp);
		MemDb::SetRealData(regStart+34,tmp,false);//机内温度
	}

	//功率因数
	start = 10201;
	length = 1;
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = ((short) values[0]) * 0.001;
		MemDb::SetRealData(regStart+51,tmp,false);//功率因数
	}

	start = 10302;
	length = 20;
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = ((((short)values[1])&0xFFFF)<<16)|(((short)values[0])&0xFFFF);
//		zlog_info(Util::m_zlog,"累计输入电能%d",data);
		MemDb::SetRealData(regStart+32,(float)data,false);//累计输入电能
		data = ((((short)values[3])&0xFFFF)<<16)|(((short)values[2])&0xFFFF);
//		zlog_info(Util::m_zlog,"累计输出电能%d",data);
		MemDb::SetRealData(regStart+33,(float)data,false);//累计输出电能
		data = ((((short)values[9])&0xFFFF)<<16)|(((short)values[8])&0xFFFF);
//		zlog_info(Util::m_zlog,"总直流功率%d",data);
		MemDb::SetRealData(regStart+37,(float)data,false);//总直流功率
		data = ((((short)values[11])&0xFFFF)<<16)|(((short)values[10])&0xFFFF);
//		zlog_error(Util::m_zlog,"values[11] = %02x,values[10] = %02x,A相有功功率%d",values[11],values[10],data);
		MemDb::SetRealData(regStart+44,(float)data,false);//A相有功功率
		data = ((((short)values[13])&0xFFFF)<<16)|(((short)values[12])&0xFFFF);
//		zlog_info(Util::m_zlog,"B相有功功率%d",data);
		MemDb::SetRealData(regStart+45,(float)data,false);//B相有功功率
		data = ((((short)values[15])&0xFFFF)<<16)|(((short)values[14])&0xFFFF);
//		zlog_info(Util::m_zlog,"C相有功功率%d",data);
		MemDb::SetRealData(regStart+46,(float)data,false);//C相有功功率
	}

	start = 10530;
	length = 4;
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = ((((short)values[1])&0xFFFF)<<16)|(((short)values[0])&0xFFFF);
//		zlog_info(Util::m_zlog,"总有功功率%d",data);
		MemDb::SetRealData(regStart+47,(float)data,false);//总有功功率
		data = ((((short)values[3])&0xFFFF)<<16)|(((short)values[2])&0xFFFF);
//		zlog_info(Util::m_zlog,"总无功功率%d",data);
		MemDb::SetRealData(regStart+48,(float)data,false);//总无功功率
	}

	start = 10736;
	length = 2;
//	zlog_info(Util::m_zlog,"读取设备状态");
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = ((((short)values[1])&0xFFFF)<<16)|(((short)values[0])&0xFFFF);
//		zlog_info(Util::m_zlog,"设备状态%d",data);
		if((data&0x00000001)>0)//工作状态
		{
			data1 = 0;//初始待机
		}
		if((data&0x00000002)>0)
		{
			data1 = 1;//紧急停机
		}
		if((data&0x00000004)>0)
		{
			data1 = 2;//待机
		}
		if((data&0x00020000)>0)
		{
			data1 = 3;//停机
		}
		if((data&0x00030007) == 0)
		{
			data1 = 4;//没有以上几个状态，认为是在有工作模式下运行，工作状态为运行
		}
		if((data&0x00010000) >0)
		{
			data1 = 5;//故障
		}
		MemDb::SetRealData(regStart+26,data1,false);//工作模式

		if((data&0x00000008)>0)
		{
			data1 = 1;//恒流充电
		}
		else if((data&0x00000010)>0)
		{
			data1 = 2;//恒压充电
		}
		else if((data&0x00000020)>0)
		{
			data1 = 3;//恒功率充电
		}
		else if((data&0x00000100)>0)
		{
			data1 = 4;//恒流放电
		}
		else if((data&0x00000200)>0)
		{
			data1 = 5;//恒功放电
		}
		else if((data&0x00000400)>0)
		{
			data1 = 6;//恒功率放电
		}
		else if((data&0x00002000)>0)
		{
			data1 = 7;//独立逆变
		}
		else if((data&0x00004000)>0)
		{
			data1 = 8;//计时充电
		}
		else if((data&0x00008000)>0)
		{
			data1 = 9;//计时放电
		}
		else if((data&0x0000E738) == 0)
		{
			data1 = 0;//没有以上工作模式，认为是在初始待机、待机、紧急停机、停机状态下运行
		}
		MemDb::SetRealData(regStart+27,data1,false);//工作模式
	}

	start = 10740;
	length = 4;
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		data = ((((short)values[1])&0xFFFF)<<16)|(((short)values[0])&0xFFFF);
//		zlog_info(Util::m_zlog,"values[1] = %02x,values[0] = %02x",values[1],values[0]);
//		zlog_info(Util::m_zlog,"data = %02x",data);
		data1 = ((((short)values[3])&0xFFFF)<<16)|(((short)values[2])&0xFFFF);
//		zlog_info(Util::m_zlog,"values[3] = %02x,values[2] = %02x",values[3],values[2]);
//		zlog_info(Util::m_zlog,"data1 = %02x",data1);
		if((data&0x00000001)>0)//直流过压
		{
			tmp1 = tmp1|0x00001000;
		}
		if((data&0x00000002)>0)//直流欠压
		{
			tmp1 = tmp1|0x00002000;
		}
		if((data&0x00000004)>0)//直流过流
		{
			tmp1 = tmp1|0x00008000;
		}
		if((data&0x00000008)>0)//交流过压
		{
			tmp1 = tmp1|0x00100000;
		}
		if((data&0x00000010)>0)//交流欠压
		{
			tmp1 = tmp1|0x00200000;
		}
		if((data&0x00000020)>0)//电网过频
		{
			tmp1 = tmp1|0x00040000;
		}
		if((data&0x00000040)>0)//电网欠频
		{
			tmp1 = tmp1|0x00080000;
		}
		if((data&0x00000080)>0)//交流过流
		{
			tmp1 = tmp1|0x00400000;
		}
//		if((data&0x00000100)>1)//交流漏电保护
//		{
//
//		}
		if((data&0x00000200)>0)//孤岛保护
		{
			tmp1 = tmp1|0x00000400;
		}
		if((data&0x00000400)>0)//相序保护
		{
			tmp1 = tmp1|0x04000000;
		}
//		if((data&0x00000800)>1)//独立逆变保护
//		{
//
//		}
//		if((data&0x00001000)>1)//交流电压不平衡
//		{
//
//		}
		if((data&0x00002000)>0)//交流电流 不平衡
		{
			tmp1 = tmp1|0x00800000;
		}
		if(((data&0x00004000)>0)||((data&0x00008000)>0)
				||((data&0x00010000)>0)||((data&0x00020000)>0)
				||((data&0x00040000)>0)||((data&0x00080000)>0)
				||((data&0x00100000)>0)||((data1&0x00008000)>0)
				||((data1&0x00010000)>0)||((data1&0x00020000)>0))//过温保护
		{
			tmp1 = tmp1|0x20000000;
		}
		if(((data&0x01000000)>0)||((data&0x02000000)>0))//接触器故障
		{
			tmp1 = tmp1|0x00000080;
		}
		if(((data&0x04000000)>0)||((data&0x08000000)>0)
				||((data&0x10000000)>0)||((data&0x20000000)>0)
				||((data1&0x00000001)>0)||((data1&0x00000002)>0)
				||((data1&0x00000004)>0)||((data1&0x00000008)>0)
				||((data1&0x00000010)>0)||((data1&0x00000020)>0))//采样通道异常
		{
			tmp1 = tmp1|0x00000010;
		}
		if(((data1&0x00000400)>0)||((data1&0x00001000)>0))//通讯故障
		{
			tmp1 = tmp1|0x00000004;
		}

		if(tmp1>0)//故障
		{
			MemDb::SetRealData(regStart+29,1,false);//故障
			MemDb::SetRealData(regStart+30,tmp1,false);//故障码1
			MemDb::SetRealData(regStart+31,0,false);//故障码2
		}
		else
		{
			MemDb::SetRealData(regStart+29,0,false);//无故障
			MemDb::SetRealData(regStart+30,tmp1,false);//故障码1
			MemDb::SetRealData(regStart+31,0,false);//故障码2
		}
	}

	//读取设置反馈值
	start = 10055;
	length = 10;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		if((values[0]&0x0002)>0)
		{
			data = 0;
			MemDb::SetRealData(regStart+1,data,false);//停机
		}
		else if((values[0]&0x0008)>0)
		{
			data = 1;//恒流充电
			MemDb::SetRealData(regStart+3,data,false);
		}
		else if((values[0]&0x0010)>0)
		{
			data = 3;//恒压充电
			MemDb::SetRealData(regStart+3,data,false);
		}
		else if((values[0]&0x0020)>0)
		{
			data = 5;//恒功率充电
			MemDb::SetRealData(regStart+3,data,false);
		}
		else if((values[0]&0x0100)>0)
		{
			data = 2;//恒流放电
			MemDb::SetRealData(regStart+3,data,false);
		}
		else if((values[0]&0x0200)>0)
		{
			data = 4;//恒压放电
			MemDb::SetRealData(regStart+3,data,false);
		}
		else if((values[0]&0x0400)>0)
		{
			data = 6;//恒功率放电
			MemDb::SetRealData(regStart+3,data,false);
		}
		else if((values[0]&0x2000)>0)
		{
			data = 7;//独立逆变
			MemDb::SetRealData(regStart+3,data,false);
		}
		else if((values[0]&0x4000)>0)
		{
			data = 8;//计时充电
			MemDb::SetRealData(regStart+3,data,false);
		}
		else if((values[0]&0x8000)>0)
		{
			data = 9;//计时放电
			MemDb::SetRealData(regStart+3,data,false);
		}

		if(values[4] == 0x55)//关闭
		{
			MemDb::SetRealData(regStart+25,0,false);//无功调节选择开关
		}
		else if(values[4] == 0xA1)
		{
			MemDb::SetRealData(regStart+25,1,false);//无功调节选择开关
		}

		data = (short)values[7];
		MemDb::SetRealData(regStart+4,data,false);//计时充放电时间1使能开关
		data = (short)values[8];
//		zlog_error(Util::m_zlog,"values[8] = %d",values[8]);
		MemDb::SetRealData(regStart+5,data,false);//计时充放电时间2使能开关
		data = (short)values[9];
//		zlog_error(Util::m_zlog,"values[9] = %d",values[9]);
		MemDb::SetRealData(regStart+6,data,false);//计时充放电时间3使能开关
	}

	start = 10090;
	length = 40;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::GetRealData(data,regStart+3);
		if(data == 1)//恒流充电
		{
			tmp = ((short) values[0])*0.1;
			MemDb::SetRealData(regStart+7,tmp,false);//恒流充电电流
		}
		else if(data == 2)//恒流放电
		{
			tmp = ((short) values[4])*0.1;
			MemDb::SetRealData(regStart+7,tmp,false);//恒流放电电流
		}
		else if(data == 3)//恒压充电
		{
			tmp = ((short) values[1])*0.1;
			MemDb::SetRealData(regStart+8,tmp,false);//恒压充电电压
			tmp = ((short) values[2])*0.1;
			MemDb::SetRealData(regStart+9,tmp,false);//恒压充电限制电流
		}
		else if(data == 4)//恒压放电
		{
			tmp = ((short) values[5])*0.1;
			MemDb::SetRealData(regStart+8,tmp,false);//恒压放电电压
			tmp = ((short) values[6])*0.1;
			MemDb::SetRealData(regStart+9,tmp,false);//恒压放电限制电流
		}
		else if(data == 5)//恒功率充电
		{
			tmp = ((short) values[3])*0.1 * 1000;
			MemDb::SetRealData(regStart+10,tmp,false);//恒功率充电功率
		}
		else if(data == 6)//恒功率放电
		{
			tmp = ((short) values[7])*0.1 * 1000;
			MemDb::SetRealData(regStart+10,tmp,false);//恒功率放电功率
		}
		else if(data == 7)//独立逆变
		{
			tmp = ((short) values[8])*0.1;
			MemDb::SetRealData(regStart+11,tmp,false);//独立逆变电压
			tmp = ((short) values[9])*0.01;
			MemDb::SetRealData(regStart+12,tmp,false);//独立逆变频率
		}
		else if(data == 8)//计时充电
		{
			tmp = ((short) values[11])*0.1;
			MemDb::SetRealData(regStart+13,tmp,false);//计时充电电压
			tmp = ((short) values[12])*0.1;
			MemDb::SetRealData(regStart+14,tmp,false);//计时充电电流
			MemDb::GetRealData(data,regStart+4);
			if(data == 1)
			{
				MemDb::SetRealData(regStart+15,((short) values[16]),false);//计时充电起始时间1（小时）
				MemDb::SetRealData(regStart+16,((short) values[17]),false);//计时充电起始时间1（分钟）
				MemDb::SetRealData(regStart+17,((short) values[18]),false);//计时充电结束时间1（小时）
				MemDb::SetRealData(regStart+18,((short) values[19]),false);//计时充电结束时间1（分钟）
			}
			MemDb::GetRealData(data,regStart+5);
			if(data == 1)
			{
				MemDb::SetRealData(regStart+15,((short) values[20]),false);//计时充电起始时间2（小时）
				MemDb::SetRealData(regStart+16,((short) values[21]),false);//计时充电起始时间2（分钟）
				MemDb::SetRealData(regStart+17,((short) values[22]),false);//计时充电结束时间2（小时）
				MemDb::SetRealData(regStart+18,((short) values[23]),false);//计时充电结束时间2（分钟）
			}
			MemDb::GetRealData(data,regStart+6);
			if(data == 1)
			{
				MemDb::SetRealData(regStart+15,((short) values[32]),false);//计时充电起始时间3（小时）
				MemDb::SetRealData(regStart+16,((short) values[33]),false);//计时充电起始时间3（分钟）
				MemDb::SetRealData(regStart+17,((short) values[34]),false);//计时充电结束时间3（小时）
				MemDb::SetRealData(regStart+18,((short) values[35]),false);//计时充电结束时间3（分钟）
			}
		}
		else if(data == 9)//计时放电
		{
			tmp = ((short) values[13])*0.1;
			MemDb::SetRealData(regStart+13,tmp,false);//计时放电电压
			tmp = ((short) values[14])*0.1;
			MemDb::SetRealData(regStart+14,tmp,false);//计时放电电流
			MemDb::GetRealData(data,regStart+4);
			if(data == 1)
			{
				MemDb::SetRealData(regStart+19,((short) values[20]),false);//计时放电起始时间1（小时）
				MemDb::SetRealData(regStart+20,((short) values[21]),false);//计时放电起始时间1（分钟）
				MemDb::SetRealData(regStart+21,((short) values[22]),false);//计时放电结束时间1（小时）
				MemDb::SetRealData(regStart+22,((short) values[23]),false);//计时放电结束时间1（分钟）
			}
			MemDb::GetRealData(data,regStart+5);
			if(data == 1)
			{
				MemDb::SetRealData(regStart+19,((short) values[28]),false);//计时放电起始时间2（小时）
				MemDb::SetRealData(regStart+20,((short) values[29]),false);//计时放电起始时间2（分钟）
				MemDb::SetRealData(regStart+21,((short) values[30]),false);//计时放电结束时间2（小时）
				MemDb::SetRealData(regStart+22,((short) values[31]),false);//计时放电结束时间2（分钟）
			}
			MemDb::GetRealData(data,regStart+6);
			if(data == 1)
			{
				MemDb::SetRealData(regStart+19,((short) values[36]),false);//计时放电起始时间3（小时）
				MemDb::SetRealData(regStart+20,((short) values[37]),false);//计时放电起始时间3（分钟）
				MemDb::SetRealData(regStart+21,((short) values[38]),false);//计时放电结束时间3（小时）
				MemDb::SetRealData(regStart+22,((short) values[39]),false);//计时放电结束时间3（分钟）
			}
		}
	}

	start = 10172;
	length = 2;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = ((short)values[0])*0.001;//功率因数
		MemDb::SetRealData(regStart+23,tmp,false);
		tmp = ((short)values[1])*0.1;//无功比例
		MemDb::SetRealData(regStart+24,tmp,false);
	}

	MemDb::SetRealData(regStart,comm,false);//通讯状态
	return ret;
}

int SunGrowPcs::Sc50Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	int regStart = dev->regStart;
	int tmp = 0;

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	//判断是否有无功调节选择开关设置
	if (MemDb::GetRealDataWrited(realData, regStart + 25, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			if(realData.pv.data.iValue == 0)//关闭，功率因数恢复1，无功比例恢复0
			{
				tmp = 0x55;
			}
			else if(realData.pv.data.iValue == 1)//功率因数设置有效，无功比例恢复0
			{
				tmp = 0xA1;
			}
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_info(Util::m_zlog,"无功调节选择开关命令错误");
			ret = ErrorInfo::ERR_WRITE;
		}
		ret = rtu.PresetSingleRegister(addr,10059,tmp);
	}

	//判断是否有运行设置
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		tmp = 0;
		if (realData.pv.type == RealData::INT)
		{
			if(realData.pv.data.iValue == 0)
			{
				zlog_error(Util::m_zlog,"运行设置停止tmp = %02x",tmp);
				tmp = tmp|0x0002;
				ret = rtu.PresetSingleRegister(addr,10055,tmp);
			}
//			else if(realData.pv.data.iValue == 1)//启动
//			{
//
//			}
			else if(realData.pv.data.iValue == 2)//待机
			{
				tmp = tmp|0x0004;
				zlog_error(Util::m_zlog,"运行设置启动tmp = %02x",tmp);
				ret = rtu.PresetSingleRegister(addr,10055,tmp);
			}
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_info(Util::m_zlog,"运行设置命令错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否功率因数设置命令
	if (MemDb::GetRealDataWrited(realData, regStart + 23, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = rtu.PresetSingleRegister(addr,10172,realData.pv.data.iValue*1000);
		}
		else if(realData.pv.type == RealData::DB)
		{
			ret = rtu.PresetSingleRegister(addr,10172,realData.pv.data.dbValue*1000);
		}
	}

	//判断是否无功比例设置命令
	if (MemDb::GetRealDataWrited(realData, regStart + 24, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = rtu.PresetSingleRegister(addr,10173,realData.pv.data.dbValue*10);
		}
		else if(realData.pv.type == RealData::DB)
		{
			ret = rtu.PresetSingleRegister(addr,10173,realData.pv.data.dbValue*10);
		}
	}

	MemDb::GetRealData(realData, regStart + 3);
	if (realData.pv.type == RealData::INT)
	{
		if (realData.pv.data.iValue == 1)//恒流充电
		{
			zlog_debug(Util::m_zlog,"恒流充电");
			ret = PresetState(regStart + 7, 10090, 0x0008, addr, 1);
		}
		else if (realData.pv.data.iValue == 2)//恒流放电
		{
			zlog_debug(Util::m_zlog,"恒流放电");
			ret = PresetState(regStart + 7, 10094, 0x0100, addr, 2);
		}
		else if (realData.pv.data.iValue == 3)//恒压充电
		{
			zlog_debug(Util::m_zlog,"恒压充电");
			ret = PresetState(regStart + 8, 10091, 0x0010, addr, 3);
		}
		else if (realData.pv.data.iValue == 4)//恒压放电
		{
			zlog_debug(Util::m_zlog,"恒压放电");
			ret = PresetState(regStart + 8, 10095, 0x0200, addr, 4);
		}
		else if (realData.pv.data.iValue == 5)//恒功率充电
		{
			zlog_debug(Util::m_zlog,"恒功率充电");
			ret = PresetState(regStart + 10, 10093, 0x0020, addr, 5);
		}
		else if (realData.pv.data.iValue == 6)//恒功率放电
		{
			zlog_debug(Util::m_zlog,"恒功率放电");
			ret = PresetState(regStart + 10, 10097, 0x0400, addr, 6);
		}
		else if (realData.pv.data.iValue == 7)//独立逆变
		{
			zlog_debug(Util::m_zlog,"独立逆变");
			ret = PresetState(regStart + 11, 10098, 0x2000, addr, 7);
		}
		else if (realData.pv.data.iValue == 8)//计时充电
		{
			zlog_debug(Util::m_zlog,"计时充电");
			ret = PresetState(regStart + 13, 10101, 0x4000, addr, 8);
		}
		else if (realData.pv.data.iValue == 9)//计时放电
		{
			zlog_debug(Util::m_zlog,"计时放电");
			ret = PresetState(regStart + 13, 10103, 0x8000, addr, 9);
		}
	}
	else if (realData.pv.type == RealData::DB)
	{
		zlog_error(Util::m_zlog,"工作模式控制命令错误");
		ret = ErrorInfo::ERR_WRITE;
	}

	//判断是否有计时充放电时间1使能开关
	if (MemDb::GetRealDataWrited(realData, regStart + 4, true))
	{
		if(realData.pv.type == RealData::INT)
		{
			ret = rtu.PresetSingleRegister(addr,10062,realData.pv.data.iValue);//充放电时间1使能开关
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_error(Util::m_zlog,"充放电时间1使能开关命令错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}
	return ret;
}

int SunGrowPcs::PresetState(int start,int regStart,int data,int addr,int value)
{
	int ret = ErrorInfo::ERR_OK;
	short tmp = 0;
	RealData realData;
	RealData realData1;
//	RealData realData2;
//	RealData realData3;
//	RealData realData4;
//	RealData realData5;
//	RealData realData6;

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	//恒流充电、恒流放电、
	if ((value == 1) || (value == 2))
	{
		tmp = 0;
		if (MemDb::GetRealDataWrited(realData, start, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, regStart,
						realData.pv.data.iValue*10);
				tmp = tmp | data;
				ret = rtu.PresetSingleRegister(addr, 10055, tmp);
			}
			else if (realData.pv.type == RealData::DB)
			{
				ret = rtu.PresetSingleRegister(addr, regStart,
						realData.pv.data.dbValue*10);
				tmp = tmp | data;
				ret = rtu.PresetSingleRegister(addr, 10055, tmp);
			}
		}
	}
	//恒功率充电、恒功率放电
	else if((value == 5) || (value == 6))
	{
		tmp = 0;
		if (MemDb::GetRealDataWrited(realData, start, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				zlog_error(Util::m_zlog,"设置功率 = %d",realData.pv.data.iValue);
				ret = rtu.PresetSingleRegister(addr, regStart,
						realData.pv.data.iValue * 0.01);
				tmp = tmp | data;
//				zlog_error(Util::m_zlog,"tmp = %02x",tmp);
				ret = rtu.PresetSingleRegister(addr, 10055, tmp);
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"设置功率 = %.2f",realData.pv.data.dbValue);
				ret = rtu.PresetSingleRegister(addr, regStart,
						realData.pv.data.dbValue * 0.01);
				tmp = tmp | data;
//				zlog_error(Util::m_zlog,"tmp = %02x",tmp);
				ret = rtu.PresetSingleRegister(addr, 10055, tmp);
			}
		}
	}
	//恒压充电、恒压放电、独立逆变
	else if ((value == 3) || (value == 4) || (value == 7))
	{
//		zlog_error(Util::m_zlog,"start = %d",start);
		if (MemDb::GetRealDataWrited(realData, start, true))//充放电电压
		{
			zlog_error(Util::m_zlog,"充放电电压");
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, regStart,
						realData.pv.data.iValue * 10);
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"设置电压 = %.2f",realData.pv.data.dbValue);
				ret = rtu.PresetSingleRegister(addr, regStart,
						realData.pv.data.dbValue * 10);
			}
		}

		if (MemDb::GetRealDataWrited(realData1, start + 1, true))
		{
			if (realData1.pv.type == RealData::INT)
			{
				if (value == 7)
				{
					ret = rtu.PresetSingleRegister(addr, regStart + 1,
							realData1.pv.data.iValue * 100);
				}
				else
				{
					ret = rtu.PresetSingleRegister(addr, regStart + 1,
							realData1.pv.data.iValue * 10);
				}
			}
			else if (realData1.pv.type == RealData::DB)
			{
				if (value == 7)
				{
					zlog_error(Util::m_zlog,"设置频率 = %.2f",realData1.pv.data.dbValue);
					ret = rtu.PresetSingleRegister(addr, regStart + 1,
							realData1.pv.data.dbValue * 100);
				}
				else
				{
					zlog_error(Util::m_zlog,"设置电流 = %.2f",realData1.pv.data.dbValue);
					ret = rtu.PresetSingleRegister(addr, regStart + 1,
							realData1.pv.data.dbValue * 10);
				}
			}
		}

		if(value == 7)
		{
			if (MemDb::GetRealDataWrited(realData, start - 8, true))
			{
				tmp = tmp | data;
				ret = rtu.PresetSingleRegister(addr, 10055, tmp);
			}
		}
		else
		{
			if (MemDb::GetRealDataWrited(realData, start - 5, true))
			{
				tmp = tmp | data;
				ret = rtu.PresetSingleRegister(addr, 10055, tmp);
			}
		}

	}
	//计时充电、计时放电
	else if ((value == 8) || (value == 9))
	{
		if (MemDb::GetRealDataWrited(realData, start, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, regStart,
						realData.pv.data.iValue * 10);//计时充电电压
			}
			else if (realData.pv.type == RealData::DB)
			{
				ret = rtu.PresetSingleRegister(addr, regStart,
						realData.pv.data.dbValue * 10);
			}
		}

		if (MemDb::GetRealDataWrited(realData, start + 1, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, regStart + 1,
						realData.pv.data.iValue * 10);//计时充电电流
			}
			else if (realData.pv.type == RealData::DB)
			{
				ret = rtu.PresetSingleRegister(addr, regStart + 1,
						realData.pv.data.dbValue * 10);
			}
		}

		if (MemDb::GetRealDataWrited(realData, start - 10, true))
		{
			tmp = tmp | data;
			ret = rtu.PresetSingleRegister(addr, 10055, tmp);
		}
	}

	if (value == 8)//计时充电
	{
		if (MemDb::GetRealDataWrited(realData, start + 2, true))//计时充电起始时间（小时）
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, 10106,
						realData.pv.data.iValue);//计时充电起始时间1（小时）
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"计时充电起始时间1（小时）命令错误");
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		if (MemDb::GetRealDataWrited(realData, start + 3, true))//计时充电起始时间（分钟）
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, 10107,
						realData.pv.data.iValue);//计时充电起始时间（分钟）
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"计时充电起始时间1（分钟）命令错误");
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		if (MemDb::GetRealDataWrited(realData, start + 4, true))//计时充电结束时间（小时）
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, 10108,
						realData.pv.data.iValue);//计时充电结束时间（小时）
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"计时充电结束时间（小时）命令错误");
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		if (MemDb::GetRealDataWrited(realData, start + 5, true))//计时充电结束时间（分钟）
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, 10109,
						realData.pv.data.iValue);//计时充电结束时间（分钟）
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"//计时充电结束时间（分钟）命令错误");
				ret = ErrorInfo::ERR_WRITE;
			}
		}
	}
	else if (value == 9)//计时放电
	{
		if (MemDb::GetRealDataWrited(realData, start + 6, true))//计时放电起始时间（小时）
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, 10110,
						realData.pv.data.iValue);//计时放电起始时间（小时）
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"计时放电起始时间（小时）命令错误");
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		if (MemDb::GetRealDataWrited(realData, start + 7, true))//计时放电起始时间（分钟）
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, 10111,
						realData.pv.data.iValue);//计时放电起始时间（分钟）
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"计时放电起始时间（分钟）命令错误");
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		if (MemDb::GetRealDataWrited(realData, start + 8, true))//计时放电结束时间1（小时）
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, 10112,
						realData.pv.data.iValue);//计时放电结束时间1（小时）
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"计时放电结束时间1（小时）命令错误");
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		if (MemDb::GetRealDataWrited(realData, start + 9, true))//计时放电结束时间（分钟）
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(addr, 10113,
						realData.pv.data.iValue);//计时放电结束时间（分钟）
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_error(Util::m_zlog,"//计时放电结束时间（分钟）命令错误");
				ret = ErrorInfo::ERR_WRITE;
			}
		}
	}
	return ret;
}
