/*
 * ControlClient.cpp
 *
 *   创建日期: 2013-4-1
 *   作         者: mzh
 */

#include "ControlClient.h"
#include "../../db/MemDb.h"
#include "../../db/Device.h"
#include "MasterManager.h"

ControlClient::ControlClient() :
	m_failNumber(3), m_timeSync(10), m_timeout(500000)
{
	// TODO 构造函数
	m_masterManager = NULL;//站内通讯规约兼容103协议主站管理类
	m_slaveConfig = NULL;//被连接的从设备

	//TCP客户端
	m_tcpA = NULL;//A网段TCP客户端
	m_tcpB = NULL;//B网段TCP客户端
	m_regLength = 0;
	m_dataNumber = 1;//数据编号

	m_networkALink = 0;
	m_networkBLink = 0;
}

ControlClient::~ControlClient()
{
	// TODO 析构函数
}

void ControlClient::SetMasterManager(MasterManager* manager)
{
	this->m_masterManager = manager;
}

void ControlClient::SetSlaveConfig(SlaveConfig* slaveConfig)
{
	this->m_slaveConfig = slaveConfig;
}

bool ControlClient::IsTcpOpened()
{
	bool isOpen = false;
	if ((NULL != m_tcpA) && (m_tcpA->IsOpened()))
	{
		isOpen = true;
	}

	if ((NULL != m_tcpB) && (m_tcpB->IsOpened()))
	{
		isOpen = true;
	}

	return isOpen;
}

bool ControlClient::OpenedTcp(TcpClient** tcp, Address& addr)
{
	bool ret = false;
	TcpClient* tcpClient = *tcp;

	if (NULL == tcpClient)
	{
		tcpClient = new TcpClient();
	}

	ret = tcpClient->Connect(addr);
	*tcp = tcpClient;
	if (!ret)
	{
		zlog_warn(Util::m_zlog, "TCP打开失败");
		return false;
	}
	return true;
}

bool ControlClient::OpenedTcp()
{
	bool isOpen = false;
	const int port = m_masterManager->GetCtrlPort();//TCP控制端口
	ControllerConfig* config = m_masterManager->GetControllerConfig();

	//TCP客户端
	//A网使用
	if ((NULL == m_tcpA) || (!m_tcpA->IsOpened()))
	{
		zlog_warn(Util::m_zlog,"config->networkAUsed = %d,m_slaveConfig->config.networkAUsed = %d"
				,config->networkAUsed,m_slaveConfig->config.networkAUsed);
		if ((config->networkAUsed) && (m_slaveConfig->config.networkAUsed))
		{
			//打开TCP客户端
			Address addrA(m_slaveConfig->config.ipA, port);
			zlog_warn(Util::m_zlog, "打开A网TCP:IP=%s",m_slaveConfig->config.ipA.c_str());
			OpenedTcp(&m_tcpA, addrA);
			//认为有一个网络正常就认为时正常的
			isOpen = isOpen || m_tcpA->IsOpened();

			zlog_debug(Util::m_zlog, "站内设备TCP控制线程:打开A网TCP客户端%s",m_tcpA->IsOpened()?"成功":"失败");
		}
	}

	if ((NULL == m_tcpB) || (!m_tcpB->IsOpened()))
	{
		zlog_debug(Util::m_zlog,"config->networkBUsed = %d,m_slaveConfig->config.networkBUsed = %d"
				,config->networkBUsed,m_slaveConfig->config.networkBUsed);
		if ((config->networkBUsed) && m_slaveConfig->config.networkBUsed)
		{
			//打开TCP客户端
			Address addrB(m_slaveConfig->config.ipB, port);
			zlog_warn(Util::m_zlog, "打开B网TCP:IP=%s",m_slaveConfig->config.ipB.c_str());
			OpenedTcp(&m_tcpB, addrB);
			//认为有一个网络正常就认为时正常的
			isOpen = isOpen || m_tcpB->IsOpened();
			zlog_debug(Util::m_zlog, "站内设备TCP控制线程:打开B网TCP客户端%s",m_tcpB->IsOpened()?"成功":"失败");
		}
	}

	return isOpen;
}

void ControlClient::CloseTcp()
{
	zlog_warn(Util::m_zlog, "站内设备TCP控制线程:关闭TCP客户端");
	if ((NULL != m_tcpA) && (m_tcpA->IsOpened()))
	{
		m_tcpA->Close();
	}

	if ((NULL != m_tcpB) && (m_tcpB->IsOpened()))
	{
		m_tcpB->Close();
	}
}

