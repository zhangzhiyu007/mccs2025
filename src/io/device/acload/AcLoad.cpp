/*
 * AcLoad.cpp
 *
 *  Created on: 2013-10-21
 *      Author: liuhaitao
 */

#include "AcLoad.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

#include "AAC3810.h"
#include "AcFaultSimulator.h"
#include "./ChargePileControl.h"
#include "ChromaLoad.h"
#include "PvAcLoad.h"
// #include "RlcLoad.h"
#include "ShWenShunLoad.h"

#include "./RLCLoad.h"

AcLoad::AcLoad() {
    // TODO Auto-generated constructor stub
    m_com       = NULL;
    m_ctrlCom   = NULL;
    m_tcpClient = NULL;
}

AcLoad::~AcLoad() {
    // TODO Auto-generated destructor stub
}

void AcLoad::SetCom(Com *com) { m_com = com; }

void AcLoad::SetCtrlCom(Com *ctrlCom) { m_ctrlCom = ctrlCom; }

// 设置TcpClient指针
void AcLoad::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int AcLoad::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ErrorInfo::ERR_NULL;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_Parwa: // 菊水皇家
        ret = this->PvAcLoadRead(dev);
        break;
    case DevManu::Manu_VILVA: // 威尔华
        ret = this->VilvaLoadRead(dev);
        break;
    case DevManu::Manu_SHWENSHUN: // 上海文顺
        ret = this->ShWenShunLoadRead(dev);
        break;
    case DevManu::Manu_CHROMA: // Chroma
        ret = this->ChromaLoadRead(dev);
        break;
    case DevManu::Manu_ZJNC:
        ret = this->AAC3810LoadRead(dev);
        break;
    case DevManu::Manu_XVJI_Rlc:
        ret = this->RlcRead(dev);
        break;
    case DevManu::Manu_XVJI_AcF:
        ret = this->AcFaultSimulatorRead(dev);
        break;
    case DevManu::Manu_XVJI_CPC:
        ret = this->ChargePileControlRead(dev);
        break;
    default:
        break;
    };

    return ret;
}

int AcLoad::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_Parwa: // 菊水皇家
        ret = this->PvAcLoadPreset(dev);
        break;
    case DevManu::Manu_VILVA: // 威尔华
        ret = this->VilvaLoadPreset(dev);
        break;
    case DevManu::Manu_SHWENSHUN: // 上海文顺
        ret = this->ShWenShunLoadPreset(dev);
        break;
    case DevManu::Manu_CHROMA: // Chroma
        ret = this->ChromaLoadPreset(dev);
        break;
    case DevManu::Manu_ZJNC:
        ret = this->AAC3810LoadPreset(dev);
        break;
    case DevManu::Manu_XVJI_Rlc: //
        ret = this->RlcPreset(dev);
        break;
    case DevManu::Manu_XVJI_AcF: //
        ret = this->AcFaultSimulatorPreset(dev);
        break;
    case DevManu::Manu_XVJI_CPC:
        ret = this->ChargePileControlPreset(dev);
        break;
    default:
        break;
    };

    return ret;
}
int AcLoad::AcFaultSimulatorRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    AcFaultSimulator ac;
    ac.SetTcpClient(m_tcpClient);
    ret = ac.Read(dev);
    return ret;
}

int AcLoad::AcFaultSimulatorPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    AcFaultSimulator ac;
    ac.SetTcpClient(m_tcpClient);
    ret = ac.Preset(dev);
    return ret;
}

int AcLoad::RlcRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    RLCLoad rlc;
    rlc.SetTcp(m_tcpClient);
    ret = rlc.Read(dev);
    return ret;
}
int AcLoad::RlcPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    RLCLoad rlc;
    rlc.SetTcp(m_tcpClient);
    ret = rlc.Preset(dev);
    return ret;
}

int AcLoad::PvAcLoadRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    PvAcLoad ac;
    ac.SetTcp(this->m_tcpClient);
    ret = ac.Read(dev);
    return ret;
}

int AcLoad::PvAcLoadPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    PvAcLoad ac;
    ac.SetTcp(this->m_tcpClient);
    ret = ac.Preset(dev);
    return ret;
}

int AcLoad::VilvaLoadRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    m_ac.SetCom(this->m_com);
    ret = m_ac.Read(dev);
    return ret;
}

int AcLoad::VilvaLoadPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    m_ac.SetCtrlCom(this->m_ctrlCom);
    ret = m_ac.Preset(dev);
    return ret;
}

int AcLoad::ShWenShunLoadRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ShWenShunLoad ac;
    ac.SetTcp(m_tcpClient);
    ret = ac.Read(dev);
    return ret;
}

int AcLoad::ShWenShunLoadPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ShWenShunLoad ac;
    ac.SetTcp(m_tcpClient);
    ret = ac.Preset(dev);
    return ret;
}

int AcLoad::ChromaLoadRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ChromaLoad ac;
    ac.SetTcp(m_tcpClient);
    ret = ac.Read(dev);
    return ret;
}

int AcLoad::ChromaLoadPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ChromaLoad ac;
    ac.SetTcp(m_tcpClient);
    ret = ac.Preset(dev);
    return ret;
}

int AcLoad::AAC3810LoadRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    AAC3810 load;
    load.SetTcpClient(m_tcpClient);
    ret = load.Read(dev);

    return ret;
}

int AcLoad::AAC3810LoadPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    AAC3810 load;
    load.SetTcpClient(m_tcpClient);

    ret = load.Preset(dev);
    return ret;
}

int AcLoad::ChargePileControlRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ChargerController cc;
    cc.SetTcp(m_tcpClient);
    ret = cc.Read(dev);
    return ret;
}

int AcLoad::ChargePileControlPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ChargerController cc;
    cc.SetTcp(m_tcpClient);
    ret = cc.Write(dev);
    return ret;
}