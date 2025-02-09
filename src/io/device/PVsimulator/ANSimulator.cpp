/*
 * ANSimultor.cpp
 *
 *  Created on: 2013-10-10
 *      Author: zhoulei
 */

#include "ANSimulator.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

ANSimultor::ANSimultor()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
	m_delayTime = 1000;
	m_retries = 3;
}

ANSimultor::~ANSimultor()
{
	// TODO Auto-generated destructor stub
}

void ANSimultor::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void ANSimultor::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}


int ANSimultor::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_ACLT_AN51://ACLT-AN51
		ret = this->AcltAn51Read(dev);//浙江电科院项目用的AN53设备，读写都是AN53系列
		break;
	default:
		break;
	};

	return ret;
}

int ANSimultor::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_ACLT_AN51://
		ret = this->AcltAn51Preset(dev); //实际写的是AN53系列
		break;
	default:
		break;
	};

	return ret;
}

int ANSimultor::AcltAn51Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	zlog_info(Util::m_zlog,"读取ACLT-AN51数据开始");
	ret = this->AcltAnRead(dev);
	return ret;
}

int ANSimultor::AcltAn51Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	zlog_info(Util::m_zlog,"设置ACLT-AN51数据开始");
	ret = this->AcltAnPreset(dev);
	return ret;
}

int ANSimultor::AcltAn51010Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	zlog_info(Util::m_zlog,"读取ACLT-AN51010数据开始");
	ret = this->AcltAnRead(dev);
	return ret;
}

int ANSimultor::AcltAn51010Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	zlog_info(Util::m_zlog,"设置ACLT-AN51010数据开始");
	ret = this->AcltAnPreset(dev);
	return ret;
}

