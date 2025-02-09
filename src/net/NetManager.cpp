/*
 * NetManager.cpp
 *
 *   创建日期: 2014-1-10
 *   作          者: 马中华
 */

#include "NetManager.h"
#include "../util/Util.h"
#include "../db/Device.h"
#include "../db/MemDb.h"
#include "../db/Db.h"

NetManager::NetManager()
{
	// TODO 构造函数

}

NetManager::~NetManager()
{
	// TODO 析构函数
}

int NetManager::Init()
{
	zlog_error(Util::m_zlog, "启动站内通讯");
	int ret = ErrorInfo::ERR_OK;
	bool success = false;

	//1、启动从站内通讯线程
	zlog_error(Util::m_zlog, "启动从站站内通讯");
	success = m_slaveManager.Init();
	if(success)
	{
		zlog_error(Util::m_zlog, "启动从站站内通讯成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "启动从站站内通讯失败");
		return ErrorInfo::ERR_FAILED;
	}

	//2、启动主站内通讯线程
	//(1)如果本控制器是协调控制器，则不需要启动主站内通讯线程
	Device* dev = Device::GetInstance();
	if (NULL != dev)
	{
		if (DevControllerType::SLAVE == dev->GetControllerType())
		{
			zlog_error(Util::m_zlog, "启动站内通讯结束");
			return ret;
		}
	}

	//(2)启动主站站内通讯
	zlog_error(Util::m_zlog, "站内通讯,启动主站站内通讯");
	ret = m_masterManger.Init();
	if (ErrorInfo::ERR_OK == ret)
	{
		zlog_error(Util::m_zlog, "启动主站站内通讯成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "启动主站站内通讯失败");
		return ErrorInfo::ERR_FAILED;
	}

	zlog_error(Util::m_zlog, "启动站内通讯结束");
	return ret;
}

void NetManager::Uninit()
{
	zlog_error(Util::m_zlog, "关闭站内通讯");
	//1、关闭主站站内通讯
	m_masterManger.Uninit();
	//2、关闭从站站内通讯
	m_slaveManager.Uninit();

	zlog_error(Util::m_zlog, "关闭站内通讯结束");
	return;
}
