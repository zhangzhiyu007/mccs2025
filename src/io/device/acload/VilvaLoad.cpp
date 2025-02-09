/*
 * VilvaLoad.cpp
 *
 *  创建日期: 2016-5-30
 *  作          者: 刘海涛
 */

#include "VilvaLoad.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

VilvaLoad::VilvaLoad()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_ctrlCom = NULL;
	m_value = 0;
}

VilvaLoad::~VilvaLoad()
{
	// TODO Auto-generated destructor stub
}

void VilvaLoad::SetCom(Com* com)
{
	m_com = com;
}

void VilvaLoad::SetCtrlCom(Com* ctrlCom)
{
	m_ctrlCom = ctrlCom;
}

int VilvaLoad::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_VILVA_30K:
		ret = this->VilvaRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int VilvaLoad::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_VILVA_30K:
		ret = this->VilvaPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int VilvaLoad::VilvaRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 42;
	int length = 28;

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
	FloatArray values;
	values.clear();

	ret = rtu.ReadOutputFloatRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::SetRealData(regStart + 46, values[0], false);//A相电压
		MemDb::SetRealData(regStart + 47, values[1], false);//B相电压
		MemDb::SetRealData(regStart + 48, values[2], false);//C相电压
		MemDb::SetRealData(regStart + 49, values[3], false);//A相电流
		MemDb::SetRealData(regStart + 50, values[4], false);//B相电流
		MemDb::SetRealData(regStart + 51, values[5], false);//C相电流
		MemDb::SetRealData(regStart + 40, values[6], false);//A相有功
		MemDb::SetRealData(regStart + 41, values[10], false);//A相无功
		MemDb::SetRealData(regStart + 42, values[7], false);//B相有功
		MemDb::SetRealData(regStart + 43, values[11], false);//B相无功
		MemDb::SetRealData(regStart + 44, values[8], false);//C相有功
		MemDb::SetRealData(regStart + 45, values[12], false);//C相无功
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int VilvaLoad::VilvaPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	RealData realData;
	float tmp = 0;

	//判断是否有加载/卸载命令下发
	if (MemDb::GetRealData(realData, regStart + 1, true))
	{
		if(realData.pv.data.iValue == 1)
		{
			//加载
			Util::SetBitValue(m_value, 13, true);
			MemDb::SetRealData(regStart + 36, 1, false);//运行
		}
		else if(realData.pv.data.dbValue == 0)
		{
			//卸载
			Util::SetBitValue(m_value, 13, false);
			Util::SetBitValue(m_value, 0, false);//A相1kW
			Util::SetBitValue(m_value, 1, false);//A相2kW
			Util::SetBitValue(m_value, 2, false);//A相2kW
			Util::SetBitValue(m_value, 3, false);//A相5kW
			Util::SetBitValue(m_value, 4, false);//B相1kW
			Util::SetBitValue(m_value, 5, false);//B相2kW
			Util::SetBitValue(m_value, 6, false);//B相2kW
			Util::SetBitValue(m_value, 7, false);//B相5kW
			Util::SetBitValue(m_value, 8, false);//C相1kW
			Util::SetBitValue(m_value, 9, false);//C相2kW
			Util::SetBitValue(m_value, 10, false);//C相2kW
			Util::SetBitValue(m_value, 11, false);//C相5kW
			MemDb::SetRealData(regStart + 36, 0, false);//停止
		}

		ret = PlcPackData(m_value);
	}

	//判断是否有电源开关	命令下发
	if (MemDb::GetRealData(realData, regStart + 2, true))
	{
		if(realData.pv.data.iValue == 1)
		{
			//开
			Util::SetBitValue(m_value, 12, true);
		}
		else if(realData.pv.data.iValue == 0)
		{
			//关
			Util::SetBitValue(m_value, 12, false);
		}
		ret = PlcPackData(m_value);
	}

	//判断是否有A相有功功率命令下发
	if (MemDb::GetRealData(realData, regStart + 5, true))
	{
		tmp = realData.pv.data.dbValue;
		if((tmp > 0) && (tmp < 1500))//1kW
		{
			Util::SetBitValue(m_value, 0, true);//A相1kW
			Util::SetBitValue(m_value, 1, false);//A相2kW
			Util::SetBitValue(m_value, 2, false);//A相2kW
			Util::SetBitValue(m_value, 3, false);//A相5kW
		}
		else if((tmp >= 1500) && (tmp < 2500))//2kW
		{
			Util::SetBitValue(m_value, 0, false);//A相1kW
			Util::SetBitValue(m_value, 1, true);//A相2kW
			Util::SetBitValue(m_value, 2, false);//A相2kW
			Util::SetBitValue(m_value, 3, false);//A相5kW
		}
		else if((tmp >= 2500) && (tmp < 3500))//3kW
		{
			Util::SetBitValue(m_value, 0, true);//A相1kW
			Util::SetBitValue(m_value, 1, true);//A相2kW
			Util::SetBitValue(m_value, 2, false);//A相2kW
			Util::SetBitValue(m_value, 3, false);//A相5kW
		}
		else if((tmp >= 3500) && (tmp < 4500))//4kW
		{
			Util::SetBitValue(m_value, 0, false);//A相1kW
			Util::SetBitValue(m_value, 1, true);//A相2kW
			Util::SetBitValue(m_value, 2, true);//A相2kW
			Util::SetBitValue(m_value, 3, false);//A相5kW
		}
		else if((tmp >= 4500) && (tmp < 5500))//5kW
		{
			Util::SetBitValue(m_value, 0, true);//A相1kW
			Util::SetBitValue(m_value, 1, true);//A相2kW
			Util::SetBitValue(m_value, 2, true);//A相2kW
			Util::SetBitValue(m_value, 3, false);//A相5kW
		}
		else if((tmp >= 5500) && (tmp < 6500))//6kW
		{
			Util::SetBitValue(m_value, 0, true);//A相1kW
			Util::SetBitValue(m_value, 1, false);//A相2kW
			Util::SetBitValue(m_value, 2, false);//A相2kW
			Util::SetBitValue(m_value, 3, true);//A相5kW
		}
		else if((tmp >= 6500) && (tmp < 7500))//7kW
		{
			Util::SetBitValue(m_value, 0, false);//A相1kW
			Util::SetBitValue(m_value, 1, true);//A相2kW
			Util::SetBitValue(m_value, 2, false);//A相2kW
			Util::SetBitValue(m_value, 3, true);//A相5kW
		}
		else if((tmp >= 7500) && (tmp < 8500))//8kW
		{
			Util::SetBitValue(m_value, 0, true);//A相1kW
			Util::SetBitValue(m_value, 1, true);//A相2kW
			Util::SetBitValue(m_value, 2, false);//A相2kW
			Util::SetBitValue(m_value, 3, true);//A相5kW
		}
		else if((tmp >= 8500) && (tmp < 9500))//9kW
		{
			Util::SetBitValue(m_value, 0, false);//A相1kW
			Util::SetBitValue(m_value, 1, true);//A相2kW
			Util::SetBitValue(m_value, 2, true);//A相2kW
			Util::SetBitValue(m_value, 3, true);//A相5kW
		}
		else if(tmp >= 9500)//10kW
		{
			Util::SetBitValue(m_value, 0, true);//A相1kW
			Util::SetBitValue(m_value, 1, true);//A相2kW
			Util::SetBitValue(m_value, 2, true);//A相2kW
			Util::SetBitValue(m_value, 3, true);//A相5kW
		}
		else
		{
			Util::SetBitValue(m_value, 0, false);//A相1kW
			Util::SetBitValue(m_value, 1, false);//A相2kW
			Util::SetBitValue(m_value, 2, false);//A相2kW
			Util::SetBitValue(m_value, 3, false);//A相5kW
		}

		ret = PlcPackData(m_value);
	}

	sleep(2);
	//判断是否有B相有功功率命令下发
	if (MemDb::GetRealData(realData, regStart + 8, true))
	{
		tmp = realData.pv.data.dbValue;
		if((tmp > 0) && (tmp < 1500))//1kW
		{
			Util::SetBitValue(m_value, 4, true);//B相1kW
			Util::SetBitValue(m_value, 5, false);//B相2kW
			Util::SetBitValue(m_value, 6, false);//B相2kW
			Util::SetBitValue(m_value, 7, false);//B相5kW
		}
		else if((tmp >= 1500) && (tmp < 2500))//2kW
		{
			Util::SetBitValue(m_value, 4, false);//B相1kW
			Util::SetBitValue(m_value, 5, true);//B相2kW
			Util::SetBitValue(m_value, 6, false);//B相2kW
			Util::SetBitValue(m_value, 7, false);//B相5kW
		}
		else if((tmp >= 2500) && (tmp < 3500))//3kW
		{
			Util::SetBitValue(m_value, 4, true);//B相1kW
			Util::SetBitValue(m_value, 5, true);//B相2kW
			Util::SetBitValue(m_value, 6, false);//B相2kW
			Util::SetBitValue(m_value, 7, false);//B相5kW
		}
		else if((tmp >= 3500) && (tmp < 4500))//4kW
		{
			Util::SetBitValue(m_value, 4, false);//B相1kW
			Util::SetBitValue(m_value, 5, true);//B相2kW
			Util::SetBitValue(m_value, 6, true);//B相2kW
			Util::SetBitValue(m_value, 7, false);//B相5kW
		}
		else if((tmp >= 4500) && (tmp < 5500))//5kW
		{
			Util::SetBitValue(m_value, 4, true);//B相1kW
			Util::SetBitValue(m_value, 5, true);//B相2kW
			Util::SetBitValue(m_value, 6, true);//B相2kW
			Util::SetBitValue(m_value, 7, false);//B相5kW
		}
		else if((tmp >= 5500) && (tmp < 6500))//6kW
		{
			Util::SetBitValue(m_value, 4, true);//B相1kW
			Util::SetBitValue(m_value, 5, false);//B相2kW
			Util::SetBitValue(m_value, 6, false);//B相2kW
			Util::SetBitValue(m_value, 7, true);//B相5kW
		}
		else if((tmp >= 6500) && (tmp < 7500))//7kW
		{
			Util::SetBitValue(m_value, 4, false);//B相1kW
			Util::SetBitValue(m_value, 5, true);//B相2kW
			Util::SetBitValue(m_value, 6, false);//B相2kW
			Util::SetBitValue(m_value, 7, true);//B相5kW
		}
		else if((tmp >= 7500) && (tmp < 8500))//8kW
		{
			Util::SetBitValue(m_value, 4, true);//B相1kW
			Util::SetBitValue(m_value, 5, true);//B相2kW
			Util::SetBitValue(m_value, 6, false);//B相2kW
			Util::SetBitValue(m_value, 7, true);//B相5kW
		}
		else if((tmp >= 8500) && (tmp < 9500))//9kW
		{
			Util::SetBitValue(m_value, 4, false);//B相1kW
			Util::SetBitValue(m_value, 5, true);//B相2kW
			Util::SetBitValue(m_value, 6, true);//B相2kW
			Util::SetBitValue(m_value, 7, true);//B相5kW
		}
		else if(tmp >= 9500)//10kW
		{
			Util::SetBitValue(m_value, 4, true);//B相1kW
			Util::SetBitValue(m_value, 5, true);//B相2kW
			Util::SetBitValue(m_value, 6, true);//B相2kW
			Util::SetBitValue(m_value, 7, true);//B相5kW
		}
		else
		{
			Util::SetBitValue(m_value, 4, false);//B相1kW
			Util::SetBitValue(m_value, 5, false);//B相2kW
			Util::SetBitValue(m_value, 6, false);//B相2kW
			Util::SetBitValue(m_value, 7, false);//B相5kW
		}

		ret = PlcPackData(m_value);
	}

	sleep(2);

	//判断是否有C相有功功率命令下发
	if (MemDb::GetRealData(realData, regStart + 11, true))
	{
		tmp = realData.pv.data.dbValue;
		if((tmp > 0) && (tmp < 1500))//1kW
		{
			Util::SetBitValue(m_value, 8, true);//C相1kW
			Util::SetBitValue(m_value, 9, false);//C相2kW
			Util::SetBitValue(m_value, 10, false);//C相2kW
			Util::SetBitValue(m_value, 11, false);//C相5kW
		}
		else if((tmp >= 1500) && (tmp < 2500))//2kW
		{
			Util::SetBitValue(m_value, 8, false);//C相1kW
			Util::SetBitValue(m_value, 9, true);//C相2kW
			Util::SetBitValue(m_value, 10, false);//C相2kW
			Util::SetBitValue(m_value, 11, false);//C相5kW
		}
		else if((tmp >= 2500) && (tmp < 3500))//3kW
		{
			Util::SetBitValue(m_value, 8, true);//C相1kW
			Util::SetBitValue(m_value, 9, true);//C相2kW
			Util::SetBitValue(m_value, 10, false);//C相2kW
			Util::SetBitValue(m_value, 11, false);//C相5kW
		}
		else if((tmp >= 3500) && (tmp < 4500))//4kW
		{
			Util::SetBitValue(m_value, 8, false);//C相1kW
			Util::SetBitValue(m_value, 9, true);//C相2kW
			Util::SetBitValue(m_value, 10, true);//C相2kW
			Util::SetBitValue(m_value, 11, false);//C相5kW
		}
		else if((tmp >= 4500) && (tmp < 5500))//5kW
		{
			Util::SetBitValue(m_value, 8, true);//C相1kW
			Util::SetBitValue(m_value, 9, true);//C相2kW
			Util::SetBitValue(m_value, 10, true);//C相2kW
			Util::SetBitValue(m_value, 11, false);//C相5kW
		}
		else if((tmp >= 5500) && (tmp < 6500))//6kW
		{
			Util::SetBitValue(m_value, 8, true);//C相1kW
			Util::SetBitValue(m_value, 9, false);//C相2kW
			Util::SetBitValue(m_value, 10, false);//C相2kW
			Util::SetBitValue(m_value, 11, true);//C相5kW
		}
		else if((tmp >= 6500) && (tmp < 7500))//7kW
		{
			Util::SetBitValue(m_value, 8, false);//C相1kW
			Util::SetBitValue(m_value, 9, true);//C相2kW
			Util::SetBitValue(m_value, 10, false);//C相2kW
			Util::SetBitValue(m_value, 11, true);//C相5kW
		}
		else if((tmp >= 7500) && (tmp < 8500))//8kW
		{
			Util::SetBitValue(m_value, 8, true);//C相1kW
			Util::SetBitValue(m_value, 9, true);//C相2kW
			Util::SetBitValue(m_value, 10, false);//C相2kW
			Util::SetBitValue(m_value, 11, true);//C相5kW
		}
		else if((tmp >= 8500) && (tmp < 9500))//9kW
		{
			Util::SetBitValue(m_value, 8, false);//C相1kW
			Util::SetBitValue(m_value, 9, true);//C相2kW
			Util::SetBitValue(m_value, 10, true);//C相2kW
			Util::SetBitValue(m_value, 11, true);//C相5kW
		}
		else if(tmp >= 9500)//10kW
		{
			Util::SetBitValue(m_value, 8, true);//C相1kW
			Util::SetBitValue(m_value, 9, true);//C相2kW
			Util::SetBitValue(m_value, 10, true);//C相2kW
			Util::SetBitValue(m_value, 11, true);//C相5kW
		}
		else
		{
			Util::SetBitValue(m_value, 8, false);//C相1kW
			Util::SetBitValue(m_value, 9, false);//C相2kW
			Util::SetBitValue(m_value, 10, false);//C相2kW
			Util::SetBitValue(m_value, 11, false);//C相5kW
		}

		ret = PlcPackData(m_value);
	}
	return ret;
}