int ANSimultor::AcltAnRead(Device::SlaveDev* dev)//AN53系列
{
	zlog_info(Util::m_zlog,"读取艾诺光伏电池模拟器数据");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	float tmp = 0;
	int tmp1 = 0;

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

	//查询电源输出状态
	//msleep(500);
	ret = AnRead(addr, 0xF0, 0x00, 9, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::SetRealData(regStart + 12, values[0], false);
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源输出状态失败，结果=%d",dev->name.c_str(),ret);
	}
	msleep(100);

	//查询电压输出值
	switch(dev->devModel)
	{
	//	case DevModel::Model_ACLT_AN51:
	//		ret = AnRead(addr, 0xF0, 0x10, 10, values);
	//		break;
	case DevModel::Model_ACLT_AN51:
		//msleep(500);
		ret = AnRead(addr, 0xF0, 0x10, 10, values);
		break;
	default:
		break;
	}

	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.1;
		MemDb::SetRealData(regStart + 15, tmp, false);//电压输出值
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源电压输出值失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询电流输出值
	msleep(100);
	ret = AnRead(addr, 0xF0, 0x11, 11, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.01;
		MemDb::SetRealData(regStart + 16, tmp, false);//电流输出值
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源电流输出值失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询功率输出值
	msleep(100);
	ret = AnRead(addr, 0xF0, 0x12, 11, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.001 ;//* 1000;
		MemDb::SetRealData(regStart + 17, tmp, false);//功率输出值 单位kW
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源功率输出值失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询当前状态
	msleep(100);
	ret = AnRead(addr, 0xF0, 0xEB, 9, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		if (values[0] == 1)//待机状态
		{
			MemDb::SetRealData(regStart + 11, 1, false);//待机状态
		}
		else if (values[0] == 2)//启动状态
		{
			MemDb::SetRealData(regStart + 11, 2, false);//启动状态
		}
		else if (values[0] == 3)
		{
			tmp1 = tmp1 | 0x00000001;//hardfault报警
		}
		else if (values[0] == 4)
		{
			tmp1 = tmp1 | 0x00000002;//OVP报警
		}

		if(tmp1 > 0)
		{
			MemDb::SetRealData(regStart + 14, tmp1, false);//错误码
			MemDb::SetRealData(regStart + 13, 1, false);//错误状态
		}
		else
		{
			MemDb::SetRealData(regStart + 11, values[0], false);//当前状态
			MemDb::SetRealData(regStart + 13, 0, false);//无错误
		}
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源当前状态失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询设定电压值
	msleep(100);
	ret = AnRead(addr, 0xA5, 0x00, 10, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.1;
		MemDb::SetRealData(regStart + 3, tmp, false);//设定电压值
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源设定电压值失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询设定电流值
	msleep(100);
	ret = AnRead(addr, 0xA5, 0x01, 11, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.01;
		MemDb::SetRealData(regStart + 4, tmp, false);//设定电流值
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源设定电流值失败，结果=%d",dev->name.c_str(),ret);
	}
//	msleep(150);

	//查询设定功率值
	msleep(100);
	ret = AnRead(addr, 0xA5, 0x02, 11, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.001;
		MemDb::SetRealData(regStart + 5, tmp, false);//查询设定功率值
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源OVP（限压）失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询设定OVP（限压）值
	msleep(100);
	ret = AnRead(addr, 0xA5, 0x03, 10, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.1;
		MemDb::SetRealData(regStart + 6, tmp, false);//设定OVP（限压）值
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源OVP（限压）失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询设定开路电压（Voc）
	msleep(100);
	ret = AnRead(addr, 0xA5, 0x41, 10, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.1;
		MemDb::SetRealData(regStart + 7, tmp, false);//开路电压（Voc）
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源开路电压（Voc）失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询设定短路电流（Isc）
	msleep(100);
	ret = AnRead(addr, 0xA5, 0x42, 10, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.01;
		MemDb::SetRealData(regStart + 8, tmp, false);//短路电流（Isc）
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源短路电流（Isc）失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询设定最大功率点电压（Vmp）
	msleep(100);
	ret = AnRead(addr, 0xA5, 0x43, 10, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.1;
		MemDb::SetRealData(regStart + 9, tmp, false);//最大功率点电压(Vmp)
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源最大功率点电压（Vmp）失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);

	//查询设定最大功率点电流（Imp）
	msleep(100);
	ret = AnRead(addr, 0xA5, 0x44, 10, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.01;
		MemDb::SetRealData(regStart + 10, tmp, false);//最大功率点电流（Imp）
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源Imp失败，结果=%d",dev->name.c_str(),ret);
	}


	//查询设定EN50530设定值（EN50530）
	msleep(100);
	ret = AnRead(addr, 0xE5, 0x11, 22, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.01;
		MemDb::SetRealData(regStart + 45, tmp, false);//（EN50530）Vmp
		tmp = values[1] * 0.001;
		MemDb::SetRealData(regStart + 46, tmp, false);//（EN50530）Pmp
		tmp = values[2] ;
		MemDb::SetRealData(regStart + 47, tmp, false);//（EN50530）FF
		tmp = values[3] ;
		MemDb::SetRealData(regStart + 48, tmp, false);//（EN50530）Irr
		tmp = values[4] ;
		MemDb::SetRealData(regStart + 49, tmp, false);//（EN50530）启动缓升时间

	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电源Imp失败，结果=%d",dev->name.c_str(),ret);
	}

	MemDb::SetRealData(regStart + 0, comm, false);
	return ret;
}

int ANSimultor::AcltAnPreset(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"设置艾诺光伏电池模拟器数据");
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	RealData voc;
	RealData isc;
	RealData vmp;
	RealData imp;
	int data = 0;
	int temp = 0;
	int regStart = dev->regStart;

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	//设置光伏模拟器页面 00: 常规界面   01:序列  02：光伏sas  03:光伏 en50530
	if (MemDb::GetRealData(realData, regStart + 18, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
		values.push_back(realData.pv.data.iValue);
		zlog_warn(Util::m_zlog, "读取%s电源短路电流（Isc5555555555）失败，结果=%d",realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
		values.push_back(realData.pv.data.dbValue);
		zlog_warn(Util::m_zlog, "读取%s电源短路电流（Isc77777777777777）失败，结果=%f",realData.pv.data.dbValue);
		}
		ret = this->AnPreset(addr, 0x5A, 0x70, 9, values);
	}


	//判断是否有启停命令
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			values.clear();
			if (realData.pv.data.iValue == 1)//启动
			{
				ret = this->AnPreset(addr, 0x0F, 0xFF, 8, values);
			}
			else if (realData.pv.data.iValue == 0)//停止
			{
				ret = this->AnPreset(addr, 0x0F, 0x00, 8, values);
			}
			else if (realData.pv.data.iValue == 2)//解除报警
			{
				ret = this->AnPreset(addr, 0x0F, 0x03, 8, values);
			}
//			else if(realData.pv.data.iValue == 3)//光伏模拟
//			{
//				ret = this->AnPreset(addr, 0x0F, 0xA0, 8, values);
//			}
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_info(Util::m_zlog,"启停命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断是否电压设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 19, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 10);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 10);
		}
	
		ret = this->AnPreset(addr, 0x5A, 0x00, 10, values);
	}

	//判断是否电流设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 20, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 100);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 100);
		}
		ret = this->AnPreset(addr, 0x5A, 0x01, 11, values);
	}

	//判断是否功率设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 21, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 1000);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 1000);
		}
		ret = this->AnPreset(addr, 0x5A, 0x02, 10, values);
	}


#if 0
	//判断是否电压上限、下限设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 22, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 100);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 100);
		}
		ret = this->AnPreset(addr, 0x5A, 0x63, 12, values);
	}

	//判断是否电流上限、下限设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 24, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 100);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 100);
		}
		ret = this->AnPreset(addr, 0x5A, 0x64, 14, values);
	}
