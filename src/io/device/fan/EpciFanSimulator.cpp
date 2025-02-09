/*
 * EpciFanSimulator.cpp
 *
 *  Created on: 2014-2-23
 *      Author: liuhaitao
 */

#include "EpciFanSimulator.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

EpciFanSimulator::EpciFanSimulator()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

EpciFanSimulator::~EpciFanSimulator()
{
	// TODO Auto-generated destructor stub
}

void EpciFanSimulator::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void EpciFanSimulator::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int EpciFanSimulator::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HN_Wind_05S:
		ret = this->Wind10SRead(dev);//10KW双馈风机模拟器
		break;
	default:
		break;
	};

	return ret;
}

int EpciFanSimulator::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HN_Wind_05S:
		ret = this->Wind10SPreset(dev);//10KW双馈风机模拟器
		break;
	default:
		break;
	};

	return ret;
}

int EpciFanSimulator::Wes6930Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取中电建投风机模拟器WES-6930数据开始");
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 50;
	int length = 87;
	int data = 0;
	float tmp = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断网络是否打开
	if (!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcp);

	//读取实时反馈信息
	ret = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		data = (short)values[0];
		MemDb::SetRealData(regStart + 0,data,false);//通讯状态
		data = (short)values[1];
		MemDb::SetRealData(regStart + 33,data,false);//运行状态
		data = (short)values[3];
		MemDb::SetRealData(regStart + 34,data,false);//当前执行方式
		data = (short)values[6];
		MemDb::SetRealData(regStart + 35,data,false);//故障状态
		data = (short)values[12];
		MemDb::SetRealData(regStart + 36,data,false);//变频器报警状态1
		data = (short)values[13];
		MemDb::SetRealData(regStart + 37,data,false);//变频器报警状态2
		data = (short)values[14];
		MemDb::SetRealData(regStart + 38,data,false);//变频器报警状态3
		data = (short)values[15];
		MemDb::SetRealData(regStart + 39,data,false);//变频器报警状态4
		data = (short)values[17];
		MemDb::SetRealData(regStart + 40,data,false);//逆变器故障1
		data = (short)values[18];
		MemDb::SetRealData(regStart + 41,data,false);//逆变器故障2
		data = (short)values[22];
		MemDb::SetRealData(regStart + 42,data,false);//系统报警状态1
		tmp = (short)values[30] * 0.01;
		MemDb::SetRealData(regStart + 43,tmp,false);//电网频率
		tmp = (short)values[31] * 0.01;
		MemDb::SetRealData(regStart + 44,tmp,false);//发电频率
		data = (short)values[32];
		MemDb::SetRealData(regStart + 45,data,false);//实时转速
		tmp = (short)values[33] * 0.01;
		MemDb::SetRealData(regStart + 46,tmp,false);//输出扭矩
		tmp = (short)values[4] * 0.01;
		MemDb::SetRealData(regStart + 47,tmp,false);//当前风速
//		tmp = (short)values[34] * 0.1;
//		MemDb::SetRealData(regStart + 47,tmp,false);//当前启动时间
//		tmp = (short)values[35] * 0.1;
//		MemDb::SetRealData(regStart + 48,tmp,false);//当前停止时间
		tmp = (short)values[39] * 0.1;
		MemDb::SetRealData(regStart + 49,tmp,false);//AB线电压模拟器输入电压
		tmp = (short)values[42] * 0.1;
		MemDb::SetRealData(regStart + 50,tmp,false);//模拟器A相输入电流
		data = (short)values[48];
		MemDb::SetRealData(regStart + 51,(float)data,false);//模拟器输入总有功功率
		data = (short)values[52];
		MemDb::SetRealData(regStart + 52,(float)data,false);//模拟器输入总无功功率
		tmp = (short)values[53] * 0.001;
		MemDb::SetRealData(regStart + 53,tmp,false);//市电平均功率因数
