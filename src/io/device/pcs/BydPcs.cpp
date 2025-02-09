/*
 * BydPcs.cpp
 *
 *   创建日期: 2013-10-8
 *   作          者: 马中华
 */

#include "BydPcs.h"

#include "../../com/Com.h"

BydPcs::BydPcs()
{
	// TODO 构造函数
	m_com = NULL;
}

BydPcs::~BydPcs()
{
	// TODO 析构函数
}

void BydPcs::SetCom(Com* com)
{
	m_com = com;
}

int BydPcs::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 6:
		ret = this->AcltBegRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int BydPcs::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case 6:
		ret = this->AcltBegPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int BydPcs::AcltBegRead(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取比亚迪PCS数据开始");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;

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

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int start = 0x0100;
	int length = 90;
	float tmp = 0;
	int tmp1 = 0;
	int tmp2 = 0;
	int tmp3 = 0;
	int regStart = dev->regStart;

	zlog_info(Util::m_zlog,"addr = %d,regStart = %02X",addr,regStart);

	//读取比亚迪PCS数据
	zlog_info(Util::m_zlog,"读取PCS故障数据和软压板信息");
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		//保存通讯正常
		comm = true;

		//控制器故障
		if ((values[19] & 0x0100) > 0)
		{
			tmp1 = tmp1 | 0x00000001;
		}
		if ((values[63] & 0xFFFF) > 0)
		{
			tmp1 = tmp1 | 0x00000001;
		}

		//IGBT故障
		if ((values[52] & 0x0111) > 0)
		{
			tmp1 = tmp1 | 0x00000002;
		}
		if ((values[51] & 0x0080) > 0)
		{
			tmp1 = tmp1 | 0x00000002;
		}

		//内部通讯故障
		if ((values[37] & 0x0015) > 0)
		{
			tmp1 = tmp1 | 0x00000004;
		}
		if ((values[53] & 0x0080))
		{
			tmp1 = tmp1 | 0x00000004;
		}
		if ((values[64] & 0x0003) > 0)
		{
			tmp1 = tmp1 | 0x00000004;
		}

		//传感器失效
		if ((values[17] & 0x0078) > 0)
		{
			tmp1 = tmp1 | 0x00000008;
		}

		//采样通道异常
		if ((values[19] & 0x001F) > 0)
		{
			tmp1 = tmp1 | 0x00000010;
		}
		if ((values[41] & 0x3F))
		{
			tmp1 = tmp1 | 0x00000010;
		}

		//急停故障
		if ((values[19] & 0x1000) > 0)
		{
			tmp1 = tmp1 | 0x00000020;
		}
		if ((values[51] & 0x0100))
		{
			tmp1 = tmp1 | 0x00000020;
		}

		//系统状态异常
		if ((values[19] & 0x8000) > 0)
		{
			tmp1 = tmp1 | 0x00000040;
		}

		//接触器故障
		if ((values[20] & 0x0001) > 0)
		{
			tmp1 = tmp1 | 0x00000080;
		}
		if ((values[21] & 0x03FF) > 0)
		{
			tmp1 = tmp1 | 0x00000080;
		}
		if ((values[51] & 0x0007) > 0)
		{
			tmp1 = tmp1 | 0x00000080;
		}

		//断路器故障
		if ((values[19] & 0x0600) > 0)
		{
			tmp1 = tmp1 | 0x00000100;
		}
		if ((values[51] & 0x0038) > 0)
		{
			tmp1 = tmp1 | 0x00000100;
		}

		//孤岛保护
		if ((values[43] & 0x0001) > 0)
		{
			tmp1 = tmp1 | 0x00000400;
		}

		//低电压穿越
		if ((values[48] & 0x0700) > 0)
		{
			tmp1 = tmp1 | 0x00000800;
		}

		//直流欠压
		if ((values[45] & 0x0002) > 0)
		{
			tmp1 = tmp1 | 0x00002000;
		}

		//直流电压异常
		if ((values[20] & 0x000c) > 0)
		{
			tmp1 = tmp1 | 0x00004000;
		}
		if ((values[35] & 0x00F0) > 0)
		{
			tmp1 = tmp1 | 0x00004000;
		}

		//直流过流
		if ((values[35] & 0x000c) > 0)
		{
			tmp1 = tmp1 | 0x00008000;
		}

		//电网过压
		if (((values[43] & 0x0006) > 0) | ((values[43] & 0x0060) > 0)
				| ((values[43] & 0x0600) > 0))
		{
			tmp1 = tmp1 | 0x00010000;
		}

		//电网欠压
		if (((values[43] & 0x0018) > 0) | ((values[43] & 0x0180) > 0)
				| ((values[43] & 0x1800) > 0))
		{
			tmp1 = tmp1 | 0x00020000;
		}

		//电网过频
		if ((values[43] & 0x6000) > 0)
		{
			tmp1 = tmp1 | 0x00040000;
		}

		//电网欠频
		if (((values[43] & 0x8000) > 0) | ((values[44] & 0x0001) > 0))
		{
			tmp1 = tmp1 | 0x00080000;
		}

		//交流过流
		if ((values[47] & 0x0700) > 0)
		{
			tmp1 = tmp1 | 0x00400000;
		}

		//三相电流不平衡
		if ((values[47] & 0x0080) > 0)
		{
			tmp1 = tmp1 | 0x00800000;
		}

		//过载保护
		if ((values[53] & 0x0018) > 0)
		{
			tmp1 = tmp1 | 0x01000000;
		}

		//交流预充未满
		if ((values[48] & 0x00E0) > 0)
		{
			tmp1 = tmp1 | 0x02000000;
		}

		//相序错误
		if ((values[19] & 0x2000) > 0)
		{
			tmp1 = tmp1 | 0x04000000;
		}
		if ((values[48] & 0x0018) > 0)
		{
			tmp1 = tmp1 | 0x04000000;
		}

		//频率异常
		if ((values[19] & 0x4000) > 0)
		{
			tmp1 = tmp1 | 0x08000000;
		}

		//锁相异常
		if ((values[53] & 0x0020) > 0)
		{
			tmp1 = tmp1 | 0x10000000;
		}

		//过温保护
		if ((values[38] & 0x0078) > 0)
		{
			tmp1 = tmp1 | 0x20000000;
		}

		//系统告警标志
		if ((values[0] & 0x0001) > 0)
		{
			tmp1 = tmp1 | 0x40000000;
		}

		//系统保护标志
		if ((values[0] & 0x0002) > 0)
		{
			tmp1 = tmp1 | 0x80000000;
		}

		//系统降额标志
		if ((values[0] & 0x0004) > 0)
		{
			tmp2 = tmp2 | 0x00000001;
		}

		if ((tmp1 == 0) && (tmp2 == 0))
		{
			MemDb::SetRealData(regStart + 17, 0, false);//无故障
			MemDb::SetRealData(regStart + 18, tmp1, false);//错误码
			MemDb::SetRealData(regStart + 19, tmp2, false);//错误码
		}
		else
		{
			MemDb::SetRealData(regStart + 17, 1, false);//故障
			MemDb::SetRealData(regStart + 18, tmp1, false);//错误码
			MemDb::SetRealData(regStart + 19, tmp2, false);//错误码
		}

		if(values[1] == 1)//停止
		{
			MemDb::SetRealData(regStart + 13, 0, false);//停止
		}
		else if(values[1] == 2)//热备
		{
			MemDb::SetRealData(regStart + 13, 3, false);//热备
		}
		else if(values[1] == 4)//运行
		{
			MemDb::SetRealData(regStart + 13, 4, false);//运行
		}

		if(values[3] == 2)//P/Q模式
		{
			MemDb::SetRealData(regStart + 14, 1, false);//P/Q模式
		}
		else if(values[3] == 5)//离网模式
		{
			MemDb::SetRealData(regStart + 14, 2, false);//离网模式
		}
		else if(values[3] == 6)//UPS模式
		{
			MemDb::SetRealData(regStart + 14, 5, false);//UPS模式
		}

		//东北这个项目没有一下功能
//		MemDb::SetRealData(regStart + 15, values[4], false);//电网异常恢复状态
//
//		//软压板信息
//		if ((values[88] & 0x01) > 0)//CAN断线保护状态
//		{
//			tmp3 = tmp3 | 0x00000001;
//		}
//		if ((values[88] & 0x02) > 0)//温度异常1保护状态
//		{
//			tmp3 = tmp3 | 0x00000002;
//		}
//		if ((values[88] & 0x04) > 0)//功率翻转保护状态
//		{
//			tmp3 = tmp3 | 0x00000004;
//		}
//		if ((values[89] & 0x01) > 0)//电网异常恢复使能
//		{
//			tmp3 = tmp3 | 0x00000008;
//		}
//		if ((values[89] & 0x02) > 0)//远程通信故障检测使能
//		{
//			tmp3 = tmp3 | 0x00000010;
//		}
//
//		zlog_info(Util::m_zlog,"tmp3 = %d",tmp3);
//		MemDb::SetRealData(regStart + 16, tmp3, false);//软压板信息

		zlog_info(Util::m_zlog,"读取比亚迪PCS故障信息结束");
	}
	else
	{
		zlog_warn(Util::m_zlog, "地址%d读取读取故障数据失败，结果=%d",addr,ret);
	}

	//获取比亚迪PCS实时数据
	zlog_info(Util::m_zlog,"获取比亚迪PCS实时数据");
	start = 0x0200;
	length = 78;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if (ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		tmp = ((short) values[0]) * 0.1;
		MemDb::SetRealData(regStart + 26, tmp, false);//直流电压
		tmp = ((short) values[1]) * 0.1;
		MemDb::SetRealData(regStart + 27, tmp, false);//直流电流
		tmp3 = ((short) values[2]) * 100;
		MemDb::SetRealData(regStart + 28, (float)tmp3, false);//直流功率
		tmp = ((short) values[22]) * 0.1;
		MemDb::SetRealData(regStart + 29, tmp, false);//Uab线电压
		tmp = ((short) values[23]) * 0.1;
		MemDb::SetRealData(regStart + 30, tmp, false);//Ubc线电压
		tmp = ((short) values[24]) * 0.1;
		MemDb::SetRealData(regStart + 31, tmp, false);//Uca线电压
		tmp = ((short) values[19]) * 0.1;
		MemDb::SetRealData(regStart + 32, tmp, false);//A相电流
		tmp = ((short) values[20]) * 0.1;
		MemDb::SetRealData(regStart + 33, tmp, false);//B相电流
		tmp = ((short) values[21]) * 0.1;
		MemDb::SetRealData(regStart + 34, tmp, false);//C相电流
		tmp3 = ((short) values[16]) * 100;
		MemDb::SetRealData(regStart + 38, (float)tmp3, false);//三相有功功率和
		tmp3 = ((short) values[17]) * 100;
		MemDb::SetRealData(regStart + 39, (float)tmp3, false);//三相无功功率和
		tmp = ((short) values[29]) * 0.001;
		MemDb::SetRealData(regStart + 40, tmp, false);//三相功率因数和
		tmp = ((short) values[28]) * 0.01;
		MemDb::SetRealData(regStart + 41, tmp, false);//系统频率
		tmp3 = (short)(values[48]);
		MemDb::SetRealData(regStart + 43, tmp3, false);//允许充电功率(KW)
		tmp3 = (short)(values[49]);
		MemDb::SetRealData(regStart + 44, tmp3, false);//允许放电功率
		tmp3 = (short)(values[50]);
		MemDb::SetRealData(regStart + 45, tmp3, false);//允许视在功率
		tmp3 = (short)(values[51]);
		MemDb::SetRealData(regStart + 46, tmp3, false);//允许无功功率
		MemDb::SetRealData(regStart + 21, (float)(((values[30]) | (values[31] << 8))),
				false);//累计输入电能
		MemDb::SetRealData(regStart + 22, (float)(((values[32]) | (values[33] << 8))),
				false);//累计输出电能

		start = 0x0240;
		length = 13;
		ret = rtu.ReadOutputRegisters(values, addr, start, length);
		if (ret == ErrorInfo::ERR_OK)
		{
			tmp3 = (short) values[6];
			MemDb::SetRealData(regStart + 25, (float) tmp3, false);//机内温度
		}

		zlog_info(Util::m_zlog,"获取比亚迪PCS实时数据结束");
	}
	else
	{
		zlog_warn(Util::m_zlog, "地址%d读取读取实时数据失败，结果=%d",addr,ret);
	}

	//读取设置反馈值
	if (addr == 100)
	{
		zlog_info(Util::m_zlog,"读取2#PCS设置反馈值");
		ret = AcltBegReadFeedBack(regStart, 0xF400, rtu);
	}
	else if (addr == 101)
	{
		zlog_info(Util::m_zlog,"读取1#PCS设置反馈值");
		ret = AcltBegReadFeedBack(regStart, 0xF450, rtu);
	}

	MemDb::SetRealData(regStart + 0, comm, false);

	//获取比亚迪PCS控制模式
	start = 0xF000;
	length = 1;
	ret = rtu.ReadOutputRegisters(values, 10, start, length);
	tmp3 = (short)values[0];//控制模式1手动、2自动
	MemDb::SetRealData(regStart + 47, tmp3, false);

	zlog_info(Util::m_zlog,"读取比亚迪PCS数据结束");
	return ret;
}

