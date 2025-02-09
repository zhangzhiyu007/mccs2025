/*
 * UploadClient.cpp
 *
 *   创建日期: 2013-4-5
 *   作         者: mzh
 */

#include "UploadClient.h"
#include "SlaveManeger.h"
#include "../../db/Device.h"
#include "../../db/MemDb.h"

UploadClient::UploadClient() :
	m_sendTime(50)
{
	// TODO 构造函数
	m_dataNumber = 0;
	m_slaveManager = NULL;
	m_udp = NULL;//UDP指针

	m_networkA = Iec103::NO_USED;
	m_networkB = Iec103::NO_USED;

	m_isMaster = false;
	m_slaveDevOwn.clear();
}

UploadClient::~UploadClient()
{
	// TODO 析构函数
	//关闭UDP病释放内存
	CloseUdp();
	if (NULL != m_udp)
	{
		delete m_udp;
		m_udp = NULL;
	}
}

void UploadClient::SetMasterManager(SlaveManager* manager)
{
	this->m_slaveManager = manager;
	if (NULL != m_slaveManager)
	{
		ControllerConfig* config = m_slaveManager->GetControllerConfig();
		if (NULL != config)
		{
			switch (config->controllerType)
			{
			case DevControllerType::MASTER:
			case DevControllerType::Mult:
				this->m_isMaster = true;
				break;
			default:
				break;
			}
		}
	}
}

bool UploadClient::IsUdpOpened()
{
	bool isOpen = false;
	if ((NULL != m_udp) && (m_udp->IsOpened()))
	{
		isOpen = true;
	}

	return isOpen;
}

bool UploadClient::OpenedUdp()
{
	zlog_warn(Util::m_zlog, "站内设备UDP上传线程:打开UDP");
	UdpNode* udp = NULL;//UDP指针
	bool isOpen = false;
	const int port = m_slaveManager->GetSlaveUploadPort();//绑定从站端口
	ControllerConfig* config = m_slaveManager->GetControllerConfig();//从站配置
	const bool isServer = false;//服务器端
	const bool isBroadcast = true;//无效

	//UDP客户端,广播模式
	//UDP服务器端,等待数据到来,只需要等待数据
	udp = new UdpNode();
	zlog_info(Util::m_zlog,"上传端口:%d",port);
	udp->Open(isServer, isBroadcast, port);
	if (!udp->IsOpened())
	{
		zlog_warn(Util::m_zlog, "网络打开失败");
	}

	m_udp = udp;

	//认为有一个网络正常就认为时正常的
	isOpen = udp->IsOpened();

	//设置A\B网段是否通讯正常
	if (!isOpen)
	{
		if (config->networkAUsed)
		{
			m_networkA = Iec103::ABNORMAL;//通讯异常
		}

		if (config->networkBUsed)
		{
			m_networkB = Iec103::ABNORMAL;//通讯异常
		}
	}
	else
	{
		if (config->networkAUsed)
		{
			m_networkA = Iec103::NORMAL;//通讯正常
		}

		if (config->networkBUsed)
		{
			m_networkB = Iec103::NORMAL;//通讯正常
		}
	}
	zlog_warn(Util::m_zlog, "站内设备UDP监视线程:打开UDP%s",isOpen?"成功":"失败");

	return isOpen;

}

void UploadClient::CloseUdp()
{
	if (NULL != m_udp)
	{
		if (m_udp->IsOpened())
		{
			m_udp->Close();
		}
	}
	return;
}

void UploadClient::Run()
{
	zlog_info(Util::m_zlog, "站内设备从站UDP监视线程");

	this->OpenedUdp();
	//读取属于本控制器的从设备
	this->GetSlaveDevOwn();

	//计算时间差
	StatTimer startTimer;
	startTimer.Initialize();
	startTimer.SetStartTime();//起始时间

	while (true)
	{
		//1、外部命令退出
		if (m_state == Thread::STOPPING)
		{
			zlog_warn(Util::m_zlog, "退出站内设备从站UDP监视线程");
			break;
		}

		//2、判断UDP是否打开
		//只要有一个时打开的即认为是打开的
		if (!this->IsUdpOpened())
		{
			zlog_warn(Util::m_zlog, "没有UDP打开就关闭线程");
			//没有打开就关闭线程
			break;
		}

		//3.定时上传数据
		startTimer.SetEndTime();
		if (m_sendTime < startTimer.GetMilliSeconds())
		{
			zlog_debug(Util::m_zlog,"定时时间内上传数据");
			this->SendRealData();
			startTimer.SetStartTime();//起始时间
			zlog_debug(Util::m_zlog,"定时时间内上传数据结束");
		}

		msleep(10);
	}

	//关闭UDP客户端
	this->CloseUdp();
	//MemDb::Uninit();

	m_state == Thread::STOPPED;
	zlog_warn(Util::m_zlog, "站内设备从站UDP监视线程退出");
}

