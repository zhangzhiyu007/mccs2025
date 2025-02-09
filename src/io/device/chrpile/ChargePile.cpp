/*
 * ChargePile.cpp
 *
 *   创建日期: 2013-10-9
 *   作          者: 马中华
 */

#include "ChargePile.h"
// #include "ChargePileControl.h"

ChargePile::ChargePile() {
    // TODO 构造函数

    m_tcpClient = NULL;
}

ChargePile::~ChargePile() {
    // TODO 析构函数
}

void ChargePile::SetTcp(TcpClient *tcpClient) {
    assert(NULL != tcpClient);
    m_tcpClient = tcpClient;
}

void ChargePile::SetIsTcp(bool IsTcp) { m_isTcp = IsTcp; }

void ChargePile::SetCan(Can *can) { assert(can != NULL); }

int ChargePile::Read(Device::SlaveDev *dev, Can::FrameInfo &frame) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ErrorInfo::ERR_NULL;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    // case DevManu::Manu_XVJI_CPC:
    //     ret = this->ChargePileControlRead(dev);
    //     break;
    default:
        break;
    };

    return ret;
}

int ChargePile::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ErrorInfo::ERR_NULL;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    // case DevManu::Manu_XVJI_CPC:
    //     ret = this->ChargePileControlPreset(dev);
    //     break;
    default:
        break;
    };

    return ret;
}

// int ChargePile::ChargePileControlRead(Device::SlaveDev *dev) {
//     int ret = ErrorInfo::ERR_OK;
//     ChargerController cc;
//     cc.SetTcp(m_tcpClient);
//     ret = cc.Read(dev);
//     return ret;
// }

// int ChargePile::ChargePileControlPreset(Device::SlaveDev *dev) {
//     int ret = ErrorInfo::ERR_OK;
//     ChargerController cc;
//     cc.SetTcp(m_tcpClient);
//     ret = cc.Write(dev);
//     return ret;
// }
