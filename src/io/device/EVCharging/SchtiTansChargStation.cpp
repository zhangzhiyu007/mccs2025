/*
 * SchtiTansChargStation.cpp
 *
 *  创建日期: 2016-4-15
 *  作          者: 刘海涛
 */

#include "SchtiTansChargStation.h"

SchtiTansChargStation::SchtiTansChargStation()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
	m_startTimer.Initialize();
	m_startTimer.SetStartTime();
}

SchtiTansChargStation::~SchtiTansChargStation()
{
	// TODO Auto-generated destructor stub
}

void SchtiTansChargStation::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void SchtiTansChargStation::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}

int SchtiTansChargStation::Read(Device::SlaveDev* dev, String recvData)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_TCZ_J:
		ret = this->TczJRead(dev, recvData);
		break;
	default:
		break;
	};

	return ret;
}

int SchtiTansChargStation::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_TCZ_J:
		ret = this->TczJPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SchtiTansChargStation::TczJRead(Device::SlaveDev* dev, String recvData)
{
	zlog_debug(Util::m_zlog,"四川泰坦豪特充电桩");
	int ret = ErrorInfo::ERR_OK;
	int id = 0;
	int sendID = 0;
	String data;
	String sendData;
	data.clear();
	sendData.clear();

	//1、解析数据
	if(recvData.size() == 13)
	{
		ret = UnpackData(recvData, id, data);
	}

	if(id == 0x18A0FF60)//账单报文
	{
		zlog_warn(Util::m_zlog,"账单报文");
		if(data[0] == 0x06)
		{
			//发送账单确认报文
			zlog_warn(Util::m_zlog,"发送账单确认报文");
			sendID = 0x18EA60A0;
			sendData.push_back(0x01);
			sendData.push_back(0x0F);
			sendData.push_back(0x00);
			sendData.push_back(0x00);
			sendData.push_back(0x00);
			sendData.push_back(0x0A);
			sendData.push_back(0x00);
			sendData.push_back(0x00);

			ret = SendData(id, sendData);
		}
	}

	//2、发送对时报文
	m_startTimer.SetEndTime();
	if (m_startTimer.GetMilliSeconds() > 9000)
	{
		id = 0x0411FFA0;
		sendData.clear();
		//获取系统时间
		struct tm *temptm;
		time_t temptime;
		temptime = time(0);
		temptm = localtime(&temptime);

		sendData.push_back((UCHAR) (temptm->tm_year - 100));//年
		sendData.push_back((UCHAR) (temptm->tm_mon + 1));//月
		sendData.push_back((UCHAR) temptm->tm_mday);//日
		sendData.push_back((UCHAR) temptm->tm_hour);//时
		sendData.push_back((UCHAR) temptm->tm_min);//分
		sendData.push_back((UCHAR) temptm->tm_sec);//秒
		sendData.push_back(0);//保留
		sendData.push_back(0);//保留

		ret = SendData(id, sendData);

		//重新计时
		m_startTimer.SetStartTime();
	}

	//3
	return ret;
}

int SchtiTansChargStation::TczJPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	return ret;
}

int SchtiTansChargStation::UnpackData(String recvData, int& id, String& data)
{
	int ret = ErrorInfo::ERR_OK;

	id = 0;
	data.clear();

	id = (recvData.at(1) << 24) | (recvData.at(2) << 16)
		| (recvData.at(3) << 8) | (recvData.at(4));

	for (int i = 5; i < 13; i++)
	{
		data.push_back(recvData[i]);
	}

	return ret;
}

int SchtiTansChargStation::SendData(int id, String sendData)
{
	int ret = ErrorInfo::ERR_OK;
	String sendBuf;
	sendBuf.clear();

	sendBuf.push_back(0x88);
	sendBuf.push_back((id >> 24) & 0xFF);
	sendBuf.push_back((id >> 16) & 0xFF);
	sendBuf.push_back((id >> 8) & 0xFF);
	sendBuf.push_back(id & 0xFF);
	for(int i = 0; i < sendData.size(); i++)
	{
		sendBuf.push_back(sendData.at(i));
	}

	//发送数据
	printf("发送数据：");
	for(int i = 0; i < sendBuf.size(); i++)
	{
		printf("%02x ",sendBuf[i]);
	}
	printf("\n");
	ret = m_tcp->SendFlush(sendBuf);

	return ret;
}