int UploadClient::SendRealData()
{
	zlog_debug(Util::m_zlog, "站内设备从站UDP上传线程发送实时数据");
	int i = 0;
	Device::SlaveDev* slaveDev = NULL;//
	PtrArray data;
	int start = 0;
	int len = 0;
	unsigned int j = 0;
	int dataLen = ARRAY_LENGTH_128 - 1;
	RealData* realDataTmp = NULL;

	//1、集中控制器上传
	//1）判断是否是集中控制器
	if(this->m_isMaster)
	{
		//集中控制器就需要上传数据
		//根据地址读取数据
		//根据地址读取数据
		start = 0;
		len = 400;

		for (i = start; i < (start + len - dataLen); i += dataLen)
		{
			//发送实时数据
			zlog_debug(Util::m_zlog,"获取实时数据 start = %d",i);
			GetRealData(data, i, dataLen);
			for (j = 0; j < data.size(); j++)
			{
				realDataTmp = (RealData*) data[j];
#if UNUSED
				if (realDataTmp->pv.type == RealData::DB)
				{
					zlog_warn(Util::m_zlog,"data[%d]=%.2f",j,realDataTmp->pv.data.dbValue);
				}
				else if (realDataTmp->pv.type == RealData::INT)
				{
					zlog_warn(Util::m_zlog,"data[%d]=%d",j,realDataTmp->pv.data.iValue);
				}
#endif
			}
			//发送数据
			zlog_debug(Util::m_zlog,"获取实时数据,数量=%d",data.size());
			if (data.size() >= 0)
			{
				this->SendRealData(data);
				this->ReleaseRealData(data);
			}

			usleep(5000);
		}

		//剩下数据
		int lenTmp = start + len - i;
		if (lenTmp > 0)
		{
			zlog_debug(Util::m_zlog,"剩余数据");
			GetRealData(data, i, lenTmp);
			for (j = 0; j < (int) data.size(); j++)
			{
				realDataTmp = (RealData*) data[j];
#if UNUSED
				if (realDataTmp->pv.type == RealData::DB)
				{
					zlog_debug(Util::m_zlog,"data[%d]=%.2f",j,realDataTmp->pv.data.dbValue);
				}
				else if (realDataTmp->pv.type == RealData::INT)
				{
					zlog_debug(Util::m_zlog,"data[%d]=%d",j,realDataTmp->pv.data.iValue);
				}
#endif
			}

			zlog_debug(Util::m_zlog,"获取实时数据,数量=%d",data.size());
			if (data.size() >= 0)
			{
				this->SendRealData(data);
				this->ReleaseRealData(data);
			}
		}

	}


	//2、从设备数据上传
	if (m_slaveDevOwn.empty())//没有从设备
	{
		zlog_info(Util::m_zlog, "站内设备从站UDP上传线程没有从设备");
		return ErrorInfo::ERR_NULL;
	}

	//3.循环从站数据
	zlog_debug(Util::m_zlog,"循环从站数据");
	PtrArray::iterator iter;
	for (iter = m_slaveDevOwn.begin(); iter != m_slaveDevOwn.end(); iter++)
	{
		slaveDev = reinterpret_cast<Device::SlaveDev*> (*iter);
		if (NULL == slaveDev)
		{
			continue;
		}

		start = slaveDev->regStart;
		len = slaveDev->regLen;
		zlog_debug(Util::m_zlog,"循环从站数据寄存器地址=%d,寄存器长度=%d",start,len);

		if(slaveDev->devType == DeviceType::DEVICE_TYPE_DI_MODULE)
		{
			continue;
		}

		//根据地址读取数据
		for (i = start; i < (start + len - dataLen); i += dataLen)
		{
			//发送实时数据
			zlog_debug(Util::m_zlog,"获取实时数据 start = %d",start);
			GetRealData(data, i, dataLen);
			for (j = 0; j < data.size(); j++)
			{
				realDataTmp = (RealData*) data[j];
#if UNUSED
				if (realDataTmp->pv.type == RealData::DB)
				{
					zlog_debug(Util::m_zlog,"data[%d]=%.2f",j,realDataTmp->pv.data.dbValue);
				}
				else if (realDataTmp->pv.type == RealData::INT)
				{
					zlog_debug(Util::m_zlog,"data[%d]=%d",j,realDataTmp->pv.data.iValue);
				}
#endif
			}
			//发送数据
			zlog_debug(Util::m_zlog,"获取实时数据,数量=%d",data.size());
			if (data.size() >= 0)
			{
				this->SendRealData(data);
				this->ReleaseRealData(data);
			}

			usleep(5000);
		}

		//剩下数据
		int lenTmp = start + len - i;
		if (lenTmp > 0)
		{
			zlog_debug(Util::m_zlog,"剩余数据");
			GetRealData(data, i, lenTmp);
			for (j = 0; j < (int) data.size(); j++)
			{
				realDataTmp = (RealData*) data[j];
#if UNUSED
				if (realDataTmp->pv.type == RealData::DB)
				{
					zlog_debug(Util::m_zlog,"data[%d]=%.2f",j,realDataTmp->pv.data.dbValue);
				}
				else if (realDataTmp->pv.type == RealData::INT)
				{
					zlog_debug(Util::m_zlog,"data[%d]=%d",j,realDataTmp->pv.data.iValue);
				}
#endif
			}

			zlog_debug(Util::m_zlog,"获取实时数据,数量=%d",data.size());
			if (data.size() >= 0)
			{
				this->SendRealData(data);
				this->ReleaseRealData(data);
			}

			zlog_debug(Util::m_zlog,"发送完毕");
			usleep(5000);
		}
	}

	zlog_debug(Util::m_zlog,"循环从站数据上传结束");
	return ErrorInfo::ERR_OK;
}