int ControlClient::GetControlData(PtrArray& array)
{
	array.clear();
	RealData* data = NULL;
	RealData realData;

	int i = 0;
	//int num = 0;
	int regStart = 0;
	int regLength =0;

	bool ret = false;
	int maxLength = ARRAY_LENGTH_128 - 1;

	//得到实例化并初始化
	Device* device = Device::GetInstance();

	//获取从设备信息指针
	PtrArray* slaveArray = device->GetSlaveDevArray();
	//存取从设备信息数组
	Device::SlaveDev* slaveDev = NULL;
	PtrArray::iterator slaveIter;
	for (slaveIter = slaveArray->begin(); slaveIter != slaveArray->end(); slaveIter++)
	{
		slaveDev = reinterpret_cast<Device::SlaveDev*> (*slaveIter);

		//设备不属于这个线程
		if ((NULL == slaveDev) || (slaveDev->own
				!= m_slaveConfig->config.controllerAddr))
		{
			continue;
		}

		switch(slaveDev->devType)
		{
		case DeviceType::DEVICE_TYPE_DO://DO
		case DeviceType::DEVICE_TYPE_DO_OPT://交流电操
		case DeviceType::DEVICE_TYPE_DC_DO_OPT://直流电操
		case DeviceType::DEVICE_TYPE_DC_DO://直流接触器、继电器
			regLength = 1;
			break;
		case DeviceType::DEVICE_TYPE_PCS://PCS
			regLength = 36;
			break;
		case DeviceType::DEVICE_TYPE_PV://PVS
			regLength = 5;
			break;
		case DeviceType::DEVICE_TYPE_DCDC://DCDC
			regLength = 11;
			break;
		case DeviceType::DEVICE_TYPE_SIMU_DC_LOAD://可编程直流负载
			regLength = 10;
			break;
		case DeviceType::DEVICE_TYPE_SIMU_AC_LOAD://可编程交流负载
			regLength = 35;
			break;
		case DeviceType::DEVICE_TYPE_AC_LOAD://交流负载
			regLength = 18;
			break;
		case DeviceType::DEVICE_TYPE_SIMU_WT://风机模拟器
			regLength = 36;
			break;
		case DeviceType::DEVICE_TYPE_SIMU_DG://柴油机模拟器
			regLength = 30;
			break;
		case DeviceType::DEVICE_TYPE_SIMU_DC://直流模拟电源
		case DeviceType::DEVICE_TYPE_SIMU_PV://光伏模拟器
			regLength = 22;
			break;
		case DeviceType::DEVICE_TYPE_SIMU_CABLE://电缆阻抗模拟器
			regLength = 7;
			break;
		case DeviceType::DEVICE_TYPE_SIMU_FLASH://谐波闪烁网络阻抗模拟器
			regLength = 6;
			break;
		case DeviceType::DEVICE_TYPE_SIMU_AC_GRID://交流模拟电网
			regLength = 63;
			break;
		default:
			break;
		}
		regStart = slaveDev->regStart;
//		regLength = min(slaveDev->regLen, ARRAY_LENGTH_20);
		for (i = 0; i < regLength; i++)
		{
			//每次最多读取不超过20个控制量
			ret = MemDb::GetRealDataWrited(realData, regStart + i, true);
			if (!ret)
			{
				continue;
			}

			switch (realData.pv.type)
			{
			case RealData::DB:
			case RealData::INT:
				data = new RealData;
				memcpy(data, &realData, sizeof(RealData));
				array.push_back(data);
				break;
			default:
				break;
			}
		}

		if (((int)array.size()) >= maxLength)
		{
			break;
		}
	}

	return ErrorInfo::ERR_OK;
}

void ControlClient::ReleaseControlData(PtrArray& array)
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