int VilvaLoad::PlcPackData(int value)
{
	int ret = ErrorInfo::ERR_OK;
	String query;//查询字符串
	String reply;//应答字符串
	String ctrlArray;//控制数组
	int sum = 0;
	int tmp = 0;
	char temp[2];

	query.clear();
	reply.clear();
	ctrlArray.resize(24);
	for(unsigned int i = 0; i < ctrlArray.size(); i++)
	{
		ctrlArray[i] = 0x30;
	}

	for(unsigned int i = 0; i < 14; i++)
	{
		if(Util::GetBitValue(value, i) == 1)
		{
			ctrlArray[i] = 0x31;
		}
	}

	//标准头
	query.push_back(0x05);
	query.push_back(0x30);
	query.push_back(0x30);
	query.push_back(0x46);
	query.push_back(0x46);
	query.push_back(0x42);
	query.push_back(0x57);
	query.push_back(0x31);
	query.push_back(0x59);
	query.push_back(0x30);
	query.push_back(0x30);
	query.push_back(0x30);
	query.push_back(0x30);
	query.push_back(0x31);
	query.push_back(0x38);

	for(unsigned int i = 0; i < ctrlArray.size(); i++)
	{
		query.push_back(ctrlArray[i]);
	}

	//计算校验和
	for(unsigned int i = 1; i < query.size(); i++)
	{
		sum += query.at(i);
	}
	tmp = sum & 0xFF;

	sprintf(temp,"%X",tmp);
	query.push_back(temp[0]);
	query.push_back(temp[1]);

	//发送数据
	//判断串口是否打开
	if (!m_ctrlCom->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ret = m_ctrlCom->TxRxMsg(reply,query,2);
	return ret;
}
