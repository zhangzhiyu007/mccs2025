/*
 * UdpManagerServer.cpp
 *
 *   创建日期: 2013-4-5
 *   作         者: mzh
 */

#include "UdpManagerServer.h"
#include "SlaveManeger.h"

UdpManagerServer::UdpManagerServer()
:m_timeout(10000)
{
	// TODO 构造函数
	m_slaveManager = NULL;
	m_udp = NULL;//UDP指针
	m_dataNumber = 0;

	m_networkA = Iec103::NO_USED;
	m_networkB = Iec103::NO_USED;
}

UdpManagerServer::~UdpManagerServer()
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

void UdpManagerServer::SetSlaveManager(SlaveManager* manager)
{
	this->m_slaveManager = manager;
}

bool UdpManagerServer::IsUdpOpened()
{
	bool isOpen = false;
	if ((NULL != m_udp) && (m_udp->IsOpened()))
	{
		isOpen = true;
	}

	return isOpen;
}

bool UdpManagerServer::OpenedUdp()
{
	zlog_info(Util::m_zlog, "站内设备UDP管理线程:打开UDP");
	UdpNode* udp = NULL;//A网
	bool isOpen = false;
	const int port = m_slaveManager->GetSlaveManagerPort();//从站UDP管理端口6602
	ControllerConfig* config = m_slaveManager->GetControllerConfig();//从站配置
	const bool isServer = true;//服务器端
	const bool isBroadcast = false;//无效

	//UDP客户端,广播模式
	//打开UDP客户端
	zlog_warn(Util::m_zlog,"从站管理端口(服务器端)：%d",port);
	udp = new UdpNode();
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
	zlog_info(Util::m_zlog, "站内设备UDP管理线程:打开UDP%s",isOpen?"成功":"失败");

	return isOpen;

}

void UdpManagerServer::CloseUdp()
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


//线程执行函数
void UdpManagerServer::Run()
{
	zlog_info(Util::m_zlog, "站内设备从站UDP管理线程");

	this->OpenedUdp();

	while(true)
	{
		//1、外部命令退出
		if (m_state == Thread::STOPPING)
		{
			zlog_info(Util::m_zlog, "退出站内设备从站UDP管理线程");
			break;
		}

		//2、判断UDP是否打开
		//只要有一个时打开的即认为是打开的
		if(!this->IsUdpOpened())
		{
			//没有打开就关闭线程
			break;
		}

		//3.循环判断是否有数据收到
		if (NULL != m_udp)
		{
			RecvInfo(m_udp);
		}

		usleep(5000);
	}

	//关闭UDP客户端
	this->CloseUdp();

	m_state = Thread::STOPPED;
	zlog_info(Util::m_zlog, "站内设备从站UDP管理线程退出");
}

