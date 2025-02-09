/*
 * DiModle.cpp
 *
 *  Created on: 2014-9-16
 *      Author: liuhaitao
 */

#include "DiDoModle.h"

DiDoModle::DiDoModle()
{
	// TODO Auto-generated constructor stub
	m_isTcp = false;
	m_com = NULL;
	m_tcpClient = NULL;
}

DiDoModle::~DiDoModle()
{
	// TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void DiDoModle::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

void DiDoModle::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void DiDoModle::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

int DiDoModle::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	//判断设备型号
	switch(dev->devModel)
	{
	case DevModel::Model_HC_209:
		ret = this->HC209Read(dev);//惠测DI模块
		break;
	case DevModel::Model_HC_201:
		ret = this->HC201Read(dev);//惠测DI模块
		break;
	default:
		break;
	}

	return ret;
}

int DiDoModle::Preset(Device::SlaveDev* dev)
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
	case DevModel::Model_HC_202://hc202
		ret = this->HC202Preset(dev);
		break;
	default:
		break;
	};

	return ret;

}

int DiDoModle::HC202Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());

	switch(addr)
	{
	case 7:
	case 8:
		ret = ConfCabinet1Preset(dev);
		break;
	case 3:
	case 4:
		ret = ConfCabinet2Preset(dev);
		break;
	case 5:
	case 6:
		ret = ConfCabinet3Preset(dev);
		break;
	case 1:
	case 2:
		ret = ConfCabinet4Preset(dev);
		break;
	default:
		break;
	}

	return ret;

}

int DiDoModle::ConfCabinet1Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	UshortArray values;
	values.clear();

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	//判断地址
	if(addr == 7)
	{
	    //判断是否有S11命令下发
	    if (MemDb::GetRealData(realData, 17200, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 0, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 1, true);
	    	}
	    }

	    //判断是否有S12命令下发
	    if (MemDb::GetRealData(realData, 17201, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 3, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 2, true);
	    	}
	    }

	    //判断是否有port1命令下发
	    if (MemDb::GetRealData(realData, 17202, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 5, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 4, true);
	    	}
	    }

	    //判断是否有S13命令下发
	    if (MemDb::GetRealData(realData, 17203, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 6, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 7, true);
	    	}
	    }

	    //判断是否有S21命令下发
	    if (MemDb::GetRealData(realData, 17204, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 8, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 9, true);
	    	}
	    }

	    //判断是否有port2命令下发
	    if (MemDb::GetRealData(realData, 17205, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 11, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 10, true);
	    	}
	    }

	    //判断是否有S22命令下发
	    if (MemDb::GetRealData(realData, 17206, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 12, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 13, true);
	    	}
	    }
	}
	else if(addr == 8)
	{
	    //判断是否有S31命令下发
	    if (MemDb::GetRealData(realData, 17207, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 0, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 1, true);
	    	}
	    }

	    //判断是否有port3命令下发
	    if (MemDb::GetRealData(realData, 17208, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 3, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 2, true);
	    	}
	    }

	    //判断是否有S32命令下发
	    if (MemDb::GetRealData(realData, 17209, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 4, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 5, true);
	    	}
	    }

	    //判断是否有1#组态柜设备接入1命令下发
	    if (MemDb::GetRealData(realData, 17210, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 7, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 6, true);
	    	}
	    }

	    //判断是否有1#组态柜设备接入2命令下发
	    if (MemDb::GetRealData(realData, 17211, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 9, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 8, true);
	    	}
	    }

	    //判断是否有1#组态柜设备接入3命令下发
	    if (MemDb::GetRealData(realData, 17212, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 11, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 10, true);
	    	}
	    }
	}

	return ret;
}