void UploadClient::PackHead(Iec103::PacketHead& head)
{
	//封装包头
	Iec103 iec103;
	ControllerConfig* config = m_slaveManager->GetControllerConfig();
	head.srcAddr = config->controllerAddr;//本机地址
	head.srcType = config->sysType;//本机类型
	head.destAddr = BROADCAST_ADDR;//广播地址
	head.destType = ANY_TYPE;//任意类型都可接收
	head.dataNumber = m_dataNumber;//数据编号
	head.networkA = m_networkA;//通讯状态
	head.networkB = m_networkB;//通讯状态

	m_dataNumber++;//数据编号更新
	return;
}

bool UploadClient::GetRealData(PtrArray& data, const int& start, const int& len)
{
	data.clear();
	RealData* realDataTmp = NULL;
	RealData realData;
	bool ret = false;

	for (int i = 0; i < len; i++)
	{
		ret = MemDb::GetRealData(realData, i + start);
		if (ret)
		{
			switch (realData.pv.type)
			{
			case RealData::DB:
			case RealData::INT:
				realDataTmp = new RealData;
				memcpy(realDataTmp, &realData, sizeof(RealData));
				data.push_back(realDataTmp);
				break;
			default:
				break;
			}
		}
	}

	return true;
}

void UploadClient::ReleaseRealData(PtrArray& array)
{
	unsigned int i = 0;
	RealData* data = NULL;
	for (i = 0; i < array.size(); i++)
	{
		data = (RealData*) array[i];
		delete data;
	}

	array.clear();
}

