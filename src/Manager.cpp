/*
 * Manager.cpp
 *
 *   创建日期: 2013-9-27
 *   作          者: 马中华
 */

#include "Manager.h"
#include "./db/Db.h"
#include "./db/Device.h"
#include "./db/MemDb.h"
#include "./util/Util.h"

Manager::Manager() {
    // TODO 构造函数
}

Manager::~Manager() {
    // TODO 析构函数
}

int Manager::Init() {
    zlog_error(Util::m_zlog, "系统启动开始");
    int ret      = ErrorInfo::ERR_OK;
    bool success = false;

    // 1、实时数据库初始化
    zlog_error(Util::m_zlog, "初始化实时数据库");
    success = MemDb::Init();
    if (!success) {
        zlog_error(Util::m_zlog, "初始化实时数据库失败");
        return ErrorInfo::ERR_OPENED;
    }
    zlog_error(Util::m_zlog, "初始化实时数据库成功");

    // 2、初始化设备信息
    zlog_error(Util::m_zlog, "初始化设备信息");
    Device *dev = Device::GetInstance();
    if (NULL == dev) {
        zlog_error(Util::m_zlog, "初始化设备信息失败");
        return ErrorInfo::ERR_NULL;
    }
    zlog_error(Util::m_zlog, "初始化设备信息成功");
#if 1
    // 3、开启通讯线程
    zlog_error(Util::m_zlog, "启动IO通讯");
    ret = m_io.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动IO通讯失败");
        return ErrorInfo::ERR_FAILED;
    }
    zlog_error(Util::m_zlog, "启动IO通讯成功");
#endif
#if 1
    // 4、开启站内通讯线程
    zlog_error(Util::m_zlog, "启动站内通讯");
    ret = m_net.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动站内通讯失败");
        return ErrorInfo::ERR_FAILED;
    }
    zlog_error(Util::m_zlog, "启动站内通讯成功");
#endif

#if 1
    // 5、开启控制线程
    zlog_error(Util::m_zlog, "启动控制策略");
    ret = m_ctrl.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动控制策略失败");
        return ErrorInfo::ERR_FAILED;
    }
    zlog_error(Util::m_zlog, "启动控制策略成功");
#endif

    // 6、开启波动率线程
    m_pvVolatility.Start();

    zlog_error(Util::m_zlog, "系统启动结束");

    return true;
}

void Manager::Uninit() {
    zlog_error(Util::m_zlog, "系统关闭开始");
    //关闭控制策略
    m_ctrl.Uninit();

    //关闭站内通讯
    m_net.Uninit();

    //关闭通讯线程池
    m_io.Uninit();

    //实时数据库结束
    MemDb::Uninit();

    //设备反初始化
    Device *dev = Device::GetInstance();
    dev->FreeInstanse();

    zlog_error(Util::m_zlog, "系统关闭结束");
}