#endif

	//判断是否OVP设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 26, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 100);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 100);
		}
		ret = this->AnPreset(addr, 0x5A, 0x03, 10, values);
	}


	//判断是否Voc设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 27, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue *10);
			//zlog_warn(Util::m_zlog, "读取%s电源短路电流（Isc888888888888）失败，结果=%d",realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 10.0);
			//zlog_warn(Util::m_zlog, "读取%s电源短路电流（Isc77777777777777）失败，结果=%f",realData.pv.data.dbValue);
		}
		ret = this->AnPreset(addr, 0x5A, 0x41, 10, values);

		MemDb::GetRealData(data,11);//配合曲线下发功能
		if (data == 2)
		{
			values.clear();
			msleep(200);
			ret = this->AnPreset(addr, 0x0F, 0x0A, 8, values);
		}

	}

	//判断是否Vmp设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 29, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 10);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 10.0);
		}
		ret = this->AnPreset(addr, 0x5A, 0x42, 10, values);

		MemDb::GetRealData(data,11);//配合曲线下发功能
		if (data == 2)
		{
			values.clear();
			msleep(200);
			ret = this->AnPreset(addr, 0x0F, 0x0A, 8, values);
		}

	}

	//判断是否Isc设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 28, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 100);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 100.0);
		}
		ret = this->AnPreset(addr, 0x5A, 0x43, 10, values);

		MemDb::GetRealData(data,11);//配合曲线下发功能
		if (data == 2)
		{
			values.clear();
			msleep(200);
			ret = this->AnPreset(addr, 0x0F, 0x0A, 8, values);
		}
	}

	//判断是否Imp设定命令
	if (MemDb::GetRealDataWrited(realData, regStart + 30, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
			values.push_back(realData.pv.data.iValue * 100);
		}
		else if (realData.pv.type == RealData::DB)
		{
			values.push_back(realData.pv.data.dbValue * 100.0);
		}
		ret = this->AnPreset(addr, 0x5A, 0x44, 10, values);

		MemDb::GetRealData(data,11);//配合曲线下发功能
		if (data == 2)
		{
			values.clear();
			msleep(200);
			ret = this->AnPreset(addr, 0x0F, 0x0A, 8, values);
		}
	}

	//判断是否SAS设定命令下发
	if (MemDb::GetRealDataWrited(realData, regStart + 32, true))
	{
		values.clear();
		float Vmp = 0.0;
		float Imp = 0.0;
		float Voc = 0.0;
		float Isc = 0.0;
		float StepT = 0.0;
		if (realData.pv.data.iValue == 1)
		{
            MemDb::GetRealData(Voc, regStart + 33);
			values.push_back((int)(Voc * 100));
            MemDb::GetRealData(Isc, regStart + 34);
			values.push_back((int)(Isc * 100));
            MemDb::GetRealData(Vmp, regStart + 35);
			values.push_back((int)(Vmp * 100));
            MemDb::GetRealData(Imp, regStart + 36);
			values.push_back((int)(Imp * 100));
            MemDb::GetRealData(StepT, regStart + 37);
			values.push_back((int)(StepT * 10));
		}
		ret = this->AnPreset(addr, 0x5E, 0x01, 22, values);

//		MemDb::GetRealData(data,11);//配合曲线下发功能
//		if (data == 2)
//		{
//			values.clear();
//			msleep(200);
//			ret = this->AnPreset(addr, 0x0F, 0x0A, 8, values);
//		}
	}
	//判断是否EN50530设定命令下发
	if (MemDb::GetRealDataWrited(realData, regStart + 38, true))
	{
		values.clear();
		int FF = 0;
		int Irr = 0;
		float Vmp = 0.0;
		float Pmp = 0.0;
		float StepT = 0.0;
		if (realData.pv.data.iValue == 1)
		{
            MemDb::GetRealData(Vmp, regStart + 39);
			values.push_back((int)(Vmp * 100));
			MemDb::GetRealData(Pmp, regStart + 40);
			values.push_back((int)(Pmp * 1000));
            MemDb::GetRealData(FF, regStart + 41);
			values.push_back(FF);
			MemDb::GetRealData(Irr, regStart + 42);
			values.push_back(Irr);
            MemDb::GetRealData(StepT, regStart + 43);
			values.push_back((int)(StepT * 10));
		}
		ret = this->AnPreset(addr, 0x5E, 0x11, 22, values);

//		MemDb::GetRealData(data,11);//配合曲线下发功能
//		if (data == 2)
//		{
//			values.clear();
//			msleep(200);
//			ret = this->AnPreset(addr, 0x0F, 0x0A, 8, values);
//		}
	}


	zlog_info(Util::m_zlog,"设置光伏电池模拟器结束");
	return ret;
}