int UdpManagerServer::RecvInfo(UdpNode* udp)
{
	zlog_info(Util::m_zlog, "接收数据");
	ControllerConfig* config = m_slaveManager->GetControllerConfig();//从站控制器配置
	PtrArray* masterConfigs = m_slaveManager->GetMasterConfigs();//建立联系的主站配置信息
	PtrArray::iterator iter;
	MasterConfig* masterConfig = NULL;

	String recvStr;
	Address addr;
	Iec103 iec103;//103解析类
	Iec103::PacketHead head;//包头
	Iec103::PacketData data;//ASDU数据
	int ret = 0;
	bool success = false;

	//1.判断udp是否打开
	if (!((NULL != udp) && (udp->IsOpened())))
	{
		zlog_warn(Util::m_zlog, "UdpManagerServer：udp没有打开");
		return ErrorInfo::ERR_OPENED;
	}
	//2.读取udp数据,判断有没有数据,阻塞式读取数据
	ret = udp->Recvfrom(addr, recvStr,MAX_VALUE);
	if (ret <= 0)
	{
		zlog_warn(Util::m_zlog, "UdpManagerServer：没有收到数据");
		return ErrorInfo::ERR_NO_DATA;
	}

	//接收到数据
	zlog_info(Util::m_zlog, "UdpManagerServer：解析接收数据");

	success = iec103.Unpack(head, data, recvStr);

	zlog_info(Util::m_zlog, "UdpManagerServer：解析接收数据完毕~~~~~");
	//3.判断包头部分
	//1)校验目的地址
	zlog_info(Util::m_zlog, "UdpManagerServer：检验目的地址");
	if ((config->controllerAddr != head.destAddr)//目的地址
			&& (BROADCAST_ADDR != head.destAddr))//广播地址
	{
		zlog_info(Util::m_zlog, "UdpManagerServer：目的地不是本机");
		return ErrorInfo::ERR_INV_RESP;
	}

	//2.校验原设备地址和数据编号
	zlog_info(Util::m_zlog, "UdpManagerServer：校验设备地址和数据编号");
	success = false;
	bool isExist = false;

	for (iter = masterConfigs->begin(); iter != masterConfigs->end(); iter++)
	{
		masterConfig = reinterpret_cast<MasterConfig*> (*iter);

		//找到该地址及设备类型
		zlog_info(Util::m_zlog,"UdpManagerServer：addr = %d,headAddr = %d,type=%d,headType=%d",masterConfig->config.controllerAddr,
				head.srcAddr,masterConfig->config.sysType,head.srcType);
		if ((NULL != masterConfig) && (masterConfig->config.controllerAddr
				== head.srcAddr) && (masterConfig->config.sysType
				== head.srcType))
		{
			//找到了
			if (masterConfig->isRun && masterConfig->flag)//本从站已经与主机建立连接
			{
				zlog_info(Util::m_zlog,"UdpManagerServer：本从站已经与主机建立连接");
				success = true;
			}
			else//没有建立连接
			{
				zlog_info(Util::m_zlog,"UdpManagerServer：没有建立连接");
				success = false;//需要继续向主站发送从站信息
			}
			isExist = true;
		}
	}

	//已经建立连接就直接返回
	if (success)
	{
		zlog_info(Util::m_zlog, "UdpManagerServer：已建立链接,不再发送数据");
		return ErrorInfo::ERR_OK;
	}

	zlog_info(Util::m_zlog, "UdpManagerServer：如果主站数组没有存在就保存信息");
	//如果主站数组没有存在就保存信息
	masterConfig = NULL;
	if (!isExist)
	{
		masterConfig = new MasterConfig();
		//保存
		zlog_info(Util::m_zlog,"UdpManagerServer：保存主站数组");
		masterConfig->config.controllerAddr = head.srcAddr;
		masterConfig->config.sysType = head.srcType;
		masterConfig->config.networkAUsed = head.networkA;
		masterConfig->config.networkBUsed = head.networkB;
		masterConfig->isRun = false;
		masterConfigs->push_back(masterConfig);
		zlog_info(Util::m_zlog, "UdpManagerServer：主站信息，地址=%d，类型=%d",
				masterConfig->config.controllerAddr,masterConfig->config.sysType);
	}

	//3.如果没有建立连接并且没有返回数据,则返回从站信息
	zlog_info(Util::m_zlog,"UdpManagerServer：返回从站信息");
	success = false;
	isExist = false;
	String sendData;
	for (iter = masterConfigs->begin(); iter != masterConfigs->end(); iter++)
	{
		masterConfig = reinterpret_cast<MasterConfig*> (*iter);
		//找到该地址及设备类型
		zlog_info(Util::m_zlog,"UdpManagerServer：找到该地址及设备类型");
		if ((NULL != masterConfig) && (masterConfig->config.controllerAddr
				== head.srcAddr) && (masterConfig->config.sysType
				== head.srcType)&&(!masterConfig->isRun))
		{
			//向主站发送从站信息
			zlog_info(Util::m_zlog,"UdpManagerServer：向主站发送从站信息");
			this->PackHead(head, masterConfig);
			sendData = iec103.Pack(&head, NULL);
			Address addrA(addr.GetIPString(0),addr.GetPort());
			Address addrB(addr.GetIPString(0),addr.GetPort());

			if (Iec103::NORMAL == m_networkA)
			{
				ret = udp->Sendto(addrA, sendData, 0);
				if (ret <= 0)
				{
					zlog_info(Util::m_zlog, "UdpManagerServer：A网段发送主站信息包失败\n");
				}
				else
				{
					success=true;
				}
			}
			if (Iec103::NORMAL == m_networkB)
			{
				ret = udp->Sendto(addrB, sendData);
				if (ret <= 0)
				{
					zlog_info(Util::m_zlog, "UdpManagerServer：B网段发送主站信息包失败\n");
				}
				else
				{
					success=true;
				}
			}
		}
	}

	zlog_info(Util::m_zlog, "UdpManagerServer：接收数据结束");
	return success;
}

void UdpManagerServer::PackHead(Iec103::PacketHead&head,MasterConfig* masterConfig)
{
	ControllerConfig* config = m_slaveManager->GetControllerConfig();
	head.srcAddr = config->controllerAddr;
	head.srcType = config->sysType;
	head.destAddr = masterConfig->config.controllerAddr;
	head.destType = masterConfig->config.sysType;
	head.dataNumber = m_dataNumber;
	head.networkA = m_networkA;//通讯状态
	head.networkB = m_networkB;//通讯状态
	m_dataNumber++;

	return ;
}
