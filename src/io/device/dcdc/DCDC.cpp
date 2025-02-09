/*
 * DCDC.cpp
 *
 *  Created on: 2013-10-14
 *      Author: liuhaitao
 */

#include "DCDC.h"
#include "ECMDCDC.h"
#include "JingShiDCDC.h"
#include "STGKDCDC.h"

DCDC::DCDC() {
    // TODO Auto-generated constructor stub
    m_can       = NULL;
    m_isTcp     = false;
    m_com       = NULL;
    m_tcpClient = NULL;
}

DCDC::~DCDC() {
    // TODO Auto-generated destructor stub
}

void DCDC::SetCan(Can *can) {
    assert(can != NULL);
    m_can = can;
}

//设置是否采用以太网通讯
void DCDC::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

void DCDC::SetCom(Com *com) { m_com = com; }

//设置TcpClient指针
void DCDC::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }


int DCDC::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    if (NULL == dev) {
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_HNZY: //汇能智远
        ret = this->STGKRead(dev);
        break;
    case DevManu::Manu_ECM: // 艾克迈
        ret = this->ECMRead(dev);
        break;
    case DevManu::Manu_JINGSHI: //西安晶石
        ret = this->JingshiRead(dev);
        break;
    default:
        break;
    };

    return ret;
}
int DCDC::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    if (NULL == dev) {
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_HNZY: //汇能智远
        ret = this->STGKPreset(dev);
        break;
    case DevManu::Manu_ECM: //艾科迈
        ret = this->ECMPreset(dev);
        break;
    case DevManu::Manu_JINGSHI:
        ret = this->JingshiPreset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int DCDC::STGKRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    STGKDCDC dcdc;
    dcdc.SetIsTcp(m_isTcp);
    dcdc.SetTcp(m_tcpClient);
    ret = dcdc.Read(dev);
    return ret;
}

int DCDC::STGKPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    STGKDCDC dcdc;
    dcdc.SetIsTcp(m_isTcp);
    dcdc.SetTcp(m_tcpClient);
    ret = dcdc.Preset(dev);
    return ret;
}



int DCDC::ECMRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    EmcDCDC dcdc;

    dcdc.SetTcp(m_tcpClient);

    ret = dcdc.Read(dev);
    return ret;
}

int DCDC::ECMPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    EmcDCDC dcdc;

    dcdc.SetTcp(m_tcpClient);

    ret = dcdc.Preset(dev);

    return ret;
}

int DCDC::JingshiRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    JingshiDCDC dcdc;
    dcdc.SetTcp(this->m_tcpClient);

    ret = dcdc.Read(dev);
    return ret;
}

int DCDC::JingshiPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    JingshiDCDC dcdc;
    dcdc.SetTcp(this->m_tcpClient);
    ret = dcdc.Preset(dev);
    return ret;
}
