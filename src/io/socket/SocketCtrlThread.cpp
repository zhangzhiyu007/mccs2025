/*
 * SocketCtrlThread.cpp
 *
 *   创建日期: 2013-8-8
 *   作         者: mzh
 */

#include "SocketCtrlThread.h"
#include "../../db/Device.h"
#include "../../db/MemDb.h"
#include "../../io/device/pcs/Pcs.h"
#include "../device/ACsource/ACSource.h"
#include "../device/DiDoModle/DiDoModle.h"
#include "../device/PVsimulator/PVSimulator.h"
#include "../device/bms/BMS.h"
#include "../device/dcdc/DCDC.h"
#include "../device/dcload/DcLoad.h"
#include "../device/pvs/Pvs.h"
#include "SocketThread.h"
#include "../device/insulationMonitor/insulationMonitor.h"

#include "../device/watt/Wattmeter.h"

// #include "../device/chrpile/ChargePile.h"

/* Mingyu
 */
#include "../device/CNCU/CNCU.h"
#include "../device/acload/AcLoad.h"
#include "../device/faultSimultor/FaultSimultor.h"
#include "../device/impedancesimultor/ImpedanceSimulator.h"
#include "../device/prot/sts.h"
#include "../device/watt/HcWatt.h"
#include <netinet/tcp.h>

SocketCtrlThread::SocketCtrlThread() {
    // TODO 构造函数
    m_socketThreads = NULL;
    m_socketAddress = NULL;
}

SocketCtrlThread::~SocketCtrlThread() {
    // TODO 析构函数
}

// 保存从设备信息
void SocketCtrlThread::SetSocketTypeDev(SocketAddress *socketAddress) {
    assert(socketAddress != NULL);
    m_socketAddress = socketAddress;
}

// 保存socket线程指针
void SocketCtrlThread::SetSocketThreads(SocketThreads *socketThreads) {
    assert(socketThreads != NULL);
    m_socketThreads = socketThreads;
}

void SocketCtrlThread::SetTcpClent(TcpClient *client) {
    this->m_tcpClient = client;
}

