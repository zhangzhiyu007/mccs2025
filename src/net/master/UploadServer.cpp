/*
 * UploadServer.cpp
 *
 *   创建日期: 2013-4-1
 *   作         者: mzh
 */

#include "UploadServer.h"
#include "MasterManager.h"
#include "../../etc/socket/Buffer.h"
#include "../../db/MemDb.h"

UploadServer::UploadServer() :
	m_timeout(10000)
{
	// TODO 构造函数
	m_masterManager = NULL;
	m_udp = NULL;//UDP指针

	m_networkA = Iec103::NO_USED;
	m_networkB = Iec103::NO_USED;
}

UploadServer::~UploadServer()
{
	// TODO 析构函数
	//关闭UDP释放内存
	CloseUdp();
	if (NULL != m_udp)
	{
		delete m_udp;
		m_udp = NULL;
	}
}

void UploadServer::SetMasterManager(MasterManager* manager)
{
	this->m_masterManager = manager;
}

bool UploadServer::IsUdpOpened()
{
	bool isOpen = false;
	if ((NULL != m_udp) && (m_udp->IsOpened()))
	{
		isOpen = true;
	}

	return isOpen;
}

bool UploadServer::OpenedUdp()
{
	zlog_info(Util::m_zlog, "站内设备UDP监视线程:打开UDP");
	UdpNode* udp = NULL;//
	bool isOpen = false;
	const int port = m_masterManager->GetMasterUploadPort();//主站UDP上传端口
	ControllerConfig* config = m_masterManager->GetControllerConfig();
	const bool isServer = true;//服务器端
	const bool isBroadcast = false;//无效

	//UDP服务器端,等待数据到来,只需要等待数据
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
	zlog_info(Util::m_zlog, "站内设备UDP监视线程:打开UDP%s",isOpen?"成功":"失败");

	return isOpen;

}

void UploadServer::CloseUdp()
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

void UploadServer::Run()
{
	zlog_info(Util::m_zlog, "站内设备主站UDP服务器数据上传线程");

	//进入线程,首先打开
	this->OpenedUdp();

	//线程循环
	while (true)
	{
		//1、外部命令退出
		if (m_state == Thread::STOPPING)
		{
			zlog_info(Util::m_zlog, "退出站内设备主站UDP服务器数据上传线程");
			break;
		}

		//2、判断UDP是否打开
		//只要有一个时打开的即认为是打开的
		if (!this->IsUdpOpened())
		{
			zlog_warn(Util::m_zlog,"UDP没有打开，退出线程");
			break;
		}

		//3.循环判断是否有数据收到
		if (this->IsUdpOpened())
		{
			RecvInfo(m_udp);
		}

		msleep(10);
	}

	//关闭UDP客户端
	this->CloseUdp();

	zlog_info(Util::m_zlog, "站内设备主站UDP服务器数据上传线程退出");
	return;
}

int UploadServer::RecvInfo(UdpNode* udp)
{
	ControllerConfig* config = m_masterManager->GetControllerConfig();//控制器配置
	PtrArray* slaveConfigs = m_masterManager->GetSlaveConfig();
	SlaveConfig* slaveConfig = NULL;
	PtrArray::iterator iter;

	String recvStr;
	Address addr;//设备IP地址和端口

	Iec103 iec103;//iec103协议
	Iec103::PacketHead head;//帧头
	Iec103::PacketData data;//ASDU数据包

	int ret = 0;
	bool success = false;
	int i = 0;
	int start = 0;
	int tmp = 0;
	float fTmp=0.0;
	RealData realData;

	//没有打开
	if ((NULL == udp) || (!udp->IsOpened()))
	{
		return ErrorInfo::ERR_OPENED;
	}

	//判断有没有数据
	ret = udp->Recvfrom(addr, recvStr, MAX_VALUE);
	if (ret < 0)
	{
		return ErrorInfo::ERR_OPENED;
	}
	else if(ret <= 0)
	{
		return ErrorInfo::ERR_NO_DATA;
	}
	//接收到数据
	success = iec103.Unpack(head, data, recvStr);

	//判断包头部分
	//1.校验目的地址
	if ((config->controllerAddr != head.destAddr)//目的地址
			&& (BROADCAST_ADDR != head.destAddr))//广播地址
	{
		return ErrorInfo::ERR_INV_RESP;//不是发往本机
	}

	//如果是自己广播的，就不用读取
	if(config->controllerAddr==head.srcAddr)
	{
		return ErrorInfo::ERR_INV_RESP;//本机自己发送的
	}

	//2.校验原设备地址和数据编号
	success = false;
	zlog_debug(Util::m_zlog,"slaveConfig:%d",slaveConfigs->size());
	for (iter = slaveConfigs->begin(); iter != slaveConfigs->end(); iter++)
	{
		slaveConfig = reinterpret_cast<SlaveConfig*> (*iter);
		slaveConfig->isRun = true;//
		//找到该地址及设备类型
		if ((NULL != slaveConfig) && (slaveConfig->config.controllerAddr
				== head.srcAddr) && (slaveConfig->config.sysType
				== head.srcType) && slaveConfig->isRun)
		{
			//找到
			success = true;
			break;
		}
	}

	if (!success)
	{
		return false;
	}

	//3.保存数据
	if (Iec103::ALONE_ADDR == data.continuous)//每个数据单独地址
	{
		//校验数据长度,每个数据占7个字节
		if ((data.dataLength * 7) > data.data.size())
		{
			return false;
		}

		int reg_addr = 0;

		for (i = 0; i < data.dataLength; i++)
		{
			start = 7* i ;
			reg_addr = data.data[start + 0] + (data.data[start + 1] << 8);
			tmp = data.data[start + 3] + (data.data[start + 4] << 8)
					+ (data.data[start + 5] << 16) + (data.data[start + 6]
					<< 24);
			if (RealData::DB == (char) data.data[start + 2])//实型
			{
				memcpy(&fTmp,&tmp,sizeof(int));
				MemDb::SetRealData(reg_addr, fTmp);
			}
			else//整形
			{
				MemDb::SetRealData(reg_addr, tmp);
			}
		}
	}
	else//连续地址
	{
		//校验数据长度,每个数据占5个字节
		if ((data.dataLength * 5) > data.data.size())
		{
			return false;
		}

		for (i = 0; i < data.dataLength; i++)
		{
			start = 5* i ;
			tmp = data.data[start + 1] + (data.data[start + 2] << 8)
					+ (data.data[start + 3] << 16) + (data.data[start + 4]
					<< 24);
			if (RealData::DB == (char) data.data[start])//实型
			{
				memcpy(&fTmp,&tmp,sizeof(int));
				MemDb::SetRealData(data.dataStart + i, fTmp);
			}
			else//整形
			{
				MemDb::SetRealData(data.dataStart + i, tmp);
			}
		}
	}

	return ErrorInfo::ERR_OK;
}
