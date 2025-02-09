/*
 * BMS.cpp
 *
 *  Created on: 2013-10-14
 *      Author: liuhaitao
 */

#include "BMS.h"
#include "AquaNa.h"
#include "FgyBMS.h"
#include "FlyWheel.h"
#include "ChromaBattery.h"
#include "GreeBatt.h"
#include "SolidLi.h"
#include "SunGrowBms.h"
#include "TaikaiBMS.h"
#include "VRB.h"
#include "Vana.h"
#include "ZheDaBatt.h"

BMS::BMS() {
    // TODO Auto-generated constructor stub
    m_can   = NULL;
    m_isTcp = false;
    m_com   = NULL;
}

BMS::~BMS() {
    // TODO Auto-generated destructor stub
}

void BMS::SetCan(Can *can) {
    assert(can != NULL);
    m_can = can;
}

void BMS::SetCom(Com *com) {
    assert(com != NULL);
    m_com = com;
}

//设置是否采用以太网通讯
void BMS::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

//设置TcpClient指针
void BMS::SetTcp(TcpClient *tcpClient) { m_tcp = tcpClient; }

int BMS::Read(Device::SlaveDev *dev, Can::FrameInfo &frame) {
    int ret     = ErrorInfo::ERR_OK;
    int devManu = 0;
    int addr    = atoi(dev->slaveAddr.c_str());

    BYTE id[4] = {0, 0, 0, 0};

    id[0] = (frame.id >> 24) & 0xFF;
    id[1] = (frame.id >> 16) & 0xFF;
    id[2] = (frame.id >> 8) & 0xFF;
    id[3] = (frame.id >> 0) & 0xFF;

    if ((id[2] == 0x32) && (id[3] == 0xc8 + addr)) {
        devManu = DevManu::Manu_FGY; //峰谷源
    } else                           //其他公司设备
    {
    }

    if (DevManu::Manu_FGY == devManu) //峰谷源
    {
        ret = this->FgyBMSRead(dev, frame);
    }

    return ret;
}

int BMS::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    if (NULL == dev) {
        ret = ErrorInfo::ERR_NULL;
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_SUNGROW: //合肥阳光
        ret = this->SunGrowBmsRead(dev);
        break;
    case DevManu::Manu_CHROMA: // chroma
        ret = this->ChromaBattRead(dev);
        break;
    case DevManu::Manu_Zheda: //浙大
        ret = this->ZhedaBattRead(dev);
        break;
    case DevManu::Manu_GREE: //格力
        ret = this->GreeBattRead(dev);
        break;
    case DevManu::Manu_BENAN: // 贲安能源
        ret = this->BenAnBattRead(dev);
        break;
    case DevManu::Manu_DALIANHUAWU: //
        ret = this->VRBRead(dev);
        break;
    case DevManu::Manu_SHENYANGWEIKONG: //沈阳微控
        ret = this->FlyWheelRead(dev);
        break;
    case DevManu::Manu_HUAGONGWULI: // 中科院化工物理所
        ret = this->VanaRead(dev);
        break;
    case DevManu::Manu_HAIBOSICHUANG:
        ret = this->SolidLiRead(dev);
        break;
    case DevManu::Manu_TAIKAI:
        ret = this->TaikaiBMSRead(dev);
        break;
    default:
        break;
    };

    return ret;
}

int BMS::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    if (NULL == dev) {
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_FGY:
        ret = this->FgyBMSPreset(dev); //峰谷源
        break;
    case DevManu::Manu_SUNGROW:
        ret = this->SunGrowBmsPreset(dev);
        break;
    case DevManu::Manu_CHROMA: // chroma
        ret = this->ChromaBattPreset(dev);
        break;
    case DevManu::Manu_GREE: //格力
        ret = this->GreeBattPreset(dev);
        break;
    case DevManu::Manu_SHENYANGWEIKONG: // 沈阳微控
        ret = this->FlyWheelPreset(dev);
        break;
    case DevManu::Manu_HUAGONGWULI: // 中科院化工物理所
        ret = this->VanaPreset(dev);
        break;
    case DevManu::Manu_HAIBOSICHUANG:
        ret = this->SolidLiPreset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int BMS::FgyBMSRead(Device::SlaveDev *dev, Can::FrameInfo &frame) {
    int ret = ErrorInfo::ERR_OK;

    FgyBMS bms;
    bms.SetCan(this->m_can);
    ret = bms.Read(dev, frame);

    return ret;
}

int BMS::FgyBMSPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    FgyBMS bms;
    bms.SetCan(this->m_can);
    ret = bms.Preset(dev);
    return ret;
}

int BMS::SunGrowBmsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SunGrowBms bms;
    bms.SetCom(this->m_com);
    ret = bms.Read(dev);
    return ret;
}

int BMS::SunGrowBmsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SunGrowBms bms;
    bms.SetCom(this->m_com);
    ret = bms.Preset(dev);
    return ret;
}

int BMS::ChromaBattRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ChromaBattery m_batt;
    m_batt.SetTcp(this->m_tcp);
    ret = m_batt.Read(dev);
    return ret;
}

int BMS::ChromaBattPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ChromaBattery m_batt;
    m_batt.SetTcp(this->m_tcp);
    ret = m_batt.Preset(dev);
    return ret;
}

int BMS::GreeBattRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    GreeBatt batt;
    batt.SetTcp(this->m_tcp);
    batt.SetIsTcp(m_isTcp);
    ret = batt.Read(dev);
    return ret;
}

int BMS::GreeBattPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    GreeBatt batt;
    batt.SetTcp(this->m_tcp);
    batt.SetIsTcp(m_isTcp);
    ret = batt.Preset(dev);
    return ret;
}

int BMS::ZhedaBattRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ZheDaBatt bms;
    bms.SetTcp(this->m_tcp);
    ret = bms.Read(dev);
    return ret;
}

int BMS::BenAnBattRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    AquaNa aquaNa;
    aquaNa.SetTcp(this->m_tcp);

    ret = aquaNa.Read(dev);
    return ret;
}

int BMS::VRBRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    VRB bms;
    bms.SetTcp(this->m_tcp);

    ret = bms.Read(dev);

    return ret;
}

int BMS::FlyWheelRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    FlyWheel bms;

    bms.SetTcp(this->m_tcp);

    ret = bms.Read(dev);

    return ret;
}

int BMS::FlyWheelPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    FlyWheel bms;
    bms.SetTcp(this->m_tcp);

    ret = bms.Preset(dev);

    return ret;
}

int BMS::VanaRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    Vana bms;
    bms.SetTcp(this->m_tcp);

    ret = bms.Read(dev);

    return ret;
}

int BMS::VanaPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    Vana bms;
    bms.SetTcp(m_tcp);
    ret = bms.Preset(dev);

    return ret;
}

int BMS::SolidLiRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SolidLi bms;
    bms.SetTcp(m_tcp);
    ret = bms.Read(dev);

    return ret;
}
int BMS::SolidLiPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    SolidLi bms;
    bms.SetTcp(m_tcp);
    ret = bms.Preset(dev);

    return ret;
}

int BMS::TaikaiBMSRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    TaikaiBMS bms;
    bms.SetTcp(this->m_tcp);
    ret = bms.Read(dev);

    return ret;
}
