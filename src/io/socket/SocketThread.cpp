/*
 * SocketThread.cpp
 *
 *  Created on: 2013-4-10
 *      Author: liuhaitao
 */

#include "SocketThread.h"
#include "../../db/Device.h"
#include "../../db/MemDb.h"
#include "../../io/device/pcs/Pcs.h"
#include "../device/ACsource/ACSource.h"
#include "../device/CNCU/CNCU.h" // Mingyu
#include "../device/DiDoModle/DiDoModle.h"
#include "../device/PVsimulator/PVSimulator.h"
#include "../device/acload/AcLoad.h"
#include "../device/bms/BMS.h"
#include "../device/dcdc/DCDC.h"
#include "../device/dcload/DcLoad.h"
#include "../device/faultSimultor/FaultSimultor.h"          // Mingyu
#include "../device/impedancesimultor/ImpedanceSimulator.h" // Mingyu
#include "../device/insulationMonitor/insulationMonitor.h"  // Mingyu
#include "../device/powerDetection/PowerDetection.h"
#include "../device/prot/sts.h"
#include "../device/pvs/Pvs.h"
#include "../device/signalmodule/SignalModule.h"
#include "../device/watt/Wattmeter.h"

// #include "../device/chrpile/ChargePile.h"

SocketThread::SocketThread() {
    // TODO Auto-generated constructor stub
    m_socketThreads = NULL;
    m_socketAddress = NULL;
    m_socketSlaveDevs.clear();
}

SocketThread::~SocketThread() {
    // TODO Auto-generated destructor stub
    m_socketThreads = NULL;
}

// 保存从设备信息
void SocketThread::SetSocketTypeDev(SocketAddress *socketAddress) {
    m_socketAddress = socketAddress;
}

// 保存socket线程指针
void SocketThread::SetSocketThreads(SocketThreads *socketThreads) {
    assert(socketThreads != NULL);
    m_socketThreads = socketThreads;
}

void SocketThread::SetTcpClent(TcpClient *client) {
    this->m_tcpClient = client;
}