//		tmp = (short)values[57] * 0.1;
//		MemDb::SetRealData(regStart + 54,tmp,false);//模拟器输出机侧AB线电压
//		tmp = (short)values[60] * 0.1;
//		MemDb::SetRealData(regStart + 55,tmp,false);//模拟器输出机侧A相电流
//		data = (short)values[66];
//		MemDb::SetRealData(regStart + 56,(float)data,false);//模拟器输出机侧总有功功率
//		data = (short)values[70];
//		MemDb::SetRealData(regStart + 57,(float)data,false);//模拟器输出机侧总无功功率
//		tmp = (short)values[71] * 0.001;
//		MemDb::SetRealData(regStart + 58,tmp,false);//发电平均功率因数
		tmp = (short)values[75] * 0.1;
		MemDb::SetRealData(regStart + 59,tmp,false);//模拟器输出网侧AB线电压
		tmp = (short)values[78] * 0.1;
		MemDb::SetRealData(regStart + 60,tmp,false);//模拟器输出网侧A相电流
		data = (short)values[81];
		MemDb::SetRealData(regStart + 61,(float)data,false);//模拟器输出网侧总有功功率
		data = (short)values[82];
		MemDb::SetRealData(regStart + 62,(float)(data/10.0),false);//日发电量
		data = ((((short)values[83])&0xFFFF)<<16)|(((short)values[84])&0xFFFF);
		MemDb::SetRealData(regStart + 63,(float)(data/10.0),false);//月发电量
		data = ((((short)values[85])&0xFFFF)<<16)|(((short)values[86])&0xFFFF);
		MemDb::SetRealData(regStart + 64,(float)(data/10.0),false);//总发电量
	}

	//读取设置反馈值
	start = 0;
	length = 43;
	ret = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		data = (short)values[0];
		MemDb::SetRealData(regStart + 1,data,false);//启动停止
		tmp = (short)values[2] * 0.1;
//		MemDb::SetRealData(regStart + 2,tmp,false);//启动时间
//		tmp = (short)values[3] * 0.1;
//		MemDb::SetRealData(regStart + 3,tmp,false);//停止时间
		tmp = (short)values[5] * 0.01;
		MemDb::SetRealData(regStart + 4,tmp,false);//切入风速
		tmp = (short)values[6] * 0.01;
		MemDb::SetRealData(regStart + 5,tmp,false);//切出风速
		tmp = (short)values[7] * 0.01;
		MemDb::SetRealData(regStart + 6,tmp,false);//额定风速
		tmp = (short)values[8] * 0.01;
		MemDb::SetRealData(regStart + 7,tmp,false);//叶片半径
		tmp = (short)values[9] * 0.001;
		MemDb::SetRealData(regStart + 8,tmp,false);//空气密度
		tmp = (short)values[10] * 0.001;
		MemDb::SetRealData(regStart + 9,tmp,false);//风能利用率
		data = (short)values[11];
		MemDb::SetRealData(regStart + 3,data,false);//风机类型
		data = (short)values[12];
		MemDb::SetRealData(regStart + 2,data,false);//叶片数
		data = (short)values[20];
		MemDb::SetRealData(regStart + 10,data,false);//功率输出方式
		tmp = (short)values[21] * 0.01;
		MemDb::SetRealData(regStart + 11,tmp,false);//风速设定值
		data = (short)values[22];
		MemDb::SetRealData(regStart + 12,data,false);//通用曲线类型
		data = (short)values[23];
		MemDb::SetRealData(regStart + 13,data,false);//通用曲线周期
		tmp = (short)values[24] * 0.01;
		MemDb::SetRealData(regStart + 14,tmp,false);//通用曲线幅值
		data = (short)values[25];
		MemDb::SetRealData(regStart + 15,data,false);//通用曲线控制周期
		data = (short)values[26];
		MemDb::SetRealData(regStart + 16,data,false);//历史曲线地址
		data = (short)values[27];
		MemDb::SetRealData(regStart + 17,data,false);//功率曲线执行方式
		data = (short)values[28];
		MemDb::SetRealData(regStart + 18,data,false);//曲线开始执行时间
		data = (short)values[29];
		MemDb::SetRealData(regStart + 19,data,false);//开始曲线
		data = (short)values[30];
		MemDb::SetRealData(regStart + 20,data,false);//结束曲线
		data = (short)values[31];
		MemDb::SetRealData(regStart + 21,data,false);//开始曲线执行时间（年）
		data = (short)values[32];
		MemDb::SetRealData(regStart + 22,data,false);//开始曲线执行时间（月）
		data = (short)values[33];
		MemDb::SetRealData(regStart + 23,data,false);//开始曲线执行时间（日）
		data = (short)values[34];
		MemDb::SetRealData(regStart + 24,data,false);//开始曲线执行时间（时）
		data = (short)values[35];
		MemDb::SetRealData(regStart + 25,data,false);//开始曲线执行时间（分）
		data = (short)values[36];
		MemDb::SetRealData(regStart + 26,data,false);//开始曲线执行时间（秒）
		data = (short)values[37];
		MemDb::SetRealData(regStart + 27,data,false);//开始曲线执行时间（年）
		data = (short)values[38];
		MemDb::SetRealData(regStart + 28,data,false);//开始曲线执行时间（月）
		data = (short)values[39];
		MemDb::SetRealData(regStart + 29,data,false);//开始曲线执行时间（日）
		data = (short)values[40];
		MemDb::SetRealData(regStart + 30,data,false);//开始曲线执行时间（时）
		data = (short)values[41];
		MemDb::SetRealData(regStart + 31,data,false);//开始曲线执行时间（分）
		data = (short)values[42];
		MemDb::SetRealData(regStart + 32,data,false);//开始曲线执行时间（秒）
	}

	zlog_info(Util::m_zlog,"读取中电建投风机模拟器WES-6930数据结束");
	return ret;
}

