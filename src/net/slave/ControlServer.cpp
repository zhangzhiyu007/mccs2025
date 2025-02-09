/*
 * ControlServer.cpp
 *
 *   创建日期: 2013-4-5
 *   作         者: mzh
 */

#include "ControlServer.h"
#include "../../etc/socket/Buffer.h"
#include "SlaveManeger.h"
#include "../../db/MemDb.h"

#define CFG_DATETIM_LIB 1
#include "libs_emfuture_odm.h"

ControlServer::ControlServer(Socket* sock, void* param) :
	TcpService(sock), m_slaveManager((SlaveManager*) param), m_timeout(100000)
{
	// TODO 构造函数
	pthread_mutex_init(&m_mutex, NULL);
	m_masterConfig = NULL;
	m_networkA = true;
}

ControlServer::~ControlServer()
{
	// TODO 析构函数
	pthread_mutex_destroy(&m_mutex);
}

void ControlServer::Lock()
{
	pthread_mutex_lock(&m_mutex);
}

void ControlServer::Unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

void ControlServer::Run()
{
	zlog_warn(Util::m_zlog, "Run");
	//得到当前网段
	Address addr = this->m_socket.Getpeername();
	string ip = addr.GetIPString(0);
	ControllerConfig* config = m_slaveManager->GetControllerConfig();//控制器配置
	if (config->ipA == ip)
	{
		m_networkA = true;
	}
	else
	{
		m_networkA = false;
	}

	zlog_warn(Util::m_zlog, "连接设备IP=%s",ip.c_str());

	while (true)
	{
		//退出线程
		if (m_state == Thread::STOPPING)
		{
			zlog_warn(Util::m_zlog, "退出TCP服务器控制线程");
			break;
		}

		//判断是否连接
		if (!m_socket.IsOpened())
		{
			//连接断开
			zlog_warn(Util::m_zlog, "客户端断开连接,停止线程工作");
			break;
		}

		//阻塞式读取数据
		if (ErrorInfo::ERR_OPENED == RecvInfo())
		{
			//连接断开
			zlog_warn(Util::m_zlog, "客户端断开连接,停止线程工作,%d",(int)m_masterConfig);
			Cancel();
		}

		usleep(5000);
	}

	//重新置标志位
	zlog_warn(Util::m_zlog, "退出控制线程：重新置标志位");
	if (NULL != m_masterConfig)
	{
		if (m_networkA)
		{
			m_masterConfig->networkA = Iec103::ABNORMAL;
		}
		else
		{
			m_masterConfig->networkB = Iec103::ABNORMAL;
		}

		if ((Iec103::NORMAL != m_masterConfig->networkA) && (Iec103::NORMAL
				!= m_masterConfig->networkB))
		{
			m_masterConfig->isRun = false;
			m_masterConfig->flag = false;
		}

		m_masterConfig->dataNumber=0;//计数清零
	}

	m_state == Thread::STOPPED;
	zlog_warn(Util::m_zlog, "退出控制线程");
	return;
}