int BydPcs::AcltBegReadFeedBack(int regStart, int start, ModbusRtuMaster rtu)
{
	int length = 61;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	float tmp = 0;

	ret = rtu.ReadOutputRegisters(values, 10, start, length);
	if (ret == ErrorInfo::ERR_OK)
	{
		if (values[0] == 0)//启动
		{
			MemDb::SetRealData(regStart + 1, 1, false);//启动、停止控制
		}
		else if (values[0] == 1)//停止
		{
			MemDb::SetRealData(regStart + 1, 0, false);//启动、停止控制
		}
		if (values[1] == 2)//P/Q模式
		{
			MemDb::SetRealData(regStart + 3, 1, false);
		}
		else if (values[1] == 5)//离网模式
		{
			MemDb::SetRealData(regStart + 3, 2, false);
		}
		else if (values[1] == 6)//UPS模式
		{
			MemDb::SetRealData(regStart + 3, 5, false);
		}
		if (values[60] == 0)//进入计划孤岛
		{
			MemDb::SetRealData(regStart + 4, 1, false);
		}
		else if (values[60] == 1)//退出计划孤岛
		{
			MemDb::SetRealData(regStart + 4, 2, false);
		}
		tmp = ((short) values[7]) * 100;
		MemDb::SetRealData(regStart + 6, tmp, false);//有功目标值
		tmp = ((short) values[8]) * 100;
		MemDb::SetRealData(regStart + 7, tmp, false);//无功目标值
		tmp = ((short) values[5]) * 0.1;
		MemDb::SetRealData(regStart + 8, tmp, false);//电压目标值
		tmp = ((short) values[43]) * 0.01;
		MemDb::SetRealData(regStart + 9, tmp, false);//频率目标值

		zlog_info(Util::m_zlog,"读取反馈数据成功");
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取反馈数据失败");
	}

	return ret;
}