int EpciFanSimulator::Wes6930Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;

	//判断以太网口是否打开
	if (!m_tcp->IsOpened())
	{
		zlog_warn(Util::m_zlog,"EpciFanSimulator::Wes6930Preset：以太网没有打开");
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcp);

	//判断是否有启动停止设置
	ret = PresetStart(addr,regStart,client);

	//判断是否有风机类型设置
	ret = PresetFanType(addr,regStart,client);
#if 0
	//判断是否有停止时间设置
	ret = PresetStopTime(addr,regStart,client);

	//判断是否有切入风速设置
	ret = PresetCutInSpeed(addr,regStart,client);

	//判断是否有切出风速设置
	ret = PresetCutOutSpeed(addr,regStart,client);

	//判断是否有额定风速设置
	ret = PresetRatedWindSpeed(addr,regStart,client);

	//判断是否有叶片半径设置
	ret = PresetEROB(addr,regStart,client);

	//判断是否有空气密度设置
	ret = PresetAirDensity(addr,regStart,client);

	//判断是否有风能利用率设置
	ret = PresetRPC(addr,regStart,client);
#endif

	//判断是否有功率输出方式设置
	ret = PresetPOutMode(addr,regStart,client);

	//判断是否有风速设置
	ret = PresetWindSpeed(addr,regStart,client);

	//判断是否通用曲线类型设置
	ret = PresetGeneralCurveType(addr,regStart,client);

	//判断是否通用曲线周期设置
	ret = PresetGeneralCurveCycle(addr,regStart,client);

	//判断是否通用曲线幅值设置
	ret = PresetGeneralAmplitude(addr,regStart,client);

	//判断是否有通用曲线控制周期设置
	ret = PresetGeneralCurveConCycle(addr,regStart,client);

	//判断是否历史曲线地址设置
	ret = PresetHisCurveAddr(addr,regStart,client);

	//判断是否功率曲线执行方式设置
	ret = PresetPowerCurOfEx(addr,regStart,client);

	//判断是否曲线开始执行时间设置
	ret = PresetCurStartTime(addr,regStart,client);

	//判断是否有开始曲线设置
	ret = SetStartCur(addr,regStart,client);

	//判断是否有结束曲线设置
	ret = SetStopCur(addr,regStart,client);

	//判断是否有曲线开始执行时间设置
	ret = PresetCurStartExTime(addr,regStart,client);

	//判断是否有曲线结束循环执行时间设置
	ret = PresetCurStopExTime(addr,regStart,client);

	zlog_info(Util::m_zlog,"设置命令结束");

	return ret;
}