// 线程运行
void SocketThread::Run() {
    zlog_warn(Util::m_zlog, "以太网线程运行:IP=%s,port=%d",
              m_socketAddress->ip.c_str(), m_socketAddress->port);

    // 查找指定以太网从设备
    zlog_warn(Util::m_zlog, "查找指定以太网从设备");
    FindSocketSlaveDevs();
    zlog_warn(Util::m_zlog, "查找指定以太网从设备,数量=%d",
              m_socketSlaveDevs.size());

    // 设备IP和端口地址
    Address addr(m_socketAddress->ip, m_socketAddress->port);
    TcpClient *client = m_tcpClient;

    // 存取从设备信息数组
    Device::SlaveDev *slaveDev = NULL;
    PtrArray::iterator slaveIter;
    int ret = ErrorInfo::ERR_OK;
    String recvData;
    recvData.clear();

    // 通讯状态
    bool comSuccess = true;
    int uNoResponse = 0; // 当前读取失败的次数
    int openFailed  = 0;

    // pcs
    Pcs pcs;
    pcs.SetIsTcp(true);
    pcs.SetTcp(client);

    // // ChargePile
    // ChargePile chargePile;
    // chargePile.SetIsTcp(true);
    // chargePile.SetTcp(client);

    // dcdc
    DCDC dcdc;
    dcdc.SetIsTcp(true);
    dcdc.SetTcp(client);

    // 电表
    Wattmeter watt;
    watt.SetIsTcp(true);
    watt.SetTcp(client);

    // 电能质量
    PowerDetection power;
    power.SetIsTcp(true);
    power.SetTcp(client);

    // DIDO模块
    DiDoModle dido;
    dido.SetIsTcp(true);
    dido.SetTcp(client);

    // 光伏模拟器
    PVSimulator simulator;
    simulator.SetIsTcp(true);
    simulator.SetTcp(client);

    // Pvs
    Pvs pvs;
    pvs.SetIsTcp(true);
    pvs.SetTcp(client);

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

    // 复合节点控制器 - Mingyu
    CNCU cncu;
    cncu.SetIsTcp(true);
    cncu.SetTcp(client);

    // 线路阻抗模拟器
    ImpedanceSimulator ImpSim;
    ImpSim.SetIsTcp(true);
    ImpSim.SetTcp(client);

    // 故障模拟器
    FaultSimultor FaultSim;
    FaultSim.SetTcp(client);

    // 绝缘监测仪
    InsulationMonitor insulationMonitor;
    insulationMonitor.SetIsTcp(true);
    insulationMonitor.SetTcp(client);

    // 交流负载
    AcLoad acLoad;
    acLoad.SetTcp(client);

    // 晶石 STS
    STS sts;
    sts.SetTcp(client);

    zlog_debug(Util::m_zlog, "以太网从设备采集主程序执行");
    while (true) {
        // 判断线程是否结束
        if (m_state == Thread::STOPPING) {
            zlog_warn(Util::m_zlog, "socket线程结束");
            break;
        }

        // 判断是否超过计算次数，认为通讯有问题
        if (uNoResponse > ARRAY_LENGTH_16) {
            zlog_warn(Util::m_zlog, "超过计算次数，网口有问题，关闭网口");
            uNoResponse = 0;
            comSuccess  = false;
            openFailed  = 0;
            client->Close();
        }

        // 判断socket是否打开
        if (!client->IsOpened()) {
            // 如果socket关闭，则重新打开socket
            zlog_warn(Util::m_zlog,
                      "ip = %s,port = %d,socket关闭，重新打开socket",
                      addr.GetIPString().c_str(), addr.GetPort());
            comSuccess = client->Connect(addr);
            if (comSuccess) {
                openFailed = 0;
            } else {
                openFailed++;
                if (openFailed > 3) {
                    sleep(60);
                } else {
                    sleep(1);
                }
            }

            continue;
        }

        for (slaveIter = m_socketSlaveDevs.begin();
             slaveIter != m_socketSlaveDevs.end(); slaveIter++) {
            slaveDev = reinterpret_cast<Device::SlaveDev *>(*slaveIter);

            if (NULL == slaveDev) {
                continue;
            }

            // 判断设备名称
            zlog_debug(Util::m_zlog, "判断设备名称devtype = %d",
                       slaveDev->devType);
            switch (slaveDev->devType) {
            // 判断设备厂商和型号来确定具体通信协议
            case DeviceType::DEVICE_TYPE_PCS:          // PCS
            case DeviceType::DEVICE_TYPE_BIPOLAR_ACDC: // 双极性AC/DC
                ret = pcs.Read(slaveDev);
                break;
            // case DeviceType::DEVICE_TYPE_CHARGE_PILE: // 充电桩
            //     ret = chargePile.Preset(slaveDev);
            case DeviceType::DEVICE_TYPE_DI_MODULE: // DI模块
                ret = dido.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_POWER_DETEC: // 电能质量检测
                ret = power.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIGNAL_MODULE: // 模拟量采集
            case DeviceType::DEVICE_TYPE_WATT:          // 电表
                ret = watt.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_DCDC: // 电能质量检测
                ret = dcdc.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIMU_PV: // 光伏模拟器
                ret = simulator.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_PVS_XV: // PVS
                ret = pvs.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SINMU_LIB: // 电池模拟器
            case DeviceType::DEVICE_TYPE_LIION:     // 电池
                ret = bms.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIMU_DC_LOAD: // 直流负载
            case DeviceType::DEVICE_TYPE_DC_LOAD:      // 直流负载
                ret = dcload.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIMU_AC_GRID: // 电网模拟器
                ret = acSource.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_CNCU: // 复合节电控制器 - Mingyu
                ret = cncu.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIMU_CABLE: // 线路阻抗模拟器 - Mingyu
                ret = ImpSim.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SINMU_GZ: // 故障模拟器 - Mingyu
                ret = FaultSim.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_INSULATION_MONITOR: // 绝缘监测仪 -
                                                             //  Mingyu
                ret = insulationMonitor.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_SIMU_RLC: // RLC负载
                ret = acLoad.Read(slaveDev);
                break;
            case DeviceType::DEVICE_TYPE_STS: // 晶石 STS 泰开项目
                ret = sts.Read(slaveDev);
                break;

            default:
                break;
            };

            msleep(400);
        }
        msleep(300);
    }
}

void SocketThread::FindSocketSlaveDevs() {
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
            zlog_debug(Util::m_zlog, "从设备:%s,%s", slaveDev->name.c_str(),
                       commType->desc.c_str());
            m_socketSlaveDevs.push_back(slaveDev);
        }
    }

    return;
}
