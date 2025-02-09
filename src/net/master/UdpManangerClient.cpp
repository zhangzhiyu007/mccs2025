/*
 * UdpManangerClient.cpp
 *
 *   创建日期: 2013-4-2
 *   作         者: mzh
 */

#include "UdpManangerClient.h"
#include "../../etc/socket/Buffer.h"
#include "MasterManager.h"
#include "ControlClient.h"

UdpManangerClient::UdpManangerClient():m_sendTime(10),m_timeout(100000)
{
	// TODO 构造函数
	//清空数组
	m_masterManager = NULL;
	m_udp = NULL;//UDP指针
	m_dataNumber = 0;

	m_networkA = Iec103::NO_USED;
	m_networkB = Iec103::NO_USED;
}

UdpManangerClient::~UdpManangerClient()
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

void UdpManangerClient::SetMasterManager(MasterManager* manager)
{
	this->m_masterManager = manager;
}

bool UdpManangerClient::IsUdpOpened()
{
	bool isOpen = false;
	if ((NULL != m_udp) && (m_udp->IsOpened()))
	{
		isOpen = true;
	}

	return isOpen;
}

bool UdpManangerClient::OpenedUdp()
{
	zlog_info(Util::m_zlog, "站内设备UDP管理端口线程:打开UDP");
	UdpNode* udp = NULL;//
	bool isOpen = false;
	const int port = m_masterManager->GetMasterManagerPort();//绑定主站UDP管理端口
	ControllerConfig* config = m_masterManager->GetControllerConfig();
	const bool isServer = false;//客户端
	const bool isBroadcast = true;//广播模式

	zlog_warn(Util::m_zlog, "port = %d",port);
	//UDP客户端,广播模式
	//打开UDP客户端
	udp = new UdpNode();
	udp->Open(isServer, isBroadcast, port);

	if (!udp->IsOpened())
	{
		zlog_warn(Util::m_zlog, "A网打开失败");
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

	zlog_info(Util::m_zlog, "站内设备UDP管理端口线程:打开UDP%s",isOpen?"成功":"失败");

	return isOpen;

}

void UdpManangerClient::CloseUdp()
{
	zlog_info(Util::m_zlog, "站内设备UDP主站管理端口线程:关闭UDP");

	if (NULL != m_udp)
	{
		if (m_udp->IsOpened())
		{
			m_udp->Close();
		}
	}
	return;
}

void UdpManangerClient::PackHead(Iec103::PacketHead& head)
{
	ControllerConfig* config = m_masterManager->GetControllerConfig();
	head.srcAddr = config->controllerAddr;
	head.srcType = config->sysType;
	head.destAddr = BROADCAST_ADDR;
	head.destType = ANY_TYPE;
	head.dataNumber = m_dataNumber;
	head.networkA = m_networkA;//通讯状态
	head.networkB = m_networkB;//通讯状态

	m_dataNumber++;
	return ;
}

//线程执行函数
void UdpManangerClient::Run()
{
	zlog_warn(Util::m_zlog, "站内设备UDP管理端口线程");

	//主机设备信息
	const int port = m_masterManager->GetSlaveManagerPort();//从站UDP管理端口
	ControllerConfig* config = m_masterManager->GetControllerConfig();

	Address addrBroadcastA(config->ipBroadcastA, port);//A网段广播地址
	Address addrBroadcastB(config->ipBroadcastB, port);//B网段广播地址
	String sendData;//发送数据
	Iec103 iec103;
	Iec103::PacketHead head;
	this->PackHead(head);//封装发送数据的包头

	//1.打开UDP
	bool isOpen = this->OpenedUdp();
	if (!isOpen)
	{
		zlog_warn(Util::m_zlog, "站内设备UDP管理端口线程:打开UDP失败");
	}
	else
	{
		zlog_info(Util::m_zlog, "站内设备UDP管理端口线程:打开成功");

		//打开成功后,立即发送广播数据
		sendData = iec103.Pack(&head, NULL);//封装包,对于UDP管理信息只有包头没有包数据
		this->SendInfo(sendData);//发送广播信息
	}

	//计算时间差
	StatTimer startTimer;
	startTimer.Initialize();
	startTimer.SetStartTime();//起始时间
	//局部变量,用于判断返回值

	//2、循环发送数据
	while(true)
	{
		//1、外部命令退出
		if ((m_state == Thread::STOPPING) || (m_state == Thread::STOPPED))
		{
			zlog_info(Util::m_zlog, "退出站内设备UDP管理端口线程");
			break;
		}

		//2、判断UDP是否打开
		//只要有一个时打开的即认为是打开的
		if(!this->IsUdpOpened())
		{
			//没有打开就关闭线程
			break;
		}

		//3、定期发送数据
		startTimer.SetEndTime();
		if (m_sendTime < startTimer.GetSeconds())
		{
			//超过定时间隔时间,发送数据
			//封装数据
			this->PackHead(head);//封装发送数据的帧头
			sendData = iec103.Pack(&head, NULL);//封装帧
			this->SendInfo(sendData);

			//重新计时
			startTimer.SetStartTime();
		}

		//4\接收数据
		if (NULL != m_udp)
		{
			RecvInfo(m_udp);
		}

		usleep(5000);
	}

	//关闭UDP客户端
	this->CloseUdp();

	m_state = Thread::STOPPED;
	zlog_warn(Util::m_zlog, "站内设备UDP管理端口线程退出");
	return;
}

//发送主站信息包
int UdpManangerClient::SendInfo(const String& dataSend)
{
	zlog_info(Util::m_zlog, "发送主站信息包");

	if (!this->IsUdpOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	if (NULL == m_masterManager)
	{
		return ErrorInfo::ERR_NULL;
	}

	//得到主站配置信息
	//主机设备信息
	const int port = m_masterManager->GetSlaveManagerPort();//从站UDP管理端口
	ControllerConfig* config = m_masterManager->GetControllerConfig();
	//主机设备信息
	Address addrBroadcastA(config->ipBroadcastA, port);//A网段广播地址
	Address addrBroadcastB(config->ipBroadcastB, port);//B网段广播地址

	int ret = 0;
	int success = ErrorInfo::ERR_OPENED;
	//启动时发送数据
	if (Iec103::NORMAL == m_networkA)
	{
		ret = m_udp->Sendto(addrBroadcastA, dataSend);
		zlog_info(Util::m_zlog,"向A网发送数据");
		if (ret <= 0)
		{
			zlog_info(Util::m_zlog, "向A网段发送主站信息包失败");
		}
		else
		{
			success = ErrorInfo::ERR_OK;
		}
	}

	if (Iec103::NORMAL == m_networkB)
	{
		zlog_info(Util::m_zlog,"向B网发送数据");
		ret = m_udp->Sendto(addrBroadcastB, dataSend);
		if (ret <= 0)
		{
			zlog_info(Util::m_zlog, "向B网段发送主站信息包失败");
		}
		else
		{
			success = ErrorInfo::ERR_OK;
		}
	}

	return success;
}

int UdpManangerClient::RecvInfo(UdpNode* udp)
{
	zlog_info(Util::m_zlog,"UdpManangerClient：接收从站管理端口数据");

	String recvData;
	Address addr;

	//判断指针是否为空
	if ((NULL == udp) || (!udp->IsOpened()))
	{
		return ErrorInfo::ERR_OPENED;
	}

	//1.接收数据,非阻塞式读取数据
	int num=udp->Recvfrom(addr, recvData,MAX_VALUE,0,this->m_timeout, 0);
	if(num<0)
	{
		return ErrorInfo::ERR_OPENED;
	}
	else if ( num<= 0)
	{
		return ErrorInfo::ERR_NO_DATA;
	}

	//2.解析数据
	zlog_info(Util::m_zlog,"UdpManangerClient：接收从站管理端口数据");
	Iec103 iec103;
	Iec103::PacketHead head;
	Iec103::PacketData data;

	bool ret = iec103.Unpack(head, data, recvData);//进行解包
	if (!ret)
	{
		zlog_info(Util::m_zlog,"UdpManangerClient：解析数据包失败");
		return ErrorInfo::ERR_FAILED;
	}

	//3.判断是否发向本站
	ControllerConfig* config = m_masterManager->GetControllerConfig();//本机网络参数

	if ((head.destAddr != config->controllerAddr)
			&& (head.destAddr != BROADCAST_ADDR))
	{
		zlog_info(Util::m_zlog,"UdpManangerClient：不是发到本机");
		return ErrorInfo::ERR_FAILED;
	}

	//4.判断是否第一次收到该从站信息,如果是就设置标志
	//使管理线程可以开启控制线程
	zlog_info(Util::m_zlog,"UdpManangerClient：判断是否第一次收到该从站信息,如果是就设置标志");
	//得到从站控制器配置参数数组
	PtrArray* slaveConfigs = m_masterManager->GetSlaveConfig();
	SlaveConfig* slaveConfig = NULL;
	PtrArray::iterator iter;

	for (iter = slaveConfigs->begin(); iter != slaveConfigs->end(); iter++)
	{
		slaveConfig = reinterpret_cast<SlaveConfig*> (*iter);
		zlog_info(Util::m_zlog,"UdpManangerClient：从数据库中查找是否保存该设备");
		//找到该地址及设备类型
		if ((NULL != slaveConfig)
				&&(slaveConfig->config.controllerAddr == head.srcAddr)
				&& (slaveConfig->config.sysType == head.srcType))
		{
			zlog_info(Util::m_zlog,"UdpManangerClient：从数据库中找到该设备");
			if ((!slaveConfig->flag))
			{
				zlog_info(Util::m_zlog,"UdpManangerClient：第一次接收到该设备，没有建立TCP控制连接");
				if (head.networkA == Iec103::NORMAL)
				{
					slaveConfig->config.networkAUsed = true;
				}
				else
				{
					slaveConfig->config.networkAUsed = false;
				}

				if (head.networkB == Iec103::NORMAL)
				{
					slaveConfig->config.networkBUsed = true;
				}
				else
				{
					slaveConfig->config.networkBUsed = false;
				}

				zlog_warn(Util::m_zlog, "UdpManangerClient：开启TCP线程");
				//开启TCP线程
				ControlClient* client = new ControlClient();
				client->SetMasterManager(this->m_masterManager);
				slaveConfig->thread = client;
				slaveConfig->isRun = true;
				slaveConfig->flag = true;
				client->SetSlaveConfig(slaveConfig);
				client->Start();

				//设置收到新从站信息标志
				m_isRecvRemoteInfo = false;
			}

			break;
		}
	}

	return ErrorInfo::ERR_OK;
}