int EpciFanSimulator::Wind10SRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 50;
	int length = 96;
	int data = 0;
	float tmp = 0.0;
	int tmp1 = 0;
	int fault1 = 0;
	int fault2 = 0;
	int fault3 = 0;
	int fault4 = 0;
	int fault5 = 0;
	int fault6 = 0;
	int fault7 = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断网络是否打开
	if (!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcp);

	//读取实时反馈信息
	ret = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		data = (short)values[0];
		MemDb::SetRealData(regStart + 0,data,false);//通讯状态
		data = (short)values[1];
		MemDb::SetRealData(regStart + 36,data,false);//运行状态
		data = (short)values[3];
		MemDb::SetRealData(regStart + 38,data,false);//当前执行方式
		tmp = (short)values[30] * 0.01;
		MemDb::SetRealData(regStart + 42,tmp,false);//电网频率
		tmp = (short)values[31] * 0.01;
		MemDb::SetRealData(regStart + 43,tmp,false);//发电频率
		data = (short)values[32];
		MemDb::SetRealData(regStart + 44,data,false);//实时转速
		tmp = (short)values[33] * 0.01;
		MemDb::SetRealData(regStart + 45,tmp,false);//输出扭矩
		tmp = (short)values[4] * 0.01;
		MemDb::SetRealData(regStart + 39,tmp,false);//当前风速
		tmp = (short)values[53] * 0.001;
		MemDb::SetRealData(regStart + 46,tmp,false);//市电平均功率因数
		tmp = (short)values[57] * 0.1;
		MemDb::SetRealData(regStart + 47,tmp,false);//模拟器输出机侧定子AB线电压
		tmp = (short)values[60] * 0.1;
		MemDb::SetRealData(regStart + 48,tmp,false);//模拟器输出机侧定子A相电流
		tmp = (short)values[63] * 0.1;
		MemDb::SetRealData(regStart + 49,tmp,false);//模拟器输出机侧转子电流
		tmp = (short)values[66] * 0.1;
		MemDb::SetRealData(regStart + 50,tmp,false);//直流电压
		tmp = (short)values[67] * 0.1;
		MemDb::SetRealData(regStart + 51,tmp,false);//直流电流
		data = (short)values[68];
		MemDb::SetRealData(regStart + 52,(float)data,false);//直流功率
		tmp = (short)values[72] * 0.1;
		MemDb::SetRealData(regStart + 53,tmp,false);//模拟器输出网侧AB线电压
		tmp = (short)values[75] * 0.1;
		MemDb::SetRealData(regStart + 54,tmp,false);//模拟器输出网侧A相电流
		tmp = (short)values[81] * 0.1;
		MemDb::SetRealData(regStart + 55,tmp,false);//模拟器输出AB线电压
		tmp = (short)values[84] * 0.1;
		MemDb::SetRealData(regStart + 56,tmp,false);//模拟器输出A相电流
		data = (short)values[90];
		MemDb::SetRealData(regStart + 57,(float)data,false);//模拟器输出总有功功率
		data = (short)values[94];
		MemDb::SetRealData(regStart + 58,(float)data,false);//模拟器输出总无功功率
		tmp = (short)values[95] * 0.001;
		MemDb::SetRealData(regStart + 59,tmp,false);//模拟器平均功率因数

		//故障状态
		fault1 = (short)values[12];
		fault2 = (short)values[13];
		fault3 = (short)values[14];
		fault4 = (short)values[15];
		fault5 = (short)values[16];
		fault6 = (short)values[17];
		fault7 = (short)values[18];
		if(((fault1 & 0x0200)>0)||((fault4 & 0x0080)>0))
		{
			tmp1 = tmp1 | 0x00000001;//控制器故障
		}

		if(((fault2 & 0x0800)>0)|| ((fault2 & 0x0004)>0)
				||((fault2 & 0x0008)>0)||((fault2 & 0x0040)>0)
				||((fault2 & 0x0200)>0)||((fault2 & 0x0800)>0)
				||((fault4 & 0x0800)>0)||((fault5 & 0x0080)>0)
				||((fault6 & 0x0001)>0)||((fault7 & 0x0001)>0))
		{
			tmp1 = tmp1 | 0x00000002;//硬件故障
		}

		if((fault2 & 0x0002)>0)
		{
			tmp1 = tmp1 | 0x00000008;//传感器故障
		}

		if((fault3 & 0x0008)>0)
		{
			tmp1 = tmp1 | 0x00000010;//软件故障
		}

		if(((fault2 & 0x2000)>0)||((fault5 & 0x0040)>0))
		{
			tmp1 = tmp1 | 0x00000020;//急停
		}

		if(((fault1 & 0x0400)>0)||((fault3 & 0x0400)>0)
				||((fault3 & 0x0800)>0)||((fault3 & 0x1000)>0)
				||((fault3 & 0x2000)>0)||((fault3 & 0x4000)>0))
		{
			tmp1 = tmp1 | 0x00000040;//系统状态异常
		}

		if((fault1 & 0x8000)>0)
		{
			tmp1 = tmp1 | 0x00000200;//接地故障
		}

		if((fault3 & 0x8000)>0)
		{
			tmp1 = tmp1 | 0x00000400;//参数设置错误
		}

		if(((fault1 & 0x2000)>0)||((fault1 & 0x4000)>0)
				||((fault2 & 0x4000)>0)||((fault2 & 0x8000)>0))
		{
			tmp1 = tmp1 | 0x00000800;//外部故障
		}

		if((fault4 & 0x0004)>0)
		{
			tmp1 = tmp1 | 0x00002000;//直流欠压
		}

		if(((fault6 & 0x0004)>0))
		{
			tmp1 = tmp1 | 0x00001000;//直流过压
		}

		if((fault6 & 0x0010)>0)
		{
			tmp1 = tmp1 | 0x00010000;//电网过压
		}

		if((fault6 & 0x0020)>0)
		{
			tmp1 = tmp1 | 0x00020000;//电网欠压
		}

		if((fault6 & 0x0080)>0)
		{
			tmp1 = tmp1 | 0x00040000;//电网过频
		}

		if((fault6 & 0x0040)>0)
		{
			tmp1 = tmp1 | 0x00080000;//电网欠频
		}

		if((fault7 & 0x0002)>0)
		{
			tmp1 = tmp1 | 0x00100000;//母线过压
		}

		if((fault7 & 0x0004)>0)
		{
			tmp1 = tmp1 | 0x00200000;//母线过压
		}

		if((fault7 & 0x0008)>0)
		{
			tmp1 = tmp1 | 0x00400000;//母线过流
		}

		if(((fault2 & 0x0080)>0)||((fault7 & 0x0010)>0))
		{
			tmp1 = tmp1 | 0x00800000;//超速
		}

		if((fault7 & 0x0080)>0)
		{
			tmp1 = tmp1 | 0x10000000;//并网失败
		}

		if(((fault1 & 0x0004)>0)||((fault1 & 0x0100)>0)
				||((fault1 & 0x1000)>0)||((fault4 & 0x0100)>0)
				||((fault4 & 0x0200)>0))
		{
			tmp1 = tmp1 | 0x20000000;//过温保护
		}

		if((fault2 & 0x1000)>0)
		{
			tmp1 = tmp1 | 0x40000000;//配置文件错误
		}

		if(tmp1 > 0)//有故障
		{
			MemDb::SetRealData(regStart + 40, 1,false);//故障状态
			MemDb::SetRealData(regStart + 41, tmp1,false);//故障码
		}
		else
		{
			MemDb::SetRealData(regStart + 40, 0,false);//故障状态
			MemDb::SetRealData(regStart + 41, tmp1,false);//故障码
		}
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s实时数据失败",dev->name.c_str());
	}

	//读取设置反馈值
	start = 0;
	length = 43;
	ret = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		data = (short)values[0];
		MemDb::SetRealData(regStart + 1,data,false);//启动停止