int DiDoModle::ConfCabinet2Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	UshortArray values;
	values.clear();

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	//判断地址
	if(addr == 3)
	{
	    //判断是否有S41命令下发
	    if (MemDb::GetRealData(realData, 17213, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 0, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 1, true);
	    	}
	    }

	    //判断是否有port4命令下发
	    if (MemDb::GetRealData(realData, 17214, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 2, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 3, true);
	    	}
	    }

	    //判断是否有S42命令下发
	    if (MemDb::GetRealData(realData, 17215, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 4, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 5, true);
	    	}
	    }

	    //判断是否有S51命令下发
	    if (MemDb::GetRealData(realData, 17216, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 6, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 7, true);
	    	}
	    }

	    //判断是否有S52命令下发
	    if (MemDb::GetRealData(realData, 17217, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 8, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 9, true);
	    	}
	    }

	    //判断是否有port5命令下发
	    if (MemDb::GetRealData(realData, 17218, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 11, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 10, true);
	    	}
	    }

	    //判断是否有S53命令下发
	    if (MemDb::GetRealData(realData, 17219, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 12, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 13, true);
	    	}
	    }
	}
	else if(addr == 4)
	{
	    //判断是否有S61命令下发
	    if (MemDb::GetRealData(realData, 17220, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 0, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 1, true);
	    	}
	    }

	    //判断是否有port6命令下发
	    if (MemDb::GetRealData(realData, 17221, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 3, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 2, true);
	    	}
	    }

	    //判断是否有S62命令下发
	    if (MemDb::GetRealData(realData, 17222, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 4, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 5, true);
	    	}
	    }

	    //判断是否有2#组态柜设备接入1命令下发
	    if (MemDb::GetRealData(realData, 17223, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 7, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 6, true);
	    	}
	    }

	    //判断是否有2#组态柜设备接入2命令下发
	    if (MemDb::GetRealData(realData, 17224, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 9, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 8, true);
	    	}
	    }

	    //判断是否有2#组态柜设备接入3命令下发
	    if (MemDb::GetRealData(realData, 17225, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 11, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 10, true);
	    	}
	    }
	}

	return ret;
}

int DiDoModle::ConfCabinet3Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	UshortArray values;
	values.clear();

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	//判断地址
	if(addr == 5)
	{
	    //判断是否有S71命令下发
	    if (MemDb::GetRealData(realData, 17226, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 0, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 1, true);
	    	}
	    }

	    //判断是否有S72命令下发
	    if (MemDb::GetRealData(realData, 17227, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 2, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 3, true);
	    	}
	    }

	    //判断是否有port7命令下发
	    if (MemDb::GetRealData(realData, 17228, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 5, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 4, true);
	    	}
	    }

	    //判断是否有S73命令下发
	    if (MemDb::GetRealData(realData, 17229, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 6, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 7, true);
	    	}
	    }

	    //判断是否有S81命令下发
	    if (MemDb::GetRealData(realData, 17230, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 8, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 9, true);
	    	}
	    }

	    //判断是否有port8命令下发
	    if (MemDb::GetRealData(realData, 17231, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 11, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 10, true);
	    	}
	    }

	    //判断是否有S82命令下发
	    if (MemDb::GetRealData(realData, 17232, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 12, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 13, true);
	    	}
	    }
	}
	else if(addr == 6)
	{
	    //判断是否有S91命令下发
	    if (MemDb::GetRealData(realData, 17233, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 0, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 1, true);
	    	}
	    }

	    //判断是否有port9命令下发
	    if (MemDb::GetRealData(realData, 17234, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 3, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 2, true);
	    	}
	    }

	    //判断是否有S92命令下发
	    if (MemDb::GetRealData(realData, 17235, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 4, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 5, true);
	    	}
	    }

	    //判断是否有3#组态柜设备接入1命令下发
	    if (MemDb::GetRealData(realData, 17236, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 7, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 6, true);
	    	}
	    }

	    //判断是否有3#组态柜设备接入2命令下发
	    if (MemDb::GetRealData(realData, 17237, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 9, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 8, true);
	    	}
	    }

	    //判断是否有3#组态柜设备接入3命令下发
	    if (MemDb::GetRealData(realData, 17238, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 11, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 10, true);
	    	}
	    }
	}

	return ret;
}

