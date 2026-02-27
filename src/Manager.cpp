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
    : m_memDbInited(false),
      m_deviceInited(false),
      m_ioStarted(false),
      m_netStarted(false),
      m_ctrlStarted(false) {
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
        this->Uninit();
        return ErrorInfo::ERR_OPENED;
    }
    m_memDbInited = true;
    zlog_error(Util::m_zlog, "初始化实时数据库成功");

    // 2、初始化设备信息
    zlog_error(Util::m_zlog, "初始化设备信息");
    Device *dev = Device::GetInstance();
    if (NULL == dev) {
        zlog_error(Util::m_zlog, "初始化设备信息失败");
        this->Uninit();
        return ErrorInfo::ERR_NULL;
    }
    if (!dev->Init()) {
        zlog_error(Util::m_zlog, "初始化设备信息失败");
        this->Uninit();
        return ErrorInfo::ERR_FAILED;
    }
    m_deviceInited = true;
    zlog_error(Util::m_zlog, "初始化设备信息成功");
#if 1
    // 3、开启通讯线程
    zlog_error(Util::m_zlog, "启动IO通讯");
    ret = m_io.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动IO通讯失败");
        this->Uninit();
        return ErrorInfo::ERR_FAILED;
    }
    m_ioStarted = true;
    zlog_error(Util::m_zlog, "启动IO通讯成功");
#endif
#if 1
    // 4、开启站内通讯线程
    zlog_error(Util::m_zlog, "启动站内通讯");
    ret = m_net.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动站内通讯失败");
        this->Uninit();
        return ErrorInfo::ERR_FAILED;
    }
    m_netStarted = true;
    zlog_error(Util::m_zlog, "启动站内通讯成功");
#endif

#if 1
    // 5、开启控制线程
    zlog_error(Util::m_zlog, "启动控制策略");
    ret = m_ctrl.Init();
    if (ErrorInfo::ERR_OK != ret) {
        zlog_error(Util::m_zlog, "启动控制策略失败");
        this->Uninit();
        return ErrorInfo::ERR_FAILED;
    }
    m_ctrlStarted = true;
    zlog_error(Util::m_zlog, "启动控制策略成功");
#endif

    // 6、开启波动率线程
    m_pvVolatility.Start();

    zlog_error(Util::m_zlog, "系统启动结束");

    return ret;
}

void Manager::Uninit() {
    zlog_error(Util::m_zlog, "系统关闭开始");
    //关闭控制策略
    if (m_ctrlStarted) {
        m_ctrl.Uninit();
        m_ctrlStarted = false;
    }

    //关闭站内通讯
    if (m_netStarted) {
        m_net.Uninit();
        m_netStarted = false;
    }

    //关闭通讯线程池
    if (m_ioStarted) {
        m_io.Uninit();
        m_ioStarted = false;
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