//		data = (short)values[1];
//		MemDb::SetRealData(regStart + 2,data,false);//低电压穿越
		data = (short)values[2];
		MemDb::SetRealData(regStart + 3,data,false);//故障复位
		data = (short)values[4];
		MemDb::SetRealData(regStart + 4,(float)data,false);//无功设定
		tmp = (short)values[5] * 0.01;
		MemDb::SetRealData(regStart + 5,tmp,false);//切入风速
		tmp = (short)values[6] * 0.01;
		MemDb::SetRealData(regStart + 6,tmp,false);//切出风速
		tmp = (short)values[7] * 0.01;
		MemDb::SetRealData(regStart + 7,tmp,false);//额定风速
		tmp = (short)values[8] * 0.01;
		MemDb::SetRealData(regStart + 8,tmp,false);//叶片半径
		tmp = (short)values[9] * 0.001;
		MemDb::SetRealData(regStart + 9,tmp,false);//空气密度
		tmp = (short)values[10] * 0.001;
		MemDb::SetRealData(regStart + 10,tmp,false);//风能利用率
		data = (short)values[11];
		MemDb::SetRealData(regStart + 11,data,false);//叶片数
		data = (short)values[12];
		MemDb::SetRealData(regStart + 12,data,false);//风机类型
		data = (short)values[20];
		MemDb::SetRealData(regStart + 13,data,false);//功率输出方式
		tmp = (short)values[21] * 0.01;
		MemDb::SetRealData(regStart + 14,tmp,false);//风速设定值
		data = (short)values[22];
		MemDb::SetRealData(regStart + 15,data,false);//通用曲线类型
		data = (short)values[23];
		MemDb::SetRealData(regStart + 16,data,false);//通用曲线周期
		tmp = (short)values[24] * 0.01;
		MemDb::SetRealData(regStart + 17,tmp,false);//通用曲线幅值
		data = (short)values[25];
		MemDb::SetRealData(regStart + 18,data,false);//通用曲线控制周期
		data = (short)values[26];
		MemDb::SetRealData(regStart + 19,data,false);//历史曲线地址
		data = (short)values[27];
		MemDb::SetRealData(regStart + 20,data,false);//功率曲线执行方式
		data = (short)values[28];
		MemDb::SetRealData(regStart + 21,data,false);//曲线开始执行时间
		data = (short)values[29];
		MemDb::SetRealData(regStart + 22,data,false);//开始曲线
		data = (short)values[30];
		MemDb::SetRealData(regStart + 23,data,false);//结束曲线
		data = (short)values[31];
		MemDb::SetRealData(regStart + 24,data,false);//开始曲线执行时间（年）
		data = (short)values[32];
		MemDb::SetRealData(regStart + 25,data,false);//开始曲线执行时间（月）
		data = (short)values[33];
		MemDb::SetRealData(regStart + 26,data,false);//开始曲线执行时间（日）
		data = (short)values[34];
		MemDb::SetRealData(regStart + 27,data,false);//开始曲线执行时间（时）
		data = (short)values[35];
		MemDb::SetRealData(regStart + 28,data,false);//开始曲线执行时间（分）
		data = (short)values[36];
		MemDb::SetRealData(regStart + 29,data,false);//开始曲线执行时间（秒）
		data = (short)values[37];
		MemDb::SetRealData(regStart + 30,data,false);//开始曲线执行时间（年）
		data = (short)values[38];
		MemDb::SetRealData(regStart + 31,data,false);//开始曲线执行时间（月）
		data = (short)values[39];
		MemDb::SetRealData(regStart + 32,data,false);//开始曲线执行时间（日）
		data = (short)values[40];
		MemDb::SetRealData(regStart + 33,data,false);//开始曲线执行时间（时）
		data = (short)values[41];
		MemDb::SetRealData(regStart + 34,data,false);//开始曲线执行时间（分）
		data = (short)values[42];
		MemDb::SetRealData(regStart + 35,data,false);//开始曲线执行时间（秒）
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设置反馈数据错误",dev->name.c_str());
	}

	zlog_info(Util::m_zlog,"读取%s设置反馈数据结束",dev->name.c_str());
	return ret;

}