void ControlClient::Run()
{
	zlog_warn(Util::m_zlog, "站内设备控制线程");

	//计算时间差
	StatTimer startTimer;
	startTimer.Initialize();
	startTimer.SetStartTime();//起始时间

	//1.建立与客户端连接
	this->OpenedTcp();

	//线程循环
	while (true)
	{
		//1、外部命令退出
		if (m_state == Thread::STOPPING)
		{
			zlog_warn(Util::m_zlog, "退出站内设备控制线程");
			break;
		}

		//2、判断TCP是否打开
		//只要有一个时打开的即认为是打开的
		if (!this->IsTcpOpened())
		{
			zlog_warn(Util::m_zlog,"网络断开，退出线程");
			//没有打开就关闭线程
			break;
		}

		//3.循环判断是否需要下发数据
		if (SendCtrlData() == ErrorInfo::ERR_INV_RESP)
		{
			zlog_warn(Util::m_zlog,"无效请求，退出线程");
			break;
		}

		//4.定时下发时间同步指令
		startTimer.SetEndTime();
		if (m_timeSync < (int) startTimer.GetSeconds())
		{
			//如果有一个TCP断开连接就要定时尝试建立连接
			OpenedTcp();

			//超过定时间隔时间,发送数据
			if (ErrorInfo::ERR_OK != this->SendTimeSync())
			{
				zlog_warn(Util::m_zlog,"发送定时数据失败");
			}

			//重新计时
			startTimer.SetStartTime();
		}

		usleep(10000);
	}

	//关闭TCP
	this->CloseTcp();
	m_slaveConfig->flag = false;//设置与从站通讯关闭标志
	m_slaveConfig->isRun = false;//设置从站线程结束

	m_state = Thread::STOPPED;
	zlog_warn(Util::m_zlog, "站内设备控制线程退出");
	return;
}

int ControlClient::SendCtrlData()
{
	PtrArray data;
	this->GetControlData(data);//读取当前是否有需要下置数据
	if (data.empty())//没有数据可写
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
	int src = 0;

	//为每个寄存器设置单独地址
	zlog_info(Util::m_zlog,"下发数据大小%d",data.size());
	for (i = 0; i < data.size(); i++)
	{
		realData = (RealData*) data[i];
		if (NULL == realData)
		{
			continue;
		}
		zlog_info(Util::m_zlog,"下发地址%d",realData->addr);
		dataStr.push_back((char) realData->addr);//寄存器地址
		dataStr.push_back((char) (realData->addr >> 8));
		if (RealData::DB == realData->pv.type)
		{
			zlog_info(Util::m_zlog,"下发值%.2f",realData->pv.data.dbValue);
			dataStr.push_back(RealData::DB);//实型
			tmp = realData->pv.data.dbValue;
			memcpy(&src, &tmp, sizeof(tmp));
			//保存数据
			dataStr.push_back((src & 0xFF));
			dataStr.push_back((src >> 8) & 0xFF);
			dataStr.push_back((src >> 16) & 0xFF);
			dataStr.push_back((src >> 24) & 0xFF);
		}
		else
		{
			zlog_info(Util::m_zlog,"下发值%d",realData->pv.data.iValue);
			dataStr.push_back(RealData::INT);//整形
			//保存数据
			dataStr.push_back((realData->pv.data.iValue));
			dataStr.push_back((realData->pv.data.iValue >> 8));
			dataStr.push_back((realData->pv.data.iValue >> 16));
			dataStr.push_back((realData->pv.data.iValue >> 24));
		}

		num++;
	}

	//释放内存
	this->ReleaseControlData(data);

	//如果没有数据，就返回
	if (0 == num)
	{
		return ErrorInfo::ERR_NO_DATA;
	}

	//封装ASDU
	Iec103::PacketData packetData;
	packetData.type = Iec103::CTRL_DATA;//控制数据下发
	packetData.destAddr = m_slaveConfig->config.controllerAddr;//目的地址
	packetData.dataStart = 0;//数据起始地址
	packetData.dataLength = num;//数据长度
	packetData.continuous = Iec103::ALONE_ADDR;//1表示单独地址
	packetData.prm = 1;//启动报文位,1由主站向从站传输
	packetData.fcb = 1;//帧记数位:主站每向从站发送新一轮的“发送/确认”或“请求/响应”传输服务时，将FCB取反
	packetData.fcv = 1;//帧记数有效位
	packetData.fun = 0x03;//功能码,发送/确认帧,传送数据
	packetData.data = dataStr;

	//封装发送数据
	Iec103 iec103;
	String sendData = iec103.Pack(&head, &packetData);//封装发送数据

	int ret = 0;
	int successA = ErrorInfo::ERR_FAILED;
	int successB = ErrorInfo::ERR_FAILED;
	int success = ErrorInfo::ERR_FAILED;
	num = 0;

	do
	{
		if ((NULL != m_tcpA) && m_tcpA->IsOpened())
		{
			ret = m_tcpA->SendFlush(sendData);

			if (ret <= 0)
			{
				m_tcpA->Close();
			}
		}

		if ((NULL != m_tcpB) && m_tcpB->IsOpened())
		{
			ret = m_tcpB->SendFlush(sendData);

			if (ret <= 0)
			{
				m_tcpB->Close();
			}
		}

		//接受数据
		if ((NULL != m_tcpA) && m_tcpA->IsOpened())
		{
			successA = RecvInfo(m_tcpA);
			if (successA == ErrorInfo::ERR_OPENED)
			{
				m_tcpA->Close();
			}
		}

		if ((NULL != m_tcpB) && m_tcpB->IsOpened())
		{
			successB = RecvInfo(m_tcpB);
			if (successB == ErrorInfo::ERR_OPENED)
			{
				m_tcpB->Close();
			}
		}

		if ((ErrorInfo::ERR_OK != successA) && (ErrorInfo::ERR_OK != successB))
		{
			success = ErrorInfo::ERR_INV_RESP;
		}
		else
		{
			success = ErrorInfo::ERR_OK;
		}

		num++;
	} while ((success != ErrorInfo::ERR_OK) && IsTcpOpened() && (num
			< m_failNumber));

	zlog_info(Util::m_zlog,"success=%d,num = %d",success,num);
	return success;
}