int ControlServer::RecvInfo()
{
	Address addr = this->m_socket.Getpeername();//被连接主机地址
	int num = 0;
	String recvStr;
	Iec103 iec103;
	Iec103::PacketHead head;
	Iec103::PacketData data;
	bool success = false;
	PtrArray::iterator iter;

	num = m_socket.Recv(recvStr, MAX_VALUE);

	if (num < 0)
	{
		//说明连接已经断开,则返回
		zlog_warn(Util::m_zlog,"连接已经断开,则返回");
		return ErrorInfo::ERR_OPENED;
	}
	else if (num == 1)
	{
		return ErrorInfo::ERR_NO_DATA;//没有收到数据
	}
	else if (num == 0)
	{
		return ErrorInfo::ERR_NO_DATA;//没有收到数据
	}

	ControllerConfig* config = m_slaveManager->GetControllerConfig();//控制器配置
	PtrArray* masterConfigs = m_slaveManager->GetMasterConfigs();//建立联系的主站配置信息

	//解析数据
	success = iec103.Unpack(head, data, recvStr);

	//判断包头部分
	//1.校验目的地址
	if ((config->controllerAddr != head.destAddr)//目的地址
			&& (BROADCAST_ADDR != head.destAddr))//广播地址
	{
		zlog_info(Util::m_zlog,"校验目的地址");
		return ErrorInfo::ERR_INV_RESP;
	}

	//2.校验原设备地址和数据编号
	bool isExist = false;

	if (NULL == m_masterConfig)
	{
		if (NULL != masterConfigs)//还没有该线程的主站信息,就在第一次接收到数据时保存
		{
			MasterConfig* masterConfig = NULL;
			for (iter = masterConfigs->begin(); iter != masterConfigs->end(); iter++)
			{
				masterConfig = reinterpret_cast<MasterConfig*> (*iter);
				//找到该地址及设备类型
				if ((NULL != masterConfig)
						&& (masterConfig->config.controllerAddr == head.srcAddr)
						&& (masterConfig->config.sysType == head.srcType))
				{
					isExist = true;
					m_masterConfig = masterConfig;

					if (m_networkA)
					{
						m_masterConfig->networkA = Iec103::NORMAL;
					}
					else
					{
						m_masterConfig->networkB = Iec103::NORMAL;
					}

					m_masterConfig->dataNumber = 0;

					break;
				}
			}
		}
	}
	else//以后只需要检查已经保存的即可
	{
		if ((m_masterConfig->config.controllerAddr == head.srcAddr)
				&& (m_masterConfig->config.sysType == head.srcType))
		{
			isExist = true;
		}
	}

	if (!isExist)
	{
		zlog_warn(Util::m_zlog,"不存在");
		return ErrorInfo::ERR_INV_RESP;
	}

	//判断是否已经有该数据了
	this->Lock();//加锁防止另一线程也比较数据编号
	bool isNew = iec103.DataComparison(head.dataNumber,
			m_masterConfig->dataNumber);
	if (!isNew)
	{
		zlog_debug(Util::m_zlog,"数据相同:接收到编号=%d,控制器编号=%d",head.dataNumber,m_masterConfig->dataNumber);
		success = true;
	}
	else//说明是新数据
	{
		//保存数据
		//3.保存数据
		switch (data.type)
		{
		case Iec103::CTRL_DATA://控制数据
			zlog_warn(Util::m_zlog,"控制数据");
			success = this->SaveCtrlData(data);//保存数据
			if (success)
			{
				m_masterConfig->dataNumber = head.dataNumber;
			}

			break;
		case Iec103::TIME_SYNC://时间同步数据
			success = this->SaveTimesync(data);
			if (success)
			{
				m_masterConfig->dataNumber = head.dataNumber;
			}

			break;
		default:
			success = false;
			break;
		}
	}
	this->Unlock();
	zlog_debug(Util::m_zlog,"解析结束");

	return SendInfo(success, head);
}

int ControlServer::SendInfo(bool confirm, Iec103::PacketHead head)
{
	Iec103::PacketHead packetHead;
	this->PackHead(packetHead, head);

	Iec103::PacketData packetData;

	packetData.destAddr = head.srcAddr;
	packetData.type = Iec103::CONFIRM;
	packetData.fcb = 0;
	packetData.fcv = 0;
	packetData.prm = 0;

	if (confirm)
	{
		packetData.fun = 0;
	}
	else
	{
		packetData.fun = 1;
	}

	Iec103 iec103;
	String sendStr = iec103.Pack(&packetHead, &packetData);
	int ret = m_socket.SendFlush(sendStr);
	if (ret <= 0)
	{
		return ErrorInfo::ERR_WRITE;
	}

	return ERROR_OK;
}

void ControlServer::PackHead(Iec103::PacketHead& destHead,
		Iec103::PacketHead& srcHead)
{
	//封装包头
	ControllerConfig* config = m_slaveManager->GetControllerConfig();
	destHead.srcAddr = config->controllerAddr;
	destHead.srcType = config->sysType;
	destHead.destAddr = srcHead.srcAddr;
	destHead.destType = srcHead.srcType;
	//packetHead.dataNumber = config->dataNumber;

	if (config->networkAUsed)
	{
		destHead.networkA = srcHead.networkA;
	}
	else
	{
		destHead.networkA = Iec103::NO_USED;//没有使用
	}

	if (config->networkBUsed)
	{
		destHead.networkB = srcHead.networkB;
	}
	else
	{
		destHead.networkB = Iec103::NO_USED;//没有使用
	}

	return;
}