int EpciFanSimulator::Wind10SPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;

	//判断以太网口是否打开
	if (!m_tcp->IsOpened())
	{
		zlog_warn(Util::m_zlog,"EpciFanSimulator::Wes6930Preset：以太网没有打开");
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcp);

	//判断是否有启动停止设置
	ret = PresetStart(addr,regStart,client);

	//判断是否有风机类型设置
	ret = PresetFanType(addr,regStart,client);

	//判断是否有故障复位设置
	ret = PresetFaultReset(addr,regStart,client);
#if 0
	//判断是否有低电压穿越设置
	ret = PresetLVRTC(addr,regStart,client);

	//判断是否有无功功率设置
	ret = PresetReactivePower(addr,regStart,client);

	//判断是否有切入风速设置
	ret = PresetCutInSpeed(addr,regStart,client);

	//判断是否有切出风速设置
	ret = PresetCutOutSpeed(addr,regStart,client);

	//判断是否有额定风速设置
	ret = PresetRatedWindSpeed(addr,regStart,client);

	//判断是否有叶片半径设置
	ret = PresetEROB(addr,regStart,client);

	//判断是否有空气密度设置
	ret = PresetAirDensity(addr,regStart,client);

	//判断是否有风能利用率设置
	ret = PresetRPC(addr,regStart,client);
