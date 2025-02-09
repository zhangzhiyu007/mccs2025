/*
 * WatchDog.cpp
 *   Copyright (c) 2012-2014,北京中电建投微电网科技有限公司
 *   All rights reserved.
 *   创建日期: 2014-2-23
 *   作          者: 马中华
 *
 *   版          本：1.0
 *   作          者：马中华
 *   修改内容: 看门狗功能封装
 */

#include "WatchDog.h"
#define CFG_WDT_LIB 1
#include "libs_emfuture_odm.h"

#include "Util.h"

WatchDog::WatchDog()
{
	// TODO 构造函数

}

WatchDog::~WatchDog()
{
	// TODO 析构函数
}

/*
* 名称: Open
* 功能: 打开看门狗
* 输入:
* timeout    :[in]int   看门狗超时时间，单位s
* 输出: true成功/false失败
*/
bool WatchDog::Open(int timeout)
{
	zlog_warn(Util::m_zlog, "启动看门狗，超时时间=%d s", timeout);
	int ret = EnableWtd(timeout);
	bool success = false;
	if (ErrorInfo::ERR_OK == ret)
	{
		success = true;
	}

	zlog_error(Util::m_zlog, "启动看门狗%s，超时时间=%d s",ret?"成功":"失败",timeout);
	return success;
}

/*
* 名称: Close
* 功能: 关闭看门狗
* 输入: 无
* 输出: 无
*/
bool WatchDog::Close()
{
	zlog_warn(Util::m_zlog, "关闭看门狗");
	int ret = DisableWtd();
	bool success = false;
	if (ErrorInfo::ERR_OK == ret)
	{
		success = true;
	}

	return success;
}

/*
* 名称: Feed
* 功能: 喂狗操作
* 输入: 无
* 输出: true成功/false失败
*/
bool WatchDog::Feed()
{
	zlog_debug(Util::m_zlog, "喂狗操作");
	int ret = FeedWtd();
	bool success = false;
	if (ErrorInfo::ERR_OK == ret)
	{
		success = true;
	}

	return success;
}
