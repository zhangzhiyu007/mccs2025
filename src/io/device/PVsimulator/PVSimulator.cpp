/*
 * PVSimulator.cpp
 *
 *  Created on: 2013-10-10
 *      Author: liuhaitao
 */

#include "PVSimulator.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

#include "ANSimulator.h"
#include "ANvhSimultor.h"
#include "ChromaSimultor.h"
#include "JSHJSimultor.h"
#include "N35200.h"


PVSimulator::PVSimulator() {
    // TODO Auto-generated constructor stub
    m_com       = NULL;
    m_tcpClient = NULL;
    m_isTcp     = false;
}

PVSimulator::~PVSimulator() {
    // TODO Auto-generated destructor stub
}

void PVSimulator::SetCom(Com *com) {
    assert(NULL != com);
    m_com = com;
}

void PVSimulator::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

// 设置TcpClient指针
void PVSimulator::SetTcp(TcpClient *tcpClient) {
    assert(NULL != tcpClient);
    m_tcpClient = tcpClient;
}

int PVSimulator::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_Parwa: // 菊水皇家
        ret = this->JSHJPVSimulatorRead(dev);
        break;
    case DevManu::Manu_CHROMA: // chroma
        ret = this->ChromaSimulatorRead(dev);
        break;
    case DevManu::Manu_AINUO: // 艾诺仪器
        ret = this->ANSimulatorRead(dev);
        break;
    case DevManu::Manu_AINUO_vh:
        ret = this->ANvhSimultorRead(dev);
        break;
    case DevManu::Manu_XVJI_N35200:
        ret = this->N35200Read(dev);
        break;
    default:
        break;
    };

    return ret;
}

int PVSimulator::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_Parwa: // 菊水皇家
        ret = this->JSHJPVSimulatorPreset(dev);
        break;
    case DevManu::Manu_CHROMA: // chroma
        ret = this->ChromaSimulatorPreset(dev);
        break;
    case DevManu::Manu_AINUO:
        ret = this->ANSimulatorPreset(dev);
        break;
    case DevManu::Manu_AINUO_vh:
        ret = this->ANvhSimultorPreset(dev);
        break;
    case DevManu::Manu_XVJI_N35200:
        ret = this->N35200Preset(dev);
        break;

    default:
        break;
    };

    return ret;
}


int PVSimulator::N35200Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    N35200 n35200;
    n35200.SetTcpClient(this->m_tcpClient);
    ret = n35200.Read(dev);
    return ret;
}

int PVSimulator::N35200Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    N35200 n35200;
    n35200.SetTcpClient(this->m_tcpClient);
    ret = n35200.Preset(dev);
    return ret;
}

int PVSimulator::JSHJPVSimulatorRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    JSHJSimultor simultor;
    simultor.SetCom(this->m_com);
    ret = simultor.Read(dev);
    return ret;
}

int PVSimulator::JSHJPVSimulatorPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    JSHJSimultor simultor;
    simultor.SetCom(this->m_com);
    ret = simultor.Preset(dev);
    return ret;
}

int PVSimulator::ChromaSimulatorRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ChromaSimultor simultor;
    simultor.SetTcp(this->m_tcpClient);
    ret = simultor.Read(dev);
    return ret;
}

int PVSimulator::ChromaSimulatorPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ChromaSimultor simultor;
    simultor.SetTcp(this->m_tcpClient);
    ret = simultor.Preset(dev);
    return ret;
}
int PVSimulator::ANSimulatorRead(Device::SlaveDev *dev) {

    int ret = ErrorInfo::ERR_OK;
    ANSimultor AiNuo;
    AiNuo.SetTcp(this->m_tcpClient);
    // AiNuo.SetIsTcp(m_isTcp);
    ret = AiNuo.Read(dev);

    return ret;
}

int PVSimulator::ANSimulatorPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ANSimultor AiNuo;
    // AiNuo.SetIsTcp(m_isTcp);
    AiNuo.SetTcp(this->m_tcpClient);
    ret = AiNuo.Preset(dev);

    return ret;
}

int PVSimulator::ANvhSimultorRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ANvhSimultor AiNuo;
    AiNuo.SetTcp(this->m_tcpClient);
    // AiNuo.SetIsTcp(m_isTcp);
    ret = AiNuo.Read(dev);

    return ret;
}

int PVSimulator::ANvhSimultorPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ANvhSimultor AiNuo;
    // AiNuo.SetIsTcp(m_isTcp);
    AiNuo.SetTcp(this->m_tcpClient);
    ret = AiNuo.Preset(dev);

    return ret;
}