#endif

	//判断是否有功率输出方式设置
	ret = PresetPOutMode(addr,regStart,client);

	//判断是否有风速设置
	ret = PresetWindSpeed(addr,regStart,client);

	//判断是否通用曲线类型设置
	ret = PresetGeneralCurveType(addr,regStart,client);

	//判断是否通用曲线周期设置
	ret = PresetGeneralCurveCycle(addr,regStart,client);

	//判断是否通用曲线幅值设置
	ret = PresetGeneralAmplitude(addr,regStart,client);

	//判断是否有通用曲线控制周期设置
	ret = PresetGeneralCurveConCycle(addr,regStart,client);

	//判断是否历史曲线地址设置
	ret = PresetHisCurveAddr(addr,regStart,client);

	//判断是否功率曲线执行方式设置
	ret = PresetPowerCurOfEx(addr,regStart,client);

	//判断是否曲线开始执行时间设置
	ret = PresetCurStartTime(addr,regStart,client);

	//判断是否有开始曲线设置
	ret = SetStartCur(addr,regStart,client);

	//判断是否有结束曲线设置
	ret = SetStopCur(addr,regStart,client);

	//判断是否有曲线开始执行时间设置
	ret = PresetCurStartExTime(addr,regStart,client);

	//判断是否有曲线结束循环执行时间设置
	ret = PresetCurStopExTime(addr,regStart,client);

	zlog_info(Util::m_zlog,"设置%s命令结束",dev->name.c_str());

	return ret;
}

int EpciFanSimulator::PresetStart(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;

	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器启停");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr, 0, realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"启动停止错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}
	return ret;
}

int EpciFanSimulator::PresetFanType(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;

	if (MemDb::GetRealDataWrited(realData, regStart + 12, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器风机类型");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,12,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"设置风机类型错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}
	return ret;
}

int EpciFanSimulator::PresetFaultReset(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;

	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器故障复位");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,2,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"设置故障复位错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetLVRTC(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;

	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器低电压穿越");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,1,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"设置低电压穿越错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetReactivePower(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 4, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器无功率");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue;
		}
		else if(realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue);
		}
		ret = client.PresetSingleRegister(addr,4,tmp);
	}

	return ret;
}

int EpciFanSimulator::PresetCutInSpeed(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器切入风速");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue * 100;
		}
		else if(realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue * 100);
		}
		ret = client.PresetSingleRegister(addr,5,tmp);
	}

	return ret;
}

int EpciFanSimulator::PresetCutOutSpeed(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 6, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器切出风速");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue * 100;
		}
		else if(realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue * 100);
		}
		ret = client.PresetSingleRegister(addr,6,tmp);
	}

	return ret;
}

int EpciFanSimulator::PresetRatedWindSpeed(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 7, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器额定风速");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue * 100;
		}
		else if(realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue * 100);
		}
		ret = client.PresetSingleRegister(addr,7,tmp);
	}

	return ret;
}

int EpciFanSimulator::PresetEROB(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 8, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器叶片半径");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue * 100;
		}
		else if(realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue * 100);
		}
		ret = client.PresetSingleRegister(addr,8,tmp);
	}

	return ret;
}

int EpciFanSimulator::PresetAirDensity(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 9, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器空气密度");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue * 1000;
		}
		else if(realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue * 1000);
		}
		ret = client.PresetSingleRegister(addr,9,tmp);
	}

	return ret;
}

int EpciFanSimulator::PresetRPC(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 10, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器风能利用率");
		if (realData.pv.type == RealData::INT)
		{
			if((realData.pv.data.iValue >= 0)
					&&(realData.pv.data.iValue <= 1))
			{
				tmp = realData.pv.data.iValue * 1000;
				ret = client.PresetSingleRegister(addr,10,tmp);
			}
		}
		else if(realData.pv.type == RealData::DB)
		{
			if((realData.pv.data.dbValue >= 0)
					&&(realData.pv.data.dbValue <= 1))
			{
				tmp = (int)(realData.pv.data.dbValue * 1000);
				ret = client.PresetSingleRegister(addr,10,tmp);
			}
		}
	}

	return ret;
}

int EpciFanSimulator::PresetPOutMode(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 13, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器功率输出方式");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,20,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"功率输出方式设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetWindSpeed(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 14, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器风速设定值");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue * 100;
		}
		else if(realData.pv.type == RealData::DB)
		{
			tmp = (int)(realData.pv.data.dbValue * 100);
		}
		ret = client.PresetSingleRegister(addr,21,tmp);
	}

	return ret;
}