int ANSimultor::AnRead(int addr, int type, int data, short length,
		UshortArray& values)
{
	String query;//查询数组
	String reply;//响应数组
	int error = ErrorInfo::ERR_OK;
	unsigned short crc16;
	int byte = 0;
	int i = 0;
	int regLength = 8;

	//封装数据帧
	query.clear();
	query.push_back(0x7B);//起始帧
	query.push_back(regLength >> 8);//总字节数高字节
	query.push_back(regLength & 0xFF);//总字节数低字节
	query.push_back(addr);//从机地址
	query.push_back(type);//命令类型
	query.push_back(data);//命令字

	crc16 = Check::CheckSum(query);
	query.push_back(crc16);//校验位
	query.push_back(0x7D);//结束帧

	zlog_info(Util::m_zlog,"查询命令");

	values.clear();
	values.resize(length - 8);

//	printf("下发数据:");
//	for(int i = 0; i < query.size(); i++)
//	{
//		printf("%02x ", query[i]);
//	}
//	printf("\n");

	error = m_tcpClient->TxRxMsg(reply, query, length);

//	printf("上传数据:");
//	for(int i = 0; i < reply.size(); i++)
//	{
//		printf("%02x ", reply[i]);
//	}
//	printf("\n");


	if (error == ErrorInfo::ERR_OK)
	{
		if((reply[0] != 0x7B))
		{
			zlog_warn(Util::m_zlog,"反馈数据错误");
			return ErrorInfo::ERR_INV_RESP;
		}

		if (reply[4] == 0x99)
		{
			//错误码
			//0x01 校验和错误
			//0x02 命令类型错误
			//0x03 命令字错误
			//0x04 状态不符（当前的状态不允许执行接收的命令）
			//0x05 参数错误
			//0x06 保护报警
			//0x07 超量程范围
			//0x08 命令长度错误
			zlog_warn(Util::m_zlog,"查询命令错误，错误码：%d",reply[6]);
			error = ErrorInfo::ERR_INV_RESP;
		}
		else
		{
			if (reply.size() == 9)
			{
				values[0] = reply[6];
			}
			else if(reply.size() == 11)
			{
				values[0] = (reply[6] << 16) | (reply[7] << 8) | (reply[8]);
			}
			else if (reply.size() == 22)
			{
				values[0] = (reply[6] << 16) | (reply[7] << 8) | (reply[8]);//VMP
				values[1] = (reply[9] << 16) | (reply[10] << 8) | (reply[11]);//Pmp
				values[2] = (reply[12] << 16) | (reply[13] << 8) | (reply[14]);//FF
				values[3] = (reply[15] << 16) | (reply[16] << 8) | (reply[17]);//Irr
				values[4] = reply[18] ;//启动缓升时间
			}
			else
			{
				for (i = 6; i < length - 2; i += 2)
				{
					values[byte] = (reply[i] << 8) | (reply[i + 1]);
					byte++;
				}
			}
		}
	}

	return error;
}