// 线程运行
void SocketCtrlThread::Run() {
    zlog_warn(Util::m_zlog, "以太网线程运行:IP=%s,port=%d",
              m_socketAddress->ip.c_str(), m_socketAddress->port);

    // 查找指定以太网从设备
    zlog_warn(Util::m_zlog, "查找指定以太网从设备");
    FindSocketSlaveDevs();
    zlog_warn(Util::m_zlog, "查找指定以太网从设备,数量=%d",
              m_socketSlaveDevs.size());

    // 设备IP和端口地址
    Address(m_socketAddress->ip, m_socketAddress->port);

    TcpClient *client = m_tcpClient;

    // 存取从设备信息数组
    Device::SlaveDev *slaveDev = NULL;
    PtrArray::iterator slaveIter;
    int ret = ErrorInfo::ERR_OK;

    // 通讯状态
    bool comSuccess       = true;
    int uNoResponse       = 0;  // 当前读取失败的次数
    const int NO_RESPONSE = 10; // 连续该次数下认为通讯失败
    int openFailed        = 0;

    // // ChargePile
    // ChargePile chargePile;
    // chargePile.SetIsTcp(true);
    // chargePile.SetTcp(client);

    // 电表
    Wattmeter watt;
    watt.SetIsTcp(true);
    watt.SetTcp(client);

    // 绝缘监测仪
    InsulationMonitor insulationMonitor;
    insulationMonitor.SetIsTcp(true);
    insulationMonitor.SetTcp(client);

    // pcs
    Pcs pcs;
    pcs.SetIsTcp(true);
    pcs.SetTcp(client);

    // dcdc
    DCDC dcdc;
    dcdc.SetIsTcp(true);
    dcdc.SetTcp(client);

    // DIDO模块
    DiDoModle dido;
    dido.SetIsTcp(true);
    dido.SetTcp(client);

    // Pvs
    Pvs pvs;
    pvs.SetIsTcp(true);
    pvs.SetTcp(client);

    // 光伏模拟器
    PVSimulator simulator;
    simulator.SetIsTcp(true);
    simulator.SetTcp(client);

    // BMS
    BMS bms;
    bms.SetIsTcp(true);
    bms.SetTcp(client);

    // 电网模拟器
    ACSource acSource;
    acSource.SetTcp(client);

    // 直流负载
    DcLoad dcload;
    dcload.SetIsTcp(true);
    dcload.SetTcp(client);

    /* Mingyu
     */
    // 复合节点控制器
    CNCU cncu;
    cncu.SetIsTcp(true);
    cncu.SetTcp(client);

    /*	Mingyu - 2021-03-22
     *	线路阻抗模拟器
     */
    ImpedanceSimulator ImpSim;
    ImpSim.SetIsTcp(true);
    ImpSim.SetTcp(client);
    /*	Mingyu - 2021-03-26
     *	故障模拟器
     */
    FaultSimultor FaultSim;
    FaultSim.SetTcp(client);

    AcLoad acLoad;
    acLoad.SetTcp(client);

    HcWatt hcWatt;
    hcWatt.SetIsTcp(true);
    hcWatt.SetTcp(client);

    // 晶石STS
    STS sts;
    sts.SetTcp(client);

    zlog_warn(Util::m_zlog, "以太网从设备采集主程序执行");
    while (true) {
        // 判断线程是否结束
        if (m_state == Thread::STOPPING) {
            zlog_warn(Util::m_zlog, "socket线程结束");
            break;
        }

        // 判断socket是否打开
        if (!client->IsOpened()) {
            sleep(1);
            continue;
        }

        for (slaveIter = m_socketSlaveDevs.begin();
             slaveIter != m_socketSlaveDevs.end(); slaveIter++) {
            slaveDev = reinterpret_cast<Device::SlaveDev *>(*slaveIter);

            if (NULL == slaveDev) {
                continue;
            }

            // 判断设备名称
            switch (slaveDev->devType) {
            // 判断设备厂商和型号来确定具体通信协议
            case DeviceType::DEVICE_TYPE_PCS:          // PCS
            case DeviceType::DEVICE_TYPE_BIPOLAR_ACDC: // 双极性AC/DC
                ret = pcs.Preset(slaveDev);
                break;
            // case DeviceType::DEVICE_INSULATION_MONITOR: // 绝缘监测仪
            //     ret = insulationMonitor.Preset(slaveDev);
            //     break;
            case DeviceType::DEVICE_TYPE_DO_MODULE: // Do模块
                ret = dido.Preset(slaveDev);
                break;
            // case DeviceType::DEVICE_TYPE_CHARGE_PILE: // 充电桩
            //     ret = chargePile.Preset(slaveDev);
            case DeviceType::DEVICE_TYPE_DCDC: // dcdc
                ret = dcdc.Preset(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIMU_PV: // 光伏模拟器
                ret = simulator.Preset(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_PVS_XV: // 光伏逆变器
                ret = pvs.Preset(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SINMU_LIB: // 电池模拟器
            case DeviceType::DEVICE_TYPE_LIION:     // 电池
                ret = bms.Preset(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIMU_DC_LOAD: // 直流负载
            case DeviceType::DEVICE_TYPE_DC_LOAD:      // 直流负载
                ret = dcload.Preset(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIMU_AC_GRID: // 电网模拟器
                ret = acSource.Preset(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_CNCU: // Mingyu add on 20200217
                ret = cncu.Preset(slaveDev);   // 复合节点控制器
                break;
            case DeviceType::DEVICE_TYPE_SIMU_CABLE: // Mingyu add on 20210322
                ret = ImpSim.Preset(slaveDev);       // 复合节点控制器
                break;
            case DeviceType::DEVICE_TYPE_SINMU_GZ: // Mingyu add on 20210327
                ret = FaultSim.Preset(slaveDev);   // 线路故障模拟器
                break;
            case DeviceType::DEVICE_TYPE_SIMU_RLC: // RLC负载
                ret = acLoad.Preset(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_WATT:
                ret = hcWatt.Preset(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_STS:
                ret = sts.Preset(slaveDev);
                break;
            default:
                break;
            };

            msleep(50);
        }

        msleep(100);
    }
}

void SocketCtrlThread::FindSocketSlaveDevs() {
    // 查找该端口和IP下的设备

    // 得到实例并初始化
    Device *device = Device::GetInstance();

    // 获取网口从设备信息指针
    PtrArray *slaveArray = device->GetSlaveDevArray();

    // 读取通讯类型指针
    PtrArray *constCommTypeArray = device->GetConstCommTypeArray();
    // 以太网类型
    Device::ConstCommType *commType =
        (Device::ConstCommType *)constCommTypeArray->at(1);

    // 获取本控制器地址
    int controllerAddr = device->GetControllerAddr();
    // 存取从设备信息数组
    Device::SlaveDev *slaveDev = NULL;
    PtrArray::iterator slaveIter;

    // 1、读取相同IP和端口设备
    m_socketSlaveDevs.clear();
    for (slaveIter = slaveArray->begin(); slaveIter != slaveArray->end();
         slaveIter++) {
        slaveDev = reinterpret_cast<Device::SlaveDev *>(*slaveIter);

        if ((NULL != slaveDev) &&
            (slaveDev->commType == commType->type)       // 以太网类型
            && (m_socketAddress->port == slaveDev->port) // 指定IP
            && (m_socketAddress->ip == slaveDev->ip) &&
            (slaveDev->own == controllerAddr)) // 指定端口
        {
            //			zlog_warn(Util::m_zlog, "从设备:%s,%s",
            //					slaveDev->name.c_str(),
            // commType->desc.c_str());

            m_socketSlaveDevs.push_back(slaveDev);
        }
    }

    return;
}