int BydPcs::AcltBegPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	UshortArray data;
	data.clear();

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	int regStart = dev->regStart;

	//判断是否有手自动命令（默认手动）
	if (MemDb::GetRealDataWrited(realData, regStart + 48, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = rtu.PresetSingleRegister(10, 0xF200, realData.pv.data.iValue);
		}
	}

	//获取PCSMaster手自动模式
	MemDb::GetRealData(realData,regStart + 47);
	if(realData.pv.data.iValue == 1)//手动模式
	{
		//判断是否有启停命令
		if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				zlog_warn(Util::m_zlog,"比亚迪PCS启停命令%d",realData.pv.data.iValue);
				//有数据变化
				if (realData.pv.data.iValue == 1)
				{
					//启动
					if (addr == 100)
					{
						ret = rtu.PresetSingleRegister(10, 0xF400, 0x00);
					}
					else if (addr == 101)
					{
						ret = rtu.PresetSingleRegister(10, 0xF450, 0x00);
					}
				}
				else if (realData.pv.data.iValue == 0)
				{
					//停止
					if (addr == 100)
					{
						ret = rtu.PresetSingleRegister(10, 0xF400, 0x01);
					}
					else if (addr == 101)
					{
						ret = rtu.PresetSingleRegister(10, 0xF450, 0x01);
					}
				}
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_info(Util::m_zlog,"比亚迪PCS启停命令错误：%f",realData.pv.data.dbValue);
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		//判断是否有工作模式设置命令
		if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				if (realData.pv.data.iValue == 1)//P/Q模式
				{
					// P/Q模式
					if (addr == 100)
					{
						ret = rtu.PresetSingleRegister(10, 0xF401, 2);
					}
					else if (addr == 101)
					{
						ret = rtu.PresetSingleRegister(10, 0xF451, 2);
					}
				}
				else if (realData.pv.data.iValue == 2)//离网模式
				{
					// 离网模式
					if (addr == 100)
					{
						ret = rtu.PresetSingleRegister(10, 0xF401, 5);
					}
					else if (addr == 101)
					{
						ret = rtu.PresetSingleRegister(10, 0xF451, 5);
					}
				}
				else if (realData.pv.data.iValue == 5)//UPS模式
				{
					// UPS模式
					if (addr == 100)
					{
						ret = rtu.PresetSingleRegister(10, 0xF401, 6);
					}
					else if (addr == 101)
					{
						ret = rtu.PresetSingleRegister(10, 0xF451, 6);
					}
				}
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_warn(Util::m_zlog,"设置工作模式失败");
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		//判断是否有故障清楚报警命令
		if (MemDb::GetRealDataWrited(realData, regStart + 5, true))
		{
			if(addr == 101)
			{
				ret = rtu.PresetSingleRegister(10, 0xF461, 1);//故障清除
				ret = rtu.PresetSingleRegister(10, 0xF462, 1);//告警清除
			}
			else if(addr == 100)
			{
				ret = rtu.PresetSingleRegister(10, 0xF411, 1);//故障清除
				ret = rtu.PresetSingleRegister(10, 0xF412, 1);//告警清除
			}
		}

		//判断是否目标控制设置命令
		if (MemDb::GetRealDataWrited(realData, regStart + 4, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				if (realData.pv.data.iValue == 1)
				{
					//进入计划性孤岛
					if (addr == 100)
					{
						ret = rtu.PresetSingleRegister(10, 0xF43C, 0);
					}
					else if (addr == 101)
					{
						ret = rtu.PresetSingleRegister(10, 0xF48C, 0);
					}
				}
				else if (realData.pv.data.iValue == 2)
				{
					//退出计划性孤岛
					if (addr == 100)
					{
						ret = rtu.PresetSingleRegister(10, 0xF43C, 1);
					}
					else if (addr == 101)
					{
						ret = rtu.PresetSingleRegister(10, 0xF48C, 1);
					}
				}
			}
			else if (realData.pv.type == RealData::DB)
			{
				zlog_warn(Util::m_zlog,"设置计划孤岛失败");
				ret = ErrorInfo::ERR_WRITE;
			}
		}

		//判断是否有有功功率目标值设置命令
		if (addr == 100)
		{
			ret = AcltBegPreSetValueP(regStart + 6, 0xF407, rtu);
		}
		else if (addr == 101)
		{
			ret = AcltBegPreSetValueP(regStart + 6, 0xF457, rtu);
		}

		//判断是否有无功功率目标值设置命令
		if (addr == 100)
		{
			ret = AcltBegPreSetValueP(regStart + 7, 0xF408, rtu);
		}
		else if (addr == 101)
		{
			ret = AcltBegPreSetValueP(regStart + 7, 0xF458, rtu);
		}

		//判断是否有电压目标值设置命令
		if (addr == 100)
		{
			ret = AcltBegPreSetValueV(regStart + 8, 0xF405, rtu);
		}
		else if (addr == 101)
		{
			ret = AcltBegPreSetValueV(regStart + 8, 0xF455, rtu);
		}

		//判断是否有频率目标值设置命令
		if (MemDb::GetRealDataWrited(realData, regStart + 9, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				if (addr == 100)
				{
					ret = rtu.PresetSingleRegister(10, 0xF42B,
							realData.pv.data.iValue * 100);
				}
				else if (addr == 101)
				{
					ret = rtu.PresetSingleRegister(10, 0xF47B,
							realData.pv.data.iValue * 100);
				}
			}
			else if (realData.pv.type == RealData::DB)
			{
				if (addr == 100)
				{
					ret = rtu.PresetSingleRegister(10, 0xF42B,
							realData.pv.data.dbValue * 100);
				}
				else if (addr == 101)
				{
					ret = rtu.PresetSingleRegister(10, 0xF47B,
							realData.pv.data.dbValue * 100);
				}
			}
		}
	}
	else if(realData.pv.data.iValue == 2)//自动模式
	{
		//判断是否有并联模式切换设置命令
		if (MemDb::GetRealDataWrited(realData, regStart + 49, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(10, 0xF201, realData.pv.data.iValue);
			}
		}

		//判断是否有并网并联有功功率目标设置
		if (MemDb::GetRealDataWrited(realData, regStart + 50, true))
		{
			if (realData.pv.type == RealData::DB)
			{
				ret = rtu.PresetSingleRegister(10, 0xF20A, realData.pv.data.dbValue*10);
			}
		}

		//判断是否有并网并联无功功率目标设置
		if (MemDb::GetRealDataWrited(realData, regStart + 51, true))
		{
			if (realData.pv.type == RealData::DB)
			{
				ret = rtu.PresetSingleRegister(10, 0xF20B, realData.pv.data.dbValue*10);
			}
		}

		//判断是否有并联启停控制设置命令
		if (MemDb::GetRealDataWrited(realData, regStart + 52, true))
		{
			if (realData.pv.type == RealData::INT)
			{
				ret = rtu.PresetSingleRegister(10, 0xF20D, realData.pv.data.iValue);
			}
		}
	}
	zlog_info(Util::m_zlog,"设置PCS参数结束");
	return ret;
}

int BydPcs::AcltBegPreSetValueV(int regStart, int start, ModbusRtuMaster rtu)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;

	if (MemDb::GetRealDataWrited(realData, regStart, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = rtu.PresetSingleRegister(10, start, (short)(realData.pv.data.iValue*10));
		}
		else if (realData.pv.type == RealData::DB)
		{
			ret = rtu.PresetSingleRegister(10, start, (short)(realData.pv.data.dbValue*10));
		}
	}

	return ret;
}

int BydPcs::AcltBegPreSetValueP(int regStart, int start, ModbusRtuMaster rtu)
{
	int ret = ErrorInfo::ERR_OK;
	RealData realData;

	if (MemDb::GetRealDataWrited(realData, regStart, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			ret = rtu.PresetSingleRegister(10, start, (short)(realData.pv.data.iValue*0.01));
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"功率值 = %.2f",realData.pv.data.dbValue);
			ret = rtu.PresetSingleRegister(10, start, (short)(realData.pv.data.dbValue*0.01));
		}
	}

	return ret;
}