int ANSimultor::AnPreset(int addr, int type, int data, short length,
		UshortArray values)
{
	int error = ErrorInfo::ERR_OK;
	String query;
	String reply;
	unsigned short crc16;
	int retry = 0;
	unsigned int i = 0;

	//封装控制类命令
	query.clear();
	query.push_back(0x7B);//起始帧
	query.push_back(length >> 8);//总字节数高字节
	query.push_back(length & 0xFF);//总字节数低字节
	query.push_back(addr);//从机地址
	query.push_back(type);//命令类型
	query.push_back(data);//命令字

	switch ( length )
	{
	case 9:
		query.push_back(values[0]);
		break;
	case 11:
		query.push_back(((int) (values[0])) >> 16);
		query.push_back(((int) (values[0])) >> 8);
		query.push_back(((int) (values[0])) & 0xFF);
		break;
	case 10:
	case 8:
		for (i = 0; i < values.size(); i++)
	    {
			query.push_back(((int) (values[i])) >> 8);
			query.push_back(((int) (values[i])) & 0xFF);
		}
		break;
	case 22:
			query.push_back(((int) (values[0])) >> 16);
			query.push_back(((int) (values[0])) >> 8);
			query.push_back(((int) (values[0])) & 0xFF);
			query.push_back(((int) (values[1])) >> 16);
			query.push_back(((int) (values[1])) >> 8);
			query.push_back(((int) (values[1])) & 0xFF);
			query.push_back(((int) (values[2])) >> 16);
			query.push_back(((int) (values[2])) >> 8);
			query.push_back(((int) (values[2])) & 0xFF);
			query.push_back(((int) (values[3])) >> 16);
			query.push_back(((int) (values[3])) >> 8);
			query.push_back(((int) (values[3])) & 0xFF);

			query.push_back(((int) (values[4])) >> 8);
			query.push_back(((int) (values[4])) & 0xFF);

		break;

	default:
		zlog_warn(Util::m_zlog,"未识别的命令字");
		break;
	}

//	for (i = 0; i < values.size(); i++)
//	//for (i = 0; i < 1; i++)
//	{
//		query.push_back(((int) (values[i])) >> 8);
//		query.push_back(((int) (values[i])) & 0xFF);
//	}
	crc16 = Check::CheckSum(query);
	query.push_back(crc16);//校验和
	query.push_back(0x7D);//结束帧

	zlog_info(Util::m_zlog,"查询命令");
	do
	{
		printf("下发数据:");
		for(int i = 0; i < query.size(); i++)
		{
			printf("%02x ", query[i]);
		}
		printf("\n");

		error = m_tcpClient->TxRxMsg(reply, query, 9);

		printf("上传数据:");
		for(int i = 0; i < reply.size(); i++)
		{
			printf("%02x ", reply[i]);
		}
		printf("\n");

		if (error == ErrorInfo::ERR_OK)
		{
			//判断应答地址,命令类型,命令是否与查询一致
			if((reply[0] != 0x7B))
			{
				zlog_warn(Util::m_zlog,"反馈数据错误");
				return ErrorInfo::ERR_INV_RESP;
			}
			if (reply[4] == 0x99)
			{
				//错误码
				//0x01 校验和错误
				//0x02 命令类型错误
				//0x03 命令字错误
				//0x04 状态不符（当前的状态不允许执行接收的命令）
				//0x05 参数错误
				//0x06 保护报警
				//0x07 超量程范围
				//0x08 命令长度错误
				zlog_warn(Util::m_zlog,"查询命令错误，错误码：%d",reply[6]);
				error = ErrorInfo::ERR_INV_RESP;
				break;
			}
			break;
		}
		else
		{
			usleep(m_delayTime * 1000);
		}
		retry++;
	} while ((retry < m_retries) && (ErrorInfo::ERR_OK != error));
	return error;
}
