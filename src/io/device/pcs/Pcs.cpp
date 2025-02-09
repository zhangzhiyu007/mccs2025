/*
 * Pcs.cpp
 *
 *   创建日期: 2013-6-24
 *   作         者: mzh
 */

#include "Pcs.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

#include "./JingshiPcs.h"
#include "BipolarAcdc.h"
#include "BosPcs.h"
#include "BydPcs.h"
#include "SinexcelPcs.h"
#include "SunGrowPcs.h"
#include "EcmHFIPcs.h"

Pcs::Pcs() {
    // TODO 构造函数
    m_isTcp     = false;
    m_com       = NULL;
    m_tcpClient = NULL;
}

Pcs::~Pcs() {
    // TODO 析构函数
}

//设置是否采用以太网通讯
void Pcs::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

void Pcs::SetCom(Com *com) { m_com = com; }

//设置TcpClient指针
void Pcs::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int Pcs::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_BYD: //比亚迪
        ret = this->BydPcsRead(dev);
        break;
    case DevManu::Manu_SUNGROW: //合肥阳光
        ret = this->SunGrowPcsRead(dev);
        break;
    case DevManu::Manu_BOS: //博奥斯
        ret = this->BosPcsRead(dev);
        break;
    case DevManu::Manu_SINEXCEL: //深圳盛弘
        ret = this->SinexcelPcsRead(dev);
        break;
    case DevManu::Manu_ECM: //艾科迈
        ret = this->EcmPcsRead(dev);
        break;
    case DevManu::Manu_TSINGHUA: //清华
        ret = this->BipolarAcdcRead(dev);
        break;
    case DevManu::Manu_JINGSHI: // 晶石
        ret = this->JingshiPcsRead(dev);
        break;
    case DevManu::Manu_XVJI: // 许继
        ret = this->EcmHFIPcsRead(dev);
        break;
    default:
        break;
    };

    return ret;
}

int Pcs::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_BYD: //比亚迪
        ret = this->BydPcsPreset(dev);
        break;
    case DevManu::Manu_SUNGROW: //合肥阳光
        ret = this->SunGrowPcsPreset(dev);
        break;
    case DevManu::Manu_BOS: //博奥斯
        ret = this->BosPcsPreset(dev);
        break;
    case DevManu::Manu_SINEXCEL: //深圳盛弘
        ret = this->SinexcelPcsPreset(dev);
        break;
    case DevManu::Manu_ECM: //艾科迈
        ret = this->EcmPcsPreset(dev);
        break;
    case DevManu::Manu_TSINGHUA: //清华
        ret = this->BipolarAcdcPreset(dev);
        break;
    case DevManu::Manu_XVJI: // 许继
        ret = this->EcmHFIPcsPreset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int Pcs::BydPcsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    BydPcs pcs;
    pcs.SetCom(this->m_com);
    ret = pcs.Read(dev);
    return ret;
}

int Pcs::BydPcsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    BydPcs pcs;
    pcs.SetCom(this->m_com);
    ret = pcs.Preset(dev);
    return ret;
}

int Pcs::SunGrowPcsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SunGrowPcs pcs;
    pcs.SetCom(this->m_com);
    ret = pcs.Read(dev);
    return ret;
}

int Pcs::SunGrowPcsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SunGrowPcs pcs;
    pcs.SetCom(this->m_com);
    ret = pcs.Preset(dev);
    return ret;
}

int Pcs::BosPcsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    BosPcs pcs;
    pcs.SetTcp(this->m_tcpClient);
    ret = pcs.Read(dev);
    return ret;
}

int Pcs::BosPcsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    BosPcs pcs;
    pcs.SetTcp(this->m_tcpClient);
    ret = pcs.Preset(dev);
    return ret;
}

int Pcs::SinexcelPcsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SinexcelPcs pcs;
    if (dev->commType == 2) {
        pcs.SetTcp(this->m_tcpClient);
    } else if (dev->commType == 1) {
        pcs.setCom(this->m_com);
    }

    ret = pcs.Read(dev);
    return ret;
}

int Pcs::SinexcelPcsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SinexcelPcs pcs;
    if (dev->commType == 2) {
        pcs.SetTcp(this->m_tcpClient);
    } else if (dev->commType == 1) {
        pcs.setCom(this->m_com);
    }
    ret = pcs.Preset(dev);
    return ret;
}

int Pcs::EcmPcsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    m_ecmPcs.SetTcp(this->m_tcpClient);
    m_ecmPcs.SetIsTcp(m_isTcp);
    ret = m_ecmPcs.Read(dev);
    return ret;
}

int Pcs::EcmPcsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    m_ecmPcs.SetTcp(this->m_tcpClient);
    m_ecmPcs.SetIsTcp(m_isTcp);
    ret = m_ecmPcs.Preset(dev);
    return ret;
}

int Pcs::BipolarAcdcRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    BipolarAcdc acdc;
    acdc.SetTcp(this->m_tcpClient);
    acdc.SetIsTcp(m_isTcp);
    ret = acdc.Read(dev);
    return ret;
}

int Pcs::BipolarAcdcPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    BipolarAcdc acdc;
    acdc.SetTcp(this->m_tcpClient);
    acdc.SetIsTcp(m_isTcp);
    ret = acdc.Preset(dev);
    return ret;
}

int Pcs::JingshiPcsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    JingshiPcs acdc;
    acdc.SetTcp(this->m_tcpClient);

    ret = acdc.Read(dev);

    return ret;
}

int Pcs::JingshiPcsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    JingshiPcs acdc;
    acdc.SetTcp(this->m_tcpClient);

    ret = acdc.Preset(dev);

    return ret;
}

int Pcs::EcmHFIPcsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    EcmHFIPcs pcs;
    pcs.SetTcp(this->m_tcpClient);

    ret = pcs.Read(dev);

    return ret;
}

int Pcs::EcmHFIPcsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    EcmHFIPcs pcs;
    pcs.SetTcp(this->m_tcpClient);

    ret = pcs.Preset(dev);

    return ret;
}