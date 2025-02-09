/*
 * BipolarAcdc.cpp
 *
 *  创建日期: 2019-8-1
 *  作          者: 刘海涛
 */

#include "BipolarAcdc.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

BipolarAcdc::BipolarAcdc()
{
	// TODO Auto-generated constructor stub
	m_isTcp = false;
	m_com = NULL;
	m_tcpClient = NULL;
}

BipolarAcdc::~BipolarAcdc()
{
	// TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void BipolarAcdc::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

void BipolarAcdc::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void BipolarAcdc::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int BipolarAcdc::Read(Device::SlaveDev* dev)
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
	case DevModel::Model_ADG_B40075://双极性AC/DC
		ret = this->BipolarAcdcRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int BipolarAcdc::Preset(Device::SlaveDev* dev)
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
	case DevModel::Model_ADG_B40075://双极性AC/DC
		ret = this->BipolarAcdcPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int BipolarAcdc::BipolarAcdcRead(Device::SlaveDev* dev)
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

	String values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 0;
	int data = 0;
	float tmp = 0.0;
	int fault = 0;

	start = 0;
	length = 12;
	//读取电压电流数据
	ret = rtu.ReadBipolarRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		tmp = ((values[0] * 100) + (values[1]) + (values[2] * 0.01) + (values[3] * 0.0001));
		MemDb::SetRealData(regStart + 17, tmp, false);//输出电压

		tmp = ((values[4] * 100) + (values[5]) + (values[6] * 0.01) + (values[7] * 0.0001));
		MemDb::SetRealData(regStart + 18, tmp, false);//输出电流

		tmp = ((values[8] * 100) + (values[9]) + (values[10] * 0.01) + (values[11] * 0.0001)) * 1000.0;
		MemDb::SetRealData(regStart + 19, tmp, false);//输出功率
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取%s电压电流数据失败",dev->name.c_str());
	}
#if 1
	start = 0x0018;
	length = 1;
	//读取状态数据
	ret = rtu.ReadBipolarRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		data = Util::GetBitValue(values[0], 3);
		MemDb::SetRealData(regStart + 13, data, false);//工作状态

		if(Util::GetBitValue(values[0], 0))
		{
			MemDb::SetRealData(regStart + 14, 2, false);//定电压模式
		}
		else if(Util::GetBitValue(values[0], 1))
		{
			MemDb::SetRealData(regStart + 14, 1, false);//定电流模式
		}
		else if(Util::GetBitValue(values[0], 2))
		{
			MemDb::SetRealData(regStart + 14, 0, false);//定功率模式
		}
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取%s状态数据数据失败",dev->name.c_str());
	}

	start = 0x0019;
	length = 3;
	fault = 0;
	//读取异常状态数据
	ret = rtu.ReadBipolarRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		for(int i = 0; i < 8; i++)
		{
			Util::SetBitValue(fault, i, Util::GetBitValue(values[0], i));
		}

		for(int i = 0; i < 8; i++)
		{
			Util::SetBitValue(fault, i + 8, Util::GetBitValue(values[1], i));
		}

		for(int i = 0; i < 8; i++)
		{
			Util::SetBitValue(fault, i + 16, Util::GetBitValue(values[1], i));
		}
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取%s异常状态数据数据失败",dev->name.c_str());
	}

	start = 0x0838;
	length = 24;
	//读取限制值数据
	ret = rtu.ReadBipolarRegisters(values,addr,start,length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		tmp = ((values[0] * 100) + (values[1]) + (values[2] * 0.01) + (values[3] * 0.0001));
		MemDb::SetRealData(regStart + 20, tmp, false);//设定电压最大值

		tmp = ((values[4] * 100) + (values[5]) + (values[6] * 0.01) + (values[7] * 0.0001));
		MemDb::SetRealData(regStart + 21, tmp, false);//设定电压最小值

		tmp = ((values[8] * 100) + (values[9]) + (values[10] * 0.01) + (values[11] * 0.0001));
		MemDb::SetRealData(regStart + 22, tmp, false);//设定电流最大值

		tmp = ((values[12] * 100) + (values[13]) + (values[14] * 0.01) + (values[15] * 0.0001));
		MemDb::SetRealData(regStart + 23, tmp, false);//设定电流最小值

		tmp = ((values[16] * 100) + (values[17]) + (values[18] * 0.01) + (values[19] * 0.0001)) * 1000.0;
		MemDb::SetRealData(regStart + 24, tmp, false);//设定功率最大值

		tmp = ((values[20] * 100) + (values[21]) + (values[22] * 0.01) + (values[23] * 0.0001)) * 1000.0;
		MemDb::SetRealData(regStart + 25, tmp, false);//设定功率最小值
	}
	else
	{
		comm = false;
		zlog_warn(Util::m_zlog,"读取%s限制值数据失败",dev->name.c_str());
	}
