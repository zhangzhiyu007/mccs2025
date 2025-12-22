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

Manager::Manager()
    : m_memDbInited(false), m_deviceInited(false), m_ioInited(false),
      m_netInited(false), m_ctrlInited(false), m_pvStarted(false) {}

Manager::~Manager() {}

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
    m_memDbInited = true;

    // 2、初始化设备信息
    zlog_error(Util::m_zlog, "初始化设备信息");
    Device *dev = Device::GetInstance();
    if (NULL == dev) {
        zlog_error(Util::m_zlog, "初始化设备信息失败");
        return ErrorInfo::ERR_NULL;
    }
    if (!dev->Init()) {
        zlog_error(Util::m_zlog, "初始化设备信息失败");
        ret = ErrorInfo::ERR_FAILED;
        goto fail;
    }
    zlog_error(Util::m_zlog, "初始化设备信息成功");
    m_deviceInited = true;
#if 1
    // 3、开启通讯线程
    zlog_error(Util::m_zlog, "启动IO通讯");
    ret = m_io.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动IO通讯失败");
        ret = ErrorInfo::ERR_FAILED;
        goto fail;
    }
    zlog_error(Util::m_zlog, "启动IO通讯成功");
    m_ioInited = true;
#endif
#if 1
    // 4、开启站内通讯线程
    zlog_error(Util::m_zlog, "启动站内通讯");
    ret = m_net.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动站内通讯失败");
        ret = ErrorInfo::ERR_FAILED;
        goto fail;
    }
    zlog_error(Util::m_zlog, "启动站内通讯成功");
    m_netInited = true;
#endif

#if 1
    // 5、开启控制线程
    zlog_error(Util::m_zlog, "启动控制策略");
    ret = m_ctrl.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动控制策略失败");
        ret = ErrorInfo::ERR_FAILED;
        goto fail;
    }
    zlog_error(Util::m_zlog, "启动控制策略成功");
    m_ctrlInited = true;
#endif

    // 6、开启波动率线程
    m_pvVolatility.Start();
    m_pvStarted = true;

    zlog_error(Util::m_zlog, "系统启动结束");

    return ret;

fail:
    Uninit();
    return ret;
}

void Manager::Uninit() {
    zlog_error(Util::m_zlog, "系统关闭开始");

    if (m_pvStarted) {
        m_pvVolatility.Stop();
        m_pvStarted = false;
    }
    //关闭控制策略
    if (m_ctrlInited) {
        m_ctrl.Uninit();
        m_ctrlInited = false;
    }

    //关闭站内通讯
    if (m_netInited) {
        m_net.Uninit();
        m_netInited = false;
    }

    //关闭通讯线程池
    if (m_ioInited) {
        m_io.Uninit();
        m_ioInited = false;
    }

    //实时数据库结束
    if (m_memDbInited) {
        MemDb::Uninit();
        m_memDbInited = false;
    }

    //设备反初始化
    if (m_deviceInited) {
        Device *dev = Device::GetInstance();
        dev->FreeInstanse();
        m_deviceInited = false;
    }

    zlog_error(Util::m_zlog, "系统关闭结束");
}