int ControlClient::SendTimeSync()
{
	//封装包头
	Iec103::PacketHead head;
	this->PackHead(head);

	//封装时间
	String dataStr;
	dataStr.clear();

	//strut timeval {long tv_sec; /* 秒数 */long tv_usec; /* 微秒数 */};
	struct timeval tv;//时间结构体
	//获得精确计时时间
	gettimeofday(&tv, NULL);
	int msecond = tv.tv_usec / 1000;//得到毫秒

	struct tm *temptm;
	time_t temptime;
	temptime = time(0);
	temptm = localtime(&temptime);
	msecond += temptm->tm_sec * 1000;//添加秒

	dataStr.push_back((UCHAR) (msecond & 0x0FF));//ms低字节
	dataStr.push_back((UCHAR) (msecond >> 8));//ms高字节
	dataStr.push_back((UCHAR) temptm->tm_min);//分
	dataStr.push_back((UCHAR) temptm->tm_hour);//时
	int wday;
	if (temptm->tm_wday == 0)
	{
		wday = 7;
	}
	else
	{
		wday = temptm->tm_wday;
	}
	dataStr.push_back((UCHAR) ((wday << 5) + temptm->tm_mday));//星期(高4位)/日(低4位)
	dataStr.push_back((UCHAR) temptm->tm_mon + 1);//月
	dataStr.push_back((UCHAR) (temptm->tm_year - 100));//年

	//封装ASDU
	Iec103::PacketData packetData;
	packetData.type = Iec103::TIME_SYNC;//控制数据下发
	packetData.destAddr = m_slaveConfig->config.controllerAddr;//目的地址
	packetData.dataStart = 0;//数据起始地址
	packetData.dataLength = 7;//数据长度
	packetData.continuous = Iec103::ALONE_ADDR;//1表示单独地址
	packetData.prm = 1;//启动报文位,1由主站向从站传输
	packetData.fcb = 1;//帧记数位:主站每向从站发送新一轮的“发送/确认”或“请求/响应”传输服务时，将FCB取反
	packetData.fcv = 1;//帧记数有效位
	packetData.fun = 0x03;//功能码,发送/确认帧,传送数据
	packetData.data = dataStr;

	//封装发送数据
	Iec103 iec103;
	String sendData = iec103.Pack(&head, &packetData);

	int ret = 0;
	int successA = ErrorInfo::ERR_OK;
	int successB = ErrorInfo::ERR_OK;
	int success = ErrorInfo::ERR_OK;
	int num = 0;

	do
	{
		if ((NULL != m_tcpA) && m_tcpA->IsOpened())
		{
			ret = m_tcpA->SendFlush(sendData);
			if (ret <= 0)
			{
				m_tcpA->Close();
			}
		}

		if ((NULL != m_tcpB) && m_tcpB->IsOpened())
		{
			ret = m_tcpB->SendFlush(sendData);
			if (ret <= 0)
			{
				m_tcpB->Close();
			}
		}

		//接受数据
		if ((NULL != m_tcpA) && m_tcpA->IsOpened())
		{
			successA = RecvInfo(m_tcpA);
			if (successA == ErrorInfo::ERR_OPENED)
			{
				m_tcpA->Close();
				zlog_warn(Util::m_zlog, "A网断开连接,关闭端口");
			}
		}

		if ((NULL != m_tcpB) && m_tcpB->IsOpened())
		{
			successB = RecvInfo(m_tcpB);
			if (successB == ErrorInfo::ERR_OPENED)
			{
				m_tcpB->Close();
				zlog_warn(Util::m_zlog, "B网断开连接,关闭端口");
			}
		}

		success = successA && successB;

		num++;
	} while ((ErrorInfo::ERR_OK != success) && this->IsTcpOpened() && (num
			< m_failNumber));

	return success;
}