bool ControlServer::SaveCtrlData(Iec103::PacketData& data)
{
	bool success = false;
	int i = 0;
	int start = 0;
	int tmp = 0;
	float fTmp = 0.0;
	if (Iec103::ALONE_ADDR == data.continuous)//每个数据单独地址
	{
		zlog_debug(Util::m_zlog,"每个数据单独地址");
		//校验数据长度,每个数据占7个字节
		if ((data.dataLength * 7) > data.data.size())
		{
			zlog_debug(Util::m_zlog, "校验数据长度不正确:dataLength=%d>size=%d",data.dataLength*7,data.data.size());
			success = false;
		}
		else
		{
			int reg_addr = 0;

			zlog_debug(Util::m_zlog,"收到数据长度：%d",data.dataLength);
			for (i = 0; i < data.dataLength; i++)
			{
				start = 7* i ;
				reg_addr = data.data[start + 0] + (data.data[start + 1] << 8);
				tmp = data.data[start + 3] + (data.data[start + 4] << 8)
						+ (data.data[start + 5] << 16) + (data.data[start + 6]
						<< 24);
				if (RealData::DB == data.data[start + 2])//实型
				{
					memcpy(&fTmp, &tmp, sizeof(int));
					MemDb::SetRealData(reg_addr, fTmp, true);
					zlog_warn(Util::m_zlog,"数据地址=%d,值=%.2f",reg_addr,fTmp);
				}
				else//整形
				{
					MemDb::SetRealData(reg_addr, tmp, true);
					zlog_warn(Util::m_zlog,"数据地址=%d,值=%d",reg_addr,tmp);
				}
			}

			success = true;

		}
	}
	else//连续地址
	{
		zlog_debug(Util::m_zlog,"连续地址");
		//校验数据长度,每个数据占5个字节
		if ((data.dataLength * 5) > data.data.size())
		{
			zlog_warn(Util::m_zlog, "校验数据长度不正确");
			success = false;
		}
		else
		{
			for (i = 0; i < data.dataLength; i++)
			{
				start = 5* i ;
				tmp = data.data[start + 1] + (data.data[start + 2] << 8)
						+ (data.data[start + 3] << 16) + (data.data[start + 4]
						<< 24);
				if (RealData::DB == (char) data.data[start])//实型
				{
					memcpy(&fTmp, &tmp, sizeof(int));
					MemDb::SetRealData(data.dataStart + i, fTmp, true);
				}
				else//整形
				{
					MemDb::SetRealData(data.dataStart + i, tmp, true);
				}
			}
			success = true;
		}
	}

	zlog_debug(Util::m_zlog,"每个数据单独地址结束");
	return success;
}

bool ControlServer::SaveTimesync(Iec103::PacketData& data)
{
	zlog_debug(Util::m_zlog,"时间同步帧");
//	unsigned short tm[6];
//
//	tm[5] = (data.data[0] + (data.data[1] << 8)) / 1000;
//	tm[4] = data.data[2];
//	tm[3] = data.data[3];
//	tm[2] = data.data[4] & 0x1F;
//	tm[1] = data.data[5];
//	tm[0] = data.data[6] + 2000;
//
//	zlog_warn(Util::m_zlog,"时间同步帧:%d-%d-%d %d:%d:%d",tm[0],tm[1],tm[2],tm[3],tm[4],tm[5]);
//	SetRtcTime(tm, 1);
//	zlog_debug(Util::m_zlog,"时间同步帧结束");

	UshortArray tm;
	tm.push_back(data.data[6] + 2000);
	tm.push_back(data.data[5]);
	tm.push_back(data.data[4] & 0x1F);
	tm.push_back(data.data[3]);
	tm.push_back(data.data[2]);
	tm.push_back((data.data[0] + (data.data[1] << 8)) / 1000);
	tm.push_back((data.data[0] + (data.data[1] << 8)) % 1000);
	bool success = Util::SetSystemTime(tm);
	zlog_debug(Util::m_zlog,"时间同步帧结果=%d",success);
	return true;
}