int DiDoModle::ConfCabinet4Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	UshortArray values;
	values.clear();

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	//判断地址
	if(addr == 1)
	{
	    //判断是否有S101命令下发
	    if (MemDb::GetRealData(realData, 17239, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 0, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 1, true);
	    	}
	    }

	    //判断是否有port10命令下发
	    if (MemDb::GetRealData(realData, 17240, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 3, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 2, true);
	    	}
	    }

	    //判断是否有S102命令下发
	    if (MemDb::GetRealData(realData, 17241, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 4, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 5, true);
	    	}
	    }

	    //判断是否有S111命令下发
	    if (MemDb::GetRealData(realData, 17242, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 6, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 7, true);
	    	}
	    }

	    //判断是否有S112命令下发
	    if (MemDb::GetRealData(realData, 17243, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 9, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 8, true);
	    	}
	    }

	    //判断是否有port11命令下发
	    if (MemDb::GetRealData(realData, 17244, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 11, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 10, true);
	    	}
	    }

	    //判断是否有S113命令下发
	    if (MemDb::GetRealData(realData, 17245, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 12, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 13, true);
	    	}
	    }
	}
	else if(addr == 2)
	{
	    //判断是否有S121命令下发
	    if (MemDb::GetRealData(realData, 17246, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 0, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 1, true);
	    	}
	    }

	    //判断是否有port12命令下发
	    if (MemDb::GetRealData(realData, 17247, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 3, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 2, true);
	    	}
	    }

	    //判断是否有S122命令下发
	    if (MemDb::GetRealData(realData, 17248, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 4, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 5, true);
	    	}
	    }

	    //判断是否有4#组态柜设备接入1命令下发
	    if (MemDb::GetRealData(realData, 17249, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 7, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 6, true);
	    	}
	    }

	    //判断是否有4#组态柜设备接入2命令下发
	    if (MemDb::GetRealData(realData, 17250, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 9, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 8, true);
	    	}
	    }

	    //判断是否有4#组态柜设备接入3命令下发
	    if (MemDb::GetRealData(realData, 17251, true))
	    {
	    	if(realData.pv.data.iValue == 1)//闭合
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 11, true);
	    	}
	    	else if(realData.pv.data.iValue == 0)//断开
	    	{
	    		ret = rtu.ForceSingleCoil(addr, 10, true);
	    	}
	    }
	}

	return ret;
}