#endif
	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int BipolarAcdc::BipolarAcdcPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	RealData realData;
	String values;
	values.clear();
	int data = 0;
	int byte1 = 0;
	int byte2 = 0;
	int byte3 = 0;
	int byte4 = 0;
	float tmp = 0;
	float temp = 0.0;
	float volMax = 0.0;
	float volMin = 0.0;
	float curMax = 0.0;
	float curMin = 0.0;
	float powerMax = 0.0;
	float powerMin = 0.0;

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

    //判断是否有启停设置命令下发
    if (MemDb::GetRealData(realData, regStart + 2, true))
    {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = rtu.PresetBipolarRegisters(addr, 0x0118, 1, values);
    }

    //判断是否有远端控制设置命令下发
    if (MemDb::GetRealData(realData, regStart + 1, true))
    {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = rtu.PresetBipolarRegisters(addr, 0x0138, 1, values);
    }

    //判断是否有恒定功率模式设置命令下发
    if (MemDb::GetRealData(realData, regStart + 3, true))
    {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = rtu.PresetBipolarRegisters(addr, 0x012F, 1, values);
    }

    //判断是否有故障复位设置命令下发
    if (MemDb::GetRealData(realData, regStart + 4, true))
    {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = rtu.PresetBipolarRegisters(addr, 0x012D, 1, values);
    }

    //判断是否有电压设定设置命令下发
    if (MemDb::GetRealData(realData, regStart + 5, true))
    {
    	byte1 = 0;
    	byte2 = 0;
    	byte3 = 0;
    	byte4 = 0;
    	values.clear();
        tmp = realData.pv.data.dbValue;

        //获取电压设定最大值/最小值
        MemDb::GetRealData(volMax, regStart + 20);
        MemDb::GetRealData(volMin, regStart + 21);
        if(tmp < volMin)
        {
        	tmp = volMin;
        }

        if(tmp > volMax)
        {
        	tmp = volMax;
        }

        byte1 = (int)(tmp / 100);
        values.push_back(byte1);
        byte2 = (int)(tmp - (byte1 * 100));
        values.push_back(byte2);
        temp = (tmp - (byte1 * 100) - byte2) * 100;
        byte3 = (int)(temp);
        values.push_back(byte3);
        byte4 = (int)((temp - byte3) * 100);
        ret = rtu.PresetBipolarRegisters(addr, 0x0100, 4, values);
    }

    //判断是否有电流设定设置命令下发
    if (MemDb::GetRealData(realData, regStart + 6, true))
    {
    	byte1 = 0;
    	byte2 = 0;
    	byte3 = 0;
    	byte4 = 0;
    	values.clear();
        tmp = realData.pv.data.dbValue;

        //获取电压设定最大值/最小值
        MemDb::GetRealData(curMax, regStart + 22);
        MemDb::GetRealData(curMin, regStart + 23);
        if(tmp < curMin)
        {
        	tmp = curMin;
        }

        if(tmp > curMax)
        {
        	tmp = curMax;
        }

        byte1 = (int)(tmp / 100);
        values.push_back(byte1);
        byte2 = (int)(tmp - (byte1 * 100));
        values.push_back(byte2);
        temp = (tmp - (byte1 * 100) - byte2) * 100;
        byte3 = (int)(temp);
        values.push_back(byte3);
        byte4 = (int)((temp - byte3) * 100);
        ret = rtu.PresetBipolarRegisters(addr, 0x0104, 4, values);
    }

    //判断是否有功率设定设置命令下发
    if (MemDb::GetRealData(realData, regStart + 7, true))
    {
    	byte1 = 0;
    	byte2 = 0;
    	byte3 = 0;
    	byte4 = 0;
    	values.clear();
        tmp = realData.pv.data.dbValue;

        //获取电压设定最大值/最小值
        MemDb::GetRealData(powerMax, regStart + 24);
        MemDb::GetRealData(powerMin, regStart + 25);
        if(tmp < powerMin)
        {
        	tmp = powerMin;
        }

        if(tmp > powerMax)
        {
        	tmp = powerMax;
        }

        tmp = tmp * 0.001;

        byte1 = (int)(tmp / 100);
        values.push_back(byte1);
        byte2 = (int)(tmp - (byte1 * 100));
        values.push_back(byte2);
        temp = (tmp - (byte1 * 100) - byte2) * 100;
        byte3 = (int)(temp);
        values.push_back(byte3);
        byte4 = (int)((temp - byte3) * 100);
        ret = rtu.PresetBipolarRegisters(addr, 0x0108, 4, values);
    }

    //判断是否有过压保护点设置命令下发
    if (MemDb::GetRealData(realData, regStart + 8, true))
    {
    	byte1 = 0;
    	byte2 = 0;
    	byte3 = 0;
    	byte4 = 0;
    	values.clear();
        tmp = realData.pv.data.dbValue;

        byte1 = (int)(tmp / 100);
        values.push_back(byte1);
        byte2 = (int)(tmp - (byte1 * 100));
        values.push_back(byte2);
        temp = (tmp - (byte1 * 100) - byte2) * 100;
        byte3 = (int)(temp);
        values.push_back(byte3);
        byte4 = (int)((temp - byte3) * 100);
        ret = rtu.PresetBipolarRegisters(addr, 0x0121, 4, values);
    }

    //判断是否有过流保护点设置命令下发
    if (MemDb::GetRealData(realData, regStart + 9, true))
    {
    	byte1 = 0;
    	byte2 = 0;
    	byte3 = 0;
    	byte4 = 0;
    	values.clear();
        tmp = realData.pv.data.dbValue;

        byte1 = (int)(tmp / 100);
        values.push_back(byte1);
        byte2 = (int)(tmp - (byte1 * 100));
        values.push_back(byte2);
        temp = (tmp - (byte1 * 100) - byte2) * 100;
        byte3 = (int)(temp);
        values.push_back(byte3);
        byte4 = (int)((temp - byte3) * 100);
        ret = rtu.PresetBipolarRegisters(addr, 0x0125, 4, values);
    }

    //判断是否有过功率保护点设置命令下发
    if (MemDb::GetRealData(realData, regStart + 10, true))
    {
    	byte1 = 0;
    	byte2 = 0;
    	byte3 = 0;
    	byte4 = 0;
    	values.clear();
        tmp = realData.pv.data.dbValue * 0.001;

        byte1 = (int)(tmp / 100);
        values.push_back(byte1);
        byte2 = (int)(tmp - (byte1 * 100));
        values.push_back(byte2);
        temp = (tmp - (byte1 * 100) - byte2) * 100;
        byte3 = (int)(temp);
        values.push_back(byte3);
        byte4 = (int)((temp - byte3) * 100);
        ret = rtu.PresetBipolarRegisters(addr, 0x0129, 4, values);
    }

	return ret;
}

