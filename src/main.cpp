/*
 * main.cpp
 *
 *   创建日期: 2013-9-27
 *   作          者: 马中华
 */

#include "./Manager.h"
#include "./db/Device.h"
#include "./util/Util.h"

//主函数
int main(int argc, char **argv) {
    bool ret = false;
    // 1、初始化
    ret = Util::Init();
    if (!ret) {
        return 0;
    }
    // 记录系统启动动作
    zlog_error(Util::m_zlog_sysStatus, "系统启动");
    //
    // 2、启动服务器管理接口
    zlog_error(Util::m_zlog, "启动服务器管理接口");
    Manager manager;
    ret = manager.Init();
    if (!ret) {
        zlog_error(Util::m_zlog, "启动服务器管理接口失败");
        Util::Uninit();
        return 0;
    }

    zlog_error(Util::m_zlog, "启动服务器管理接口成功");

    // 3、看门狗
    WatchDog watchDog;
#if UNUSED
    Device *dev = Device::GetInstance();
    ret         = watchDog.Open(60);
    zlog_error(Util::m_zlog, "启动看门狗%s，超时时间=60 s",
               ret ? "成功" : "失败");
#endif

    while (true) {
        //喂狗操作
        watchDog.Feed();
        sleep(1);
    }

    zlog_error(Util::m_zlog, "系统关闭");
    zlog_error(Util::m_zlog_sysStatus, "系统关闭");
    //关闭看门狗
    watchDog.Close();

    //管理服务关闭
    manager.Uninit();

    //关闭所有信息
    Util::Uninit();

    return 0;
}