int DiDoModle::HC201Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"开始读取%s数据",dev->name.c_str());
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int start = 0x0000;
	int length = 16;
	UshortArray values;

	//判断设备是否为空
	if(dev == NULL)
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

	ret = rtu.ReadInputStatus(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK)
	{
		if(m_tcpClient->Getpeername().GetPort() == 4004)
		{
			switch(addr)
			{
			case 1:
				MemDb::SetRealData(17300, !((short)values[0]), false);//S11反馈
				MemDb::SetRealData(17301, !((short)values[1]), false);//S12反馈
				MemDb::SetRealData(17302, !((short)values[2]), false);//port1反馈
				MemDb::SetRealData(17303, !((short)values[3]), false);//S13反馈
				MemDb::SetRealData(17304, !((short)values[4]), false);//S21反馈
				MemDb::SetRealData(17305, !((short)values[5]), false);//port2反馈
				MemDb::SetRealData(17306, !((short)values[6]), false);//S22反馈
				MemDb::SetRealData(17307, !((short)values[7]), false);//S31反馈
				MemDb::SetRealData(17308, !((short)values[8]), false);//port3反馈
				MemDb::SetRealData(17309, !((short)values[9]), false);//S32反馈
				MemDb::SetRealData(17310, !((short)values[10]), false);//1#组态柜设备接入1反馈
				MemDb::SetRealData(17311, !((short)values[11]), false);//1#组态柜设备接入2反馈
				MemDb::SetRealData(17312, !((short)values[12]), false);//1#组态柜设备接入3反馈
				break;
			case 2:
				MemDb::SetRealData(17313, !((short)values[0]), false);//S41反馈
				MemDb::SetRealData(17314, !((short)values[1]), false);//port4反馈
				MemDb::SetRealData(17315, !((short)values[2]), false);//S42反馈
				MemDb::SetRealData(17316, !((short)values[3]), false);//S51反馈
				MemDb::SetRealData(17317, !((short)values[4]), false);//S52反馈
				MemDb::SetRealData(17318, !((short)values[5]), false);//port5反馈
				MemDb::SetRealData(17319, !((short)values[6]), false);//S53反馈
				MemDb::SetRealData(17320, !((short)values[7]), false);//S61反馈
				MemDb::SetRealData(17321, !((short)values[8]), false);//port6反馈
				MemDb::SetRealData(17322, !((short)values[9]), false);//S62反馈
				MemDb::SetRealData(17323, !((short)values[10]), false);//2#组态柜设备接入1反馈
				MemDb::SetRealData(17324, !((short)values[11]), false);//2#组态柜设备接入2反馈
				MemDb::SetRealData(17325, !((short)values[12]), false);//2#组态柜设备接入3反馈
				break;
			case 3:
				MemDb::SetRealData(17326, !((short)values[0]), false);//S71反馈
				MemDb::SetRealData(17327, !((short)values[1]), false);//S72反馈
				MemDb::SetRealData(17328, !((short)values[2]), false);//port7反馈
				MemDb::SetRealData(17329, !((short)values[3]), false);//S73反馈
				MemDb::SetRealData(17330, !((short)values[4]), false);//S81反馈
				MemDb::SetRealData(17331, !((short)values[5]), false);//port8反馈
				MemDb::SetRealData(17332, !((short)values[6]), false);//S82反馈
				MemDb::SetRealData(17333, !((short)values[7]), false);//S91反馈
				MemDb::SetRealData(17334, !((short)values[8]), false);//port9反馈
				MemDb::SetRealData(17335, !((short)values[9]), false);//S92反馈
				MemDb::SetRealData(17336, !((short)values[10]), false);//3#组态柜设备接入1反馈
				MemDb::SetRealData(17337, !((short)values[11]), false);//3#组态柜设备接入2反馈
				MemDb::SetRealData(17338, !((short)values[12]), false);//3#组态柜设备接入3反馈
				break;
			case 4:
				MemDb::SetRealData(17339, !((short)values[0]), false);//S101反馈
				MemDb::SetRealData(17340, !((short)values[1]), false);//port10反馈
				MemDb::SetRealData(17341, !((short)values[2]), false);//S102反馈
				MemDb::SetRealData(17342, !((short)values[3]), false);//S111反馈
				MemDb::SetRealData(17343, !((short)values[4]), false);//S112反馈
				MemDb::SetRealData(17344, !((short)values[5]), false);//port11反馈
				MemDb::SetRealData(17345, !((short)values[6]), false);//S113反馈
				MemDb::SetRealData(17346, !((short)values[7]), false);//S121反馈
				MemDb::SetRealData(17347, !((short)values[8]), false);//port12反馈
				MemDb::SetRealData(17348, !((short)values[9]), false);//S122反馈
				MemDb::SetRealData(17349, !((short)values[10]), false);//4#组态柜设备接入1反馈
				MemDb::SetRealData(17350, !((short)values[11]), false);//4#组态柜设备接入2反馈
				MemDb::SetRealData(17351, !((short)values[12]), false);//4#组态柜设备接入3反馈
				break;
			default:
				break;
			}
		}
		else if(m_tcpClient->Getpeername().GetPort() == 4002)
		{
			switch(addr)
			{
			case 1:
				MemDb::SetRealData(17352, !((short)values[0]), false);//交流配电柜主开关反馈
				MemDb::SetRealData(17353, !((short)values[1]), false);//电网反馈
				MemDb::SetRealData(17354, !((short)values[2]), false);//电网模拟器输出开关反馈
				MemDb::SetRealData(17355, !((short)values[3]), false);//检测区开关反馈
				MemDb::SetRealData(17356, !((short)values[4]), false);//DC/AC变换器2开关反馈
				break;
			case 4:
				MemDb::SetRealData(17357, !((short)values[0]), false);//交直流配电柜主开关
				break;
			default:
				break;
			}
		}
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s数据失败",dev->name.c_str());
	}


	return ret;
}

int DiDoModle::HC209Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"开始读取%s数据",dev->name.c_str());
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 8;
	UshortArray values;

	//判断设备是否为空
	if(dev == NULL)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if(!(m_com->IsOpen()))
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster modbus;
	modbus.SetCom(m_com);

	ret = modbus.ReadInputStatus(values, addr, start, length);

	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		//开关模块读取的断路器状态和实际相反（接常开点的情况下）
		MemDb::SetRealData(regStart + 1, !(values[0]), false);//QF14状态
		MemDb::SetRealData(regStart + 2, !(values[1]), false);//QF15状态
		MemDb::SetRealData(regStart + 3, !(values[2]), false);//QF16状态
		MemDb::SetRealData(regStart + 4, !(values[3]), false);//QF17状态
		MemDb::SetRealData(regStart + 5, !(values[4]), false);//QF18状态
		MemDb::SetRealData(regStart + 6, !(values[5]), false);//QF19状态
		MemDb::SetRealData(regStart + 7, !(values[6]), false);//QF20状态
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s数据失败",dev->name.c_str());
	}

	MemDb::SetRealData(regStart + 0, comm, false);

	zlog_info(Util::m_zlog,"读取%s数据结束",dev->name.c_str());

	return ret;
}
