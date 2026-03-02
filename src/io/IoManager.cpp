/*
 * IoManager.cpp
 *
 *   创建日期: 2014-1-10
 *   作          者: 马中华
 */

#include "IoManager.h"
#include "../util/Util.h"
#include "../db/Device.h"
#include "../db/MemDb.h"
#include "../db/Db.h"

IoManager::IoManager()
{
	// TODO 构造函数
	m_check.addr = 1;
	m_check.regStart = 0;
	m_check.regEnd = 0xFFFF;
}

IoManager::~IoManager()
{
	// TODO 析构函数
}

/*
 * 名称: Init
 * 功能: 初始化IO通讯相关线程
 * 输出: 成功/失败
 */
int IoManager::Init()
{
	zlog_warn(Util::m_zlog, "初始化IO通讯相关线程");
	int ret = ErrorInfo::ERR_OK;
#if 0
	//1、开启串口线程池
	zlog_warn(Util::m_zlog, "开启串口线程池");
	//m_comThreads.SetManager(this);
	ret = m_comThreads.OpenThreads();
	if (ErrorInfo::ERR_OK == ret)
	{
		zlog_error(Util::m_zlog, "开启串口线程池成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "开启串口线程池失败");
		return ErrorInfo::ERR_FAILED;
	}

	//2、开启CAN口线程池
	zlog_warn(Util::m_zlog,"开启CAN口线程池");
	//m_canThreads.SetManager(this);
	ret = m_canThreads.OpenThreads();
	if (ErrorInfo::ERR_OK == ret)
	{
		zlog_error(Util::m_zlog, "开启CAN口线程池成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "开启CAN口线程池失败");
		return ErrorInfo::ERR_FAILED;
	}
#endif
	//3、开启socket线程池
	zlog_warn(Util::m_zlog,"开启socket线程池");
	//m_socketThreads.SetServerManager(this);
	ret = m_socketThreads.OpenThreads();
	if (ErrorInfo::ERR_OK == ret)
	{
		zlog_error(Util::m_zlog, "开启socket线程池成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "开启socket线程池失败");
		return ErrorInfo::ERR_FAILED;
	}


#if 1
	//5、开启Modbus TCP从站线程
	zlog_info(Util::m_zlog, "开启Modbus TCP从站服务器");
	Device* dev = Device::GetInstance();
	m_check.addr = dev->GetControllerAddr();
	m_tcpThreadedServer.SetParam(&m_check);
	m_tcpThreadedServer.Start(m_modbusTcpFactory, 502);
	zlog_warn(Util::m_zlog, "初始化IO通讯相关线程结束");
#endif
	return ErrorInfo::ERR_OK;
}

/*
 * 名称: Unint
 * 功能: 停止IO通讯相关线程
 * 输出: 无
 */
void IoManager::Uninit()
{
	zlog_warn(Util::m_zlog, "关闭IO通讯相关线程");
	//关闭以太网通讯线程
	m_tcpThreadedServer.Stop();

	//1、关闭以太网通讯(失败时重试，避免短暂阻塞导致长期半关闭)
	const int kSocketCloseRetryMax = 3;
	int socketCloseRet = ErrorInfo::ERR_FAILED;
	for (int retry = 0; retry < kSocketCloseRetryMax; ++retry)
	{
		socketCloseRet = m_socketThreads.CloseThreads();
		if (socketCloseRet == ErrorInfo::ERR_OK)
		{
			break;
		}
		zlog_error(Util::m_zlog,
				"关闭socket线程池失败(第%d/%d次)，准备重试",
				(retry + 1), kSocketCloseRetryMax);
		if (retry + 1 < kSocketCloseRetryMax)
		{
			msleep(1000);
		}
	}
	if (socketCloseRet != ErrorInfo::ERR_OK)
	{
		zlog_error(Util::m_zlog,
				"关闭socket线程池最终失败，可能仍有线程/资源处于保留状态");
	}

	//2、关闭CAN通讯
	m_canThreads.CloseThreads();
	//3、关闭串口通讯
	m_comThreads.CloseThreads();

	zlog_warn(Util::m_zlog, "关闭IO通讯相关线程结束");
	return;
}