int UploadClient::SendRealData(PtrArray& data)
{
	if (data.size() <= 0)
	{
		return ErrorInfo::ERR_NO_DATA;
	}

	//封装包头
	Iec103::PacketHead head;
	this->PackHead(head);

	//封装数据
	String dataStr;
	dataStr.clear();
	RealData* realData = NULL;
	int num = 0;
	unsigned int i = 0;
	float tmp = 0;
	unsigned char array[4];
	//	realData = (RealData*) data[0];
	//	dataStr.push_back((char) realData->addr);//寄存器地址
	//	dataStr.push_back((char) (realData->addr >> 8));

	for (i = 0; i < data.size(); i++)
	{
		realData = (RealData*) data[i];

		if (NULL == realData)
		{
			continue;
		}

		dataStr.push_back((char) (realData->addr & 0xFF));//寄存器地址
		dataStr.push_back((char) (realData->addr >> 8));
		if (RealData::DB == realData->pv.type)
		{
			dataStr.push_back(RealData::DB);//实型
			tmp = realData->pv.data.dbValue;
			//保存数据
			memcpy(array, &tmp, sizeof(float));
			dataStr.push_back(array[0]);
			dataStr.push_back(array[1]);
			dataStr.push_back(array[2]);
			dataStr.push_back(array[3]);
		}
		else
		{
			dataStr.push_back(RealData::INT);//整形
			//保存数据
			dataStr.push_back((realData->pv.data.iValue) & 0xFF);
			dataStr.push_back((realData->pv.data.iValue >> 8) & 0xFF);
			dataStr.push_back((realData->pv.data.iValue >> 16) & 0xFF);
			dataStr.push_back((realData->pv.data.iValue >> 24) & 0xFF);
		}

		num++;
	}

	//封装ASDU
	Iec103::PacketData packetData;
	packetData.type = Iec103::REAL_DATA;//控制数据下发
	packetData.destAddr = BROADCAST_ADDR;//目的地址
	packetData.dataStart = 0;//数据起始地址
	packetData.dataLength = num;//数据长度
	packetData.continuous = Iec103::ALONE_ADDR;//单独地址
	packetData.prm = 0;//启动报文位,0由从站向主站发送数据
	packetData.fcb = 0;//1表示从设备有1级用户数据要向控制系统传送
	packetData.fcv = 0;//从设备可以接受数据
	packetData.fun = 0x03;//功能码,发送/确认帧,传送数据
	packetData.data = dataStr;

	//封装发送数据
	Iec103 iec103;
	String sendData;
	sendData = iec103.Pack(&head, &packetData);

	//发送数据
	int ret = 0;
	ControllerConfig* config = m_slaveManager->GetControllerConfig();
	Address addrA(config->ipBroadcastA, m_slaveManager->GetMasterUploadPort());//从站UDP上传端口
	Address addrB(config->ipBroadcastB, m_slaveManager->GetMasterUploadPort());

	if (m_udp->IsOpened())
	{
		if (Iec103::NORMAL == m_networkA)
		{
			ret = m_udp->Sendto(addrA, sendData);
		}

		if (Iec103::NORMAL == m_networkB)
		{
			zlog_debug(Util::m_zlog,"B网上传数据");
			ret = m_udp->Sendto(addrB, sendData);
		}
	}

	return ErrorInfo::ERR_OK;
}

int UploadClient::GetSlaveDevOwn()
{
	zlog_debug(Util::m_zlog, "读取属于自己的从设备");
	//1.读取从站设备
	Device* device = Device::GetInstance();
	if (NULL == device)
	{
		zlog_warn(Util::m_zlog, "读取属于自己的从设备:NULL==device");
		return ErrorInfo::ERR_NULL;
	}

	PtrArray* slaveDevArray = device->GetSlaveDevArray();//得到从设备信息
	if ((NULL == slaveDevArray) || (slaveDevArray->empty()))//没有从设备
	{
		zlog_warn(Util::m_zlog, "读取属于自己的从设备:(NULL==slaveDevArray)||(slaveDevArray->empty())");
		return ErrorInfo::ERR_NULL;
	}

	ControllerConfig* config = m_slaveManager->GetControllerConfig();
	if (NULL == config)
	{
		zlog_warn(Util::m_zlog, "读取属于自己的从设备:NULL==config");
		return ErrorInfo::ERR_NULL;
	}

	//3.循环读取从设备
	zlog_debug(Util::m_zlog,"循环读取从设备");
	PtrArray::iterator iter;
	Device::SlaveDev* slaveDev = NULL;//
	m_slaveDevOwn.clear();
	for (iter = slaveDevArray->begin(); iter != slaveDevArray->end(); iter++)
	{
		slaveDev = reinterpret_cast<Device::SlaveDev*> (*iter);

		if (NULL == slaveDev)
		{
			continue;
		}

		if (slaveDev->own == config->controllerAddr)
		{
			m_slaveDevOwn.push_back(slaveDev);
		}
	}

	zlog_debug(Util::m_zlog,"读取属于自己的从设备结束");
	return ErrorInfo::ERR_OK;
}