int ControlClient::RecvInfo(TcpClient* tcp)
{
	int ret = 0;

	if (NULL == tcp)
	{
		return ErrorInfo::ERR_NULL;//指针为空
	}

	//1\判断是又有数据
	String recvData;
	ret = tcp->Recv(recvData, MAX_VALUE, 0, m_timeout);
	if (ret < 0)
	{
		zlog_warn(Util::m_zlog,"没有接收到数据，断开连接");
		return ErrorInfo::ERR_OPENED;//断开连接
	}
	else if (ret == 0)
	{
		zlog_debug(Util::m_zlog,"没有接收到数据");
		return ErrorInfo::ERR_NO_DATA;//没有数据可读
	}

	//进行解包
	Iec103 iec103;
	Iec103::PacketHead head;
	Iec103::PacketData data;

	//解析包头
	bool success = iec103.Unpack(head, data, recvData);//进行解包
	if (!success)
	{
		return ErrorInfo::ERR_INV_RESP;//应答错误
	}

	//3.判断是否发向本站
	ControllerConfig* config = m_masterManager->GetControllerConfig();//本机网络参数

	if ((head.destAddr != config->controllerAddr) && (head.destAddr
			!= BROADCAST_ADDR))
	{
		zlog_warn(Util::m_zlog,"不是发往本站");
		return ErrorInfo::ERR_INV_RESP;//应答错误
	}

	//判断是否是从站发出的
	if (head.srcAddr != m_slaveConfig->config.controllerAddr)
	{
		zlog_warn(Util::m_zlog,"不是从站发出的数据");
		return ErrorInfo::ERR_INV_RESP;//应答错误
	}

	//4.对ASDU部分进行解包
	success = false;
	if ((data.type == Iec103::CONFIRM)//确认帧
			&& (data.fun == 0)//肯定确认帧
			&& (data.prm == 0))//从站发向主站
	{
		//判断数据编码
		if (iec103.DataComparison(head.dataNumber, m_slaveConfig->dataNumber))
		{
			//已经存在
			zlog_warn(Util::m_zlog,"数据已经存在");
		}
		else
		{
			m_slaveConfig->dataNumber = head.dataNumber;
		}

		return ErrorInfo::ERR_OK;
	}

	return ErrorInfo::ERR_FAILED;
}

void ControlClient::PackHead(Iec103::PacketHead& head)
{
	//封装包头
	ControllerConfig* config = m_masterManager->GetControllerConfig();
	head.srcAddr = config->controllerAddr;
	head.srcType = config->sysType;
	head.destAddr = m_slaveConfig->config.controllerAddr;
	head.destType = m_slaveConfig->config.sysType;
	m_dataNumber++;
	head.dataNumber = m_dataNumber;

	if (config->networkAUsed)
	{
		if (m_tcpA->IsOpened())
		{
			head.networkA = Iec103::NORMAL;//通讯正常
		}
		else
		{
			head.networkA = Iec103::ABNORMAL;//通讯失败
		}
	}
	else
	{
		head.networkA = Iec103::NO_USED;//没有使用
	}

	if (config->networkBUsed)
	{
		if (m_tcpB->IsOpened())
		{
			head.networkB = Iec103::NORMAL;//通讯正常
		}
		else
		{
			head.networkB = Iec103::ABNORMAL;//通讯失败
		}
	}
	else
	{
		head.networkB = Iec103::NO_USED;//没有使用
	}

	return;
}