int EpciFanSimulator::PresetGeneralCurveType(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 15, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器通用曲线类型");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,22,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"通用曲线类型设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetGeneralCurveCycle(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 16, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器通用曲线周期");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,23,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"通用曲线周期设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetGeneralAmplitude(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	int tmp = 0;

	if (MemDb::GetRealDataWrited(realData, regStart + 17, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器通用曲线幅值");
		if (realData.pv.type == RealData::INT)
		{
			tmp = realData.pv.data.iValue * 100;
		}
		else if (realData.pv.type == RealData::DB)
		{
			tmp = (int) (realData.pv.data.dbValue * 100);
		}

		ret = client.PresetSingleRegister(addr, 24, tmp);
	}


	return ret;
}

int EpciFanSimulator::PresetGeneralCurveConCycle(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 18, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器通用曲线控制周期");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,25,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"通用曲线控制周期设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetHisCurveAddr(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 19, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器历史曲线地址");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,26,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"历史曲线地址设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetPowerCurOfEx(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 20, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器功率曲线执行方式");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,27,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"功率曲线执行方式设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetCurStartTime(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 21, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器曲线开始执行时间");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,28,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"曲线开始执行时间设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::SetStartCur(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 22, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器开始曲线");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,29,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"开始曲线设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::SetStopCur(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;
	if (MemDb::GetRealDataWrited(realData, regStart + 23, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器结束曲线");
		if (realData.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,30,realData.pv.data.iValue);
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"结束曲线设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}

int EpciFanSimulator::PresetCurStartExTime(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realDataYear;
	RealData realDataMon;
	RealData realDataDay;
	RealData realDataHour;
	RealData realDataMin;
	RealData realDataSec;

	if (MemDb::GetRealDataWrited(realDataYear, regStart + 24, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器年");
		if (realDataYear.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,31,realDataYear.pv.data.iValue);
		}
		else if (realDataYear.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"开始时间年设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataMon, regStart + 25, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器月");
		if (realDataMon.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,32,realDataMon.pv.data.iValue);
		}
		else if (realDataMon.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"开始时间月设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataDay, regStart + 26, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器日");
		if (realDataDay.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,33,realDataDay.pv.data.iValue);
		}
		else if (realDataDay.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"开始时间日设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataHour, regStart + 27, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器时");
		if (realDataHour.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,34,realDataHour.pv.data.iValue);
		}
		else if (realDataHour.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"开始时间时设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataMin, regStart + 28, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器分");
		if (realDataMin.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,35,realDataMin.pv.data.iValue);
		}
		else if (realDataMin.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"开始时间分设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataSec, regStart + 29, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器秒");
		if (realDataSec.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,36,realDataSec.pv.data.iValue);
		}
		else if (realDataSec.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"开始时间秒设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}
	return ret;
}

int EpciFanSimulator::PresetCurStopExTime(int addr, int regStart, ModbusTcpClient client)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realDataYear;
	RealData realDataMon;
	RealData realDataDay;
	RealData realDataHour;
	RealData realDataMin;
	RealData realDataSec;

	if (MemDb::GetRealDataWrited(realDataYear, regStart + 30, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器年");
		if (realDataYear.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,37,realDataYear.pv.data.iValue);
		}
		else if (realDataYear.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"结束时间年设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataMon, regStart + 31, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器月");
		if (realDataMon.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,38,realDataMon.pv.data.iValue);
		}
		else if (realDataMon.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"结束时间月设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataDay, regStart + 32, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器日");
		if (realDataDay.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,39,realDataDay.pv.data.iValue);
		}
		else if (realDataDay.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"结束时间日设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataHour, regStart + 33, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器时");
		if (realDataHour.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,40,realDataHour.pv.data.iValue);
		}
		else if (realDataHour.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"结束时间时设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataMin, regStart + 34, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器分");
		if (realDataMin.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,41,realDataMin.pv.data.iValue);
		}
		else if (realDataMin.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"结束时间分设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	if (MemDb::GetRealDataWrited(realDataSec, regStart + 35, true))
	{
		zlog_info(Util::m_zlog,"设置命令风力发电机模拟器秒");
		if (realDataSec.pv.type == RealData::INT)
		{
			ret = client.PresetSingleRegister(addr,42,realDataSec.pv.data.iValue);
		}
		else if (realDataSec.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"结束时间秒设置错误");
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	return ret;
}
